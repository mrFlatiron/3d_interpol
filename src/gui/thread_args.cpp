#include "thread_args.h"
#include "sparse_matrix/msr_dqgmres_initializer.h"
#include "computational_components.h"

thread_common_args::thread_common_args (const int p, computational_components *components,
                                        least_squares_interpol *interpol_ptr)
{
  cond_mutex = new pthread_mutex_t;
  pthread_mutex_init (cond_mutex, 0);
  cv = new pthread_cond_t;
  pthread_cond_init (cv, 0);
  interpol = interpol_ptr;

  initializer = new msr_dqgmres_initializer (1, p, )
}
