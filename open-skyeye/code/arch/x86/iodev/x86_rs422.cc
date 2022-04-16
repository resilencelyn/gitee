#define BX_PLUGGABLE

#include "iodev.h"

#if BX_SUPPORT_RS422 


#include <stdio.h>
#include <skyeye_config.h>
#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_exec.h>
#include <skyeye_cell.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_iface.h>
#include <skyeye_dev_bus_intf.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_core.h>
#include "skyeye_uart.h"
#include "skyeye_thread.h"
#include <skyeye_emif.h>
#include <skyeye_data_intf.h>

#include "cpu/cpu.h"
#include "x86_rs422_fifo.h"
#include "x86_rs422.h"
#include "virt_timer.h"

bx_rs422_c *rs422Device = NULL;
#define LOG_THIS rs422Device->
#define BX_RS422_THIS this->

#define RAISE_SIGNAL 1
#define LOWER_SIGNAL 0
/* CRC余式表 */
unsigned short usA_CrcTab[256] = 
{ 
        0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
        0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
        0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
        0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
        0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
        0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
        0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
        0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
        0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
        0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
        0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
        0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
        0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
        0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
        0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
        0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
        0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
        0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
        0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
        0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
        0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
        0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
        0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
        0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
        0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
        0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
        0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
        0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
        0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
        0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
        0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
        0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
} ;

/* 计算CRC校验和 */
unsigned short CalCRC(unsigned char *ucData_ptr, unsigned int uiLen){
	unsigned short usCrc = 0 ;
    	unsigned char ucTemp ;

    	if( NULL != ucData_ptr )
    	{
        	while(  0 != uiLen ) 
        	{
            		uiLen -- ;
            		ucTemp = (unsigned char) ( usCrc/256 ) ; /* 以8位二进制数的形式暂存CRC的高8位 */
            		usCrc <<= 8 ; /* 左移8位*/
            		usCrc ^= usA_CrcTab[ ucTemp ^ (*ucData_ptr) ]; /* 高8位和当前字节相加后再查表求CRC ，再加上以前的CRC */
            		ucData_ptr ++ ;
        	}
    	}
    	else
    	{
        	;
    	}

    	return( usCrc ) ;
}

static exception_t x86_rs422_read(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count);
static exception_t x86_rs422_write(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count);

int librs422_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  rs422Device = new bx_rs422_c();
  bx_devices.pluginrs422 = rs422Device;
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, rs422Device, BX_PLUGIN_RS422);

  return(0); // Success
}

void librs422_LTX_plugin_fini(void)
{
  delete rs422Device;
}

struct bx_rs422_s_type {
  Bit16u rs422_registers[8]; /* different rs422 value */
} bx_rs422_s;

bx_rs422_c::bx_rs422_c()
{
	put("IODBG");
}

void bx_rs422_c::init(void)
{
	DEV_register_ioread_handler_range(this, read_handler, RS422_BASE, RS422_BASE + RS422_LEN, "BOCHS RS422", 4);
	DEV_register_iowrite_handler_range(this, write_handler,RS422_BASE, RS422_BASE + RS422_LEN, "BOCHS RS422", 4);
}

Bit32u bx_rs422_c::read_handler(void *this_ptr, Bit32u addr, unsigned io_len)
{
	bx_rs422_c *bx_rs422_ptr = (bx_rs422_c *) this_ptr;
	return bx_rs422_ptr->read(addr, io_len);
}

Bit32u bx_rs422_c::read(Bit32u addr, unsigned io_len)
{
	Bit32u data = 0;
	x86_rs422_read((conf_object_t*)rs422Device->obj, addr - RS422_BASE, &data, io_len);
	return data;
}

void bx_rs422_c::write_handler(void *this_ptr, Bit32u addr, Bit32u dvalue, unsigned io_len)
{
	bx_rs422_c *bx_rs422_ptr = (bx_rs422_c *) this_ptr;
	bx_rs422_ptr->write(addr, dvalue, io_len);
}

void bx_rs422_c::write(Bit32u addr, Bit32u dvalue, unsigned io_len)
{
	conf_object_t *obj = rs422Device->obj;
	x86_rs422_write(rs422Device->obj, addr - RS422_BASE, &dvalue, io_len);
	
	return;
}

void bx_rs422_c::mem_read(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, void *data){
	return;
}

void bx_rs422_c::mem_write(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, void *data){
}

static void rs422_set_signal(conf_object_t *obj, int level)
{
    x86_rs422_dev *dev = obj->obj;
    	DEV_pic_lower_irq(dev->interrupt_number);
    	DEV_pic_raise_irq(dev->interrupt_number);
    /*
    if (level == RAISE_SIGNAL)
    	DEV_pic_raise_irq(dev->interrupt_number);
    else 
    	DEV_pic_lower_irq(dev->interrupt_number);
	*/
}

