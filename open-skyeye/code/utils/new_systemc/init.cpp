#include "skyeye_class.h"
#include "skyeye_types.h"

#include "at_1_phase_top.h"             // top module
#include "tlm.h"                        // TLM header

extern void init_skyeye_device();
example_system_top *top_ptr;

void init_skyeye_class()
{
    init_skyeye_device();
}

int top_interrupt(int line)
{

}

int top_write(unsigned target, unsigned offset, void *buf, unsigned len)
{
    top_ptr->m_initiator_1.m_traffic_gen.traffic_send(target, offset, buf, len);
    return 0;
}

int top_read(unsigned target, unsigned offset, void *buf, unsigned len)
{
    top_ptr->m_initiator_1.m_traffic_gen.traffic_receive(target, offset, buf, len);
    return 0;
}

void setup_systemc()
{
    top_ptr = new example_system_top("top");    // instantiate a exmaple top module

    sc_clock pclk("pclk", 1500, SC_PS); //4032 SC_PS

    top_ptr->pclk(pclk);
    sc_core::sc_start();
    return;
}
