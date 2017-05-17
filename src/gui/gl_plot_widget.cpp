#include "gl_plot_widget.h"
#include "GL/glu.h"
#include "kernel/least_squares_interpol.h"



QSize gl_plot_widget::sizeHint () const
{
  return QSize (640, 480);
}

gl_plot_widget::gl_plot_widget (QWidget *parent) : QGLWidget (parent)
{
  m_camera_lift = 1;
  m_camera_angle_xy = 0;
  m_indices = nullptr;
  m_vertices = nullptr;
  m_colors = nullptr;
  m_interpolator = nullptr;
  m_vertices_uptodate = false;
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

  double r = pow ((m_x_max - m_x_min), 2) +
             pow ((m_y_max - m_y_min), 2) +
             pow ((m_z_max - m_z_min), 2);

  r = sqrt (r);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity ();
  glOrtho (1.1 * m_x_min, 1.1 * m_x_max, -r , r, - 1.1 * r, 1.1 * r);


  glMatrixMode(GL_MODELVIEW);

  glLoadIdentity();
//  gluLookAt (r + 2, r + 2, r + 2, -r, -r, -r, 0, 0, 1);
//  max = sqrt (a1 * a1 + b1 * b1);
//  gluLookAt (r * cos (m_camera_angle_xy), r * sin (m_camera_angle_xy), r, 0.0,0.0,0.0,0.0,0.0,1.0);
  gluLookAt (sqrt (0.2) * cos (m_camera_angle_xy), sqrt (0.2) * sin (m_camera_angle_xy), m_camera_lift * sqrt (0.2), 0, 0, 0, 0, 0, 1);




  glColor3f(1.0,0.0,0.0);
  glBegin(GL_LINES);
      glVertex3f(m_x_min, m_y_min, m_z_min);
      glVertex3f(m_x_max,  m_y_min, m_z_min);
      glColor3f(0.0,1.0,0.0);
      glVertex3f(m_x_min, m_y_min, m_z_min);
      glVertex3f(m_x_min, m_y_max, m_z_min);
      glColor3f(0.0,0.0,1.0);
      glVertex3f(m_x_min, m_y_min, m_z_min);
      glVertex3f(m_x_min, m_y_min, m_z_max);
  glEnd();




//  GLfloat faceColor[4] = {0.9, 0.5, 0.1, 1.0};
//  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, faceColor);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3,GL_FLOAT, 0 , m_vertices);

  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1,1);

  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
//  glDrawElements(GL_TRIANGLES, 6 * m * n, GL_UNSIGNED_SHORT, m_indices);

  glBegin (GL_TRIANGLES);
  {
    for (int i = 0; i < 6 * m * n; i++)
      {
        int index = m_indices[i];
        glColor3f (m_colors[3 * index], m_colors[3 * index + 1], m_colors[3 * index + 2]);
        glArrayElement (m_indices[i]);
      }
  }
  glEnd ();
  glDisable(GL_POLYGON_OFFSET_FILL);

  if (m <= 20 && n < 20)
    {
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

      glLineWidth(1.0f);
      glColor3f(0.0,0.2,0.0);
      glBegin (GL_TRIANGLES);
      {
        for (int i = 0; i < 6 * m * n; i++)
          glArrayElement (m_indices[i]);
      }
      glEnd ();

    }


  glDisableClientState(GL_VERTEX_ARRAY);

  qglColor(QColor::fromCmykF(1.0,1.0,1.0,0.0));





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

void gl_plot_widget::fill_vertices ()
{
  if (m_vertices_uptodate)
    return;
  m_full_packs = 0;
  int m = m_interpol_meta.m;
  int n = m_interpol_meta.n;
  if (m_vertices)
    delete[] m_vertices;
  if (m_indices)
    delete[] m_indices;
  if (m_colors)
    delete[] m_colors;


  m_vertices = new GLfloat[3 * (m + 1) * (n + 1)];
  m_colors = new GLfloat[3 * (m + 1) * (n + 1)];

  int iter = 0;

  for (int i = 0; i <= m; i++)
    for (int j = 0; j <= n; j++)
      {
        double x, y, z;
        m_interpolator->map_to_xy ((double)i / m, (double)j / n, x, y);

        z = m_interpolator->node_val (i, j);

        update_bounds (x, y, z);

        m_vertices[3 * iter] = (GLfloat)x;
        m_vertices[3 * iter + 1] = (GLfloat)y;
        m_vertices[3 * iter + 2] = (GLfloat)z;
        iter++;
      }

  m_indices = new int[6 * m * n];
  iter = 0;
  for (int i = 0; i < m; i++)
    for (int j = 0; j < n; j++)
      {
        int v[6];
        v[0] = i * (n + 1) + j;
        v[1] = v[0] + 1;
        v[2] = v[1] + n + 1;
        v[3] = v[0];
        v[4] = v[2];
        v[5] = v[0] + n + 1;
        for (int k = 0; k < 6; k++)
          {
            m_indices[iter] = v[k];
            iter++;
          }
      }
  fill_colors ();
  m_vertices_uptodate = true;
}

void gl_plot_widget::fill_colors ()
{
  int vert_count = (m_interpol_meta.m + 1) * (m_interpol_meta.n + 1);
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

void gl_plot_widget::camera_update (int direction)
{
  if (direction > 0)
    m_camera_angle_xy += 0.25 * 2 * M_PI;
  else
    m_camera_angle_xy -= 0.25 * 2 * M_PI;

  m_camera_angle_xy = fmod (m_camera_angle_xy, 2 * M_PI);
  update ();
}

void gl_plot_widget::camera_left ()
{
  m_camera_angle_xy += 0.1;
  m_camera_angle_xy = fmod (m_camera_angle_xy, 2 * M_PI);
  update ();
}

void gl_plot_widget::camera_right ()
{
  m_camera_angle_xy -= 0.1;
  m_camera_angle_xy = fmod (m_camera_angle_xy, 2 * M_PI);
  update ();
}

void gl_plot_widget::camera_up ()
{
  m_camera_lift += 0.05;
  update ();
}

void gl_plot_widget::camera_down ()
{
  m_camera_lift -= 0.05;
  update ();
}



void gl_plot_widget::mousePressEvent (QMouseEvent *event)
{
  switch (event->button ())
    {
    case Qt::LeftButton:
//      camera_update (1);
      break;
    case Qt::RightButton:
//        camera_update (-1);
        break;
    default:
      break;
    }
}

void gl_plot_widget::keyPressEvent(QKeyEvent *event)
{
  switch(event->key())
  {
      case 74:
          camera_update (+1);
          break;
      case 75:
          camera_update (-1);
          break;
    default:
      break;
    }
}
