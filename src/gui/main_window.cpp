#include "main_window.h"
#include "gui/gl_plot_widget.h"
#include "kernel/least_squares_interpol.h"
#include "lib/sparse_matrix/msr_thread_dqgmres_solver.h"
#include "lib/sparse_matrix/msr_dqgmres_initializer.h"
#include "test_functions/test_functions.h"
#include "computational_components.h"
#include "ttime/ttime.h"
#include "thread_args.h"
#include "thread_ret.h"
#include <cmath>
#include <vector>
#include <cstdlib>

#include <QSpinBox>
#include <QIcon>
#include <QLineEdit>
#include <QLabel>
#include <QString>
#include <QPushButton>
#include <QToolBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QProgressBar>
#include <QKeySequence>
#include <QComboBox>



main_window::main_window (const double a0, const double a1, const double b0, const double b1) : QDialog (),
  m_a0 (a0),
  m_a1 (a1),
  m_b0 (b0),
  m_b1 (b1)
{
  m_interpol = nullptr;
  m_timer = new QTimer (this);
  m_timer->setSingleShot (true);
  create_widgets ();
  set_layouts ();
  do_connects ();
}

main_window::~main_window ()
{
    if (m_interpol)
        delete m_interpol;
}

QSize main_window::sizeHint () const
{
  return QSize (1024, 780);
}



void main_window::create_widgets ()
{
  m_glwidget = new gl_plot_widget (this);

  m_phi_partition = new QSpinBox (this);
  m_phi_partition->setMinimum (1);
  m_phi_partition->setValue (1);
  m_phi_partition->setMaximum (10000000);

  m_r_partition = new QSpinBox (this);
  m_r_partition->setMinimum (1);
  m_r_partition->setValue (1);
  m_r_partition->setMaximum (1000000);

  m_threads = new QSpinBox (this);
  m_threads->setMinimum (1);
  m_threads->setValue (4);

  m_rhs_time = new QLineEdit (this);
  m_rhs_time->setReadOnly (true);

  m_solution_time = new QLineEdit (this);
  m_solution_time->setReadOnly (true);

  m_matrix_time = new QLineEdit (this);
  m_matrix_time->setReadOnly (true);

  m_max_residual = new QLineEdit (this);
  m_max_residual->setReadOnly (true);

  m_avg_residual = new QLineEdit (this);
  m_avg_residual->setReadOnly (true);

  m_l2 = new QLineEdit (this);
  m_l2->setReadOnly (true);

  m_graph_cb = new QComboBox (this);
  m_graph_cb->addItem ("Approximation");
  m_graph_cb->addItem ("Residual");

  m_compute_pb = new QToolButton (this);
  m_compute_pb->setIcon (QIcon (":/icons/plot_3d.png"));
  m_compute_pb->setShortcut (QKeySequence ("F5"));

  m_turn_left = new QToolButton (this);
  m_turn_left->setIcon (QIcon (":/icons/left_arrow.png"));
  m_turn_left->setAutoRepeat (true);
  m_turn_left->setShortcut (QKeySequence ("Ctrl+H"));

  m_turn_right = new QToolButton (this);
  m_turn_right->setIcon (QIcon (":/icons/right_arrow.png"));
  m_turn_right->setAutoRepeat (true);
  m_turn_right->setShortcut (QKeySequence ("Ctrl+J"));

  m_camera_up = new QToolButton (this);
  m_camera_up->setIcon (QIcon (":/icons/up_arrow.png"));
  m_camera_up->setAutoRepeat (true);
  m_camera_up->setShortcut (QKeySequence ("Ctrl+K"));

  m_camera_down = new QToolButton (this);
  m_camera_down->setIcon (QIcon (":/icons/down_arrow.png"));
  m_camera_down->setAutoRepeat (true);
  m_camera_down->setShortcut (QKeySequence ("Ctrl+L"));

  m_zoom_in = new QToolButton (this);
  m_zoom_in->setIcon (QIcon (":/icons/plus.png"));
  m_zoom_in->setAutoRepeat (true);
  m_zoom_in->setShortcut (QKeySequence("Ctrl++"));

  m_zoom_out = new QToolButton (this);
  m_zoom_out->setIcon (QIcon (":/icons/minus.png"));
  m_zoom_out->setAutoRepeat (true);
  m_zoom_out->setShortcut (QKeySequence("Ctrl+-"));

  m_head_toolbar = new QToolBar (this);
  m_head_toolbar->setOrientation (Qt::Horizontal);

  m_head_toolbar->addWidget (m_compute_pb);
  m_head_toolbar->addWidget (m_turn_left);
  m_head_toolbar->addWidget (m_turn_right);
  m_head_toolbar->addWidget (m_camera_up);
  m_head_toolbar->addWidget (m_camera_down);
  m_head_toolbar->addWidget (m_zoom_in);
  m_head_toolbar->addWidget (m_zoom_out);

  m_progress_bar = new QProgressBar (this);
  m_progress_bar->setRange (0, 3);
  m_progress_bar->setValue (0);

}

