#include <cstdio>
#include <cstdlib>
#include "kernel/least_squares_interpol.h"
#include "sparse_matrix/msr_dqgmres_initializer.h"
#include "sparse_matrix/msr_thread_dqgmres_solver.h"
#include "sparse_matrix/msr_matrix.h"
#include "containers/simple_vector.h"
#include "kernel/workers/solver.h"
#include "ttime/ttime.h"
#include <vector>
#include <cmath>
#include <ctime>

using namespace std;

double f (const double x, const double y)
{
  (void)x;
  (void)y;
  return 5;
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

  least_squares_interpol configurer (a0, a1, b0, b1, m, n);

  msr_matrix system;
  printf ("Setting Gramm matrix...\n");

  double set_system_time  = get_monotonic_time ();
  configurer.set_system (system);
  set_system_time = get_monotonic_time () - set_system_time;

  printf ("Done\n");
  printf ("Symmetry check...");
  if (system.is_symmetrical ())
    {
      printf ("OK\n");
    }
  else
    {
      printf ("FAILED\n");
      return 0;
    }

  if (system.n () < 20)
    system.dump ();

  simple_vector x_ini (system.n ());
  for (int i = 0; i < system.n (); i++)
    x_ini[i] = 0;

  simple_vector rhs (system.n ());

  printf ("Setting rhs...\n");

  double set_rhs_time = get_monotonic_time ();
  configurer.set_rhs (rhs, f);
  set_rhs_time = get_monotonic_time () - set_rhs_time;

  printf ("Done\n");

  simple_vector x_out (system.n ());

  msr_dqgmres_initializer initer (p, system, preconditioner_type::jacobi,
                                  dim, 1000, 1e-15, x_ini, x_out, rhs);


  std::vector<msr_thread_dqgmres_solver> solvers;

  for (int t = 0; t < p; t++)
    solvers.push_back (msr_thread_dqgmres_solver (t, initer));

  pthread_t pt;
  printf ("Solving linear system...\n");
  double solve_time = get_monotonic_time ();
  for (int t = 1; t < p; t++)
    pthread_create (&pt, 0, solve, solvers.data () + t);

  solve (solvers.data ());

  solve_time = get_monotonic_time () - solve_time;

  simple_vector rhs_comp (system.n ());

  system.mult_vector (x_out, rhs_comp);

  double norm = 0;
  for (int i = 0; i < system.n (); i++)
    {
      norm += (rhs[i] - rhs_comp[i]) * (rhs[i] - rhs_comp[i]);
    }

  norm = sqrt (norm);



  configurer.set_expansion_coefs (&x_out);

  int maxpair[] = {0, 0};
  double maxval = -1;
  double l2  = 0;
  double avg = 0;
  double func_max = 0;
  for (int i = 0; i <= 2000; i++)
    for (int j = 0; j <= 2000; j++)
      {
        double phi = i * 1. / (2000);
        double r = j * 1. / (2000);
        double x, y;
        configurer.map_to_xy (phi, r, x, y);
        if (fabs (f (x, y)) > func_max)
          func_max = fabs (f (x,y ));
        double val = fabs (configurer.eval_phir (phi, r) - f (x, y));
        l2 += val * val;
        avg += val;
        if (val > maxval)
          {
            maxval = val;
            maxpair[0]= i;
            maxpair[1] = j;
          }
      }
  l2 = sqrt (l2);
  avg /= (4e6);
  printf ("Solution L2 residual             : %.6le\n", norm);
  printf ("Grid Max residual                : %.6le in node : (%d, %d)\n"
          "Grid L2 residual                 : %.6le\n"
          "Grid average residual            : %.6le\n",
          maxval, maxpair[0], maxpair[1], l2, avg);
  printf ("Grid |f(x,y)| maximum            : %.6le\n", func_max);
  double all_time = set_system_time + set_rhs_time + solve_time;
  printf ("Gramm matrix computation time    : %.6lf sec (%d %%)\n", set_system_time, (int)floor (set_system_time / all_time * 100));
  printf ("Right hand side computation time : %.6lf sec (%d %%)\n", set_rhs_time, (int)floor (set_rhs_time / all_time * 100));
  printf ("System solving time              : %.6lf sec (%d %%)\n", solve_time, (int)floor (solve_time / all_time * 100));

  return 0;
}
