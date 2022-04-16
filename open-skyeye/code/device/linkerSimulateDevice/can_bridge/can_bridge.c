#include <skyeye_lock.h>
#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_core.h>
#include <skyeye_signal.h>
#include <skyeye_emif.h>
#define DEBUG
#include <skyeye_log.h>
#include "can_bridge.h"
#include "can_bridge_fifo.h"

#define MAX_CAN_FIFO_LEN 10000

#define BUF(num)  ((unsigned long long)(buf[num]))  

int data_to_frame(char *buf, can_msg_t *msg)
{
	if((BUF(1) & 0x1) == 0){ //扩展帧

	}else{ //标准帧
		msg->id = BUF(0) & 0xff; 
		msg->ide = BUF(1) & 0x1;
		msg->dlc = BUF(2);
		if(msg->dlc > 8)
			msg->dlc = 8;
		msg->data[0] = BUF(3) & 0xff;
		msg->data[1] = BUF(4) & 0xff;
		msg->data[2] = BUF(5) & 0xff;
		msg->data[3] = BUF(6) & 0xff;
		msg->data[4] = BUF(7) & 0xff;
		msg->data[5] = BUF(8) & 0xff;
		msg->data[6] = BUF(9) & 0xff;
		msg->data[7] = BUF(10) & 0xff;
	
	}
}


int frame_to_data(char *buf, can_msg_t *msg)
{
	if ((msg->ide & 0x1) == 0){ //扩展帧
	}else{//标准帧
		buf[0] = msg->id & 0xff;
		buf[1] = msg->ide & 0x1;
		buf[2] = msg->dlc & 0xf;
		if (msg->dlc > 8)
			buf[2] = 8;
		buf[3] = msg->data[0] & 0xff;
		buf[4] = msg->data[1] & 0xff;
		buf[5] = msg->data[2] & 0xff;
		buf[6] = msg->data[3] & 0xff;
		buf[7] = msg->data[4] & 0xff;
		buf[8] = msg->data[5] & 0xff;
		buf[9] = msg->data[6] & 0xff;
		buf[10] = msg->data[7] & 0xff;
	}
	return 0;
}

static int can_bridge_receive(conf_object_t *obj, can_msg_t *msg){
	can_bridge_device* dev = obj->obj;
	WriteFIFO(dev->rec_fifo, msg, 1);
	return 0;
}

int recv_msg(can_bridge_device* dev){
	can_msg_t msg;
	msg.id = 3;
	msg.ide = 1;
	msg.dlc = 8;
	int i;
	for (i = 1; i < 9;i++)
		msg.data[i - 1] = i + dev->cnt;

	dev->cnt ++;
	WriteFIFO(dev->rec_fifo, &msg, 1);
}

static conf_object_t* new_can_bridge(char *obj_name)
{
	can_bridge_device* dev = skyeye_mm_zero(sizeof(can_bridge_device));
	dev->obj = new_conf_object(obj_name, dev);
	dev->rec_fifo = CreateFIFO(MAX_CAN_FIFO_LEN);
	return dev->obj;
}
static exception_t free_can_bridge(conf_object_t* obj)
{
	can_bridge_device* dev = obj->obj;
	FreeFIFO(dev->rec_fifo);
	skyeye_free(obj->objname);
	skyeye_free(obj->obj);
	skyeye_free(obj);
	return No_exp;
}

int can_bridge_read(conf_object_t *obj, char *buf){
	can_bridge_device* dev = obj->obj;
	int count = 0; 
	can_msg_t msg;
	if(CheckCanReadNum(dev->rec_fifo)){
		ReadFIFO(dev->rec_fifo, &msg, 1);
		count = msg.dlc;
		frame_to_data(buf, &msg);
	}

	return count;
}

int can_bridge_write(conf_object_t *obj, char *buf, size_t count){
	can_bridge_device* dev = obj->obj;
	can_msg_t msg;
	data_to_frame(buf, &msg);
	if(dev->can_linker_iface != NULL){
		dev->can_linker_iface->send_msg(dev->can_linker, obj, &msg);
	}
}

static exception_t can_bridge_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	can_bridge_device* dev = (can_bridge_device*)(obj->obj);
	dev->can_linker = obj2;
	dev->can_linker_iface = (can_linker_intf*)SKY_get_iface(dev->can_linker, CAN_LINKER_INTF);

	if (dev->can_linker_iface == NULL){
		if (dev->can_linker == NULL)
			skyeye_log(Error_log, __FUNCTION__, "Get 'can_can_linker_iface' interface from NULL object fail.");
		else
			skyeye_log(Error_log, __FUNCTION__, "Get 'can_can_linker_iface' interface from %s fail.", obj2->objname);
	}
	
	return No_exp;
}
 
static exception_t can_bridge_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	can_bridge_device* dev = (can_bridge_device*)(obj->obj);
	*obj2 = dev->can_linker;
	*port = NULL;
	return No_exp;
}

static attr_value_t get_attr_self_test(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	can_bridge_device *dev = conf_obj->obj;
	attr_value_t timer_freq = SKY_make_attr_uinteger(dev->timer_freq); 
	return timer_freq;
}
static exception_t set_attr_self_test(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	can_bridge_device *dev = conf_obj->obj;
	dev->timer_freq = SKY_attr_uinteger(*value);
	int id = -1;
	create_thread_scheduler(dev->timer_freq, Periodic_sched, recv_msg, dev, &id);
	return No_exp;
}

void can_bridge_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "self_test_timer", get_attr_self_test, NULL, set_attr_self_test, NULL, SKY_Attr_Optional, "uinteger", "timer frequency");
}
void init_can_bridge(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "can_bridge",
		.class_desc = "can_bridge",
		.new_instance = new_can_bridge,
		.free_instance = free_can_bridge,
		.module_type = SKYML_OFFCHIP,
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const can_dev_intf can_intf = {
		.receive_msg = can_bridge_receive,
	};
	SKY_register_iface(clss, CAN_DEV_INTF, &can_intf);

	static const data_inject_intf inject_intf= {
		.read = can_bridge_read,
		.write = can_bridge_write,
	};
	SKY_register_iface(clss, DATA_INJECT_INTF, &inject_intf);


	static const struct InterfaceDescription ifaces[] = 
	{
		(struct InterfaceDescription) {
			.name = DATA_INJECT_INTF, 
			.iface = &inject_intf,
		},
		(struct InterfaceDescription) {
			.name = CAN_DEV_INTF, 
			.iface = &can_intf,
		},
	};
	static const struct ConnectDescription connects[] = 
	{
		/* ConnectDescription */
		(struct ConnectDescription) {
			.name = CAN_LINKER_INTF,
			.set = can_bridge_set,
			.get = can_bridge_get,
		},

	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
	can_bridge_register_attribute(clss);
}
