#include "gl_plot_widget.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "kernel/least_squares_interpol.h"
#include "test_functions/test_functions.h"
#include "gl_triangle_painter.h"

#include "gl_tools/camera_helper.h"



QSize gl_plot_widget::sizeHint () const
{
  return QSize (640, 480);
}

gl_plot_widget::gl_plot_widget (QWidget *parent) : QGLWidget (parent)
{
  setMinimumWidth (50);
  setMinimumHeight (50);
  m_mode = graph_mode::approximation;
  m_indices = nullptr;
  m_vertices = nullptr;
  m_colors = nullptr;
  m_interpolator = nullptr;
  m_vertices_uptodate = false;
  glhandler ().camera ().nice_init ();
}

gl_plot_widget::~gl_plot_widget ()
{
  if (m_indices)
    delete[] m_indices;
  if (m_vertices)
    delete[] m_vertices;
  if (m_colors)
    delete[] m_colors;
}

void gl_plot_widget::initializeGL ()
{
  QColor qDark = QColor::fromCmykF(0.1,0.1,0.1,0.05);
  qglClearColor(qDark);
  glEnable(GL_DEPTH_TEST);
}

void gl_plot_widget::resizeGL (int width, int height)
{
  glViewport(0,0,width,height);
}

void gl_plot_widget::paintGL ()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



  if (!m_interpolator)
    return;

  int m = m_interpol_meta.m;
  int n = m_interpol_meta.n;

  glLineWidth(2.0f);
  glColor3f(0.0,0.0,1.0);
  fill_vertices ();

  glhandler ().set_modelview_identity ();
  glhandler ().to_common_volume (m_x_min, m_x_max, m_y_min, m_y_max, m_z_min, m_z_max);
  glhandler ().apply_camera ();
  glhandler ().gl_set_modelview ();
  glhandler ().gl_set_ortho ((float)width () / (float)height ());

  draw_axis ();

  m_painter.draw_fill ();

  if (m <= 20 && n <= 20)
    {
      m_painter.draw_grid ();
    }
(void)m;
 (void)n;
}

void gl_plot_widget::set_interpolator (least_squares_interpol *interpolator)
{
  m_interpolator = interpolator;
  m_vertices_uptodate = false;
  m_interpol_meta_valid = true;
  m_interpol_meta.a1 = m_interpolator->a1 ();
  m_interpol_meta.b1 = m_interpolator->b1 ();
  m_interpol_meta.m = m_interpolator->m ();
  m_interpol_meta.n = m_interpolator->n ();
}

void gl_plot_widget::set_mode (graph_mode mode)
{
  if (mode == m_mode)
    return;

  m_mode = mode;
  m_vertices_uptodate = false;
}

void gl_plot_widget::wheelEvent (QWheelEvent *event)
{
  const int step = 8 * 15;
  int steps = event->delta () / step;

  if (steps > 0)
    for (int i = 0; i < steps; i++)
      glhandler ().camera ().zoom_in ();
  else
    for (int i = 0; i > steps; i--)
      glhandler ().camera ().zoom_out ();

  event->accept();
  update ();
}

void gl_plot_widget::mousePressEvent (QMouseEvent *event)
{
  m_mouse_tracker.start_tracking (event);
}

void gl_plot_widget::mouseMoveEvent (QMouseEvent *event)
{
  QPoint delta = m_mouse_tracker.get_delta (event);

  int x = delta.x ();
  int y = delta.y ();

  double xpart = (double)x/width ();
  double ypart = (double)y/height ();

  int full_turns_x = 2;
  int full_turns_y = 1;

  double turns_x = full_turns_x * 2 * M_PI * xpart;
  double turns_y = full_turns_y * 2 * M_PI * ypart;

  glhandler ().camera ().rotate_oxy (turns_x);
  glhandler ().camera ().rotate_ozy (turns_y);
  update ();
}

void gl_plot_widget::mouseDoubleClickEvent (QMouseEvent *event)
{
  (void)event;
  glhandler ().camera ().nice_init ();
  update ();
}

gl_handler &gl_plot_widget::glhandler ()
{
  return m_glhandler;
}

