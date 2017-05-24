#include "test_functions.h"
#include <cmath>

double func (const double x, const double y)
{
  (void)x;
  (void)y;
  double p1 = 100 - 3 / sqrt (x *x + y * y);
  double p2 = sin (sqrt (x * x + y * y));
  double p3 = sqrt (246 - x * x + y  * y + 10 * sin (x) + 10 * sin (y)) / 1000;
  double retval = p1 + p2 + p3;
 return retval;
}
