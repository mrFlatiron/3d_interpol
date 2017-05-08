#include <sys/time.h>
#include <sys/resource.h>
#include <ctime>

void ttime_used(struct timeval *t);
double ttime_sec();

void ptime_used(struct timeval *t);
double ptime_sec();

void all_ttime_used(struct timeval *t);
double get_monotonic_time ();

