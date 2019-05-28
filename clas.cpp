#include "clas.h"


void decode_L6(FILE* in_f,CLAS_DATA* Cd){
    decode_L6_init(Cd);
    for (int cnt = 0;cnt<120;cnt++) {
        if(decode_L6_sf1(in_f,Cd)){printf("L6 decode1 Error\n\n");system("pause");}
        if(decode_L6_sf2(in_f,Cd)){printf("L6 decode2 Error\n\n");system("pause");}
        if(decode_L6_sf3(in_f,Cd)){printf("L6 decode3 Error\n\n");system("pause");}
        if(decode_L6_sf4(in_f,Cd)){printf("L6 decode4 Error\n\n");system("pause");}
        if(decode_L6_sf5(in_f,Cd)){printf("L6 decode5 Error\n\n");system("pause");}
        if(decode_L6_sf6(in_f,Cd)){printf("L6 decode6 Error\n\n");system("pause");}

    }
    printf("End of CLAS Data\n");
}
double Bin2Double(bool PN,unsigned int bin,double LSB,int len){
    double result = 0.0;
    if(PN&&len>1)
    {
        if(bin>>(len-1)){result=-1.0;}
        else{result=1.0;}
        result *= (int)(bin&(~(UINT32_1<<(len-1))));
        result*=LSB;
    }else {
        for (int i = 0;i<len;i++) {
            result += (int)(bin&(UINT32_1<<i));
        }
        result*=LSB;
    }
    return result;
}
unsigned int Get_Bit(unsigned int b, int l, int start,int end){
    volatile unsigned int get_bit = UINT32_1;
    unsigned int result;
    for(int i = 0;i<(l-start);i++){
        get_bit = get_bit <<1;
        if(i<(end-start))
            get_bit ++;
    }
    result = ((b&get_bit)>>(l-end));
    //printf("result = %d\n",result);
    return result;
}
unsigned int Get_Bit_for(int length, int* idx, int* remain_bit,unsigned char* L){
    unsigned int tmp;
    int cnt = 0;
    for(cnt = 0;cnt < length; cnt++){
        if(cnt == 0){
            tmp = Get_Bit(L[*idx],8,*remain_bit,*remain_bit)<<(length-cnt-1);
        }else{
            tmp += Get_Bit(L[*idx],8,*remain_bit,*remain_bit)<<(length-cnt-1);
        }
        //printf("%d %d %d  %d\n",cnt,tmp,*remain_bit,Get_Bit(L[*idx],8,*remain_bit,*remain_bit));
        if(*idx % 250 == 217 &&*remain_bit == 8){
            *idx += 39;
            *remain_bit = 2;
            continue;
        }
        else if(*remain_bit == 8){
            *idx = *idx + 1;
            *remain_bit = 1;
        }else{
            *remain_bit = *remain_bit + 1;
        }


    }
    return  tmp;
}
int Bit2Array(int * arr,int n,unsigned int t,int m){
    //printf("%d  ",n);
    //printf("%d  %d\n",( (t&(UINT32_1<<(8-m))) >> (8-m) ),t);
    int a = ( (t&(UINT32_1<<(8-m))) >> (8-m) );
    arr[n] = a;

    return 0;
}
int decode_L6_sf1( FILE*in_f ,CLAS_DATA* Cd){
    unsigned char Line[1250];
    unsigned char*L = Line;
    fread(Line,sizeof (unsigned char),1250,in_f);
    int idx = 6;
    int remain_bit = 2;
    int line = 0;
    if(decode_L6_hdr(line,Line,&idx,&remain_bit,Cd)){
        printf("Decode sf1hdr error!\n");
        return 1;
    }
    unsigned int msg = 0,st = 0;
    int sf1_ID[2]{1,2};
    int IDmax = 1;
    int ID_idx = 0;

//    int a,b;
//    for(a = 0;a<750;a++){
//        for (b = 1;b<=8;b++) {
//            int c =a,d=b;
//            unsigned int A = Get_Bit_for(12,&c,&d,Line);
//            unsigned int B = Get_Bit_for(4,&c,&d,Line);
//            if(A == 4073&&B == 2){
//                printf("Found %d %d\n",a,b);
//            }
//        }
//    }



    do{
        msg = Get_Bit_for(12,&idx,&remain_bit,Line);
        st = Get_Bit_for(4,&idx,&remain_bit,Line);
        if(msg == 4073 && (st>0&&st<12)){
            decode_L6_all[st-1](sf1_ID[ID_idx],Line,&idx,&remain_bit,Cd);
            //printf("subtype = %d\n",st);

            if(st==9){
                sf1_ID[ID_idx]=0;
                if(sf1_ID[IDmax] == 0){
                    break;
                }
                ID_idx++;
            }
        }
        else {
            printf("Msg %d Subtype %d ERROR\n",msg,st);
            printf("in 1 %d %d\n",idx,remain_bit);
            return 1;
        }

    }while(1);




    return 0;
}
int decode_L6_sf2( FILE*in_f ,CLAS_DATA* Cd){
    unsigned char Line[1250];
    unsigned char*L = Line;
    fread(Line,sizeof (unsigned char),1250,in_f);
    int idx = 6;
    int remain_bit = 2;
    int line = 0;
    if(decode_L6_hdr(line,Line,&idx,&remain_bit,Cd)){
        printf("Decode sf1hdr error!\n");
        return 1;
    }
    unsigned int msg = 0,st = 0;
    int sf2_ID[3]{3,4,13};
    int IDmax=2;
    int ID_idx = 0;
    do{
        msg = Get_Bit_for(12,&idx,&remain_bit,Line);
        st = Get_Bit_for(4,&idx,&remain_bit,Line);
        if(msg == 4073 && (st>0&&st<12)){
            decode_L6_all[st-1](sf2_ID[ID_idx],Line,&idx,&remain_bit,Cd);

            if(st==9){
                sf2_ID[ID_idx]=0;
                if(sf2_ID[IDmax] == 0){
                    break;
                }ID_idx++;
            }
        }
        else {
            printf("Msg %d Subtype %d ERROR\n",msg,st);
            printf("in 2 %d %d\n",idx,remain_bit);
            return 1;
        }
    }while(1);




    return 0;
}
int decode_L6_sf3( FILE*in_f ,CLAS_DATA* Cd){
    unsigned char Line[1250];
    unsigned char*L = Line;
    fread(Line,sizeof (unsigned char),1250,in_f);
    int idx = 6;
    int remain_bit = 2;
    int line = 0;
    if(decode_L6_hdr(line,Line,&idx,&remain_bit,Cd)){
        printf("Decode sf1hdr error!\n");
        return 1;
    }
    unsigned int msg = 0,st = 0;
    int sf3_ID[4]{5,6,15,16};
    int IDmax = 3;
    int ID_idx = 0;
    do{
        msg = Get_Bit_for(12,&idx,&remain_bit,Line);
        st = Get_Bit_for(4,&idx,&remain_bit,Line);
        if(msg == 4073 && (st>0&&st<12)){
            decode_L6_all[st-1](sf3_ID[ID_idx],Line,&idx,&remain_bit,Cd);

            if(st==9){
                sf3_ID[ID_idx]=0;
                if(sf3_ID[IDmax] == 0){
                    break;
                }ID_idx++;
            }
        }
        else {
            printf("Msg %d Subtype %d ERROR\n",msg,st);
            printf("in 3 %d %d\n",idx,remain_bit);
            return 1;
        }
    }while(1);




    return 0;
}
int decode_L6_sf4( FILE*in_f ,CLAS_DATA* Cd){
    unsigned char Line[1250];
    unsigned char*L = Line;
    fread(Line,sizeof (unsigned char),1250,in_f);
    int idx = 6;
    int remain_bit = 2;
    int line = 0;
    if(decode_L6_hdr(line,Line,&idx,&remain_bit,Cd)){
        printf("Decode sf1hdr error!\n");
        return 1;
    }
    unsigned int msg = 0,st = 0;
    int sf4_ID[4]{7,8,17,18};
    int IDmax=3;
    int ID_idx = 0;
    do{
        msg = Get_Bit_for(12,&idx,&remain_bit,Line);
        st = Get_Bit_for(4,&idx,&remain_bit,Line);
        if(msg == 4073 && (st>0&&st<12)){
            decode_L6_all[st-1](sf4_ID[ID_idx],Line,&idx,&remain_bit,Cd);

           // printf("4 subtype = %d\n",st);
            if(st==9){
                sf4_ID[ID_idx]=0;
                if(sf4_ID[IDmax] == 0){
                    break;
                }ID_idx++;
            }
        }
        else {
            printf("Msg %d Subtype %d ERROR\n",msg,st);
            printf("in 4 %d %d\n",idx,remain_bit);
            return 1;
        }
    }while(1);




    return 0;
}
int decode_L6_sf5( FILE*in_f ,CLAS_DATA* Cd){

    unsigned char Line[1250];
    unsigned char*L = Line;
    fread(Line,sizeof (unsigned char),1250,in_f);
    int idx = 6;
    int remain_bit = 2;
    int line = 0;
    if(decode_L6_hdr(line,Line,&idx,&remain_bit,Cd)){
        printf("Decode sf1hdr error!\n");
        return 1;
    }
    unsigned int msg = 0,st = 0;
    int sf5_ID[3]{9,10,19};
    int IDmax = 2;
    int ID_idx = 0;
    do{
        msg = Get_Bit_for(12,&idx,&remain_bit,Line);
        st = Get_Bit_for(4,&idx,&remain_bit,Line);
        if(msg == 4073 && (st>0&&st<12)){
            decode_L6_all[st-1](sf5_ID[ID_idx],Line,&idx,&remain_bit,Cd);

            if(st==9){
                sf5_ID[ID_idx]=0;
                if(sf5_ID[IDmax] == 0){
                    break;
                }ID_idx++;
            }
        }
        else {
            printf("Msg %d Subtype %d ERROR\n",msg,st);
            printf("in 5 %d %d\n",idx,remain_bit);
            return 1;
        }
    }while(1);
    return 0;
}
int decode_L6_sf6( FILE*in_f ,CLAS_DATA* Cd){

    unsigned char Line[1250];
    unsigned char*L = Line;
    fread(Line,sizeof (unsigned char),1250,in_f);
    int idx = 6;
    int remain_bit = 2;
    int line = 0;
    if(decode_L6_hdr(line,Line,&idx,&remain_bit,Cd)){
        printf("Decode sf1hdr error!\n");
        return 1;
    }
    unsigned int msg = 0,st = 0;
    int sf6_ID[2]{11,12};
    int IDmax = 1;
    int ID_idx = 0;
    do{
        msg = Get_Bit_for(12,&idx,&remain_bit,Line);
        st = Get_Bit_for(4,&idx,&remain_bit,Line);
        if(msg == 4073 && (st>0&&st<12)){
            decode_L6_all[st-1](sf6_ID[ID_idx],Line,&idx,&remain_bit,Cd);

            if(st==9){
                sf6_ID[ID_idx]=0;
                if(sf6_ID[IDmax] == 0){
                    break;
                }ID_idx++;
            }
        }
        else {
            printf("Msg %d Subtype %d ERROR\n",msg,st);
            printf("in 6 %d %d\n",idx,remain_bit);
            return 1;
        }
    }while(1);

    return 0;
}
int decode_L6_hdr(int line, unsigned char* L,int *idx,int* remain_bit ,CLAS_DATA* Cd){

    return 0;
}
int decode_L6_st1(int ID,unsigned char* L,int *idx,int *remain_bit,CLAS_DATA* Cd){
    unsigned int tmp = 0;
    unsigned int time = 0;
    time = Get_Bit_for(20,idx,remain_bit,L);
    tmp = Get_Bit_for(4,idx,remain_bit,L);
    tmp = Get_Bit_for(1,idx,remain_bit,L);
    tmp = Get_Bit_for(4,idx,remain_bit,L);
    for (int i = 0;i<GNSS_MAX;i++) {
        Cd->Gi[i].involve=0;
    }
    Cd->GNSSn = Get_Bit_for(4,idx,remain_bit,L);
    for (unsigned int i=0;i<Cd->GNSSn;i++) {
        unsigned int id = Get_Bit_for(4,idx,remain_bit,L);
        unsigned char sv[40];
        unsigned char sig[16];
        Cd->Gi[id].involve = 1;
        Cd->Gi[id].SVn = 0;
        Cd->Gi[id].Sign = 0;
        for(int k=0;k<40;k++){
            sv[k] = (unsigned char)Get_Bit_for(1,idx,remain_bit,L);
            if(sv[k])Cd->Gi[id].SVn++;
        }
        for(int k=0;k<16;k++){
            sig[k] = (unsigned char)Get_Bit_for(1,idx,remain_bit,L);
            if(sig[k])Cd->Gi[id].Sign++;
        }
        unsigned int idt = 0;
        idt = Get_Bit_for(1,idx,remain_bit,L);
        for (int p = 0;p<GPS_SVMAX;p++) {
            Cd->Gd[id].cd.T1_SVMask[p] = 0;
            Cd->Gd[id].cd.T1_SVMask[p] = sv[p];
            for (int q = 0;q<GPS_SIGMAX;q++) {
                Cd->Gd[id].cd.T1_SigMask[q] = sig[q];
                Cd->Gd[id].cd.T1_CellMask[p+q*GPS_SVMAX] = 0;
                if((sv[p]==1)&&(sig[q]==1)){
                    Cd->Gd[id].cd.T1_CellMask[p+q*GPS_SVMAX] = Get_Bit_for(1,idx,remain_bit,L);
                    Cd->Gd[id].cd.GPS_time[p+q*GPS_SVMAX] = time;
                }
            }
        }
    }
    return 0;
}
int decode_L6_st2(int ID,unsigned char* L,int *idx,int *remain_bit,CLAS_DATA* Cd){
    unsigned int tmp = 0;
    unsigned int epoch_time = 0;
    epoch_time = Get_Bit_for(12,idx,remain_bit,L);
    tmp = Get_Bit_for(4,idx,remain_bit,L);
    tmp = Get_Bit_for(1,idx,remain_bit,L);
    tmp = Get_Bit_for(4,idx,remain_bit,L);
    for(int i = 0;i<GNSS_MAX;i++){
        if(Cd->Gi[i].involve){
            if(i==2){
                for (int k = 0;k < GPS_SVMAX;k++) {
                    if(Cd->Gd[i].cd.T1_SVMask[k]){
                        Cd->Gd[i].cd.T2_IODE[k] =  Get_Bit_for(10,idx,remain_bit,L);
                        Cd->Gd[i].cd.T2_Delta[k] =  Get_Bit_for(15,idx,remain_bit,L);
                        Cd->Gd[i].cd.T2_DeltaA[k] =  Get_Bit_for(13,idx,remain_bit,L);
                        Cd->Gd[i].cd.T2_DeltaC[k] =  Get_Bit_for(13,idx,remain_bit,L);
                    }
                }
            }else{
                for (int k = 0;k < GPS_SVMAX;k++) {
                    if(Cd->Gd[i].cd.T1_SVMask[k]){
                        Cd->Gd[i].cd.T2_IODE[k] =  Get_Bit_for(8,idx,remain_bit,L);
                        Cd->Gd[i].cd.T2_Delta[k] =  Get_Bit_for(15,idx,remain_bit,L);
                        Cd->Gd[i].cd.T2_DeltaA[k] =  Get_Bit_for(13,idx,remain_bit,L);
                        Cd->Gd[i].cd.T2_DeltaC[k] =  Get_Bit_for(13,idx,remain_bit,L);
                    }
                }
            }
        }else {
            continue;
        }
    }
    return 0;
}
int decode_L6_st3(int ID,unsigned char* L,int *idx,int *remain_bit,CLAS_DATA* Cd){
    unsigned int tmp = 0;
    unsigned int epoch_time = 0;
    epoch_time = Get_Bit_for(12,idx,remain_bit,L);
    tmp = Get_Bit_for(4,idx,remain_bit,L);
    tmp = Get_Bit_for(1,idx,remain_bit,L);
    tmp = Get_Bit_for(4,idx,remain_bit,L);
    for(int i = 0;i<GNSS_MAX;i++){
        if(Cd->Gi[i].involve){
            for (int k = 0;k<GPS_SVMAX;k++) {
                if(Cd->Gd[i].cd.T1_SVMask[k]){
                    Cd->Gd[i].cd.T3_ClockCorr[k] = Get_Bit_for(15,idx,remain_bit,L);
                }
            }
        }
    }
    return 0;
}
int decode_L6_st4(int ID,unsigned char* L,int *idx,int *remain_bit,CLAS_DATA* Cd){
    unsigned int tmp = 0;
    unsigned int epoch_time = 0;
    epoch_time = Get_Bit_for(12,idx,remain_bit,L);
    tmp = Get_Bit_for(4,idx,remain_bit,L);
    tmp = Get_Bit_for(1,idx,remain_bit,L);
    tmp = Get_Bit_for(4,idx,remain_bit,L);

    for(int i = 0;i <GNSS_MAX;i++){
        if(Cd->Gi[i].involve){
            for(int p = 0;p<GPS_SVMAX;p++){
                for(int q = 0;q<GPS_SIGMAX;q++){
                    if(Cd->Gd[i].cd.T1_CellMask[p+q*GPS_SVMAX]){
                        Cd->Gd[i].cd.T4_CodeCorr[p+q*GPS_SVMAX] = Get_Bit_for(11,idx,remain_bit,L);
                    }
                }
            }
        }
    }

    return 0;}
