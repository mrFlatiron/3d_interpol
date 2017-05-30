#ifndef GL_PLOT_WIDGET_H
#define GL_PLOT_WIDGET_H
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL>
#include <QColor>
#include "gl_triangle_painter.h"
#include "mouse_tracker.h"
#include "gl_tools/gl_handler.h"

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

class gl_plot_widget : public QGLWidget
{
  Q_OBJECT

private:
  graph_mode m_mode;
  least_squares_interpol *m_interpolator;
  GLfloat *m_vertices;
  GLfloat *m_colors;
  GLuint *m_indices;
  gl_triangle_painter m_painter;
  GLfloat m_x_max = 1e-6;
  GLfloat m_x_min = -1e-6;
  GLfloat m_y_max = 1e-6;
  GLfloat m_y_min = -1e-6;
  GLfloat m_z_max = 1e-6;
  GLfloat m_z_min = -1e-6;
  bool m_vertices_uptodate;
  interpol_meta m_interpol_meta;
  bool m_interpol_meta_valid;

  gl_handler m_glhandler;

  mouse_tracker m_mouse_tracker;
public:
  gl_plot_widget (QWidget *parent = nullptr);
  ~gl_plot_widget ();
  QSize sizeHint () const;
  void initializeGL();
  void resizeGL(int width, int height);
  void paintGL();
  void set_interpolator (least_squares_interpol *interpolator);
  void set_mode (graph_mode mode);
  void wheelEvent (QWheelEvent *event);
  void mousePressEvent (QMouseEvent *event);
  void mouseMoveEvent (QMouseEvent *event);
  void mouseDoubleClickEvent (QMouseEvent *event);
private:
  gl_handler &glhandler ();
  void fill_vertices ();
  void fill_colors ();
  void draw_axis ();
  void update_bounds (const double x, const double y, const double z);
  void turn (direction dir);
  void mult_modelview ();
};

#endif // GL_PLOT_WIDGET_H
