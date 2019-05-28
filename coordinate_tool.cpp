#include "coordinate_tool.h"
#include <iostream>
using std::cout;using std::endl;
/***************************************************************************
 * Funtion Name : wgslla2xyz
 * Creation date: 03/23/10
 * last modified: 03/23/10
 * creator      : Shuo-Ju Yeh
 * inputs       : Latitude in rad., Longitude in rad., Altitude above ellipsoid in meters
 * outputs      : WGS-84 ECEF coordinate of local origin in meters
 ***************************************************************************/

void wgslla2xyz(double *wllh, double *xyz)
{
    double slat = 0.0, clat = 0.0, r_n = 0.0;

    if ( wllh[0] > -90.0 && wllh[0] < 90.0 && wllh[1] > -180.0 && wllh[1] < 360.0)
    {
        slat = sin( wllh[0] );
        clat = cos( wllh[0] );
        r_n = A_EARTH / sqrt( 1 - NAV_E2 * slat * slat );
        xyz[0] = (r_n + wllh[2]) * clat * cos( wllh[1] );
        xyz[1] = (r_n + wllh[2]) * clat * sin( wllh[1] );
        xyz[2] = (r_n * (1 - NAV_E2) + wllh[2] ) * slat;
    }
}

/***************************************************************************
 * Funtion Name : wgsxyz2lla
 * Creation date: 03/23/10
 * last modified: 03/23/10
 * creator      : Shuo-Ju Yeh
 * inputs       : WGS-84 ECEF coordinate of local origin in meters
 * outputs      : Latitude in rad., Longitude in rad., Altitude above ellipsoid in meters
 ***************************************************************************/

void wgsxyz2lla(const double* xyz, double* wllh)
{
  double r_N = 0.0;
  double num = 0.0, den = 0.0, p = 0.0, p2 = 0.0, old_H = 0.0;

  p2 = xyz[0] * xyz[0] + xyz[1] * xyz[1];
  p = sqrt(p2);

  wllh[1] = atan2( xyz[1], xyz[0] );

  /* interation on Lat and Height */

  wllh[0] = atan2( xyz[2] / p, 0.01 );

  r_N = A_EARTH/ sqrt( 1- NAV_E2 * sin(wllh[0]) * sin(wllh[0]) );

  wllh[2] = p / cos(wllh[0]) - r_N;
  /* iteration */
  old_H = -1e-9;
  num = xyz[2] / p;

  while ( fabs(wllh[2]- old_H) > 1e-4 )
  {

      old_H = wllh[2];
      den =  1- NAV_E2 * r_N / (r_N + wllh[2]);
      wllh[0] = atan2(num,den);
      r_N = A_EARTH / sqrt(1- NAV_E2 * sin(wllh[0]) * sin(wllh[0]));
      wllh[2] = p / cos(wllh[0])- r_N;
  };
}

/***************************************************************************
 * Funtion Name : wgsxyz2enu
 * Creation date: 03/23/10
 * last modified: 03/23/10
 * creator      : Shuo-Ju Yeh
 * purpose		: Convert from WGS-84 ECEF cartesian coordinates to
                  rectangular local-level-tangent ('East'-'North'-Up)
                  coordinates.
 * inputs       : Latitude-Longitude-Altitude in degrees,
                  ECEF coordinate in meters
 * outputs      : rectangular local-level-tangent ('East'-'North'-Up)
                  coordinates in meters
 ***************************************************************************/

void wgsxyz2enu(const double *xyz, const double *orgxyz, double *enu)
{
    double diffxyz[3];
    double orgllh[3] = {0.0};   //lat, lon, alt;

    diffxyz[0] = xyz[0] - orgxyz[0];
    diffxyz[1] = xyz[1] - orgxyz[1];
    diffxyz[2] = xyz[2] - orgxyz[2];
    wgsxyz2lla(orgxyz, orgllh);
    double R[9] = {-sin(orgllh[1]), cos(orgllh[1]), 0,
                  -sin(orgllh[0])*cos(orgllh[1]), -sin(orgllh[0])*sin(orgllh[1]), cos(orgllh[0]),
                  cos(orgllh[0])*cos(orgllh[1]), cos(orgllh[0])*sin(orgllh[1]), sin(orgllh[0])};

    MatProd(R, diffxyz, 3, 3, 1, enu);
}

