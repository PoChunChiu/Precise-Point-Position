#include "nav.h"

nav_t::nav_t()
{

    leaps= 0;          /* leap seconds (s) */
    for(int i = 0;i<SVn_max;i++){
        weekoftime[i] = 0;
        prn [i] = 0;
        iodc [i] = 0;
        sva [i] = 0;
        svh [i] = 0;
        week [i] = 0;
        code [i] = 0;
        flag [i] = 0;
        toc [i] = 0;
        roota [i] = 0.0;
        e[i] = 0.0;
        i0[i] = 0.0;
        OMG0 [i] = 0.0;
        omg [i] = 0.0;
        M0 [i] = 0.0;
        dn [i] = 0.0;
        OMGd [i] = 0.0;
        idot [i] = 0.0;
        crc [i] = 0.0;
        crs [i] = 0.0;
        cuc [i] = 0.0;
        cus [i] = 0.0;
        cic [i] = 0.0;
        cis [i] = 0.0;
        toe [i] = 0.0;   /* Toe (s) in week */
        af0 [i] = 0.0;
        af1 [i] = 0.0;
        af2 [i] = 0.0;    /* SV clock parameters (af0,af1,af2) */
        tgd [i] = 0.0;      /* TGD */
        leaps= 0;          /* leap seconds (s) */
        //eph_t eph[MAXSAT];
        //eph_t temp_eph;
        Health [i] = 1;
    }


}