#if 1
int recv_rs422_cnt = 0;
exception_t x86_rs422_recv_data(conf_object_t* conf_obj, char *buf, int Length)
{
    x86_rs422_dev *dev = conf_obj->obj;
	x86_rs422_reg_t *regs = dev->regs;
	int i, idx;
	unsigned short usCrc;

	/* 接收串口通道号和本地串口通道一致则开始接收 */
	for (i = 0; i < dev->id_cnt;i++){
		if (dev->Rs422_ID[i] == buf[0]){
			x86_rs422_msg_t rs422_msg;

			idx = dev->Rs422_ID[i] & 0xF; //串口通道号为Rs422_ID的低四位
			rs422_msg.com_id = idx;
			rs422_msg.length = Length+5;

			if (rs422_msg.length > RECV_RS422_MSG_LENGTH){
				printf("The Rs422 Msg length = %d above the length range.\n", 
					rs422_msg.length);
				return 1;
			}else {
				rs422_msg.uc_buf[0] = 0xaa; //帧头
				rs422_msg.uc_buf[1] = 0x51; //帧头
				rs422_msg.uc_buf[2] = Length; //接收数据长度

				memcpy(&rs422_msg.uc_buf[3], &buf[1], Length); //有效数据内容
				usCrc = CalCRC(&rs422_msg.uc_buf[2], Length + 1); //CRC16校验值
				rs422_msg.uc_buf[Length + 3] = usCrc % 256; //帧尾
				rs422_msg.uc_buf[Length + 4] = usCrc / 256; //帧尾
		
				WriteRs422FIFO(dev->recv_fifo, &rs422_msg, 1);
			}
		}
	}
#if 0
	printf("%s Length:%d, Rs422_ID:0x%x, Data:\n", __func__, Length, dev->Rs422_ID);
	for(i=1;i<Length+1;i++){
		printf(" %x ", buf[i]);
	}
	printf("\n----------------------\n");
#endif
	return No_exp;
}
#endif

static void rs422_msg_recv_callback(conf_object_t *opaque){
	x86_rs422_dev *dev = opaque->obj;
	x86_rs422_reg_t *regs = dev->regs;
	x86_rs422_msg_t rs422_msg;
	int idx = 0;

	if(FIFOCheckRs422ReadNum(dev->recv_fifo) == 0){
		return;
	}

	ReadRs422FIFO(dev->recv_fifo, &rs422_msg, 1);

	idx = rs422_msg.com_id;
	regs->com_id = idx;

	regs->com_fifo[idx].recv_length = rs422_msg.length;
	memcpy(regs->com_fifo[idx].recv_buf, rs422_msg.uc_buf, rs422_msg.length);

	regs->com_fifo[idx].recv_status = 1; //接收完毕
	regs->com_fifo[idx].int_status = 1;//中断标志位
	rs422_set_signal(opaque, RAISE_SIGNAL);
}

static exception_t x86_rs422_read(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count)
{
	x86_rs422_dev *dev = opaque->obj;
	x86_rs422_reg_t *regs = dev->regs;
	int card_no, channel_no, length, idx, recv_cnt;
	uint32_t value;

	if (offset >= COM_HANDLE_START && offset <= COM_HANDLE_END){
		card_no = (offset - COM_HANDLE_START) / 16;
		channel_no = ((offset - COM_HANDLE_START) / 4) % 4;
		
		*(char *)buf = dev->comHandle[card_no][channel_no];
		return No_exp;
	}
	idx = regs->com_id;
	switch (offset)
	{
		case COM_ID:
			*(uint32_t *)buf = regs->com_id;	
			break;
		case SEND_LENGTH:
			*(uint32_t *)buf = regs->com_fifo[idx].send_length;	
			break;
		case RECV_LENGTH:
			*(uint32_t *)buf = regs->com_fifo[idx].recv_length;	
			break;
		case SEND_STATUS:
			*(uint32_t *)buf = regs->com_fifo[idx].send_status;	
			break;
		case RECV_STATUS:
			*(uint32_t *)buf = regs->com_fifo[idx].recv_status;	
			break;
		case COM_RX_FIFO:
			recv_cnt = regs->com_fifo[idx].recv_cnt;
			*(uint32_t *)buf = regs->com_fifo[idx].recv_buf[recv_cnt];
		        regs->com_fifo[idx].recv_cnt ++;	
			break;
		case INT_STATUS:
			*(uint32_t *)buf = regs->com_fifo[idx].int_status;
			break;
		default: {
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in rs422_c6748\n", offset);
			break;
		}
	}
	return  No_exp;
}

