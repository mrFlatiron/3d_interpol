#ifndef THREAD_RET_H
#define THREAD_RET_H


class thread_ret
{
public:
  double set_system_time;
  double set_rhs_time;
  double system_solve_time;
  bool set_system_done;
  bool set_rhs_done;
  bool system_solved;
public:
  thread_ret ();
  void init ();
};

#endif // THREAD_RET_H