void main_window::set_layouts ()
{
  QVBoxLayout *vlo_1 = new QVBoxLayout;
  {
    QHBoxLayout *hlo_1 = new QHBoxLayout;
    {
      hlo_1->addWidget (m_graph_cb, 0, Qt::AlignLeft);
      hlo_1->addWidget (m_head_toolbar);
      hlo_1->addWidget (m_progress_bar);
    }
    vlo_1->addLayout (hlo_1);
    QHBoxLayout *hlo_2 = new QHBoxLayout;
    {
      hlo_2->addWidget (m_glwidget);
      m_glwidget->setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Expanding);
      QGridLayout *glo_1 = new QGridLayout;
      {
        glo_1->addWidget (new QLabel ("OPHI nodes:", this), 0, 0);
        glo_1->addWidget (m_phi_partition, 0, 1);
        m_phi_partition->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Fixed);

        glo_1->addWidget (new QLabel ("OR nodes:", this), 1, 0);
        glo_1->addWidget (m_r_partition, 1, 1);
        m_r_partition->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Fixed);

        glo_1->addWidget (new QLabel ("Threads:", this), 2, 0);
        glo_1->addWidget (m_threads, 2, 1);
        m_threads->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Fixed);

        glo_1->addWidget (new QLabel ("Matrix set time, sec:", this), 3, 0);
        glo_1->addWidget (m_matrix_time, 3, 1);
        m_matrix_time->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Fixed);

        glo_1->addWidget (new QLabel ("RHS set time, sec:", this), 4, 0);
        glo_1->addWidget (m_rhs_time, 4, 1);
        m_rhs_time->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Fixed);

        glo_1->addWidget (new QLabel ("System solved in, sec:", this), 5, 0);
        glo_1->addWidget (m_solution_time, 5, 1);
        m_solution_time->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Fixed);



        glo_1->addWidget (new QLabel ("Grid max residual", this), 6, 0);
        glo_1->addWidget (m_max_residual, 6, 1);
        m_max_residual->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Fixed);

        glo_1->addWidget (new QLabel ("Grid avrg residual", this), 7, 0);
        glo_1->addWidget (m_avg_residual, 7, 1);
        m_avg_residual->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Fixed);

        glo_1->addWidget (new QLabel ("Grid L2 residual:", this), 8, 0);
        glo_1->addWidget (m_l2, 8, 1);
        m_l2->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Fixed);

//        glo_1->addWidget (m_turn_left, 9, 0);
//        m_turn_left->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Fixed);

//        glo_1->addWidget (m_turn_right, 9, 1);
//        m_turn_right->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Fixed);

//        glo_1->addWidget (m_camera_up, 10, 0);
//        m_camera_up->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Fixed);

//        glo_1->addWidget (m_camera_down, 10, 1);
//        m_camera_down->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Fixed);
      }
      hlo_2->addLayout (glo_1);
    }
    vlo_1->addLayout (hlo_2);
    vlo_1->setAlignment (hlo_2, Qt::AlignTop);
  }
  setLayout (vlo_1);
}

void main_window::do_connects ()
{
  connect (m_compute_pb, SIGNAL (clicked ()), this, SLOT (interpolate ()));
  connect (this, SIGNAL (interpolation_done ()), m_glwidget, SLOT (update ()));
  connect (this, SIGNAL (interpolation_done ()), this, SLOT (enable_pb ()));
  connect (m_timer, SIGNAL (timeout ()), this, SLOT (check_if_done()));
  connect (m_turn_left, SIGNAL (pressed ()), m_glwidget, SLOT (camera_left ()));
  connect (m_turn_right, SIGNAL (pressed ()), m_glwidget, SLOT (camera_right ()));
  connect (m_camera_up, SIGNAL (pressed ()), m_glwidget, SLOT (camera_up ()));
  connect (m_camera_down, SIGNAL (pressed ()), m_glwidget, SLOT (camera_down ()));
  connect (m_zoom_in, SIGNAL (pressed ()), m_glwidget, SLOT (zoom_in ()));
  connect (m_zoom_out, SIGNAL (pressed ()), m_glwidget, SLOT (zoom_out ()));
}

graph_mode main_window::get_mode ()
{
  switch (m_graph_cb->currentIndex ())
    {
    case 0:
      return graph_mode::approximation;
    case 1:
      return graph_mode::residual;
    }
  std::abort ();
}