int decode_L6_st5(int ID,unsigned char* L,int *idx,int *remain_bit,CLAS_DATA* Cd){
    unsigned int tmp = 0;
    unsigned int epoch_time = 0;
    epoch_time = Get_Bit_for(12,idx,remain_bit,L);
    tmp = Get_Bit_for(4,idx,remain_bit,L);
    tmp = Get_Bit_for(1,idx,remain_bit,L);
    tmp = Get_Bit_for(4,idx,remain_bit,L);
    for(int i = 0;i <GNSS_MAX;i++){
        if(Cd->Gi[i].involve){
            for(int p = 0;p<GPS_SVMAX;p++){
                for(int q = 0;q<GPS_SIGMAX;q++){
                    if(Cd->Gd[i].cd.T1_CellMask[p+q*GPS_SVMAX]){
                        Cd->Gd[i].cd.T5_PhaseCorr[p+q*GPS_SVMAX] = Get_Bit_for(15,idx,remain_bit,L);
                        Cd->Gd[i].cd.T5_PhaseIdt[p+q*GPS_SVMAX] = Get_Bit_for(2,idx,remain_bit,L);
                    }
                }
            }
        }
    }

    return 0;}
int decode_L6_st6(int ID,unsigned char* L,int *idx,int *remain_bit,CLAS_DATA* Cd){
    unsigned int tmp = 0;
    unsigned int epoch_time = 0;
    epoch_time = Get_Bit_for(12,idx,remain_bit,L);
    tmp = Get_Bit_for(4,idx,remain_bit,L);
    tmp = Get_Bit_for(1,idx,remain_bit,L);
    tmp = Get_Bit_for(4,idx,remain_bit,L);

    unsigned int Code_flag= Get_Bit_for(1,idx,remain_bit,L);
    unsigned int Phase_flag= Get_Bit_for(1,idx,remain_bit,L);
    unsigned int Network= Get_Bit_for(1,idx,remain_bit,L);
    if(Network)
    {
        unsigned int Net_id= Get_Bit_for(5,idx,remain_bit,L);
        if((int)Net_id!=ID)printf("weird in st6 %d %d\n",ID,Net_id);
        for(int i = 0;i <GNSS_MAX;i++){
            if(Cd->Gi[i].involve){
                for(int p = 0;p<GPS_SVMAX;p++){
                    if(Cd->Gd[i].cd.T1_SVMask[p])tmp = Get_Bit_for(1,idx,remain_bit,L);
                        for(int q = 0;q<GPS_SIGMAX;q++){
                            Cd->Gd[i].nd[ID-1].T6_LocalMask[p + q*GPS_SVMAX] = 0;
                            if(Cd->Gd[i].cd.T1_SigMask[q]&&Cd->Gd[i].cd.T1_SVMask[p])
                                Cd->Gd[i].nd[ID-1].T6_LocalMask[p + q*GPS_SVMAX] = tmp;
                        }
                }
            }
        }
    }
    for(int i = 0;i <GNSS_MAX;i++){
        if(Cd->Gi[i].involve){
            for(int p = 0;p<GPS_SVMAX;p++){
                for(int q = 0;q<GPS_SIGMAX;q++){
                    if(Cd->Gd[i].nd[ID-1].T6_LocalMask[p + q*GPS_SVMAX]){
                        if(Code_flag){
                            Cd->Gd[i].nd[ID-1].T6_CodeBias[p + q*GPS_SVMAX] = Get_Bit_for(11,idx,remain_bit,L);
                        }
                        if(Phase_flag){
                            Cd->Gd[i].nd[ID-1].T6_PhaseBias[p + q*GPS_SVMAX] = Get_Bit_for(15,idx,remain_bit,L);
                            Cd->Gd[i].nd[ID-1].T6_PhaseIdt[p + q*GPS_SVMAX] = Get_Bit_for(2,idx,remain_bit,L);
                        }
                    }
                }
            }
        }
    }
    return 0;
}
int decode_L6_st7(int ID,unsigned char* L,int *idx,int *remain_bit,CLAS_DATA* Cd){
    unsigned int tmp = 0;
    unsigned int epoch_time = 0;
    epoch_time = Get_Bit_for(12,idx,remain_bit,L);
    tmp = Get_Bit_for(4,idx,remain_bit,L);
    tmp = Get_Bit_for(1,idx,remain_bit,L);
    tmp = Get_Bit_for(4,idx,remain_bit,L);
    for(int i = 0;i <GNSS_MAX;i++){
        if(Cd->Gi[i].involve){
            for(int p = 0;p<GPS_SVMAX;p++){
                if(Cd->Gd[i].cd.T1_SVMask[p])
                {
                    Cd->Gd[i].cd.T7_URA[p] = Get_Bit_for(6,idx,remain_bit,L);
                }
            }
        }
    }
    return 0;}
