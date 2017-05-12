#include "main_window.h"
#include "gui/gl_plot_widget.h"
#include "kernel/least_squares_interpol.h"
#include "lib/sparse_matrix/msr_thread_dqgmres_solver.h"
#include "lib/sparse_matrix/msr_dqgmres_initializer.h"
#include "test_functions/test_functions.h"
#include "ttime/ttime.h"
#include <cmath>
#include <vector>

#include <QSpinBox>
#include <QLineEdit>
#include <QLabel>
#include <QString>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

main_window::main_window (const double a0, const double a1, const double b0, const double b1) : QDialog (),
  m_a0 (a0),
  m_a1 (a1),
  m_b0 (b0),
  m_b1 (b1)
{
  create_widgets ();
  set_layouts ();
  do_connects ();
}

main_window::~main_window ()
{

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
  m_phi_partition->setMaximum (4000);

  m_r_partition = new QSpinBox (this);
  m_r_partition->setMinimum (1);
  m_r_partition->setValue (1);
  m_r_partition->setMaximum (4000);

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

  m_compute_pb = new QPushButton (this);
  m_compute_pb->setText ("Play");

  m_turn_left = new QPushButton (this);
  m_turn_left->setText ("<-");
  m_turn_left->setAutoRepeat (true);

  m_turn_right = new QPushButton (this);
  m_turn_right->setText ("->");
  m_turn_right->setAutoRepeat (true);
}

void main_window::set_layouts ()
{
  QVBoxLayout *vlo_1 = new QVBoxLayout;
  {
    QHBoxLayout *hlo_1 = new QHBoxLayout;
    {
      hlo_1->addWidget (m_glwidget);
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

        glo_1->addWidget (new QLabel ("RHS set time, sec:", this), 3, 0);
        glo_1->addWidget (m_rhs_time, 3, 1);
        m_rhs_time->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Fixed);

        glo_1->addWidget (new QLabel ("System solved in, sec:", this), 4, 0);
        glo_1->addWidget (m_solution_time, 4, 1);
        m_solution_time->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Fixed);

        glo_1->addWidget (new QLabel ("Matrix set time, sec:", this), 5, 0);
        glo_1->addWidget (m_matrix_time, 5, 1);
        m_matrix_time->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Fixed);

        glo_1->addWidget (new QLabel ("Grid max residual", this), 6, 0);
        glo_1->addWidget (m_max_residual, 6, 1);
        m_max_residual->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Fixed);

        glo_1->addWidget (new QLabel ("Grid avrg residual", this), 7, 0);
        glo_1->addWidget (m_avg_residual, 7, 1);
        m_avg_residual->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Fixed);

        glo_1->addWidget (new QLabel ("Grid L2 residual:", this), 8, 0);
        glo_1->addWidget (m_l2, 8, 1);
        m_l2->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Fixed);

        glo_1->addWidget (m_turn_left, 9, 0);
        m_turn_left->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Fixed);

        glo_1->addWidget (m_turn_right, 9, 1);
        m_turn_right->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Fixed);
      }
      hlo_1->addLayout (glo_1);
    }
    vlo_1->addLayout (hlo_1);
    vlo_1->setAlignment (hlo_1, Qt::AlignTop);
    vlo_1->addWidget (m_compute_pb);
    vlo_1->setAlignment (m_compute_pb, Qt::AlignCenter);
  }
  setLayout (vlo_1);
}

void main_window::do_connects ()
{
  connect (m_compute_pb, SIGNAL (clicked ()), this, SLOT (interpolate ()));
  connect (this, SIGNAL (interpolation_done ()), m_glwidget, SLOT (update ()));
  connect (m_turn_left, SIGNAL (pressed ()), m_glwidget, SLOT (camera_left ()));
  connect (m_turn_right, SIGNAL (pressed ()), m_glwidget, SLOT (camera_right ()));
}

void main_window::interpolate ()
{
  m_interpol = std::make_unique<least_squares_interpol> ();

  m_compute_pb->setDisabled (true);

  m_interpol->set_ellipse (m_a0, m_a1, m_b0, m_b1);
  int m = m_phi_partition->value ();
  int n = m_r_partition->value ();
  m_interpol->set_partition (m, n);
  int p = m_threads->value ();

  msr_matrix gramm;

  double set_system_time  = get_monotonic_time ();
  m_interpol->set_system (gramm);
  set_system_time = get_monotonic_time () - set_system_time;

  simple_vector x_ini ((m + 1) * (n + 1));
  simple_vector x_out ((m + 1) * (n + 1));
  simple_vector rhs ((m + 1) * (n + 1));

  double set_rhs_time = get_monotonic_time ();
  m_interpol->set_rhs (rhs, func, false);
  set_rhs_time = get_monotonic_time () - set_rhs_time;

  std::vector<msr_thread_dqgmres_solver> handlers;
  msr_dqgmres_initializer initer (p, gramm, preconditioner_type::jacobi, 5,
                                  300, 1e-15, x_ini, x_out, rhs);
  for (int t = 0; t < p; t++)
    handlers.push_back (msr_thread_dqgmres_solver (t, initer));

  pthread_t pt;
  double solve_time = get_monotonic_time ();

  for (int t = 1; t < p; t++)
    pthread_create (&pt, 0, computing_thread_worker, handlers.data () + t);

  computing_thread_worker (handlers.data ());

  solve_time = get_monotonic_time () - solve_time;

  simple_vector rhs_comp (gramm.n ());

  gramm.mult_vector (x_out, rhs_comp);


  double norm = 0;
   for (int i = 0; i < gramm.n (); i++)
     {
       norm += (rhs[i] - rhs_comp[i]) * (rhs[i] - rhs_comp[i]);
     }

   norm = sqrt (norm);



   m_interpol->set_expansion_coefs (x_out);

   double maxval = -1;
   double l2  = 0;
   double avg = 0;
   double func_max = 0;
   for (int i = 0; i <= m; i++)
     for (int j = 0; j <= n; j++)
       {
         double phi = i * 1. / (m);
         double r = j * 1. / (n);
         double x, y;
         m_interpol->map_to_xy (phi, r, x, y);
         if (fabs (m_interpol->func_val (phi, r)) > func_max)
           func_max = fabs (m_interpol->func_val (phi, r));
         double val = fabs (m_interpol->eval_phir (phi, r) - m_interpol->func_val (phi, r));
         l2 += val * val;
         avg += val;
         if (val > maxval)
           {
             maxval = val;
           }
       }
   l2 = sqrt (l2);
   avg /= ((m + 1) * (n + 1));

   m_matrix_time->setText (QString::number (set_system_time));
   m_solution_time->setText (QString::number (solve_time));
   m_rhs_time->setText (QString::number (set_rhs_time));
   m_max_residual->setText (QString::number (maxval));
   m_avg_residual->setText (QString::number (avg));
   m_l2->setText (QString::number (l2));
   m_compute_pb->setEnabled (true);
   m_glwidget->set_interpolator (m_interpol.get ());
   emit interpolation_done ();
}

void *main_window::computing_thread_worker(void *args)
{
  msr_thread_dqgmres_solver handler (*((msr_thread_dqgmres_solver *)args));

  solver_state state =  handler.dqgmres_solve ();

  if (handler.t_id () == 0)
    {
      switch (state)
        {
        case solver_state::OK:
          printf ("Converged!\n");
          break;
        case solver_state::TOO_SLOW:
          printf ("Convergence is too slow");
          break;
        case solver_state::MAX_ITER:
          printf ("Failed to converge\n");
          break;
        }
    }
    return args;
}
