#include <cstdio>
#include <cstdlib>
#include "kernel/least_squares_interpol.h"
#include "sparse_matrix/msr_dqgmres_initializer.h"
#include "sparse_matrix/msr_thread_dqgmres_solver.h"
#include "sparse_matrix/msr_matrix.h"
#include "containers/simple_vector.h"
#include "kernel/workers/solver.h"
#include <vector>
#include <cmath>

using namespace std;

double f (const double x, const double y)
{
  (void)x;
  (void)y;
  return 10;
}

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

  least_squares_interpol configurer (a0, a1, b0, b1);
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

  simple_vector rhs (system.n ());

  configurer.set_rhs (rhs, f);

  simple_vector x_out (system.n ());

  msr_dqgmres_initializer initer (p, system, preconditioner_type::identity,
                                  dim, 1000, 1e-15, x_ini, x_out, rhs);

//  msr_thread_dqgmres_solver solver (0, initer);

  std::vector<msr_thread_dqgmres_solver> solvers;

  for (int t = 0; t < p; t++)
    solvers.push_back (msr_thread_dqgmres_solver (t, initer));

  pthread_t pt;

  for (int t = 1; t < p; t++)
    pthread_create (&pt, 0, solve, solvers.data () + t);

  solve (solvers.data ());

  configurer.set_coefs (&x_out);

  int maxpair[2];
  double maxval = -1;
  for (int i = 0; i <= 2000; i++)
    for (int j = 0; j <= 2000; j++)
      {
        double phi = i * 1 / (2000);
        double r = j * 1 / (2000);
        double x, y;
        configurer.map_to_xy (phi, r, x, y);
        double val = fabs (configurer (x, y) - f (x, y));
        if (val > maxval)
          {
            maxval = val;
            maxpair[0]= i;
            maxpair[1] = j;
          }
      }

  printf ("Max residual = %.6le in pair : (%d, %d)\n", maxval, maxpair[0], maxpair[1]);

  return 0;
}
