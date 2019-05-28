#include "rtklib.h"
void initial_EKF_xP(rtk_t* rtk){



        rtk->EKF_x[EKF_STATE_NUM-1] = CLIGHT * rtk->Pos[EKF_STATE_NUM-1];
        for(int i = EKF_STATE_NUM;i < EKF_STATE_MAX ;i++){
            rtk->EKF_x[i] =i<3? 0.0:0.0;//rtk->Pos[i];
        }
         //Prepare EKF state corvariance "P"
        if(rtk->InitFlag_EKF==1)
        {
            for(int i = 0;i < EKF_STATE_MAX ;i++){
                rtk->EKF_x[i] =i<4? 0.0:rtk->obs->L[i-EKF_STATE_NUM][2]-rtk->obs->P[i-EKF_STATE_NUM][2];//rtk->Pos[i];
            }
            for(int i = 0;i < EKF_STATE_MAX ;i++)
                for(int k = 0;k<EKF_STATE_MAX ;k++){
                    rtk->EKF_P[i+EKF_STATE_MAX*k]=i==k?SQR(100.0):0.0;

                }
        return;
        }
        for(int i = 0;i < EKF_STATE_MAX ;i++){
            rtk->EKF_x[i] =i<3? 0.0:0.0;//rtk->Pos[i];
        }

}

