#ifndef OBS_H
#define OBS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <cmath>
#include <math.h>
#define MAXGPS      32
typedef struct
{
    float RINEX_ver;
    int Year;
    int Month;
    int Day;
}RINEX_Header;


#pragma pack(push,1)
typedef struct {                        /* signal index type */
    int n;                              /* number of index */
    int frq[16];                /* signal frequency (1:L1,2:L2,...) */
    unsigned char type[16];     /* type (0:C,1:L,2:D,3:S) */
} sigind_t;
#pragma pack(pop)
class obs_t
{
public:
    obs_t();
    int SVn,SVn_max,Pre_SVn = 0;
    int freq_n;
    double pre_time;
    int  prn[MAXGPS];
    double  Sv_Pos[MAXGPS][4];
    double  Pre_Sv_Pos[MAXGPS][4];
    double  time_in_sec[MAXGPS];
    double P[MAXGPS][3];
    double L[MAXGPS][3];
    double D[MAXGPS][3];
    double SNR[MAXGPS][3];
    double LLI[MAXGPS];
    double code[MAXGPS];
    int health[MAXGPS];

    RINEX_Header obsHeader;
    sigind_t signaltype;

};

#endif // OBS_H
