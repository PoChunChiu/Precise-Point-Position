#include "rtklib.h"

rtk_t::rtk_t()
{
    obs=new obs_t();
    nav=new nav_t();
    obs->SVn_max = MAXGPS;
    nav->SVn_max = MAXGPS;
    Pos[0] =  -2957014.38742;
    Pos[1] =  5075859.265572;
    Pos[2] =  2476270.078605;
    Pos[3] =  -1.282;
    for(int i = 0;i<MAXGPS*2;i++)
        EKF_R[i] = 5.0;
}

int rtk_t::read_data(ifstream& in_f1,ifstream& in_f2,ifstream& in_f3){
    if(read_rinex_obs(in_f1,obs,InitFlag))return 1;
    int cnt = 0 ;
    if(InitFlag)
    {
        do{
            read_rinex_nav(in_f2,nav,InitFlag);
            InitFlag = 0;
            cnt ++;
        }while(cnt<MAXGPS);
    }
    //read_precise_eph(in_f3,obs);
    remove_Sat();

    return 0;
}
void rtk_t::calc_satpos()
{
    int i, j, sv_index;
    double  tk, a, n0, n, mk, e, ek, sinvk, cosvk
            , vk, dtr, dk, duk, uk, drk, rk, dik, ik, omegak, pos[3]
            , t_toc, range , range_L2, tt
            , posxold, posyold, posxold1, posyold1;
    int flag = 1;

    const double myu    = 3.9860044e14; //
    const double f      = -4.442807633e-10;//
    const double omegae = 7.2921151467e-5;//

    // give pointer for obs, eph

    /*if (flag == 1)
    {
        // Iono initialize
        for (i = 0; i< MAXSAT; i++)
            obs->sv[i].Iono = 0.0;

        // Tropo initialize (Iono ?)
        for (i = 0; i< MAXSAT; i++)
            obs->sv[i].Tropo = 0.0;
    }*/

    if (flag != 2)
    {
        // Initialize
        //memset(rtk->nav.SVn_sat,0,MAXSAT*sizeof(int));

        for(i = 0; i < obs->SVn; i++)
        {

            if(obs->prn[i] == 0){continue;}
            // PRN, satellite index
            sv_index = obs->prn[i] - 1;
           // cout << "    Compare : obs prn = " << obs->prn[i]<<endl;
            //cout << "              nav prn = " << nav->prn[sv_index]<<endl;
            t_toc = obs->time_in_sec[i] - nav->toe[sv_index];
            //cout << "rinex "<<sv_index+1<<"\t"<<setprecision(15)<<t_toc<<"   "<<obs->time_in_sec[i] <<"   "<<nav->toe[sv_index]<<endl;
            if(t_toc>302400)
                t_toc-=604800;
            else if(t_toc<-302400)
                t_toc+=604800;
            //satellite time correction
            range = obs->P[i][0] + CLIGHT*(nav->af0[sv_index]+ nav->af1[sv_index] * t_toc
                                           + nav->af2[sv_index] * t_toc * t_toc);
            range_L2 = obs->P[i][1] + CLIGHT*(nav->af0[sv_index]+ nav->af1[sv_index] * t_toc
                                              + nav->af2[sv_index] * t_toc * t_toc);
            //satellite time correction
            range = range - CLIGHT * nav->tgd[sv_index];
            range_L2 = range_L2 - CLIGHT * nav->tgd[sv_index];
            nav->dts[sv_index] = (nav->af0[sv_index]+ nav->af1[sv_index] * t_toc
                                  + nav->af2[sv_index] * t_toc * t_toc);
            //cout << "rinex "<<sv_index+1<<"\t"<<setprecision(15)<<t_toc<<endl;
            nav->dts_L2[sv_index] = (nav->af0[sv_index]+ nav->af1[sv_index] * t_toc
                                     + nav->af2[sv_index] * t_toc * t_toc);
            //transmit time estimation
            tt = obs->time_in_sec[i] - range/CLIGHT;
            tk = tt - nav->toe[sv_index];

            if(tk > 302400)
                tk-=604800;
            else if(tk <-302400)
                tk+=604800;

            a     = nav->roota[sv_index]* nav->roota[sv_index];
            n0    = sqrt(myu/(a*a*a));
            n     = n0 + nav->dn[sv_index];
            mk    = nav->M0[sv_index] + n*tk;
            e     = nav->e[sv_index];

            if(nav->roota[sv_index] == 0.0)
            {
                printf("%lf can not use satellite prn = %d\n", obs->time_in_sec[i], sv_index+1);
                continue;
            }
            else
                nav->Health[sv_index] = 1;

            ek = mk;

            /* eccentric anomaly */
            while ( fabs( mk - ( ek - e * sin(ek) ) )  > 10e-12 )
                ek = mk + e * sin(ek);

            sinvk = sqrt(1.0-e*e)*sin(ek)/(1.0-e*cos(ek));
            cosvk = (cos(ek)-e)/(1.0-e*cos(ek));

            vk   = atan2(sinvk, cosvk);
            dtr  = f*e*nav->roota[sv_index]*sin(ek);

            range  = range + CLIGHT*dtr;

            dk     = vk + nav->omg[sv_index];
            duk    =  nav->cus[sv_index]*sin(2*dk)+nav->cuc[sv_index]*cos(2*dk);
            uk     = dk + duk;
            drk    = nav->crc[sv_index]*cos(2*dk)+nav->crs[sv_index]*sin(2*dk);
            rk     = a*(1.0-e*cos(ek))+drk;
            dik    = nav->cic[sv_index]*cos(2*dk)+nav->cis[sv_index]*sin(2*dk);
            ik     = nav->i0[sv_index] + dik + nav->idot[sv_index]*tk;
            omegak = nav->OMG0[sv_index] + (nav->OMGd[sv_index] - omegae)*tk-omegae*nav->toe[sv_index];

            pos[0] = rk * cos(uk) * cos(omegak) - rk * sin(uk) * cos(ik) * sin(omegak);
            pos[1] = rk * cos(uk) * sin(omegak) + rk * sin(uk) * cos(ik) * cos(omegak);
            pos[2] = rk * sin(uk) * sin(ik);

            posxold1 = pos[0];
            posyold1 = pos[1];

            for(j=1;j<=2;j++)
            {
                posxold = +omegae*pos[1]*range/CLIGHT;
                posyold = -omegae*pos[0]*range/CLIGHT;
                pos[0] = posxold + posxold1;
                pos[1] = posyold + posyold1;
            }

            obs->Sv_Pos[sv_index][0] = pos[0];
            obs->Sv_Pos[sv_index][1] = pos[1];
            obs->Sv_Pos[sv_index][2] = pos[2];


            //Sv_correctime
            obs->Sv_Pos[sv_index][3]  = range - obs->P[i][0] - dtr;
            nav->dts[sv_index] -= 2.0*pow(myu*a,0.5)*e*sin(ek)/(CLIGHT*CLIGHT);
            //printf("%d %f \n",obs->prn[i],nav->dts[sv_index]);
            if (flag == 1) // rcvn = 0
            {
                // elevation & azimuth
                //calc_direction(&obs->sv[sv_index], rtk->POSxyz[rcvn]);

                // iono delay (only GPS)
                /*calc_iono_model(&obs->sv[sv_index], &rtk->nav.ion_gps[0], &rtk->nav.ion_gps[4],
                        rtk->POSllh[rcvn], rtk->obs[rcvn].time);*/

                // Tropo delay
                //calc_tropo_model(&obs->sv[sv_index], rtk->POSllh[rcvn], rtk->GPSTIME);
            }
        }
    }
    /*else
    {
        for(i = 0; i < obs->n; i++)
        {
            // PRN
            sv_index = obs->SVn[i] - 1;

            memcpy(obs->sv[sv_index].SVxyz, rtk->obs[0].sv[sv_index].SVxyz, 3*sizeof(double));
            // elevation & azimuth
//            calc_direction(&obs->sv[sv_index], rtk->obs[1].POSxyz);
            calc_direction(&obs->sv[sv_index], rtk->POSxyz[0]);
        }
    }*/
}
void rtk_t::remove_Sat(){
    int SVn = 0;
    for(int i = 0;i<obs->SVn;i++){
        if(obs->health[i]==1)
        {
            for(int k = i+1;k<obs->SVn;k++)
            {

                obs->prn[k-1] = obs->prn[k];
                for(int j =0;j<3;j++){
                    obs->P[k-1][j] = obs->P[k][j];
                    obs->L[k-1][j] = obs->L[k][j];
                    obs->D[k-1][j] = obs->D[k][j];
                }
                obs->health[k-1]=obs->health[k];
            }
        }else{
            SVn++;
        }
    }
    obs->SVn = SVn;
}
