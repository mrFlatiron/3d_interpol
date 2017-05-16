#include "thread_ret.h"

thread_ret::thread_ret ()
{

}

void thread_ret::init ()
{
  set_rhs_done = false;
  set_system_done = false;
  system_solved = false;
  set_rhs_time = 0;
  set_system_time = 0;

}
