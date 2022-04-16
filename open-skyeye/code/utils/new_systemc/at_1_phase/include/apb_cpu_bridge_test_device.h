
#ifndef _APB_CPU_BRIDGE_TEST_DEVICE_H
#define _APB_CPU_BRIDGE_TEST_DEVICE_H

//#include "tlm.h"
#include "systemc.h"
#include "reporting.h"                                // reporting macros

//static const char *filename = "apb_cpu_bridge_test_device.h";	///< filename for reporting

SC_MODULE	(apb_cpu_bridge_test_device){
	sc_in	<bool>					presetn;
	sc_in	<bool>					pclk;//124M clk
	sc_in	<sc_uint<17> >			paddr;//6:0
	sc_in	<bool>					pwrite;
	sc_in	<bool>					penable;
	sc_in	<sc_uint<16> >			pwdata;//15:0
	sc_out	<sc_uint<16> >			prdata;
	
	sc_signal	<bool>			penable_d;
	sc_signal	<sc_uint<64> >	clk_cnt;
	
	void apb_cpu_bridge_test_device_signal();

	
	SC_CTOR	(apb_cpu_bridge_test_device):
	// nrst		("nrst"),
	// clk			("clk"),
	presetn		("presetn"),
	pclk		("pclk"),
	paddr		("paddr"),
	// psel			("psel"),
	pwrite		("pwrite"),
	penable		("penable"),
	pwdata		("pwdata"),
	prdata		("prdata")
	{
		SC_THREAD(apb_cpu_bridge_test_device_signal);
		sensitive	<<pclk.pos();
		// sensitive	<<presetn.neg();
	}
};




 
#endif
