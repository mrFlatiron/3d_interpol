#ifndef GL_PLOT_WIDGET_H
#define GL_PLOT_WIDGET_H
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL>
#include <QColor>
#include "gl_triangle_painter.h"
class QColor;
class least_squares_interpol;
class QMatrix4x4;

enum class graph_mode
{
  approximation,
  residual
};

enum class direction
{
  left,
  right,
  up,
  down,
  in,
  out
};

struct interpol_meta
{
  double a1;
  double b1;
  int m;
  int n;
};

class camera_params
{
public:
  GLfloat oxy_angle;
  GLfloat oz_angle;
  GLfloat zoom_coef;
public:
  camera_params ();
  void move (direction direct);
};

class common_volume
{
public:
  GLfloat x_min;
  GLfloat x_max;
  GLfloat y_min;
  GLfloat y_max;
  GLfloat z_min;
  GLfloat z_max;
  GLfloat xy_ratio;
  common_volume () {};
};

class gl_plot_widget : public QGLWidget
{
  Q_OBJECT

private:
  graph_mode m_mode;
  camera_params m_camera;
  least_squares_interpol *m_interpolator;
  GLfloat *m_vertices;
  GLfloat *m_colors;
  GLuint *m_indices;
  gl_triangle_painter m_painter;
  double m_x_max = 1e-6;
  double m_x_min = -1e-6;
  double m_y_max = 1e-6;
  double m_y_min = -1e-6;
  double m_z_max = 1e-6;
  double m_z_min = -1e-6;
  bool m_vertices_uptodate;
  interpol_meta m_interpol_meta;
  bool m_interpol_meta_valid;
  QMatrix4x4 m_model_view_matrix;
  common_volume m_common_volume;
public:
  gl_plot_widget (QWidget *parent = nullptr);
  ~gl_plot_widget ();
  QSize sizeHint () const;
  void initializeGL();
  void resizeGL(int width, int height);
  void paintGL();
  void set_interpolator (least_squares_interpol *interpolator);
  void set_mode (graph_mode mode);
private:
  void fill_vertices ();
  void fill_colors ();
  void draw_axis ();
  void update_bounds (const double x, const double y, const double z);
  void to_common_volume ();
  void mult_projection ();
  void set_camera ();
  void turn (direction dir);
  void mult_modelview ();
public slots:
  void camera_left ();
  void camera_right ();
  void camera_up ();
  void camera_down ();
  void zoom_in ();
  void zoom_out ();
};

#endif // GL_PLOT_WIDGET_H