void gl_plot_widget::fill_vertices ()
{
  if (m_vertices_uptodate)
    return;
  int m = m_interpol_meta.m;
  int n = m_interpol_meta.n;
  if (m_vertices)
    delete[] m_vertices;
  if (m_indices)
    delete[] m_indices;
  if (m_colors)
    delete[] m_colors;


  m_vertices = new GLfloat[3 * (2 * m + 1) * (2 * n + 1)];
  m_colors = new GLfloat[3 * (2 * m + 1) * (2 * n + 1)];

  int iter = 0;

  for (int k = 0; k <= 2 * m; k++)
    {
    for (int l = 0; l <= 2 * n; l++)
      {

        double x, y, z;
        m_interpolator->map_to_xy ((double)k / 2 / m, (double)l / 2 / n, x, y);

        switch (m_mode)
          {
          case graph_mode::approximation:
            z = m_interpolator->between_node_val (k, l);
            break;
          case graph_mode::residual:
            z = m_interpolator->between_node_val (k, l) - func (x, y);
            break;
          }

        if (k + l == 0)
          {
            m_z_min = z;
            m_z_max = z;
          }

        update_bounds (x, y, z);

        m_vertices[3 * iter] = (GLfloat)x;
        m_vertices[3 * iter + 1] = (GLfloat)y;
        m_vertices[3 * iter + 2] = (GLfloat)z;
        iter++;
      }
    }

  m_indices = new GLuint[24 * m * n];
  iter = 0;
  for (int k = 0; k < 2 * m - 1; k++)
    for (int l = 0; l < 2 * n; l++)
      {
        int v[6];
        v[0] = k * (2 * n + 1) + l;
        v[1] = v[0] + 1;
        v[2] = v[1] + 2 * n + 1;
        v[3] = v[0];
        v[4] = v[2];
        v[5] = v[0] + 2 * n + 1;
        for (int s = 0; s < 6; s++)
          {
            m_indices[iter] = v[s];
            iter++;
          }
      }
  for (int l = 0; l < 2 * n; l++)
    {
      int v[6];
      v[0] = (2 * m - 1) * (2 * n + 1) + l;
      v[1] = v[0] + 1;
      v[2] = /*n + 1 - */(l + 1);
      v[3] = v[0];
      v[4] = v[2];
      v[5] = /*n + 1 - */l;
      for (int s = 0; s < 6; s++)
        {
          m_indices[iter] = v[s];
          iter++;
        }
    }
  fill_colors ();

  m_painter.set_primary_data (m_vertices, 3 * (2 * m + 1) * (2 * n + 1),
                              m_indices, 24 * m * n,
                              m_colors);
  m_vertices_uptodate = true;
}

void gl_plot_widget::fill_colors ()
{
  int vert_count = (2 * m_interpol_meta.m + 1) * (2 * m_interpol_meta.n + 1);
  for (int i = 0; i < vert_count; i++)
    {
      double x = m_vertices[3 * i];
      double y = m_vertices[3 * i + 1];
      double z = m_vertices[3 * i + 2];
      GLfloat red = (x - m_x_min)    / (m_x_max - m_x_min);
      GLfloat green = (y - m_y_min) / (m_y_max - m_y_min);
      GLfloat blue = (z - m_z_min) / (m_z_max - m_z_min);
      m_colors[3 * i] = red;
      m_colors[3 * i + 1] = green;
      m_colors[3 * i + 2] = blue;
    }
}

void gl_plot_widget::draw_axis ()
{
  glBegin(GL_LINES);
      glColor3f(1.0,0.0,0.0);
      glVertex3f(m_x_min, m_y_min, m_z_min);
      glVertex3f(m_x_max,  m_y_min, m_z_min);
      glColor3f(0.0,1.0,0.0);
      glVertex3f(m_x_min, m_y_min, m_z_min);
      glVertex3f(m_x_min, m_y_max, m_z_min);
      glColor3f(0.0,0.0,1.0);
      glVertex3f(m_x_min, m_y_min, m_z_min);
      glVertex3f(m_x_min, m_y_min, m_z_max);
  glEnd();
}

void gl_plot_widget::update_bounds (const double x, const double y, const double z)
{
  if (x < m_x_min)
    m_x_min = x;
  if (x > m_x_max)
    m_x_max = x;
  if (y < m_y_min)
    m_y_min = y;
  if (y > m_y_max)
    m_y_max = y;
  if (z < m_z_min)
    m_z_min = z;
  if (z > m_z_max)
    m_z_max = z;
}


