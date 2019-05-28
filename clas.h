#ifndef CLAS_H
#define CLAS_H
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cstdlib>
#include <string.h>
#define UINT32_1 ((volatile uint32_t)1)
#define GPS_SVMAX     40
#define GPS_SIGMAX    16
#define GPS_CELLMAX   (GPS_SVMAX*GPS_SIGMAX)
#define GRID_MAX      32
#define NETWORK_MAX   19
#define GNSS_MAX      10
//#define PRINT
using namespace std;

#pragma pack(push,1)
typedef struct {
    unsigned int Pre;      // Preamble
    unsigned int prn = 0;       // Sat. PRN
    unsigned int msg_id = 0;    // message ID
    unsigned int alrt = 0;

} HDR;                     // Total : 104
#pragma pack(pop)

#pragma pack(push,1)
typedef struct {
    int involve;
    int SVn = 0;
    int Sign = 0;
    int Celln = 0;

} GNSS_INFO;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct {
    //time
    unsigned int GPS_epoch_time[GPS_CELLMAX];
    //subtype 6
    unsigned char T6_LocalMask [GPS_CELLMAX];
    unsigned int T6_CodeBias [GPS_CELLMAX];
    unsigned int T6_PhaseBias [GPS_CELLMAX];
    unsigned int T6_PhaseIdt  [GPS_CELLMAX];
    //subtype 8
    unsigned char T8_LocalMask [GPS_SVMAX];
    unsigned int T8_STECIdt [GPS_SVMAX];
    unsigned int T8_C00 [GPS_SVMAX];
    unsigned int T8_C01 [GPS_SVMAX];
    unsigned int T8_C10 [GPS_SVMAX];
    unsigned int T8_C11 [GPS_SVMAX];
    //subtype 9
    unsigned char T9_LocalMask [GPS_SVMAX];
    unsigned int T9_STECCorr [GRID_MAX * GPS_SVMAX];
    unsigned int T9_Dry [GRID_MAX];
    unsigned int T9_Wet [GRID_MAX];
} NETWORK_DATA;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct {
    //time
    unsigned int GPS_time[GPS_CELLMAX];
    unsigned int GPS_epoch_time[GPS_CELLMAX];
    //subtype 1
    unsigned char T1_CellMask [GPS_CELLMAX];
    unsigned char T1_SVMask [GPS_SVMAX];
    unsigned char T1_SigMask [GPS_SIGMAX];
    //subtype 2
    unsigned int T2_Delta[GPS_SVMAX];
    unsigned int T2_DeltaA[GPS_SVMAX];
    unsigned int T2_DeltaC[GPS_SVMAX];
    unsigned int T2_IODE[GPS_SVMAX];
    //subtype 3
    unsigned int T3_ClockCorr[GPS_SVMAX];
    //subtype 4
    unsigned int T4_CodeCorr[GPS_CELLMAX];
    //subtype 5
    unsigned int T5_PhaseCorr[GPS_CELLMAX];
    unsigned int T5_PhaseIdt[GPS_CELLMAX];
    //subtype 7
    unsigned int T7_URA[GPS_SVMAX];
} CELL_DATA;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct {
    CELL_DATA cd;
    NETWORK_DATA nd[NETWORK_MAX];
} GNSS_DATA;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct {
    unsigned int GNSSn;
    GNSS_INFO Gi[GNSS_MAX];
    GNSS_DATA Gd[GNSS_MAX];
    HDR hdr[5];
}CLAS_DATA;
#pragma pack(pop)



unsigned int Get_Bit(unsigned int b, int l, int offset1,int offset2);
unsigned int Get_Bit_for(int length, int* idx, int* remain_bit,unsigned char* L);
int Bit2Array(int * arr,int n,unsigned int t,int m);
int decode_L6_sf1( FILE* ,CLAS_DATA*);
int decode_L6_sf2( FILE* ,CLAS_DATA*);
int decode_L6_sf3( FILE* ,CLAS_DATA*);
int decode_L6_sf4( FILE* ,CLAS_DATA*);
int decode_L6_sf5( FILE* ,CLAS_DATA*);
int decode_L6_sf6( FILE* ,CLAS_DATA*);
int decode_L6_hdr(int line,unsigned char* L,int *idx,int* remain_bit,CLAS_DATA* Cd);
int decode_L6_st1(int ID,unsigned char* L,int *idx,int* remain_bit,CLAS_DATA* Cd);
int decode_L6_st2(int ID,unsigned char* L,int *idx,int* remain_bit,CLAS_DATA* Cd);
int decode_L6_st3(int ID,unsigned char* L,int *idx,int* remain_bit,CLAS_DATA* Cd);
int decode_L6_st4(int ID,unsigned char* L,int *idx,int* remain_bit,CLAS_DATA* Cd);
int decode_L6_st5(int ID,unsigned char* L,int *idx,int* remain_bit,CLAS_DATA* Cd);
int decode_L6_st6(int ID,unsigned char* L,int *idx,int* remain_bit,CLAS_DATA* Cd);
int decode_L6_st7(int ID,unsigned char* L,int *idx,int* remain_bit,CLAS_DATA* Cd);
int decode_L6_st8(int ID,unsigned char* L,int *idx,int* remain_bit,CLAS_DATA* Cd);
int decode_L6_st9(int ID,unsigned char* L,int *idx,int* remain_bit,CLAS_DATA* Cd);
int decode_L6_st10();
int decode_L6_st11(int ID,unsigned char* L,int *idx,int *remain_bit,CLAS_DATA* Cd);
int decode_L6_init(CLAS_DATA* Cd);
void decode_L6(FILE*,CLAS_DATA* Cd);
double Bin2Double(bool PN,unsigned int bin,double LSB,int len);
static int (*decode_L6_all[11]) (int ID,unsigned char* L,int *idx,int *remain_bit,CLAS_DATA* Cd_);
static CLAS_DATA Cd;
#endif // CLAS_H
