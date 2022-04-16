#include    "apb_cpu_bridge_test_device.h"
#include "reporting.h"


#undef  REPORT_DEFINE_GLOBALS
void apb_cpu_bridge_test_device::apb_cpu_bridge_test_device_signal()
{
    std::ostringstream  msg;
    
	while(1)
	{
		wait();
		//if(penable){
		//	prdata.write(paddr.read().to_uint());
		//}
		if(penable||penable_d)
		{
			prdata.write(paddr.read().to_uint());
			printf("%s:%d, %x\n", __func__, __LINE__, paddr.read().to_uint());
		}

		penable_d.write(penable);
	}
}
