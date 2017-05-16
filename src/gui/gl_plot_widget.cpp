#include "gl_plot_widget.h"
#include "GL/glu.h"
#include "kernel/least_squares_interpol.h"



QSize gl_plot_widget::sizeHint () const
{
  return QSize (640, 480);
}

gl_plot_widget::gl_plot_widget (QWidget *parent) : QGLWidget (parent)
{
  m_camera_angle_z = 0;
  m_camera_angle_xy = 0;
  m_indices = nullptr;
  m_vertices = nullptr;
  m_interpolator = nullptr;
  m_vertices_uptodate = false;
}

gl_plot_widget::~gl_plot_widget ()
{
  if (m_indices)
    delete[] m_indices;
  if (m_vertices)
    delete[] m_vertices;
}

void gl_plot_widget::initializeGL ()
{
  QColor qDark = QColor::fromCmykF(0.1,0.1,0.1,0.05);
  qglClearColor(qDark);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  static GLfloat lightPosition[4] = {-1.0, 1.0, 6.0, 1.0};
  glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
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

  int m = m_interpolator->m ();
  int n = m_interpolator->n ();
  double a1 = m_interpolator->a1 ();
  double b1 = m_interpolator->b1 ();




  glLineWidth(2.0f);
  glColor3f(0.0,0.0,1.0);
  fill_vertices ();
  double max = (a1 > b1) ?  a1 : b1;
  max = (max > m_z_max) ? max : m_z_max;
  double z_eye;

  if (m_z_max < 2)
    z_eye = m_z_max + 5;
  else
    z_eye = 2 * m_z_max;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity ();
  glOrtho (-max, max, -2 * max,  2 * max, 5 * max, -5 * max);


  glMatrixMode(GL_MODELVIEW);

  glLoadIdentity();
  max = sqrt (a1 * a1 + b1 * b1);
  gluLookAt (max * cos (m_camera_angle_xy), max * sin (m_camera_angle_xy), z_eye, 0.0,0.0,0.0,0.0,0.0,1.0);




//  glDisable(GL_LIGHTING);
//  glColor3f(0.0,0.0,1.0);
//  glBegin(GL_LINES);
//      glVertex3f(0.0, 0.0,0.0);
//      glVertex3f(a1,  0.0,0.0);
//      glVertex3f(0.0,0.0,0.0);
//      glVertex3f(0.0,b1, 0.0);
//      double z_axis = 1;
//      if (m_z_max > z_axis)
//        z_axis = m_z_max;
//      glVertex3f(0.0,0.0,0.0);

//      glVertex3f(0.0,0.0,z_axis);
//  glEnd();
//  glEnable(GL_LIGHTING);

  GLfloat faceColor[4] = {0.9, 0.5, 0.1, 1.0};
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, faceColor);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3,GL_FLOAT, 0 , m_vertices);

  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1,1);

  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glDrawElements(GL_TRIANGLES, 6 * m * n, GL_UNSIGNED_SHORT, m_indices);

  glDisable(GL_POLYGON_OFFSET_FILL);

  if (m <= 20 && n < 20)
    {
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      glDisable(GL_LIGHTING);
      glLineWidth(1.0f);
      glColor3f(0.0,0.2,0.0);
      glDrawElements(GL_TRIANGLES, 6 * m * n, GL_UNSIGNED_SHORT, m_indices);
      glEnable(GL_LIGHTING);
    }


  glDisableClientState(GL_VERTEX_ARRAY);

  qglColor(QColor::fromCmykF(1.0,1.0,1.0,0.0));





}

void gl_plot_widget::set_interpolator (least_squares_interpol *interpolator)
{
  m_interpolator = interpolator;
  m_vertices_uptodate = false;
}

void gl_plot_widget::fill_vertices ()
{
  if (m_vertices_uptodate)
    return;
  int m = m_interpolator->m ();
  int n = m_interpolator->n ();
  if (m_vertices)
    delete[] m_vertices;
  if (m_indices)
    delete[] m_indices;

  m_vertices = new GLfloat[3 * (m + 1) * (n + 1)];
  bool first = true;
  int iter = 0;
  int ur = 0;
  int ul = 0;
  int bl = 0;
  int br = 0;
  for (int i = 0; i <= m; i++)
    for (int j = 0; j <= n; j++)
      {
        double x, y, z;
        m_interpolator->map_to_xy ((double)i / m, (double)j / n, x, y);
        if (x * x + y * y > 16 && x * x + y * y < 4)
          {
            printf ("debug assert");
          }
        if (x > 0 && y > 0) ur++;
        if (x < 0 && y > 0) ul++;
        if (x < 0 && y < 0) bl++;
        if (x > 0 && y < 0) br++;
        z = m_interpolator->node_val (i, j);
        if (first)
          m_z_max = fabs (z);
        else if (fabs (z) > m_z_max)
          m_z_max = fabs (z);
        m_vertices[3 * iter] = (GLfloat)x;
        m_vertices[3 * iter + 1] = (GLfloat)y;
        m_vertices[3 * iter + 2] = (GLfloat)z;
        iter++;
      }

  printf ("ur = %d\n"
          "ul = %d\n"
          "bl = %d\n"
          "br = %d\n", ur, ul, bl, br);

  m_indices = new GLushort[6 * m * n];
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
  m_vertices_uptodate = true;
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
