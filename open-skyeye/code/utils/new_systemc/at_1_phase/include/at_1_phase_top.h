
#ifndef __EXAMPLE_SYSTEM_TOP_H__
#define __EXAMPLE_SYSTEM_TOP_H__

#include "apb_cpu_bridge_test_device.h"              
#include "reporting.h"                        // common reporting code
// #include "at_target_1_phase.h"                // at memory target
#include "apb_cpu_bridge.h"                // at memory target
#include "initiator_top.h"                    // processor abstraction initiator
#include "models/SimpleBusAT.h"               // Bus/Router Implementation
#include "systemc.h"               // Bus/Router Implementation

#include "sc_cpu.h"

/// Top wrapper Module
class example_system_top             
: public sc_core::sc_module                   //  SC base class
{
public:
	
/// Constructor
	
	sc_signal	<sc_uint<17> >			paddr;
	sc_signal	<bool >     			presetn;
	sc_in   	<bool >     			pclk;
	sc_signal	<bool >     			pwrite;
	sc_signal	<bool >     			penable;
	sc_signal	<sc_uint<16> >			pwdata;
	sc_signal	<sc_uint<16> >			prdata;
    
	// sc_set_time_resolution(1,SC_PS);
	
	// sc_clock pclk("pclk",1500,SC_PS);//4032 SC_PS
    
  	initiator_top           m_initiator_1;          ///< instance 1 initiator

  example_system_top              
  ( sc_core::sc_module_name name); 


//Member Variables  ===========================================================
  private:

  SimpleBusAT<1, 1>       m_bus;                  ///< simple bus
  apb_cpu_bridge       m_at_target_1_phase_1;  ///< instance 1 target
  // at_target_1_phase       m_at_target_1_phase_1;  ///< instance 1 target

  apb_cpu_bridge_test_device    m_apb_cpu_bridge_test_device;
  sc_cpu                  sc_cpu_1;
};

#endif /* __EXAMPLE_SYSTEM_TOP_H__ */
