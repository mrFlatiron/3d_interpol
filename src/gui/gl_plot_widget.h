#ifndef GL_PLOT_WIDGET_H
#define GL_PLOT_WIDGET_H
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL>
#include <QColor>
class QColor;
class least_squares_interpol;

class gl_plot_widget : public QGLWidget
{
  Q_OBJECT

private:
  float m_camera_angle_xy;
  float m_camera_angle_z;
  least_squares_interpol *m_interpolator;
  GLfloat *m_vertices;
  GLushort *m_indices;
  double m_z_max;
  bool m_vertices_uptodate;
public:
  gl_plot_widget (QWidget *parent = nullptr);
  ~gl_plot_widget ();
  QSize sizeHint () const;
  void initializeGL();
  void resizeGL(int width, int height);
  void paintGL();
  void set_interpolator (least_squares_interpol *interpolator);
  void mousePressEvent (QMouseEvent *event);
  void keyPressEvent (QKeyEvent *event);
private:
  void fill_vertices ();
public slots:
  void camera_update (int direction);
  void camera_left ();
  void camera_right ();
};

#endif // GL_PLOT_WIDGET_H
