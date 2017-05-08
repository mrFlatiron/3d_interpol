#include "ttime.h"

void ttime_used(struct timeval* t)
{
  struct rusage ru;
  getrusage(RUSAGE_THREAD, &ru);
  *t = ru.ru_utime;
}

double ttime_sec ()
{
  struct timeval t;
  ttime_used(&t);
  return t.tv_sec + double(t.tv_usec)/1e6;
}

void ptime_used (struct timeval* t)
{
  struct rusage ru;
  getrusage(RUSAGE_SELF, &ru);
  *t = ru.ru_utime;
}

double ptime_sec ()
{
  struct timeval t;
  ptime_used(&t);
  return t.tv_sec + double(t.tv_usec)/1e6;
}

void all_ttime_used (struct timeval *t)
{
  struct rusage ru;
  getrusage(RUSAGE_THREAD, &ru);
  *t = ru.ru_utime;
  t->tv_sec += ru.ru_stime.tv_sec;
  t->tv_usec += ru.ru_stime.tv_usec;
}

double get_monotonic_time ()
{
  struct timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);
  return t.tv_sec + double(t.tv_nsec)/1e9;
}