int decode_L6_st8(int ID,unsigned char* L,int *idx,int *remain_bit,CLAS_DATA* Cd){
    unsigned int tmp = 0;
    unsigned int epoch_time = 0;
    epoch_time = Get_Bit_for(12,idx,remain_bit,L);
    tmp = Get_Bit_for(4,idx,remain_bit,L);
    tmp = Get_Bit_for(1,idx,remain_bit,L);
    tmp = Get_Bit_for(4,idx,remain_bit,L);

    unsigned int STEC_typ = 0;
    STEC_typ = Get_Bit_for(2,idx,remain_bit,L);
    unsigned int Net_id = 0;
    Net_id = Get_Bit_for(5,idx,remain_bit,L);

    if((int)Net_id!=ID)printf("weird int st8 %d %d\n",ID,Net_id);
    for(int i = 0;i < GNSS_MAX;i++){
        if(Cd->Gi[i].involve){
            for(int k = 0;k< GPS_SVMAX;k++){
                Cd->Gd[i].nd[ID-1].T8_LocalMask[k]=0;
                if(Cd->Gd[i].cd.T1_SVMask[k]){
                    Cd->Gd[i].nd[ID-1].T8_LocalMask[k] = Get_Bit_for(1,idx,remain_bit,L);
                }
            }
        }
    }
    if(STEC_typ == 0){
        for(int i = 0;i < GNSS_MAX;i++){
            if(Cd->Gi[i].involve){
                for(int k = 0;k< GPS_SVMAX;k++){
                    if(Cd->Gd[i].nd[ID-1].T8_LocalMask[k]){
                        Cd->Gd[i].nd[ID-1].T8_STECIdt[k] = Get_Bit_for(6,idx,remain_bit,L);
                        Cd->Gd[i].nd[ID-1].T8_C00[k] = Get_Bit_for(14,idx,remain_bit,L);
                    }
                }
            }
        }
    }else if(STEC_typ == 1){
        for(int i = 0;i < GNSS_MAX;i++){
            if(Cd->Gi[i].involve){
                for(int k = 0;k< GPS_SVMAX;k++){
                    if(Cd->Gd[i].nd[ID-1].T8_LocalMask[k]){
                        Cd->Gd[i].nd[ID-1].T8_STECIdt[k] = Get_Bit_for(6,idx,remain_bit,L);
                        Cd->Gd[i].nd[ID-1].T8_C00[k] = Get_Bit_for(14,idx,remain_bit,L);
                        Cd->Gd[i].nd[ID-1].T8_C01[k] = Get_Bit_for(12,idx,remain_bit,L);
                        Cd->Gd[i].nd[ID-1].T8_C10[k] = Get_Bit_for(12,idx,remain_bit,L);
                    }
                }
            }
        }
    }else {
        for(int i = 0;i < GNSS_MAX;i++){
            if(Cd->Gi[i].involve){
                for(int k = 0;k< GPS_SVMAX;k++){
                    if(Cd->Gd[i].nd[ID-1].T8_LocalMask[k]){
                        Cd->Gd[i].nd[ID-1].T8_STECIdt[k] = Get_Bit_for(6,idx,remain_bit,L);
                        Cd->Gd[i].nd[ID-1].T8_C00[k] = Get_Bit_for(14,idx,remain_bit,L);
                        Cd->Gd[i].nd[ID-1].T8_C01[k] = Get_Bit_for(12,idx,remain_bit,L);
                        Cd->Gd[i].nd[ID-1].T8_C10[k] = Get_Bit_for(12,idx,remain_bit,L);
                        Cd->Gd[i].nd[ID-1].T8_C11[k] = Get_Bit_for(10,idx,remain_bit,L);
                    }
                }
            }
        }
    }
    return 0;
}
int decode_L6_st9(int ID,unsigned char* L,int *idx,int *remain_bit,CLAS_DATA* Cd){
    unsigned int tmp = 0;
    unsigned int epoch_time = 0;
    epoch_time = Get_Bit_for(12,idx,remain_bit,L);
    tmp = Get_Bit_for(4,idx,remain_bit,L);
    tmp = Get_Bit_for(1,idx,remain_bit,L);
    tmp = Get_Bit_for(4,idx,remain_bit,L);

     unsigned int Trpo_type = Get_Bit_for(2,idx,remain_bit,L);
     unsigned int STEC = Get_Bit_for(1,idx,remain_bit,L);
     unsigned int Net_id = Get_Bit_for(5,idx,remain_bit,L);
    if((int)Net_id!=ID)printf("weird in st9 %d %d\n",ID,Net_id);

    for(int i = 0;i < GNSS_MAX;i++){
        if(Cd->Gi[i].involve){
            for(int k = 0;k< GPS_SVMAX;k++){
                Cd->Gd[i].nd[ID-1].T9_LocalMask[k] = 0;
                if(Cd->Gd[i].cd.T1_SVMask[k]){
                    Cd->Gd[i].nd[ID-1].T9_LocalMask[k] = Get_Bit_for(1,idx,remain_bit,L);
                }
            }
        }
    }
    unsigned int Trpo_idt = Get_Bit_for(6,idx,remain_bit,L);
    unsigned int grid = Get_Bit_for(6,idx,remain_bit,L);
    int corr_length = 0;
    if(STEC){corr_length = 16;
    }else {corr_length = 7;}

    for (int k = 0;k < grid;k++) {
        Cd->Gd[0].nd[ID-1].T9_Dry[k] = Get_Bit_for(9,idx,remain_bit,L);
        Cd->Gd[0].nd[ID-1].T9_Wet[k] = Get_Bit_for(8,idx,remain_bit,L);
        for(int i = 0;i<GNSS_MAX;i++){
            if(Cd->Gi[i].involve){
                for(int q = 0;q< GPS_SVMAX;q++){
                    if(Cd->Gd[i].nd[ID-1].T9_LocalMask[q]){
                        Cd->Gd[i].nd[ID-1].T9_STECCorr[q + k * GPS_SVMAX] = Get_Bit_for(corr_length,idx,remain_bit,L);
                    }
                }
            }
        }
    }
    return 0;}
