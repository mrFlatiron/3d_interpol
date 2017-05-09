#include "gl_plot_widget.h"



QSize gl_plot_widget::sizeHint () const
{
  return QSize (640, 480);
}

gl_plot_widget::gl_plot_widget (QWidget *parent) : QGLWidget (parent)
{
  m_purple =  QColor::fromCmykF(0.39, 0.39, 0.0, 0.0);
}

gl_plot_widget::~gl_plot_widget ()
{

}

void gl_plot_widget::initializeGL()
{
  initializeOpenGLFunctions ();
//    qglClearColor (m_purple.dark ());
//    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_CULL_FACE);
//    glShadeModel(GL_SMOOTH);
//    glEnable(GL_LIGHTING);
//    glEnable(GL_LIGHT0);
//    glEnable(GL_MULTISAMPLE);
//    static GLfloat lightPosition[4] = { 0.5, 5.0, 7.0, 1.0 };
//    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
  glClearColor(0, 1, 0, 1);
//  glEnable(GL_DEPTH_TEST);
//  glEnable(GL_LIGHT0);
//  glEnable(GL_LIGHTING);
//  glEnable(GL_COLOR_MATERIAL);
//    GLfloat verts[] =
//    {
//        +0.0f, +1.0f,
//        -1.0f, -1.0f,
//        +1.0f, -1.0f,
//    };
//    GLuint myBufferID;
//    glGenBuffers(1, &myBufferID);
//    glBindBuffer(GL_ARRAY_BUFFER, myBufferID);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void gl_plot_widget::resizeGL(int width, int height)
{
//    int side = qMin(width, height);
//    glViewport((width - side) / 2, (height - side) / 2, side, side);

//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//#ifdef QT_OPENGL_ES_1
//    glOrthof(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);
//#else
//    glOrtho(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);
//#endif
//    glMatrixMode(GL_MODELVIEW);
}

void gl_plot_widget::paintGL()
{
  QSize viewport_size = size();
  glViewport(0, 0, viewport_size.width(), viewport_size.height());

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1, 2, 0, 1, 0, 3); // near and far match your triangle Z distance

  glMatrixMode(GL_MODELVIEW);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
//  glTranslatef(-1.5f,0.0f,-6.0f);

  glBegin(GL_TRIANGLES);
   glVertex3f( 1.0f, 0.0f, 1.0f);
   glVertex3f( 0.0f, 1.0f, -5.0f);
   glVertex3f( -1.0f, 0.0f, 0.0f);
  glEnd();


//    glRotatef(xRot / 16.0, 1.0, 0.0, 0.0);
//    glRotatef(yRot / 16.0, 0.0, 1.0, 0.0);
//    glRotatef(zRot / 16.0, 0.0, 0.0, 1.0);

}

void gl_plot_widget::set_interpolator (least_squares_interpol *interpolator)
{
  m_interpolator = interpolator;
}