void pppos(rtk_t* rtk){
    iono_free_LC(rtk->obs,rtk->nav,rtk->obs->SVn);
    int SVn = rtk->obs->SVn;

    double y[2*SVn] , x[(EKF_STATE_NUM+SVn)];
    double v[2*SVn] , H[(EKF_STATE_NUM+SVn)*(2*SVn)];
    double PH[(EKF_STATE_NUM+SVn)*(2*SVn)], Q[(2*SVn)*(2*SVn)] , K[(EKF_STATE_NUM +SVn)*(2*SVn)];
    double KH[(EKF_STATE_NUM+SVn)*(EKF_STATE_NUM+SVn)],P_update[(EKF_STATE_NUM+SVn)*(EKF_STATE_NUM+SVn)],x_update[(EKF_STATE_NUM+SVn)];
    double I_KH[(EKF_STATE_NUM+SVn)*(EKF_STATE_NUM+SVn)];
    double P[(EKF_STATE_NUM+SVn)*(EKF_STATE_NUM+SVn)];
    int prn ;
    const double omegae = 7.2921151467e-5;
    double Pos_xyz[3],SV_xyz[3],SV_enu[3];
    double sqrdistance = 0.0 ,distance = 0.0, elevation = 0.0, Tropo = 0.000005;
    // Prepare EKF measurement "y"
    for(int i = 0;i < SVn ;i++){
        y[2*i] = rtk->obs->L[i][2];
        y[2*i + 1] = rtk->obs->P[i][2];
    }


    for(int iter = 0;iter<KALMAN_ITER;iter++){

        // Prepare Measurement model "H" and "v"
        for(int i = 0; i < EKF_STATE_NUM + SVn; i++)
        {
            for(int k = 0;k<SVn;k+=1){
                prn = rtk->obs->prn[k] - 1;
                initial_EKF_xP(rtk);

                if(rtk->obs->health[prn]>0){
                    k--;
                    continue;
                }
                sqrdistance = pow(rtk->Pos[0] - rtk->obs->Sv_Pos[prn][0],2) + pow(rtk->Pos[1] - rtk->obs->Sv_Pos[prn][1],2) + pow(rtk->Pos[2] - rtk->obs->Sv_Pos[prn][2],2);
                distance = pow(sqrdistance,0.5);
                // Calculate SV elevation
                Pos_xyz[0] = rtk->Pos[0];
                Pos_xyz[1] = rtk->Pos[1];
                Pos_xyz[2] = rtk->Pos[2];
                SV_xyz[0] = rtk->obs->Sv_Pos[prn][0];
                SV_xyz[1] = rtk->obs->Sv_Pos[prn][1];
                SV_xyz[2] = rtk->obs->Sv_Pos[prn][2];
                wgsxyz2enu(SV_xyz,Pos_xyz,SV_enu);
                elevation = atan(SV_enu[2]/pow(SV_enu[0]*SV_enu[0]+SV_enu[1]*SV_enu[1],0.5));
                Tropo = SAAS_tropos_model(elevation);

                rtk->EKF_R[prn] =  5.76 + 0.003*0.003/sin(elevation)/sin(elevation);
                rtk->EKF_R[prn+MAXGPS] =  5.76 + 0.3*0.3/sin(elevation)/sin(elevation) ;


               // cout <<rtk->EKF_R[prn]<<" / " <<rtk->EKF_R[prn+MAXGPS]  <<endl;

                if(i == 0)
                {
                    //cout <<fixed<<setprecision(10)<<k<<" "<<prn+1<<" Ele = "<<elevation*180/PI<<" Tropo = "<<Tropo<<" Mease = (P)"<<setprecision(10)<<y[2*k+1]<<" (L)"<<setprecision(10)<<y[2*k]<<" Dist = "<<setprecision(10)<<distance-CLIGHT* rtk->nav->dts[prn]+Tropo<<endl;
                    //cout <<fixed<<setprecision(10)<< "dT - dt = "<<rtk->obs->Sv_Pos[prn][3]<<" - "<<rtk->Pos[3]<<rtk->obs->Sv_Pos[prn][3]- rtk->Pos[3]<<endl;
                }
                if(i < 3){
                    if(i == 0)
                    {
//                        cout << fixed<<setprecision(10)<<"G"<<prn+1<<"v = ";
//                        cout <<"(L)"<<y[(2*k)]<<" (P)"<<y[(2*k+1)]<<endl;;
//                        cout <<"Dist = "<<"(distance)"<<distance<< " - (dtdT)" <<( rtk->nav->dts[prn] )<<"/"<<rtk->obs->Sv_Pos[prn][3]- rtk->Pos[3]
//                               <<" + (Tropo)" <<Tropo<< "+ (dL)"<<rtk->EKF_x[4+k]<<"=" <<(distance - /*rtk->obs->Sv_Pos[prn][3]*/CLIGHT* rtk->nav->dts[prn]+ rtk->Pos[3] + Tropo)<<endl;
                    }
                    // v = Measurement - (geo_distance - c * dts + c * dtr + phase bias(if Carrier-Phase)

                    v[2*k] = y[2*k] - (distance - CLIGHT* rtk->nav->dts[prn] + rtk->Pos[3] + rtk->EKF_x[4+k] + Tropo);
                    v[(2*k+1)] = y[(2*k+1)] - (distance - CLIGHT* rtk->nav->dts[prn]+ rtk->Pos[3] + Tropo);

                    H[i+(EKF_STATE_NUM+SVn)*2*k] = (rtk->Pos[i] - rtk->obs->Sv_Pos[prn][i])/distance;
                    H[i+(EKF_STATE_NUM+SVn)*(2*k+1)] = (rtk->Pos[i] - rtk->obs->Sv_Pos[prn][i])/distance;
                }
                if(i == 3){
                    H[i+(EKF_STATE_NUM+SVn)*2*k] = 1.0;
                    H[i+(EKF_STATE_NUM+SVn)*(2*k+1)] = 1.0;
                }
                if(i > 3){
                    H[i+(EKF_STATE_NUM+SVn)*2*k] = k==(i-4)?1.0:0.0;
                    H[i+(EKF_STATE_NUM+SVn)*(2*k+1)]= 0.0;
                }

            }

        }
//        cout << " v = ";
//        for(int i = 0;i<(2*SVn);i++)
//        {
//            cout << v[i] <<" ";
//           // if(abs(v[i])>1000)system("Pause");
//        }cout << "end"<<endl;
        // EKF Measurement update
        EKFfilter(H,PH,Q,rtk,K);
        Measurement_Update(H,v,K,rtk);
        //Least_SQR(H,v,rtk);
        for(int i = 0;i<4;i++)
        {
            rtk->Pos[i] = rtk->Pos[i] + rtk->EKF_x[i];;
        }
        if(pow(rtk->EKF_x[0],2)+pow(rtk->EKF_x[1],2)+pow(rtk->EKF_x[2],2)<0.00000001)break;
        //cout<<"Pos = "<<setprecision(10)<<rtk->Pos[0]<<"\t"<<rtk->Pos[1]<<"\t"<<rtk->Pos[2]<<endl;
    }
    initial_EKF_xP(rtk);
    rtk->InitFlag_EKF = 0 ;
    EKFfilter(H,PH,Q,rtk,K);
    Measurement_Update(H,v,K,rtk);
    int P_idx1,P_idx2;
    for(int i = 0;i<(EKF_STATE_NUM+SVn);i++)
    {
        if(i > 3){P_idx1 = rtk->obs->prn[i-4]+3;}
        else {P_idx1 = i;}
        for(int k = 0;k<(EKF_STATE_NUM+SVn);k++)
        {
            if(k > 3){P_idx2 = rtk->obs->prn[k-4]+3;}
            else {P_idx2 = k;}
            P[k + (EKF_STATE_NUM+SVn)*i] = rtk->EKF_P[P_idx2 +(EKF_STATE_MAX)*P_idx1];
        }
    }
    matrix_matrix_product(K,H,(EKF_STATE_NUM+SVn),2*SVn,(EKF_STATE_NUM+SVn),KH); // KH = K * H
    for(int i = 0;i<(EKF_STATE_NUM+SVn);i++)
        for(int k = 0;k<(EKF_STATE_NUM+SVn);k++)
            I_KH[i+(EKF_STATE_NUM+SVn)*k] = i==k?(1.0-KH[i+(EKF_STATE_NUM+SVn)*k]):(-KH[i+(EKF_STATE_NUM+SVn)*k]); // I_KH = I - KH

    matrix_matrix_product(I_KH,P,(EKF_STATE_NUM+SVn),(EKF_STATE_NUM+SVn),(EKF_STATE_NUM+SVn),P_update); // P_update = (I - KH)*P
    for(int i = 0;i<(EKF_STATE_NUM+SVn);i++)
    {
        if(i > 3){P_idx1 = rtk->obs->prn[i-4]+3;}
        else {P_idx1 = i;}
        for(int k = 0;k<(EKF_STATE_NUM+SVn);k++)
        {
            if(k > 3){P_idx2 = rtk->obs->prn[k-4]+3;}
            else {P_idx2 = k;}
            rtk->EKF_P[P_idx2 + P_idx1*EKF_STATE_MAX] = P_update[k + i*(EKF_STATE_NUM+SVn)];
        }
    }
    for(int i = 0;i<4;i++)
    {
        rtk->Pos[i] = rtk->Pos[i] + rtk->EKF_x[i];
    }
}
void iono_free_LC(obs_t *obs,nav_t *nav,int SVn){
    double f1_sqaure = FREQL1*FREQL1;
    double f2_sqaure = FREQL2*FREQL2;
    double c1 = f1_sqaure/(f1_sqaure - f2_sqaure);
    double c2 = -f2_sqaure/(f1_sqaure - f2_sqaure);
    for(int i = 0;i<SVn;i++){
        obs->P[i][2]=c1 * obs->P[i][0] + c2 * obs->P[i][1];
        obs->L[i][2]=c1 * obs->L[i][0] * LAMBDA1 + c2 * obs->L[i][1] * LAMBDA2;
        obs->D[i][2]=c1 * obs->D[i][0] + c2 * obs->D[i][1];
    }
}
void EKFfilter(double * H, double * PH, double * Q, rtk_t*rtk, double * K){
    int SVn = rtk->obs->SVn;
    double P[(EKF_STATE_NUM+SVn)*(EKF_STATE_NUM+SVn)];
    double Ht[(2*SVn)*(EKF_STATE_NUM+SVn)];
    int P_idx1,P_idx2;

    int debug = 0;
    for(int i = 0;i<(EKF_STATE_NUM+SVn);i++)
    {
        if(i > 3){P_idx1 = rtk->obs->prn[i-4]+3;}
        else {P_idx1 = i;}
        for(int k = 0;k<(EKF_STATE_NUM+SVn);k++)
        {
            if(k > 3){P_idx2 = rtk->obs->prn[k-4]+3;}
            else {P_idx2 = k;}
            P[k + (EKF_STATE_NUM+SVn)*i] = rtk->EKF_P[P_idx2 +(EKF_STATE_MAX)*P_idx1];
            /*if(rtk->EKF_P[P_idx2 +(EKF_STATE_MAX)*P_idx1]>5000&&P_idx1==P_idx2)
            {cout<<k-4<<" New Sat : "<<P_idx1-3<<" P = "<<rtk->EKF_P[P_idx2 +(EKF_STATE_MAX)*P_idx1]<<endl;
            debug = 1;}*/
        }
    }
    /*if(debug)
    {
        cout<<"\nALL_P = "<<endl;
        for(int a = 0;a<(EKF_STATE_MAX);a++)
        {
            for(int b = 0;b<(EKF_STATE_MAX);b++)
            { cout <<setprecision(5)<<setw(4)<< rtk->EKF_P[b + (EKF_STATE_MAX)*a]<<"  ";
            }
            cout<<endl;
        }cout<<"end"<<endl;
    }*/
    /*if(debug)
    {
        cout<<"\nP = "<<endl;
        for(int a = 0;a<(EKF_STATE_NUM+SVn);a++)
        {
            for(int b = 0;b<(EKF_STATE_NUM+SVn);b++)
            { cout <<setprecision(2)<<setw(10)<< P[b + (EKF_STATE_NUM+SVn)*a]<<"  ";
            }
            cout<<endl;
        }cout<<"end"<<endl;
    }*/
    double Q_org[(2*SVn)*(2*SVn)];
    double temp[(2*SVn)*(2*SVn)];
    matrix_transpose(H,Ht,2*SVn,(EKF_STATE_NUM+SVn));
    /*Filter*/
    matrix_matrix_product(P,Ht,(EKF_STATE_NUM+SVn),(EKF_STATE_NUM+SVn),(2*SVn),PH); // PH = P * H'
    matrix_matrix_product(H,PH,(2*SVn),(EKF_STATE_NUM+SVn),(2*SVn),Q); // Q = H * P * H'
    for(int i = 0;i<(2*SVn);i++)
    {
        for(int k = 0;k<(2*SVn);k++)
        {Q_org[k + (2*SVn)*i] = Q[k + (2*SVn)*i]; }
    }

    // Q = Q + R;
    for(int i = 0;i<(2*SVn);i++)
    {
        for(int k = 0;k<(2*SVn);k++)
        {Q[k + (2*SVn)*i] += i==k?i%2==0?rtk->EKF_R[(rtk->obs->prn[i]-1)]:rtk->EKF_R[MAXGPS+(rtk->obs->prn[i]-1)]:0.0;}
    }

    if(int e = matinv(Q,(2*SVn)) != 0){cout<<"inverse error!! e = "<<e<<endl;system("pause");}// Q = Q^(-1)
    matrix_matrix_product(Q,Q_org,(2*SVn),(2*SVn),(2*SVn),temp);
    matrix_matrix_product(PH,Q,(EKF_STATE_NUM+SVn),(2*SVn),(2*SVn),K); // K = P * H' * Q^(-1)
    for(int i = 0;i<4;i++)
    {

        rtk->Pos[i] = rtk->Pos[i] + rtk->EKF_x[i];

    }
}
void Var_Update(rtk_t * rtk){


}
void Measurement_Update(double * H,double * v,double * K,rtk_t * rtk){
    int SVn = rtk->obs->SVn;
    double Kv[(EKF_STATE_NUM+SVn)];
    double KH[(EKF_STATE_NUM+SVn)*(EKF_STATE_NUM+SVn)],P_update[(EKF_STATE_NUM+SVn)*(EKF_STATE_NUM+SVn)],x_update[(EKF_STATE_NUM+SVn)];
    double I_KH[(EKF_STATE_NUM+SVn)*(EKF_STATE_NUM+SVn)];
    double P[(EKF_STATE_NUM+SVn)*(EKF_STATE_NUM+SVn)];
    int P_idx1,P_idx2;
    for(int i = 0;i<(EKF_STATE_NUM+SVn);i++)
    {
        if(i > 3){P_idx1 = rtk->obs->prn[i-4]+3;}
        else {P_idx1 = i;}
        for(int k = 0;k<(EKF_STATE_NUM+SVn);k++)
        {
            if(k > 3){P_idx2 = rtk->obs->prn[k-4]+3;}
            else {P_idx2 = k;}
            P[k + (EKF_STATE_NUM+SVn)*i] = rtk->EKF_P[P_idx2 +(EKF_STATE_MAX)*P_idx1];
        }
    }
    /*state update*/
    matrix_vector_product(K,v,(EKF_STATE_NUM+SVn),2*SVn,Kv);
    for(int i = 0;i<(EKF_STATE_NUM+SVn);i++){
        x_update[i]=Kv[i]; // x_update = x + K * v
    }
    for(int i = 0;i<(EKF_STATE_NUM+SVn);i++){
        rtk->EKF_x[i] = Kv[i]; //  x = x_update
    }
    /*corvariance update*/
    //    matrix_matrix_product(K,H,(EKF_STATE_NUM+SVn),2*SVn,(EKF_STATE_NUM+SVn),KH); // KH = K * H
    //    for(int i = 0;i<(EKF_STATE_NUM+SVn);i++)
    //        for(int k = 0;k<(EKF_STATE_NUM+SVn);k++)
    //            I_KH[i+(EKF_STATE_NUM+SVn)*k] = i==k?(1.0-KH[i+(EKF_STATE_NUM+SVn)*k]):(-KH[i+(EKF_STATE_NUM+SVn)*k]); // I_KH = I - KH
    //    matrix_matrix_product(I_KH,P,(EKF_STATE_NUM+SVn),(EKF_STATE_NUM+SVn),(EKF_STATE_NUM+SVn),P_update); // P_update = (I - KH)*P
    //    for(int i = 0;i<(EKF_STATE_NUM+SVn);i++)
    //        for(int k = 0;k<(EKF_STATE_NUM+SVn);k++)
    //            rtk->EKF_P[k + i*EKF_STATE_MAX] = P_update[k + i*(EKF_STATE_NUM+SVn)];






}
void Least_SQR(double * H, double * v,  rtk_t*rtk){
    int SVn = rtk->obs->SVn;
    double h[SVn*4];
    double ht[SVn*4];
    double V[SVn];
    double Ht[(2*SVn)*(EKF_STATE_NUM+SVn)];
    double Q[(EKF_STATE_NUM)*(EKF_STATE_NUM)];//[(EKF_STATE_NUM+SVn)*(EKF_STATE_NUM+SVn)];
    double Q_[(EKF_STATE_NUM)*(EKF_STATE_NUM)];
    double temp[(EKF_STATE_NUM)*(EKF_STATE_NUM)];
    double LS[(SVn)*(EKF_STATE_NUM)];
    double corr[4];

    for(int i = 0;i<SVn;i++)
    {
        for(int k = 0;k<4;k++)
        {
            h[k+4*i]=H[k+(EKF_STATE_NUM+SVn)*(i*2+1)];
        }
    }
    for(int i = 0;i<SVn;i++)
    {
        V[i] = v[2*i+1];
    }

    matrix_transpose(h,ht,SVn,4);

//    cout<<"h = "<<endl;
//    for(int i = 0;i<SVn;i++)
//    {
//        for(int k = 0;k<4;k++)
//        {
//            cout <<setprecision(10)<<setw(10)<<h[k+4*i]<<" ";
//        }      cout<<endl;
//    }
//    cout<<"end"<<endl;
//    cout<<"ht = "<<endl;
//    for(int i = 0;i<4;i++)
//    {
//        for(int k = 0;k<SVn;k++)
//        {
//            cout <<setprecision(3)<<setw(10)<<ht[k + (SVn)*i]<<" ";
//        }      cout<<endl;
//    }
//    cout<<"end"<<endl;
    matrix_matrix_product(ht,h,4,(SVn),4,Q);
    for(int i = 0;i<4;i++)
    {
        for(int k = 0;k<4;k++)
        {
            Q_[k + (EKF_STATE_NUM)*i] = 0.0;
            Q_[k + (EKF_STATE_NUM)*i] = Q[k + (EKF_STATE_NUM)*i];
        }
    }

    if(matinv(Q,4))cout<<"inv error"<<endl;
    matrix_matrix_product(Q,ht,4,4,SVn,LS);

//    cout<<"LS = "<<endl;
//    for(int i = 0;i<(EKF_STATE_NUM);i++)
//    {
//        for(int k = 0;k<SVn;k++)
//        {
//            cout <<setprecision(10)<<setw(10)<<LS[k + SVn*i]<<" ";
//        }      cout<<endl;
//    }
//    cout<<"end"<<endl;

    matrix_vector_product(LS,V,4,SVn,corr);
//    cout <<"corr = ";
//    for(int i = 0;i<(EKF_STATE_NUM);i++){
//        cout << corr[i]<<"\t";
//    }cout<<"end"<<endl;

    for(int i = 0;i<(EKF_STATE_NUM);i++){
        rtk->EKF_x[i] = corr[i]; //  x = x_update
    }
}
double SAAS_tropos_model(double el){

    double h = 70.0;
    double p = 1013.25*pow((1-2.2577*h/100000),5.2568);
    double T = 15.0 - 0.0065*h+273.16;
    double e = 6.108*0.7*exp((17.15*T-4684.0)/(T - 38.45));
    return (0.002277/cos(PI/2-el))*(p+(1255.0/T+0.05)*pow(tan(PI/2-el),2)) + 0.002277*(1255.0/T+0.5*e/cos(PI/2-el));


}
