#ifndef THREAD_ARGS_H
#define THREAD_ARGS_H
#include <pthread.h>
#include <vector>

class msr_dqgmres_initializer;
class least_squares_interpol;
class thread_ret;
class msr_thread_dqgmres_solver;


class thread_common_args
{
public:
  pthread_mutex_t *cond_mutex;
  pthread_cond_t *cv;
  msr_dqgmres_initializer *initializer;
  least_squares_interpol *interpol;
  std::vector<msr_thread_dqgmres_solver> *handlers;
  thread_ret *ret_struct;
public:
  thread_common_args (const int p, thread_ret *retval_ptr, least_squares_interpol *interpol_ptr);
};

#endif // THREAD_ARGS_H
