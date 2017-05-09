#include "main_window.h"
#include "gui/gl_plot_widget.h"

#include <QSpinBox>
#include <QLineEdit>
#include <QLabel>
#include <QString>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

main_window::main_window () : QDialog ()
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

  m_r_partition = new QSpinBox (this);
  m_r_partition->setMinimum (1);
  m_r_partition->setValue (1);

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

  m_l2 = new QLineEdit (this);
  m_l2->setReadOnly (true);

  m_compute_pb = new QPushButton (this);
  m_compute_pb->setText ("Play");
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
        glo_1->addWidget (new QLabel ("OX nodes:", this), 0, 0);
        glo_1->addWidget (m_phi_partition, 0, 1);
        m_phi_partition->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Fixed);
        glo_1->addWidget (new QLabel ("OY nodes:", this), 1, 0);
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
        glo_1->addWidget (new QLabel ("Grid L2 residual:", this), 7, 0);
        glo_1->addWidget (m_l2, 7, 1);
        m_l2->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Fixed);
      }
      hlo_1->addLayout (glo_1);

    }
    vlo_1->addLayout (hlo_1);
    vlo_1->setAlignment (hlo_1, Qt::AlignTop);
    vlo_1->addWidget (m_compute_pb);
    vlo_1->setAlignment (m_compute_pb, Qt::AlignCenter);
  }
  setLayout (vlo_1);
  m_glwidget->update ();
}

void main_window::do_connects ()
{

}
