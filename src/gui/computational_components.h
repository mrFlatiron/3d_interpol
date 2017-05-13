#ifndef COMPUTATIONAL_COMPONENTS_H
#define COMPUTATIONAL_COMPONENTS_H

#include "sparse_matrix/msr_matrix.h"
#include "containers/simple_vector.h"
#include "sparse_matrix/msr_thread_dqgmres_solver.h"
#include "sparse_matrix/msr_dqgmres_initializer.h"
#include <vector>
class computational_components
{
public:
  msr_matrix gramm;
  simple_vector rhs;
  simple_vector solution;
  std::vector<msr_thread_dqgmres_solver> handlers;
  msr_dqgmres_initializer *initializer;
  pthread_cond_t cv;
  pthread_mutex_t cmutex;
public:
  computational_components ();
};

#endif // COMPUTATIONAL_COMPONENTS_H
