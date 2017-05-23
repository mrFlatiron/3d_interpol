#include "gl_plot_widget.h"
#include "GL/glu.h"
#include "kernel/least_squares_interpol.h"
#include "test_functions/test_functions.h"
#include "gl_triangle_painter.h"



QSize gl_plot_widget::sizeHint () const
{
  return QSize (640, 480);
}

gl_plot_widget::gl_plot_widget (QWidget *parent) : QGLWidget (parent)
{
  setMinimumWidth (50);
  m_mode = graph_mode::approximation;
  m_indices = nullptr;
  m_vertices = nullptr;
  m_colors = nullptr;
  m_interpolator = nullptr;
  m_vertices_uptodate = false;
  m_model_view_matrix.setToIdentity ();
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
//  glMatrixMode(GL_PROJECTION);
//  glLoadIdentity();
//  double max;
//  if (!m_interpolator)
//    gluPerspective(100,width/height,0.1,15);
//  else
//    {
//      double a1 = m_interpolator->a1 ();
//      double b1 = m_interpolator->b1 ();
//      max = (a1 > b1) ? 3 * a1 : 3 * b1;
//      gluPerspective (100, width/height, 0.1, 100);
//    }

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

 to_common_volume ();
 set_camera ();
 mult_modelview ();
 mult_projection ();

 draw_axis ();

  m_painter.draw_fill ();

  if (m <= 20 && n <= 20)
    {
      m_painter.draw_grid ();
    }

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

  m_z_max = 1e-13;
  m_z_min = -1e-13;

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

void gl_plot_widget::to_common_volume ()
{
  m_model_view_matrix.setToIdentity ();
  GLfloat x_len = (m_x_max - m_x_min);
  GLfloat y_len = (m_y_max - m_y_min);
  GLfloat z_len = (m_z_max - m_z_min);
  GLfloat x_scale,
      y_scale,
      z_scale;

  if (x_len < y_len)
    {
      GLfloat aspect_ratio = y_len / x_len;
      m_common_volume.xy_ratio = aspect_ratio;
      x_scale = 2. / x_len;
      y_scale = 2 * aspect_ratio / y_len;
      z_scale = 1 / z_len;
      m_common_volume.x_max = 1;
      m_common_volume.x_min = -1;
      m_common_volume.y_max = aspect_ratio;
      m_common_volume.y_min = -aspect_ratio;
    }
  else
    {
      GLfloat aspect_ratio = x_len / y_len;
      m_common_volume.xy_ratio = aspect_ratio;
      x_scale = 2 * aspect_ratio / x_len;
      y_scale = 2. / y_len;
      z_scale = 1 / z_len;
      m_common_volume.y_max = 1;
      m_common_volume.y_min = -1;
      m_common_volume.x_max = aspect_ratio;
      m_common_volume.x_min = -aspect_ratio;
    }
  m_common_volume.z_max = 1;
  m_common_volume.z_min = 0;

  QMatrix4x4 translator (1, 0, 0, -(m_x_max + m_x_min) / 2,
                         0, 1, 0, -(m_y_max + m_y_min) / 2,
                         0, 0, 1, -m_z_min,
                         0, 0, 0, 1);

  m_model_view_matrix.scale (QVector3D (x_scale, y_scale, z_scale));
  m_model_view_matrix *= translator;

}

void gl_plot_widget::mult_projection ()
{
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
//  GLfloat diag = sqrt ((pow (m_common_volume.x_max - m_common_volume.x_min, 2) +
//                 pow (m_common_volume.y_max - m_common_volume.y_min, 2) +
//                 pow (m_common_volume.z_max - m_common_volume.z_min, 2)));
//  GLfloat semidiag = sqrt ((pow (m_common_volume.x_max - m_common_volume.x_min, 2) +
//                            pow (m_common_volume.y_max - m_common_volume.y_min, 2)));
//  GLfloat coef = semidiag * m_camera.zoom_coef;

//  glOrtho (m_camera.zoom_coef * m_common_volume.x_min,
//           m_camera.zoom_coef * m_common_volume.x_max,
//           m_camera.zoom_coef * m_common_volume.x_min,
//           m_camera.zoom_coef * m_common_volume.x_max,
//           /*m_camera.zoom_coef * */-diag,
//          /* m_camera.zoom_coef * */diag);
  GLfloat side = m_camera.zoom_coef * m_common_volume.xy_ratio;
  glOrtho (-side, side,
           -side * height () / width (), side * height ()/ width (),
           -10, 10);
}

void gl_plot_widget::set_camera ()
{
  QMatrix4x4 rotator;
  rotator.setToIdentity ();
  rotator.rotate (180. / M_PI * m_camera.oz_angle, 1, 0, 0);
  rotator.rotate (180. / M_PI * m_camera.oxy_angle, 0, 0, 1);


  rotator *= m_model_view_matrix;
  m_model_view_matrix = rotator;
}

void gl_plot_widget::mult_modelview ()
{
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();
  glMultMatrixf (m_model_view_matrix.data ());
}
void gl_plot_widget::camera_left ()
{
  m_camera.move (direction::left);
  update ();
}

void gl_plot_widget::camera_right ()
{
  m_camera.move (direction::right);
  update ();
}

void gl_plot_widget::camera_up ()
{
  m_camera.move(direction::up);
  update ();
}

void gl_plot_widget::camera_down ()
{
  m_camera.move (direction::down);
  update ();
}

void gl_plot_widget::zoom_in ()
{
  m_camera.move (direction::in);
  update ();
}

void gl_plot_widget::zoom_out ()
{
  m_camera.move (direction::out);
  update ();
}

camera_params::camera_params ()
{
  oz_angle = M_PI / 4;
  oxy_angle = M_PI / 4;
  zoom_coef = 1;
  r = sqrt (0.2);
  x_eye = r * sin (oz_angle) * cos (oxy_angle);
  y_eye = r * sin (oz_angle) * sin (oxy_angle);
  z_eye = r * cos (oz_angle);

  x_norm = -x_eye;
  y_norm  = -y_eye;
  z_norm = r / cos (oz_angle) - z_eye;
}

void camera_params::move (direction direct)
{
//  double x_old_norm = x_norm;
//  double y_old_norm = y_norm;
//  double z_old_norm = z_norm;
//  double oz_cos_old = cos (oz_angle);

//  bool oz_crossed = false;
//  bool oxy_crossed = false;
//  (void)oxy_crossed;

  switch (direct)
    {
    case direction::left:
      oxy_angle += 0.1;
      oxy_angle = fmod (oxy_angle, 2 * M_PI);
      break;
    case direction::right:
      oxy_angle -= 0.1;
      oxy_angle = fmod (oxy_angle, 2 * M_PI);
      break;
    case direction::up:
      oz_angle += 0.1;
      oz_angle = fmod (oz_angle, 2 * M_PI);
      break;
    case direction::down:
      oz_angle -= 0.1;
      oz_angle = fmod (oz_angle, 2 * M_PI);
      break;
    case direction::in:
      zoom_coef /= 1.05;
      break;
    case direction::out:
      zoom_coef *= 1.05;
      break;
    }
}