int my_cnt = 0;
static exception_t x86_rs422_write(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count)
{
	x86_rs422_dev *dev = opaque->obj;
	x86_rs422_reg_t *regs = dev->regs;
	uint32_t val = *(uint32_t *)buf;
	int idx, send_cnt, length;
	char send_buffer[COM_FIFO_SIZE] = {0};

	int i;
	idx = regs->com_id;
	switch (offset) {
		case COM_ID:
			regs->com_id = val;	
			break;
		case SEND_LENGTH:
			regs->com_fifo[idx].send_length = val;	
			break;
		case RECV_LENGTH:
			regs->com_fifo[idx].recv_length = val;	
			break;
		case SEND_STATUS:
			regs->com_fifo[idx].send_status = val;	
			break;
		case RECV_STATUS:
			regs->com_fifo[idx].recv_status = val;	
			break;
		case COM_CMD:
			regs->com_fifo[idx].com_cmd = val;
			if (regs->com_fifo[idx].com_cmd == 1) {//发送命令
				if (dev->bus_iface != NULL){
                			length = regs->com_fifo[idx].send_length;
					
                			send_buffer[0] = dev->Rs422_ID[idx]; //本地串口通道号
                			memcpy(&send_buffer[1], regs->com_fifo[idx].send_buf, length);
					
					dev->bus_iface->write_data(dev->bus_obj, opaque, send_buffer, length);
					regs->com_fifo[idx].send_cnt = 0;
				}
			}else if (regs->com_fifo[idx].com_cmd == 2){ //接收命令
				regs->com_fifo[idx].recv_status = 0;
				regs->com_fifo[idx].int_status = 0;//清中断
			//	rs422_set_signal(opaque, LOWER_SIGNAL);
				regs->com_fifo[idx].recv_cnt = 0;
			}else {
			}
			break;
		case BAUD:
			regs->config_data.m_Baud = val;
			break;
		case STOPBIT:
			regs->config_data.m_StopBits = val;
			break;
		case DATALEN:
			regs->config_data.m_DataLen = val;
			break;
		case PARITY:
			regs->config_data.m_ParitySet = val;
			break;
		case INTPOLL:
			regs->config_data.m_IntPoll = val;
			break;
		case COM_TX_FIFO:
			send_cnt = regs->com_fifo[idx].send_cnt;
			regs->com_fifo[idx].send_buf[send_cnt] = val;
		        regs->com_fifo[idx].send_cnt ++;	
			break;
		case INT_STATUS:
			regs->com_fifo[idx].int_status = val;
			break;
		case INT_CLR_REG:
			regs->com_fifo[idx].int_status = val;
			break;
		default: {
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in rs422_c6748\n", offset);
			break;
		}
	}
	return  No_exp;
}

static conf_object_t* new_x86_rs422(char* obj_name)
{
	x86_rs422_dev* dev = skyeye_mm_zero(sizeof(x86_rs422_dev));
	int i,j;
	if(dev == NULL)
		printf("alloc memmory for obj in %x fail\n", obj_name);
	x86_rs422_reg_t* regs =  skyeye_mm_zero(sizeof(x86_rs422_reg_t));
	dev->regs = regs;

	for(i=0;i<CARD_NUM;i++){
		for(j=0;j<CHANNEL_NUM;j++){
			dev->comHandle[i][j] = i * CHANNEL_NUM + j;
		}
	}

	for(i=0;i<CARD_NUM * CHANNEL_NUM;i++){
		regs->com_fifo[i].send_buf = skyeye_mm_zero(COM_FIFO_SIZE);
		regs->com_fifo[i].recv_buf = skyeye_mm_zero(COM_FIFO_SIZE);
		regs->com_fifo[i].send_cnt = 0;
		regs->com_fifo[i].recv_cnt = 0;
		regs->com_fifo[i].send_length = 0;
		regs->com_fifo[i].recv_length = 0;
		regs->com_fifo[i].send_status = 0;
		regs->com_fifo[i].recv_status = 0;
		regs->com_fifo[i].com_cmd = 0;
		regs->com_fifo[i].int_status = 0;
	}

	dev->obj = new_conf_object(obj_name, dev);
	rs422Device->obj = dev->obj;
	dev->recv_fifo = CreateRs422FIFO(1000);

	int clock_mode = SIM->get_param_enum(BXPN_CLOCK_SYNC)->get();
	bool is_realtime = (clock_mode == BX_CLOCK_SYNC_REALTIME) ||
      		(clock_mode == BX_CLOCK_SYNC_BOTH);
	bx_virt_timer.register_timer(dev->obj, rs422_msg_recv_callback, 1000, 1, 1, is_realtime, "x86_rs422");

	return dev->obj;
}

static exception_t reset_x86_rs422(conf_object_t* opaque, const char* args)
{
	x86_rs422_dev *dev = opaque->obj;

	return No_exp;        
}

