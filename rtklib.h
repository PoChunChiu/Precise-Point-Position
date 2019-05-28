#ifndef RTK_H
#define RTK_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <cmath>
#include <math.h>
#include <iomanip>
#include "obs.h"
#include "nav.h"
#include "matrix_tool.h"
#include "coordinate_tool.h"
using namespace std;
#ifndef MAXSAT
#define MAXSAT     32
#endif
#ifndef NFREQ
#define NFREQ   2
#endif
#define SOLQ_NONE   0                   /* solution status: no solution */
#define SOLQ_FIX    1                   /* solution status: fix */
#define SOLQ_FLOAT  2                   /* solution status: float */
#define SOLQ_SBAS   3                   /* solution status: SBAS */
#define SOLQ_DGPS   4                   /* solution status: DGPS/DGNSS */
#define SOLQ_SINGLE 5                   /* solution status: single */
#define SOLQ_PPP    6                   /* solution status: PPP */
#define SOLQ_DR     7                   /* solution status: dead reconing */
#define MAXSOLQ     7                   /* max number of solution status */
#define PI          (3.1415926535897932)  /* pi */
#define D2R         (PI/180.0)          /* deg to rad */
#define R2D         (180.0/PI)          /* rad to deg */
#define CLIGHT      299792458.0         /* speed of light (m/s) */
#define FREQL1      (1575420000.0)
#define FREQL2      (1227600000.0)
#define LAMBDA1     (CLIGHT/FREQL1)
#define LAMBDA2     (CLIGHT/FREQL2)
#define MAXGPS      32
#define SQR(x)      ((x)*(x))

#define EKF_STATE_NUM     4
#define EKF_STATE_MAX     36
#define KALMAN_ITER       2
#define CODE_BIAS       0.3


typedef __time32_t time_t;
//* Structure *//
typedef struct          /* time struct */
{
    time_t time;        /* time (s) expressed by standard time_t */
    double sec;         /* fraction of second under 1 s */
} gtime_t;


class rtk_t
{
public:
    rtk_t();
    int state = SOLQ_NONE;
    obs_t *obs;
    nav_t *nav;
    int InitFlag = 1;               // for data reading
    int InitFlag_EKF = 1;           // for EKF
    double Pos[4];
    double EKF_x[EKF_STATE_MAX];
    double EKF_P[EKF_STATE_MAX*EKF_STATE_MAX];
    double EKF_R[MAXGPS*2];         // carrier-phase : 0~31 ; psdo-range : 32~63
    int read_data(ifstream& in_f1,ifstream& in_f2,ifstream& in_f3);
    void calc_satpos();
    void remove_Sat();

};

#endif // RTK_H
void initial_EKF_xP(rtk_t* rtk);
void pppos(rtk_t*);
void iono_free_LC(obs_t *obs,nav_t *nav,int SVn);
void EKFfilter(double * H, double * PH, double * Q, rtk_t*rtk, double * K);
void Measurement_Update(double * H,double * v,double * K,rtk_t * rtk);
void Least_SQR(double * H, double * v,  rtk_t*rtk);
void Var_Update(rtk_t * rtk);
void xyz2enu(double *pos, double *E);
double SAAS_tropos_model(double el);
/* rinex */
int read_rinex_obs(ifstream& Obs_ptr, obs_t *rtk, int Initflag);
int read_rinex_nav(ifstream& Nav_ptr, nav_t *nav, int Initflag);
int decode_obsepoch(string line, int *flag, gtime_t *time, double*t);
void decode_eph(int *prn, double toc, const double *data, nav_t *eph);
double str2num(string s, int length);
double get_toc(string s);
int dayofweek(int y, int m, int d);
gtime_t epoch2time(const double *ep);
int str2time(string *s, int i, int n, gtime_t *t);
int decode_obsdata(string *line, obs_t* obs ,int index ,sigind_t sign);
void read_precise_eph(ifstream& Peph_ptr, obs_t *obs);
/*matrix*/
void matrix_matrix_product(double* A,double* B,int A_row,int Ac_Br,int B_col,double* C);
void matrix_vector_product(double* M,double* V,int M_row,int Mc_Vr,double* C);
void matrix_transpose(double* M,double* Mt,int r,int c);
int matinv(double *A, int n);
int ludcmp(double *A, int n, int *indx, double *d);
void lubksb(const double *A, int n, const int *indx, double *b);
extern double *mat(int n, int m);
extern int *imat(int n, int m);
void matcpy(double *A, const double *B, int n, int m);
