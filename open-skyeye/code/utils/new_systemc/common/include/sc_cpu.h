#ifndef __SC_CPU_H__
#define __SC_CPU_H__

#include "systemc.h"
#include "sim_control.h"

SC_MODULE(sc_cpu)
{


	void cpu_call();


	SC_CTOR(sc_cpu)
	{
		SC_THREAD(cpu_call);
	}
};

#endif
