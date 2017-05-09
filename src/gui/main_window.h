#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H
#include <QDialog>
#include <QSize>
#include "kernel/least_squares_interpol.h"
//#include <QtOpenGL/QGLWidget>

class QSpinBox;
class QGLWidget;
class QLabel;
class QLineEdit;
class QPushButton;
class gl_plot_widget;
class least_squares_interpol;

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
  QLineEdit *m_avg_residual;
  QLineEdit *m_l2;
  QPushButton *m_compute_pb;

  least_squares_interpol m_interpol;
  double m_a0;
  double m_a1;

  double m_b0;
  double m_b1;
public:
  main_window (const double a0, const double a1, const double b0, const double b1);
  ~main_window ();
  QSize sizeHint () const;
private:
  void create_widgets ();
  void set_layouts ();
  void do_connects ();
  static void *computing_thread_worker (void *args);
private slots:
  void interpolate ();
};

#endif // MAIN_WINDOW_H
