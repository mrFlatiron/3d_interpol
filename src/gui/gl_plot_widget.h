#ifndef GL_PLOT_WIDGET_H
#define GL_PLOT_WIDGET_H
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL>
#include <QColor>
class QColor;
class least_squares_interpol;

struct interpol_meta
{
  double a1;
  double b1;
  int m;
  int n;
};

class gl_plot_widget : public QGLWidget
{
  Q_OBJECT

private:
  static const int max_vertex_pack = 65532;
  int m_full_packs = 0;
  float m_camera_angle_xy;
  float m_camera_lift;
  least_squares_interpol *m_interpolator;
  GLfloat *m_vertices;
  int *m_indices;
  GLfloat *m_colors;
  double m_x_max = 1;
  double m_x_min = -1;
  double m_y_max = 1;
  double m_y_min = -1;
  double m_z_max = 1;
  double m_z_min = -1;
  bool m_vertices_uptodate;
  interpol_meta m_interpol_meta;
  bool m_interpol_meta_valid;
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
  void fill_colors ();
  void update_bounds (const double x, const double y, const double z);
public slots:
  void camera_update (int direction);
  void camera_left ();
  void camera_right ();
  void camera_up ();
  void camera_down ();
};

#endif // GL_PLOT_WIDGET_H