static exception_t free_x86_rs422(conf_object_t* conf_obj)
{
	x86_rs422_dev *dev = conf_obj->obj;
	FreeRs422FIFO(dev->recv_fifo);
	skyeye_free(dev->obj);
	skyeye_free(dev);
	return No_exp;
}

char* x86_rs422_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	x86_rs422_dev *dev = conf_obj->obj;
	return regs_name[id];
}

uint32_t x86_rs422_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id)
{
	x86_rs422_dev *dev = conf_obj->obj;
	uint32_t* regs_value = (uint32_t*)&(dev->regs) + id;
	return *regs_value;
}

static char* x86_rs422_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	x86_rs422_dev *dev = conf_obj->obj;
	x86_rs422_reg_t* regs = dev->regs;
	*((uint32_t*)regs + id) = value;

	return NULL;
}

static uint32_t x86_rs422_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	x86_rs422_dev *dev = conf_obj->obj;
	x86_rs422_reg_t* regs = dev->regs;
	return  17;
}

static uint32_t x86_rs422_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	x86_rs422_dev *dev = conf_obj->obj;
	x86_rs422_reg_t* regs = dev->regs;
	uint32_t regnum;
	regnum = sizeof(x86_rs422_reg_t) / 4;;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static exception_t x86_rs422_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	x86_rs422_dev *dev = obj->obj;
	dev->bus_obj = obj2;
	dev->bus_iface = (Rs422_bus_intf*)SKY_get_iface(dev->bus_obj, RS422_BUS_INTF);

	if (dev->bus_iface == NULL){
		skyeye_log(Error_log, __FUNCTION__, "Get 'rs422_bus_iface' interface from %s fail.", obj2->objname);
	}
	if (-1 == dev->bus_iface->register_device(obj2, obj)){
		printf("In %s, Register x86_rs422 device Error.\n", __FUNCTION__);
	}
	return No_exp;

}
 
static exception_t x86_rs422_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	x86_rs422_dev *dev = obj->obj;
	*obj2 = dev->bus_obj;
	*port = NULL;
	return No_exp;
}

static attr_value_t get_attr_Rs422_ID(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	x86_rs422_dev *dev = conf_obj->obj;
	attr_value_t Rs422_ID;
	

	if (idx < CARD_NUM * CHANNEL_NUM)
		Rs422_ID = SKY_make_attr_uinteger(dev->Rs422_ID[0]);

	return Rs422_ID;
}

static exception_t set_attr_Rs422_ID(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	x86_rs422_dev *dev = conf_obj->obj;
	dev->Rs422_ID[dev->id_cnt] = SKY_attr_uinteger(*value);

	dev->id_cnt ++;
	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	x86_rs422_dev *dev =  conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->interrupt_number);
	return int_number;
}

static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	x86_rs422_dev *dev =  conf_obj->obj;
	dev->interrupt_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static x86_rs422_register_attribute(conf_class_t* clss){
    SKY_register_attribute(clss, "Rs422_ID", get_attr_Rs422_ID, NULL, set_attr_Rs422_ID, NULL, SKY_Attr_Optional, "uinteger", "ID of Rs422");
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of rs422 device");
}

extern "C" void init_x86_rs422();

void init_x86_rs422(){
	static skyeye_class_t class_data = {
		class_name : "x86_rs422",
		class_desc : "x86_rs422",
		new_instance : new_x86_rs422,
		free_instance : free_x86_rs422,
		reset_instance : reset_x86_rs422,
		get_attr : NULL,
		set_attr : NULL
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	/* Register io function to the object */
	static const memory_space_intf io_memory = {
		conf_obj:NULL,
		read : x86_rs422_read,
		write : x86_rs422_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const Rs422_emif_intf rs422_emif_interface = {
		conf_obj:NULL,
		set_state:x86_rs422_recv_data,
	};
	SKY_register_iface(clss, RS422_EMIF_INTF, &rs422_emif_interface);
	static const skyeye_reg_intf reg_intf = {
		conf_obj:NULL,
		get_regvalue_by_id : x86_rs422_get_regvalue_by_id,
		get_regname_by_id : x86_rs422_get_regname_by_id,
		set_regvalue_by_id : x86_rs422_set_regvalue_by_id,
		get_regid_by_name : x86_rs422_get_regid_by_name,
		get_regnum : x86_rs422_get_regnum,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			name : SKYEYE_REG_INTF,
			iface : &reg_intf,
		},
		(struct InterfaceDescription) {
			name : MEMORY_SPACE_INTF_NAME, 
			iface : &io_memory,
		},
		(struct InterfaceDescription) {
			name : RS422_EMIF_INTF,
			iface :  &rs422_emif_interface,
		},
 	};
	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription) {
			name : RS422_BUS_INTF,
			set : x86_rs422_set,
			get : x86_rs422_get,
		},
	};

	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	x86_rs422_register_attribute(clss);
}

#endif 

