#ifndef NAV_H
#define NAV_H
#include "obs.h"
#include <iostream>
#define MAXGPS      32
using std::cout;using std::endl;

class nav_t
{
public:
    nav_t();
    int SVn,SVn_max = MAXGPS;
    int freq_n;

    //gtime_t time;
    double weekoftime[MAXGPS];
    int prn[MAXGPS];            /* satellite number */
    int iode[MAXGPS],iodc[MAXGPS];      /* IODE,IODC */
    int sva[MAXGPS];            /* SV accuracy (URA index) */
    int svh[MAXGPS];            /* SV health (0:ok) */
    int week[MAXGPS];           /* GPS/QZS: gps week, GAL: galileo week */
    int code[MAXGPS];           /* GPS/QZS: code on L2, GAL/CMP: data sources */
    int flag[MAXGPS];           /* GPS/QZS: L2 P data flag, CMP: nav type */

    double toc[MAXGPS];
                        /* SV orbit parameters */
    double roota[MAXGPS],e[MAXGPS],i0[MAXGPS],OMG0[MAXGPS],omg[MAXGPS],M0[MAXGPS],dn[MAXGPS],OMGd[MAXGPS],idot[MAXGPS];
    double crc[MAXGPS],crs[MAXGPS],cuc[MAXGPS],cus[MAXGPS],cic[MAXGPS],cis[MAXGPS];
    double toe[MAXGPS],dts[MAXGPS],dts_L2[MAXGPS];         /* Toe (s) in week */
    double af0[MAXGPS],af1[MAXGPS],af2[MAXGPS];    /* SV clock parameters (af0,af1,af2) */
    double tgd[MAXGPS];         /* TGD */
    RINEX_Header header;
    double ion_gps[8];  /* GPS iono model parameters {a0,a1,a2,a3,b0,b1,b2,b3} */
    double utc_gps[4];  /* GPS delta-UTC parameters {A0,A1,T,W} */
    int leaps;          /* leap seconds (s) */
    //eph_t eph[MAXSAT];
    //eph_t temp_eph;
    int Health[MAXGPS]; // bool 1:good to use 0: cannot use(no emph / out of life)

};

#endif // NAV_H
