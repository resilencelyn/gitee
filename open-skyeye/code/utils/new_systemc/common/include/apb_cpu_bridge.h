
#ifndef __APB_CPU_BRIDGE_H__ 
#define __APB_CPU_BRIDGE_H__

#include "tlm.h"                          		        // TLM headers
#include "tlm_utils/peq_with_get.h"                   // Payload event queue FIFO
#include "memory.h"                                   // memory storage
#include "systemc.h"

class apb_cpu_bridge                               /// apb_cpu_bridge
:         public sc_core::sc_module           	      /// inherit from SC module base clase
, virtual public tlm::tlm_fw_transport_if<>   	      /// inherit from TLM "forward interface"
{
// Member Methods =====================================================
	
  public:	  
//=====================================================================
///	@fn at_initiator_1_phase
///
///	@brief Constructor for Single Phase AT target
///
///	@details
///		Generic Single Phase target used in several examples.
///		Constructor offers several parameters for customization	
///
//=====================================================================
  
  
  unsigned int  addr;
  unsigned int  wdata;
  unsigned int  rdata;
  
	sc_in	<bool>					presetn;
    sc_in	<bool>					pclk;//124M clk
    sc_out	<sc_uint<17> >			paddr;
    sc_signal	<sc_uint<17> >			s_paddr;
	sc_out	<bool>					pwrite;
	sc_out	<bool>					penable;
	sc_out	<sc_uint<16> >			pwdata;//15:0
	sc_in	<sc_uint<16> >			prdata;
  
  void  apb_write();
  void  apb_read();
  void  apb_penable();
  
  bool              apb_write_en;
  unsigned int      apb_penable_en;
  sc_event          apb_penable_req;
  sc_event          apb_write_req;
  sc_event          apb_write_done;
  bool              apb_read_en;
  bool              apb_read_wait;
  sc_event          apb_read_req;
  sc_event          apb_read_done;
  
  apb_cpu_bridge                           
  ( sc_core::sc_module_name   module_name           ///< SC module name
  , const unsigned int        ID                    ///< target ID
  , const char                *memory_socket        ///< socket name
  , sc_dt::uint64             memory_size           ///< memory size (bytes)
  , unsigned int              memory_width          ///< memory width (bytes)
  , const sc_core::sc_time    accept_delay          ///< accept delay (SC_TIME, SC_NS)
  , const sc_core::sc_time    read_response_delay   ///< read response delay (SC_TIME, SC_NS)
  , const sc_core::sc_time    write_response_delay  ///< write response delay (SC_TIME, SC_NS)
  );

//=====================================================================
///	@brief Implementation of call from Initiator. 
//
///	@details
///		This is the ultimate destination of the nb_transport_fw call from
///		the initiator after being routed trough a Bus
//
//===================================================================== 
  
  tlm::tlm_sync_enum                                // sync status
  nb_transport_fw                    
  ( tlm::tlm_generic_payload &gp                    ///< generic payoad pointer
  , tlm::tlm_phase           &phase                 ///< transaction phase
  , sc_core::sc_time         &delay_time            ///< time taken for transport
  );
 
  //=====================================================================
  ///  @fn apb_cpu_bridge::begin_response_method
  /// 
  ///  @brief Response Processing
  ///
  ///  @details
  ///    This routine takes transaction responses from the m_response_PEQ.
  ///    It contains the state machine to manage the communication path
  ///    back to the initiator. This method is registered as an SC_METHOD
  ///    with the SystemC kernal and is sensitive to m_response_PEQ.get_event() 
  //=====================================================================

   void
   begin_response_method                             
   ( void
   );

  private:
	    
/// helper function for printing memory transactions
  void
  print_memory_transaction_helper
  ( const int 				  &ID
  , std::ostringstream  	  &partial_msg
  , tlm::tlm_generic_payload  &trans
  );
 
//==============================================================================
// Methods not Implemented for this Example
  
/// b_transport() - Blocking Transport
  void                                                // returns nothing 
  b_transport
  ( tlm::tlm_generic_payload  &payload                // ref to payload 
  , sc_core::sc_time          &delay_time             // delay time 
  );


/// Not implemented for this example but required by interface
  bool                                              // success / failure
  get_direct_mem_ptr                       
  ( tlm::tlm_generic_payload   &payload,            // address + extensions
    tlm::tlm_dmi               &dmi_data            // DMI data
  );

  
/// Not implemented for this example but required by interface
  unsigned int                                      // result
  transport_dbg                            
  ( tlm::tlm_generic_payload  &payload              // debug payload
  );

// Member Variables ===================================================
  
  public:  
	  
  typedef tlm::tlm_generic_payload  *gp_ptr;		///< generic payload pointer
  
  tlm::tlm_target_socket<>  m_memory_socket;        ///<  target socket

  private:
	  
  const unsigned int        m_ID;                   ///< target ID
        sc_dt::uint64       m_memory_size;          ///< memory size (bytes)
        unsigned int        m_memory_width;         ///< word size (bytes)
  const sc_core::sc_time    m_accept_delay;         ///< accept delay
  const sc_core::sc_time    m_read_response_delay;  ///< read response delay
  const sc_core::sc_time    m_write_response_delay; ///< write response delays
        unsigned long       m_request_count;        ///< used to calc synch transactions  
        bool                m_nb_trans_fw_prev_warning;
        bool                m_begin_resp_method_prev_warning;
        bool                m_trans_dbg_prev_warning;
        bool                m_get_dm_ptr_prev_warning;
        tlm_utils::peq_with_get<tlm::tlm_generic_payload>
                            m_response_PEQ;         ///< response payload event queue
        // memory              m_target_memory;
        sc_core::sc_event   m_end_resp_rcvd_event;
};

#endif /* __APB_CPU_BRIDGE_H__ */
