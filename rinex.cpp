#include "rtklib.h"

int read_rinex_obs(ifstream& Obs_ptr, obs_t *rtk, int Initflag)
{
    string line;
    if(Initflag == 1)
        while(getline(Obs_ptr,line)){
            if(line.find("RINEX VERSION / TYPE")!= std::string::npos){
                rtk->obsHeader.RINEX_ver = stof(line.substr(4,5));}
            else if(line.find("PGM / RUN BY / DATE")!= std::string::npos){
                rtk->obsHeader.Year = stoi(line.substr(40,4));
                rtk->obsHeader.Month = stoi(line.substr(44,2));
                rtk->obsHeader.Day= stoi(line.substr(46,2));
            }
            else if(line.find("SYS / # / OBS TYPES")!= std::string::npos){
                if(line.substr(0,1) != "G")continue;
                rtk->signaltype.n = stoi(line.substr(4,2));
                for (int i = 1;i<=rtk->signaltype.n;i++){
                    if(line.substr(3+4*i,1)=="C")rtk->signaltype.type[i-1]=0;
                    if(line.substr(3+4*i,1)=="L")rtk->signaltype.type[i-1]=1;
                    if(line.substr(3+4*i,1)=="D")rtk->signaltype.type[i-1]=2;
                    if(line.substr(3+4*i,1)=="S")rtk->signaltype.type[i-1]=3;
                    switch (stoi(line.substr(3+4*i+1,1))) {
                    case 1:
                        rtk->signaltype.frq[i-1]=1;
                        break;
                    case 2:
                        rtk->signaltype.frq[i-1]=2;
                        break;
                    default:
                        break;
                    }
                }
            }
            else if(line.find("END OF HEADER")!= std::string::npos){
                cout << "End of Obs header!" <<endl;
                break;
            }
            line.clear();
        }

    int i = 0;
    int nsat = 0;
    int n = 0;
    int flag = 0;
    gtime_t time;
    //return 1;
    while(getline(Obs_ptr,line))
    {
        if (i==0)
        {

//            memset(&obs->data, 0, 14 * sizeof(obsd_t));
//            memset(&obs->n, 0, 14 * sizeof(int));
//            memset(&rtk->ssat, 0, MAXSAT * sizeof(ssat_t));
//            cout <<&obs->data[n].time_in_sec<<endl;
            if ((nsat = decode_obsepoch(line, &flag, &time, &rtk->time_in_sec[n]))<=0)
                continue;
        }
        else if (flag<=2||flag==6)
        {
            rtk->time_in_sec[n] = rtk->time_in_sec[0] ;
            /* decode obs data */
            // read Pr,Cp,D,SNR in struct
            if (!decode_obsdata(&line,rtk,n,rtk->signaltype) && n < 14)
            {
                //cout << rtk->prn[n]<<" "<<rtk->P[n][1] <<" "<< rtk->L[n][1]<<endl;
                    n++;
            }
        }

        if (++i > nsat)
        {
            // end of an epoch data
            //rtk->SVn_max = nsat;
            rtk->SVn = n;
            i = 0;
            n = 0;
            line = "";
            return 0;
        }
    }


    printf("End of base observation file\n");

    return 1;
}

int decode_obsepoch(string line, int *flag, gtime_t *time, double* t){
    int n = 0;
    double year, month, day, hours, minutes, week;
    double seconds;
    // n: number of satellite in view
    if ((n=stoi(line.substr(32,3)))<=0) return 0;

    *flag=stoi(line.substr(31,1));

    if (3<=*flag && *flag<=5) return n;

    str2time(&line,0,27,time);

    // +1 for space
    year    = stod(line.substr(2,4));
    month   = stod(line.substr(7,2));
    day     = stod(line.substr(10,2));
    hours   = stod(line.substr(13,2));
    minutes = stod(line.substr(16,2));
    seconds = stod(line.substr(19,11));

    week = dayofweek(year, month, day);

    *t = (week*86400.0 + hours*3600.0 + minutes*60.0 + seconds);

    return n;
}
int dayofweek(int y, int m, int d) /* 0 = Sunday , 1 <= m <= 12,  y > 1752 or so */
{
    return (y + y/4 - y/100 + y/400 + "032503514624"[m-1]-'0' + d) % 7;
}
int str2time(string *L, int i, int n, gtime_t *t)
{
    string s = L->substr(2.1);
    double ep[6];
    char str[256];
    const char *p;

    if (i<0||(int)(s.length())<i||(int)sizeof(str)-1<i) return -1;
    for (s+=i;s!=""&&--n>=0;) p=s.c_str();
    if (sscanf(str,"%lf %lf %lf %lf %lf %lf",ep,ep+1,ep+2,ep+3,ep+4,ep+5)<6)
        return -1;
    if (ep[0]<100.0) ep[0]+=ep[0]<80.0?2000.0:1900.0;
    *t=epoch2time(ep);
    return 0;
}

