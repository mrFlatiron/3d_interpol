#include <cstdio>
#include <cstdlib>
#include "kernel/linear_system_configurer.h"
#include "sparse_matrix/msr_dqgmres_initializer.h"
#include "sparse_matrix/msr_thread_dqgmres_solver.h"
#include "sparse_matrix/msr_matrix.h"
#include "containers/simple_vector.h"
#include "kernel/workers/solver.h"
#include <vector>

using namespace std;

int main (int argc, char *argv[])
{
  if (argc < 9)
    {
      printf ("Usage: %s a1 a0 b1 b0 m n dim p", argv[0]);
      return 0;
    }
  double a1 = atof (argv[1]);
  double a0 = atof (argv[2]);
  double b1 = atof (argv[3]);
  double b0 = atof (argv[4]);
  int m = atoi (argv[5]);
  int n = atoi (argv[6]);
  int dim = atoi (argv[7]);
  int p = atoi (argv[8]);

  linear_system_configurer configurer (a0, a1, b0, b1);
  configurer.set_partition (m, n);

  msr_matrix system;

  configurer.set_system (system);

  if (system.is_symmetrical ())
    {
      printf ("Symmetrical\n");
    }
  else
    {
      printf ("NON SYMMETRICAL\n");
      return 0;
    }

  if (system.n () < 20)
    system.dump ();

  simple_vector x_ini (system.n ());
  for (int i = 0; i < system.n (); i++)
    x_ini[i] = 0;

  simple_vector x_solution (system.n ());
  for (int i = 0; i < system.n (); i++)
    x_solution[i] = i & 1;

  simple_vector rhs (system.n ());

  system.mult_vector (x_solution, rhs);

  simple_vector x_out (system.n ());

  msr_dqgmres_initializer initer (p, system, preconditioner_type::identity,
                                  dim, 1000, 1e-10, x_ini, x_out, rhs);

//  msr_thread_dqgmres_solver solver (0, initer);

  std::vector<msr_thread_dqgmres_solver> solvers;

  for (int t = 0; t < p; t++)
    solvers.push_back (msr_thread_dqgmres_solver (t, initer));

  pthread_t pt;

  for (int t = 1; t < p; t++)
    pthread_create (&pt, 0, solve, solvers.data () + t);

  solve (solvers.data ());

  return 0;
}