int decode_L6_st10(){
    /* TBD */
    return 0;
}
int decode_L6_st11(int ID,unsigned char* L,int *idx,int *remain_bit,CLAS_DATA* Cd){
//    sf1->st11[ID-1].msg_num = Get_Bit_for(12,idx,remain_bit,L);
//    sf1->st11[ID-1].sub_typ = Get_Bit_for(4,idx,remain_bit,L);
//    if(sf1->st11[ID-1].sub_typ!=11){
//        return 1;
//    }
//    sf1->st11[ID-1].GPS_time = Get_Bit_for(12,idx,remain_bit,L);
//    sf1->st11[ID-1].SSR = Get_Bit_for(4,idx,remain_bit,L);
//    sf1->st11[ID-1].msg_idt = Get_Bit_for(1,idx,remain_bit,L);
//    sf1->st11[ID-1].IOD = Get_Bit_for(4,idx,remain_bit,L);
//    sf1->st11[ID-1].Orbit_flag = Get_Bit_for(1,idx,remain_bit,L);
//    sf1->st11[ID-1].Clock_flag = Get_Bit_for(1,idx,remain_bit,L);
//    sf1->st11[ID-1].Network = Get_Bit_for(1,idx,remain_bit,L);
//    if(sf1->st11[ID-1].Network){ sf1->st11[ID-1].Net_id = Get_Bit_for(5,idx,remain_bit,L);
//        sf1->st11[ID-1].SVmask = (int *)malloc(sf1->SVn * sizeof (int));
//        int sv_idx =  0;
//        for(int i = 0;i <(int) sf1->st1.num_gnss;i++){
//            for(int k = 0;k<sf1->st1.Nsys[i].SVn;k++){
//                sf1->st11[ID-1].SVmask[sv_idx] = Get_Bit_for(1,idx,remain_bit,L);
//                sv_idx++;
//            }
//        }
//    }
//    sf1->st11[ID-1].IODE = (int*)malloc((sf1->SVn) * sizeof (int));
//    sf1->st11[ID-1].Delta= (int*)malloc((sf1->SVn) * sizeof (int));
//    sf1->st11[ID-1].Delta_A = (int*)malloc((sf1->SVn) * sizeof (int));
//    sf1->st11[ID-1].Delta_C = (int*)malloc((sf1->SVn) * sizeof (int));
//    int SV_idx = 0;
//    for(int i = 0;i<sf1->st1.num_gnss;i++){
//        if(sf1->st1.Nsys[i].id == 2){
//            for(int k = 0;k<sf1->st1.Nsys[i].SVn;k++){
//                if(sf1->st11[ID-1].Orbit_flag)
//                {
//                    sf1->st11[ID-1].IODE[SV_idx] = Get_Bit_for(10,idx,remain_bit,L);
//                    sf1->st11[ID-1].Delta[SV_idx] = Get_Bit_for(15,idx,remain_bit,L);
//                    sf1->st11[ID-1].Delta_A[SV_idx] = Get_Bit_for(13,idx,remain_bit,L);
//                    sf1->st11[ID-1].Delta_C[SV_idx] = Get_Bit_for(13,idx,remain_bit,L);
//                }else{
//                    sf1->st11[ID-1].IODE[SV_idx] = 0;
//                    sf1->st11[ID-1].Delta[SV_idx] = 0;
//                    sf1->st11[ID-1].Delta_A[SV_idx] = 0;
//                    sf1->st11[ID-1].Delta_C[SV_idx] = 0;
//                }
//                if(sf1->st11[ID-1].Clock_flag){
//                    sf1->st11[ID-1].Cloc_corr[SV_idx] = Get_Bit_for(15,idx,remain_bit,L);
//                }else {
//                    sf1->st11[ID-1].Cloc_corr[SV_idx] = 0;
//                }
//                SV_idx ++;
//            }
//        }else{
//            for(int k = 0;k<sf1->st1.Nsys[i].SVn;k++){
//                if(sf1->st11[ID-1].Orbit_flag)
//                {
//                    sf1->st11[ID-1].IODE[SV_idx] = Get_Bit_for(8,idx,remain_bit,L);
//                    sf1->st11[ID-1].Delta[SV_idx] = Get_Bit_for(15,idx,remain_bit,L);
//                    sf1->st11[ID-1].Delta_A[SV_idx] = Get_Bit_for(13,idx,remain_bit,L);
//                    sf1->st11[ID-1].Delta_C[SV_idx] = Get_Bit_for(13,idx,remain_bit,L);
//                }else{
//                    sf1->st11[ID-1].IODE[SV_idx] = 0;
//                    sf1->st11[ID-1].Delta[SV_idx] = 0;
//                    sf1->st11[ID-1].Delta_A[SV_idx] = 0;
//                    sf1->st11[ID-1].Delta_C[SV_idx] = 0;
//                }
//                if(sf1->st11[ID-1].Clock_flag){
//                    sf1->st11[ID-1].Cloc_corr[SV_idx] = Get_Bit_for(15,idx,remain_bit,L);
//                }else {
//                    sf1->st11[ID-1].Cloc_corr[SV_idx] = 0;
//                }
//                SV_idx ++;
//            }
//        }
//    }

    return 0;
}

int decode_L6_init(CLAS_DATA* Cd){
    decode_L6_all[0] = decode_L6_st1;
    decode_L6_all[1] = decode_L6_st2;
    decode_L6_all[2] = decode_L6_st3;
    decode_L6_all[3] = decode_L6_st4;
    decode_L6_all[4] = decode_L6_st5;
    decode_L6_all[5] = decode_L6_st6;
    decode_L6_all[6] = decode_L6_st7;
    decode_L6_all[7] = decode_L6_st8;
    decode_L6_all[8] = decode_L6_st9;
    //decode_L6_all[0] = decode_L6_st10;
    decode_L6_all[10] = decode_L6_st11;
    memset(Cd,0,sizeof (CLAS_DATA));
    //printf("sixe %d",sizeof (CLAS_DATA));
    return 0;
}