/* ============== epoch2time ==============
* convert calendar day/time to gtime_t struct
* args   : double *ep       I   day/time {year,month,day,hour,min,sec}
* return : gtime_t struct
* notes  : proper in 1970-2037 or 1970-2099 (64bit time_t)
* ========================================= */
gtime_t epoch2time(const double *ep)
{
    const int doy[]={1,32,60,91,121,152,182,213,244,274,305,335};
    gtime_t time;
    int days,sec,year=(int)ep[0],mon=(int)ep[1],day=(int)ep[2];

    if (year<1970||2099<year||mon<1||12<mon) return time;

    /* leap year if year%4==0 in 1901-2099 */
    days=(year-1970)*365+(year-1969)/4+doy[mon-1]+day-2+(year%4==0&&mon>=3?1:0);
    sec=(int)floor(ep[5]);
    time.time=(time_t)days*86400+(int)ep[3]*3600+(int)ep[4]*60+sec;
    time.sec=ep[5]-sec;
    return time;
}
int decode_obsdata(string *line, obs_t* obs ,int index ,sigind_t sign){
    double data;
    int i,j=0;
    obs->prn[index] = 0;
    if(line->substr(0,1) == "G"){}else{return 1;}
    obs->prn[index] = stoi(line->substr(1,2));

    for (i = 0; i < sign.n ; i++)
    {
        try {
            data = stod(line->substr(3+j,14));
            //cout <<"    decode obs  "<< obs->prn[index] <<"  "<<setprecision(10)<<data<<endl;
        } catch (std::invalid_argument) {
            obs->health[index] = 1;
            switch(sign.type[i])
            {
            case 0: obs-> P[index][sign.frq[i]-1] = obs-> P[index][0]; break;
            case 1: obs-> L[index][sign.frq[i]-1] = obs-> L[index][0]*LAMBDA1/LAMBDA2;break;
            case 2: obs-> D[index][sign.frq[i]-1] = obs-> D[index][0]; break;
            case 3: obs-> SNR[index][sign.frq[i]-1] = obs-> SNR[index][0]; break;
            default : return 1;
            }
            continue;
        }
        j+=16;
        //cout<<"rinex 164  "<<sign.n<<"  "<<sign.type[i]<<"  fr = "<<sign.frq[i]<<endl;
        // Pr: pseudorange
        // Cp: carrier phase measurement
        // D : Doppler
        // Cn: SNR
        switch(sign.type[i])
        {
        case 0: obs-> P[index][sign.frq[i]-1] = data; break;
        case 1: obs-> L[index][sign.frq[i]-1] = data;break;
        case 2: obs-> D[index][sign.frq[i]-1] = data; break;
        case 3: obs-> SNR[index][sign.frq[i]-1] = data; break;
        default : return 1;
        }
        obs->health[index] = 0;
    }
    return 0;
}

