
#include "at_1_phase_top.h"           	// example system top header

example_system_top::example_system_top  

( sc_core::sc_module_name name             
)
  : sc_core::sc_module                      /// Init SC base
    ( name                                 
    )
  , m_bus                                   /// Init Simple Bus
    ( "m_bus"                              
    )
  , m_at_target_1_phase_1                   /// Init intance 1 of AT target
    ( "m_at_target_1_phase_1"               // module name
    , 201                                   /// 1st Target ID is 201
    , "memory_socket_1"                     // socket name
    , 4*1024                                // memory size (bytes)
    , 4                                     // memory width (bytes)
    , sc_core::sc_time(10, sc_core::SC_NS)  // accept delay
    , sc_core::sc_time(50, sc_core::SC_NS)  // read response delay
    , sc_core::sc_time(30, sc_core::SC_NS)  // write response delay
    )
  , m_initiator_1                           /// Init Instance 1 of AT initiator
    ( "m_initiator_1"                       // module name
    , 101                                   /// 1st Initiator ID is 101
    , 0x0000000000000100                    // fitst base address
    // , 0x0000000010000100                    // second base address
    , 0x0000000000000100                    // second base address
    , 2                                     // active transactions
    )
    ,m_apb_cpu_bridge_test_device("m_apb_cpu_bridge_test_device")
    ,sc_cpu_1("cpu")
{
  /// bind TLM2 initiators to TLM2 target sockets on SimpleBus
  m_initiator_1.initiator_socket(m_bus.target_socket[0]);

  /// bind TLM2 targets to TLM2 initiator sockets on SimpleBus
  m_bus.initiator_socket[0](m_at_target_1_phase_1.m_memory_socket);

	// sc_clock pclk("pclk",1500,SC_PS);//4032 SC_PS
    
	m_at_target_1_phase_1.presetn(presetn);
	m_at_target_1_phase_1.pclk(pclk);
	m_at_target_1_phase_1.paddr(paddr);
	m_at_target_1_phase_1.pwrite(pwrite);
	m_at_target_1_phase_1.penable(penable);
	m_at_target_1_phase_1.pwdata(pwdata);
	m_at_target_1_phase_1.prdata(prdata);
    
    m_apb_cpu_bridge_test_device.presetn(presetn);
    m_apb_cpu_bridge_test_device.pclk(pclk);
    m_apb_cpu_bridge_test_device.paddr(paddr);
    m_apb_cpu_bridge_test_device.pwrite(pwrite);
    m_apb_cpu_bridge_test_device.penable(penable);
    m_apb_cpu_bridge_test_device.pwdata(pwdata);
    m_apb_cpu_bridge_test_device.prdata(prdata);

    
}

