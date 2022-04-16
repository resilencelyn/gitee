#ifndef __SC_SKYEYE_ARM_H__
#define __SC_SKYEYE_ARM_H__

#include <systemc.h>
#include "sim_control.h"

SC_MODULE(sc_skyeye_arm)
{
    //Lt_transform *trans_ptr;

    //SC_HAS_PROCESS(sc_skyeye_arm);

    void cpu_call();

    //sc_skyeye_arm(sc_core::sc_module_name module_name):
    SC_CTOR(sc_skyeye_arm)
    {

        //trans_ptr = new Lt_transform("lt_transform");
        //transform_ptr = trans_ptr;

        SC_THREAD(cpu_call);
    }
};
#endif
