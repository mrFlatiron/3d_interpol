#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H
#include <QDialog>
#include <QSize>
//#include <QtOpenGL/QGLWidget>

class QSpinBox;
class QGLWidget;
class QLabel;
class QLineEdit;
class QPushButton;
class gl_plot_widget;


class main_window : public QDialog
{
  Q_OBJECT
private:
  gl_plot_widget *m_glwidget;
  QSpinBox *m_phi_partition;
  QSpinBox *m_r_partition;
  QSpinBox *m_threads;
  QLineEdit *m_rhs_time;
  QLineEdit *m_solution_time;
  QLineEdit *m_matrix_time;
  QLineEdit *m_max_residual;
  QLineEdit *m_l2;
  QPushButton *m_compute_pb;
public:
  main_window ();
  ~main_window ();
  QSize sizeHint () const;
private:
  void create_widgets ();
  void set_layouts ();
  void do_connects ();
};

#endif // MAIN_WINDOW_H
