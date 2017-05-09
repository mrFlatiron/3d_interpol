#ifndef GL_PLOT_WIDGET_H
#define GL_PLOT_WIDGET_H
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL>
#include <QColor>
class QColor;

class gl_plot_widget : public QGLWidget, protected QOpenGLFunctions
{
  Q_OBJECT
private:
  QColor m_purple;
public:
  QSize sizeHint () const;
  gl_plot_widget (QWidget *parent = nullptr);
  ~gl_plot_widget ();
  void initializeGL();
  void resizeGL(int width, int height);
  void paintGL();
};

#endif // GL_PLOT_WIDGET_H
