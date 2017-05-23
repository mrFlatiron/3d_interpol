#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H
#include <QDialog>
#include <QSize>
#include "kernel/least_squares_interpol.h"
#include "thread_ret.h"
#include "computational_components.h"
#include <memory>

class QSpinBox;
class QGLWidget;
class QLabel;
class QLineEdit;
class QPushButton;
class QTimer;
class QProgressBar;
class QToolBar;
class QToolButton;
class QComboBox;
class gl_plot_widget;
class least_squares_interpol;
class msr_dqgmres_initializer;
class thread_ret;
class thread_args;
class computational_components;
enum class graph_mode;

class main_window : public QDialog
{
  Q_OBJECT
private:
  gl_plot_widget *m_glwidget;
  QSpinBox    *m_phi_partition;
  QSpinBox    *m_r_partition;
  QSpinBox    *m_threads;
  QLineEdit   *m_rhs_time;
  QLineEdit   *m_solution_time;
  QLineEdit   *m_matrix_time;
  QLineEdit   *m_max_residual;
  QLineEdit   *m_avg_residual;
  QLineEdit   *m_l2;
  QToolButton *m_compute_pb;
  QToolButton *m_turn_left;
  QToolButton *m_turn_right;
  QToolButton *m_camera_up;
  QToolButton *m_camera_down;
  QToolButton *m_zoom_in;
  QToolButton *m_zoom_out;
  QComboBox    *m_graph_cb;
  QProgressBar *m_progress_bar;
  QToolBar     *m_head_toolbar;

  least_squares_interpol *m_interpol;
  thread_ret m_ret_struct;
  computational_components m_components;
  std::vector<thread_args> m_thread_args;
  QTimer *m_timer;

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
  graph_mode get_mode ();
  static void *computing_thread_worker (void *args);
signals:
  void interpolation_done ();
  void buttons_ready ();
private slots:
  void interpolate ();
  void disable_pb_and_emit ();
  void enable_pb ();
  void check_if_done ();
};

#endif // MAIN_WINDOW_H
