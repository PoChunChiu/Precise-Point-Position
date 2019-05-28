#include "rtklib.h"
#include "clas.h"

int main()
{
    FILE *L6_f;
    L6_f = fopen("2017324A.l6","rb+");
    decode_L6(L6_f,&Cd);
    fclose(L6_f);

    double x = Bin2Double(1,16383,0.02,15);
    printf("%10f",x);




    rtk_t* rtk = new rtk_t();
    ifstream in_fo,in_fn,in_fp;
    ofstream out_f("pos_result.txt");
    in_fo.open("Base_1128.18O");
    in_fn.open("Base_1128.18N");
    in_fp.open("nga20293");
    int epoch = 1;
    //cout << "init : " << rtk->Pos[0] <<"  "<<rtk->Pos[1]<<"  "<<rtk->Pos[2]<<endl;
    while(1){

        if(rtk->read_data(in_fo,in_fn,in_fp)){break;}
        rtk->calc_satpos();
        // Prepare EKF state "x"
        initial_EKF_xP(rtk);
        // Precise Point Position
        pppos(rtk);



        if(epoch > 0){
            out_f <<rtk->obs->SVn<<"  ";
        }
        out_f<<fixed<<setprecision(5)<<rtk->obs->time_in_sec[0]<<"\t"<<setprecision(10)<<rtk->Pos[0]<<"\t"<<rtk->Pos[1]<<"\t"<<rtk->Pos[2]<<endl;
        cout << rtk->Pos[0] <<"  "<<rtk->Pos[1]<<"  "<<rtk->Pos[2]<<"  "<<rtk->Pos[3]<<endl;
        epoch++;
    }
    cout << "Done!!"<<endl;
    for(int i = 0;i<4;i++)
        cout << setw(10)<<setprecision(10)<<rtk->Pos[i]<<"\t";
    cout << endl;
    in_fn.close();
    in_fo.close();
    out_f.close();

    return 0;
}
