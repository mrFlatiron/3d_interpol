#ifndef COMPUTATIONAL_COMPONENTS_H
#define COMPUTATIONAL_COMPONENTS_H

#include "containers/simple_vector.h"
#include "sparse_matrix/msr_matrix.h"
class msr_dqgmres_initializer;
#include <pthread.h>
#include <vector>
class computational_components
{
public:
  msr_matrix gramm;
  simple_vector rhs;
  simple_vector solution;
  msr_dqgmres_initializer *initializer;
  pthread_barrier_t barrier;
public:
  computational_components ();
};

#endif // COMPUTATIONAL_COMPONENTS_H