int read_rinex_nav(ifstream& Nav_ptr, nav_t *nav, int Initflag)
{
    string line;
    int i, j, prn, countline = 0;
    static int flag = 0;
    double toc, data[32] = {0.0};

    if (flag == 2) return 1;

    //////////////// Nav Header reader ///////////////////////
    // Header reader
    if (Initflag==1)
    while(getline(Nav_ptr,line))
    {
        if (line.find("RINEX VERSION / TYPE")!= std::string::npos)
            nav->header.RINEX_ver = stof(line.substr(4,5));
        else if (line.find("PGM / RUN BY / DATE")!= std::string::npos)
        {
            nav->header.Year  = stoi(line.substr(40,4));
            nav->header.Month = stoi(line.substr(44,2));
            nav->header.Day   = stoi(line.substr(46,2));
        }
        else if (line.find("IONOSPHERIC CORR")!= std::string::npos)
        {
            if (line.find("GPSA")!= std::string::npos)
                for (i = 0; i< 4; i++) nav->ion_gps[i] = stod(line.substr(7+12*i,10));
            else if (line.find("GPSB")!= std::string::npos)
                for (i = 0; i< 4; i++) nav->ion_gps[i+4] = stod(line.substr(7+12*i,10));
        }
        else if (line.find("TIME SYSTEM CORR")!= std::string::npos)
        {
            if (line.find("GPUT")!= std::string::npos)
            {
                nav->utc_gps[0] = stod(line.substr(6,16));
                nav->utc_gps[1] = stod(line.substr(23,16));
                nav->utc_gps[2] = stod(line.substr(39,6));
                nav->utc_gps[3] = stod(line.substr(46,4));
            }
        }
        else if (line.find("LEAP SECONDS")!= std::string::npos)
            nav->leaps = stoi(line.substr(3,3));
        else if (line.find("END OF HEADER")!= std::string::npos)
        {
            printf("End of Nav header!\n");
            flag = 1;
            break;
        }
        line = "";
    }

    //////////////// Nav Body reader ///////////////////////

    i = 0;
    while(getline(Nav_ptr,line))
    {
        int l = line.length();
        //for(int k = 0;k<l;k++)
            //printf("%c",line[k]);
        if (countline++ == 0){
        }

        if (i==0)
        {
            prn = 0;
            // get sat PRN
            prn = stoi(line.substr(1,2));
            //cout <<"    decode nav: "<<prn<<endl;
            // get satellite system
            if(line.substr(0,1)!= "G")
            {
                countline = 0;
                continue;
            }
            // get toc field
            toc = get_toc(line);
            // decode data fields (line 1)
            for (j=0;j<3;j++)
            {
                //cout <<line.substr(23+j*19,19);
                data[i++] = str2num(line.substr(23+j*19,19),19);
                //cout << data[i-1] <<endl;
            }
           // cout <<endl;
        }
        else if (countline == 8)
        {
            for (j=0;j<2;j++)
            {
                //cout <<line.substr(4+j*19,19);
                data[i++] = str2num(line.substr(4+j*19,19),19);
            }
            //cout <<endl;
            decode_eph(&prn, toc, data, nav);
            i = 0;
            countline = 0;
            return 0;
        }
        else
        {
            for (j=0;j<4;j++)
            {
                //cout <<line.substr(4+j*19,19);
                data[i++] = str2num(line.substr(4+j*19,19),19);
            }
        }
        //for(int k = 0;k<l;k++)
            //printf("%c",line[k]);
        //printf("\n");
        line.clear();

    }
    printf("End of base navigation file\n");
    // End of nav file (flag: 2)
    flag = 2;
    //free(data);

    return 1;
}

/* ========= decode_eph ==========
 * Copy emphemeris into our designed struct
 *
 * Input: prn, toc(s), Source data
 * Output: ephermis struct
 * Retrun: None
 * ===============================*/
