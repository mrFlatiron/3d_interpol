#include "test_functions.h"
#include <cmath>

double func (const double x, const double y)
{
  (void)x;
  (void)y;
 return sin (sqrt (x * x + y * y)) / sqrt (x * x + y * y);
}