void main_window::interpolate ()
{
//  if (m_interpol)
//      delete m_interpol;
//  else
//      m_interpol = new least_squares_interpol;

    if (!m_interpol)
        m_interpol = new least_squares_interpol;
    else
      {
        pthread_barrier_destroy (&(m_components.barrier));
      }

  m_interpol->set_ellipse (m_a0, m_a1, m_b0, m_b1);
  int m = m_phi_partition->value ();
  int n = m_r_partition->value ();
  m_interpol->set_partition (m, n);
  int p = m_threads->value ();

  m_components.solution.resize ((m + 1) * (n + 1));
  m_components.rhs.resize ((m + 1) * (n + 1));
  pthread_barrier_init (&(m_components.barrier), 0, p);

  pthread_t pt;

  m_thread_args.resize (p);
  for (int i = 1; i <= p; i++)
    {
      m_thread_args[i - 1].handler = thread_handler (i, p, &(m_components.barrier), 1);
      m_thread_args[i - 1].components = &(m_components);
      m_thread_args[i - 1].ret = &m_ret_struct;
      m_thread_args[i - 1].interpol = m_interpol;
    }

  m_ret_struct.init ();

  m_compute_pb->setDisabled (true);

  for (int t = 1; t <= p; t++)
    pthread_create (&pt, 0, computing_thread_worker, m_thread_args.data () + t - 1);

  m_timer->start (100);

}

void main_window::disable_pb_and_emit ()
{
  m_compute_pb->setDisabled (true);
  m_compute_pb->blockSignals (true);
  emit buttons_ready ();
}

void main_window::enable_pb ()
{
  m_compute_pb->setEnabled (true);
}

void main_window::check_if_done ()
{
  int progress = 0;
  if (m_ret_struct.set_system_done)
    progress++;
  if (m_ret_struct.set_rhs_done)
    progress++;
  if (m_ret_struct.system_solved)
    progress++;

  m_progress_bar->setValue (progress);


  if (!m_ret_struct.system_solved)
    {
      m_timer->start (100);
    }
  else
    {
      m_interpol->set_expansion_coefs (m_components.solution);
      int m = m_interpol->m ();
      int n = m_interpol->n ();
      double maxval = -1;
      double l2  = 0;
      double avg = 0;
      double func_max = 0;
      for (int k = 0; k <= 2 * m; k++)
        for (int l = 0; l <= 2 * n; l++)
          {
            double phi = (double) k / 2 / m;
            double r = (double) l / 2 / n;
            double z = m_interpol->between_node_val (k, l);
            if (z > func_max)
              func_max = fabs (z);
            double val = fabs (z - m_interpol->func_val (phi, r));
            l2 += val * val;
            avg += val;
            if (val > maxval)
              {
                maxval = val;
              }
          }
      l2 = sqrt (l2);
      avg /= ((2 * m + 1) * (2 * n + 1));

      m_matrix_time->setText (QString::number (m_ret_struct.set_system_time));
      m_solution_time->setText (QString::number (m_ret_struct.system_solve_time));
      m_rhs_time->setText (QString::number (m_ret_struct.set_rhs_time));
      m_max_residual->setText (QString::number (maxval));
      m_avg_residual->setText (QString::number (avg));
      m_l2->setText (QString::number (l2));
      m_glwidget->set_interpolator (m_interpol);
      m_glwidget->set_mode (get_mode ());
      emit interpolation_done ();
    }

}


void *main_window::computing_thread_worker (void *args_)
{
  thread_args *args = (thread_args*)args_;

  thread_handler handler = args->handler;

  computational_components *comps = args->components;

  thread_ret *ret = args->ret;

  least_squares_interpol *interpol = args->interpol;

  if (handler.is_first ())
    {
      double begin = get_monotonic_time ();
      interpol->set_system (comps->gramm);
      ret->set_system_time = get_monotonic_time () - begin;
      ret->set_system_done = true;
    }
  handler.barrier_wait ();

  double set_rhs_time = get_monotonic_time ();
  interpol->parallel_set_rhs (handler, comps->rhs, func, false);
  set_rhs_time = get_monotonic_time () - set_rhs_time;

  if (handler.is_first ())
    {
      ret->set_rhs_time = set_rhs_time;
      ret->set_rhs_done = true;
    }

  if (handler.is_first ())
    {
      simple_vector x_ini (comps->gramm.n ());
      comps->initializer = new msr_dqgmres_initializer (handler.t_id (),
                                                        handler.p (),
                                                        comps->gramm,
                                                        preconditioner_type::jacobi,
                                                        5, 300, 1e-16, x_ini, comps->solution,
                                                        comps->rhs);
    }

  handler.barrier_wait ();


  msr_thread_dqgmres_solver solver (handler.t_id (), *(comps->initializer));

  handler.barrier_wait ();

  double system_solve_time = get_monotonic_time ();
  solver_state state =  solver.dqgmres_solve ();
  ret->system_solve_time = get_monotonic_time () - system_solve_time;

  if (handler.is_first ())
    {
      switch (state)
        {
        case solver_state::OK:
          printf ("Converged!\n");
          break;
        case solver_state::TOO_SLOW:
          printf ("Convergence is too slow\n");
          break;
        case solver_state::MAX_ITER:
          printf ("Failed to converge\n");
          break;
        }
    }
  if (handler.is_first ())
    {
      ret->system_solved = true;
      delete comps->initializer;
    }

    return args_;
}
