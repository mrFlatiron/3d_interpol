#include "computational_components.h"

computational_components::computational_components ()
{
  pthread_cond_init (cv, 0);
  pthread_mutex_init (cmutex, 0);
}
