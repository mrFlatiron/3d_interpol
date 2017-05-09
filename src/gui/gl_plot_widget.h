#ifndef GL_PLOT_WIDGET_H
#define GL_PLOT_WIDGET_H
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL>
#include <QColor>
class QColor;
class least_squares_interpol;

class gl_plot_widget : public QGLWidget, protected QOpenGLFunctions
{
  Q_OBJECT
private:
  QColor m_purple;
  least_squares_interpol *m_interpolator;
public:
  gl_plot_widget (QWidget *parent = nullptr);
  ~gl_plot_widget ();
  QSize sizeHint () const;
  void initializeGL();
  void resizeGL(int width, int height);
  void paintGL();
  void set_interpolator (least_squares_interpol *interpolator);
};

#endif // GL_PLOT_WIDGET_H