void decode_eph(int *prn, double toc, const double *data, nav_t *eph)
{
    int check;
    eph->prn[*prn-1]    =   *prn;
    eph->toc[*prn-1]    =   toc;
    eph->af0[*prn-1]    =   data[ 0];
    eph->af1[*prn-1]    =   data[ 1];
    eph->af2[*prn-1]    =   data[ 2];
    eph->iode[*prn-1]   =   (int)data[ 3];
    eph->crs[*prn-1]    =   data[ 4];
    eph->dn[*prn-1]     =   data[ 5];
    eph->M0[*prn-1]     =   data[ 6];
    eph->cuc[*prn-1]    =   data[ 7];
    eph->e[*prn-1]      =   data[ 8];
    eph->cus[*prn-1]    =   data[ 9];
    eph->roota[*prn-1]  =   data[10];
    eph->toe[*prn-1]    =   data[11];
    eph->cic[*prn-1]    =   data[12];
    eph->OMG0[*prn-1]   =   data[13];
    eph->cis[*prn-1]    =   data[14];
    eph->i0[*prn-1]     =   data[15];
    eph->crc[*prn-1]    =   data[16];
    eph->omg[*prn-1]    =   data[17];
    eph->OMGd[*prn-1]   =   data[18];
    eph->idot[*prn-1]   =   data[19]; //di0

    eph->week[*prn-1]   =   (int)data[21];      /* week */

    eph->code[*prn-1]=(int)data[20];      /* GPS: codes on L2 ch */
    eph->iodc[*prn-1]=(int)data[26];      /* IODC */
    eph->svh[*prn-1] =(int)data[24];      /* sv health */
    eph->flag[*prn-1]=(int)data[22];      /* GPS: L2 P data flag */

    eph->tgd[*prn-1]   =   data[25];      /* TGD */

    // Transmission time of message
    if ((check = data[27]) < 0)
        check+=604800;
    eph->weekoftime[*prn-1] = check;
    //eph->time[prn] = gpst2time(eph->week, check);
}

/* ============ get_toc =================
 * arg:     s date string
 *
 * return  toc in seconds
 * =======================================*/
double get_toc(string s)
{
    int week, year, month, day, hours, minutes, seconds;
    // +1 for space
    year    = stoi(s.substr(4,4));
    month   = stoi(s.substr(9,2));
    day     = stoi(s.substr(12,2));
    hours   = stoi(s.substr(15,2));
    minutes = stoi(s.substr(18,2));
    seconds = stoi(s.substr(20,2));
    week = dayofweek(2000+year, month, day);
    s.clear();

    return (week*86400 + hours*3600.0 + minutes*60.0 + seconds);
}

double str2num(string s, int length)
{
    double positive = 1.0,value = 0.0;
    int start = 1;
    string v = "0.";
    string str_value;
    int n;
    //cout << s<<endl;
    for (int i = 0;i < length;i++){
        if(s.substr(i,1)=="-")
        {
            positive = -1.0;
            start = i + 1;
        }
        if(s.substr(i,1)==".")
        {
            start = i + 1;
        }
        if(s.substr(i,1)=="D"){
            str_value = v + s.substr(start,i - start );
            value += stod(str_value);
            //cout<<"\n"<<str_value<<"\t"<<value<<endl;
            n = stoi(s.substr(i+1,length-i));
            break;
        }
    }
    value = positive * value * pow(10.0,n);
    s.clear();
    return value;
}
void read_precise_eph(ifstream& Peph_ptr, obs_t *obs){
    string line;
    int cnt_line = 0,prn,n = 0;//count_line
    int y,m,d,h,min,sec,week;
    double pre_time;
    while (obs->Pre_SVn==0) {
        getline(Peph_ptr,line);
       // cout <<line<<endl;
        if(line.substr(0,1) == "+"){
            obs->Pre_SVn = stoi(line.substr(3,3));
            break;
        }
    }
    while (getline(Peph_ptr,line)) {
        if(line.substr(0,1) == "*"){
            y = stoi(line.substr(3,4));
            m = stoi(line.substr(8,2));
            d = stoi(line.substr(11,2));
            h = stoi(line.substr(14,2));
            min = stoi(line.substr(17,8));
            sec = stoi(line.substr(20,11));
            week = dayofweek(2000+y, m, d);
            pre_time = (week*86400 + h*3600.0 + min*60.0 + sec);
            obs->pre_time=pre_time;
            cnt_line = 1;
        }
        else if(cnt_line >= 1 && line.substr(0,1) == "P"){
            prn = stoi(line.substr(2,3));
            obs->Pre_Sv_Pos[prn-1][0] = stod(line.substr(5,14));
            obs->Pre_Sv_Pos[prn-1][1] = stod(line.substr(19,14));
            obs->Pre_Sv_Pos[prn-1][2] = stod(line.substr(33,14));
            obs->Pre_Sv_Pos[prn-1][3] = stod(line.substr(47,14));
            cnt_line++;
        }
        if(cnt_line >= obs->Pre_SVn)break;
    }

}
