#ifndef THREAD_ARGS_H
#define THREAD_ARGS_H

#include "threads/thread_handler.h"

class msr_dqgmres_initializer;
class least_squares_interpol;
class thread_ret;
class msr_thread_dqgmres_solver;
class computational_components;
class thread_handler;


class thread_args
{
public:
  thread_handler handler;
  computational_components *components;
  thread_ret *ret;
  least_squares_interpol *interpol;
};

#endif // THREAD_ARGS_H
