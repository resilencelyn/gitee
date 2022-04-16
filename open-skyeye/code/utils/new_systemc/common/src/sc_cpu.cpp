#include "sc_cpu.h"
#include "systemc_exec.h"
#include "sim_control.h"

void sc_cpu::cpu_call()
{
	while(1){
		exec_post_one_cycle();
	}

}

