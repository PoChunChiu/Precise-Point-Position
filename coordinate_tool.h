# ifndef COORDINATE_H
# define COORDINATE_H

#include <math.h>
#include "matrix_tool.h"

#define A_EARTH     (6378137.0)         /* WGS-84 Earth semimajor axis in meter */
#define NAV_E2      (6.69437999014e-3)  /* First eccentricity squared           */

void wgslla2xyz(double *wllh, double *xyz);
void wgsxyz2lla(const double* xyz, double* wllh);
void wgsxyz2enu(const double* xyz, const double* orgxyz, double* enu); // (X Y Z), (X Y Z), (E N U)

#endif
