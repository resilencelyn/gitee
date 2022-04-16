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
#include <skyeye_log.h>
#include <skyeye_dev_bus_intf.h>
#include <skyeye_dev_intf.h>
#include <skyeye_uart_ops.h>
#include <skyeye_system.h>
#include <skyeye_swapendian.h>
#define DEBUG
#include "at697f_spi_device.h"
#include <skyeye_api.h>

#undef SPI_TEST

uint16 swap2(uint16_t data)
{
	return (data<<8)|(data>>8);
}

static conf_object_t* new_at697f_spi_device(const char *obj_name)
{
	at697f_spi_device_device* dev = skyeye_mm_zero(sizeof(at697f_spi_device_device));
	dev->obj = new_conf_object(obj_name, dev);
	// TODO: place the code here
	dev->tx_data.buf = skyeye_mm_zero(MAXSIZE);
	dev->tx_data.pos = 0;
	dev->count = 0;
	dev->index = 0;

	return dev->obj;
}

// Free the device object to do some memory free
static exception_t free_at697f_spi_device(conf_object_t* conf_obj)
{
	at697f_spi_device_device *dev = (at697f_spi_device_device *)conf_obj->obj;
	int i;
	for (i = 0; i < FILE_MAX_NUM; i++)
	{
		if (dev->fp[i] != NULL)
			fclose(dev->fp[i]);
	}

	skyeye_free(dev->tx_data.buf);
	skyeye_free(conf_obj->objname);
	skyeye_free(conf_obj->obj);
	skyeye_free(conf_obj);
	return No_exp;
}

static attr_value_t get_attr_filename(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
	at697f_spi_device_device* dev = conf_obj->obj;
	attr_value_t value = SKY_make_attr_string(dev->file[1]);
	return value;
}

static exception_t set_attr_filename1(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	at697f_spi_device_device* dev = conf_obj->obj;
	dev->file[1] = skyeye_strdup(SKY_attr_string(*value));
	return No_exp;
}

static exception_t set_attr_filename2(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	at697f_spi_device_device* dev = conf_obj->obj;
	dev->file[2] = skyeye_strdup(SKY_attr_string(*value));
	return No_exp;
}

static exception_t set_attr_filename3(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	at697f_spi_device_device* dev = conf_obj->obj;
	dev->file[3] = skyeye_strdup(SKY_attr_string(*value));
	return No_exp;
}

static exception_t set_attr_filename4(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	at697f_spi_device_device* dev = conf_obj->obj;
	dev->file[4] = skyeye_strdup(SKY_attr_string(*value));
	return No_exp;
}

static exception_t set_attr_filename5(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	at697f_spi_device_device* dev = conf_obj->obj;
	dev->file[5] = skyeye_strdup(SKY_attr_string(*value));
	return No_exp;
}

static exception_t set_attr_filename6(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	at697f_spi_device_device* dev = conf_obj->obj;
	dev->file[6] = skyeye_strdup(SKY_attr_string(*value));
	return No_exp;
}

static exception_t set_attr_filename7(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	at697f_spi_device_device* dev = conf_obj->obj;
	dev->file[7] = skyeye_strdup(SKY_attr_string(*value));
	return No_exp;
}

static exception_t set_attr_filename8(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	at697f_spi_device_device* dev = conf_obj->obj;
	dev->file[8] = skyeye_strdup(SKY_attr_string(*value));
	return No_exp;
}

static exception_t set_attr_filename9(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	at697f_spi_device_device* dev = conf_obj->obj;
	dev->file[9] = skyeye_strdup(SKY_attr_string(*value));
	return No_exp;
}

static exception_t set_attr_filename10(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	at697f_spi_device_device* dev = conf_obj->obj;
	dev->file[10] = skyeye_strdup(SKY_attr_string(*value));
	return No_exp;
}

static exception_t set_attr_filename11(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	at697f_spi_device_device* dev = conf_obj->obj;
	dev->file[11] = skyeye_strdup(SKY_attr_string(*value));
	return No_exp;
}

static exception_t set_attr_filename12(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	at697f_spi_device_device* dev = conf_obj->obj;
	dev->file[12] = skyeye_strdup(SKY_attr_string(*value));
	return No_exp;
}
void spi_file_register_attribute(conf_class_t* clss) 
{
	SKY_register_attribute(clss, "file1", get_attr_filename, NULL, set_attr_filename1, NULL, SKY_Attr_Optional, "string", "spi file1 name");
	SKY_register_attribute(clss, "file2", get_attr_filename, NULL, set_attr_filename2, NULL, SKY_Attr_Optional, "string", "spi file2 name");
	SKY_register_attribute(clss, "file3", get_attr_filename, NULL, set_attr_filename3, NULL, SKY_Attr_Optional, "string", "spi file3 name");
	SKY_register_attribute(clss, "file4", get_attr_filename, NULL, set_attr_filename4, NULL, SKY_Attr_Optional, "string", "spi file4 name");
	SKY_register_attribute(clss, "file5", get_attr_filename, NULL, set_attr_filename5, NULL, SKY_Attr_Optional, "string", "spi file5 name");
	SKY_register_attribute(clss, "file6", get_attr_filename, NULL, set_attr_filename6, NULL, SKY_Attr_Optional, "string", "spi file6 name");
	SKY_register_attribute(clss, "file7", get_attr_filename, NULL, set_attr_filename7, NULL, SKY_Attr_Optional, "string", "spi file7 name");
	SKY_register_attribute(clss, "file8", get_attr_filename, NULL, set_attr_filename8, NULL, SKY_Attr_Optional, "string", "spi file8 name");
	SKY_register_attribute(clss, "file9", get_attr_filename, NULL, set_attr_filename9, NULL, SKY_Attr_Optional, "string", "spi file9 name");
	SKY_register_attribute(clss, "file10", get_attr_filename, NULL, set_attr_filename10, NULL, SKY_Attr_Optional, "string", "spi file10 name");
	SKY_register_attribute(clss, "file11", get_attr_filename, NULL, set_attr_filename11, NULL, SKY_Attr_Optional, "string", "spi file11 name");
	SKY_register_attribute(clss, "file12", get_attr_filename, NULL, set_attr_filename12, NULL, SKY_Attr_Optional, "string", "spi file12 name");
}

static exception_t spi_bus_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	at697f_spi_device_device *dev = obj->obj;

	dev->spi_bus_obj = obj2;
	dev->spi_bus_iface = (spi_bus_intf *)SKY_get_iface(obj2, SPI_BUS_INTF_NAME);

	if (dev->spi_bus_iface == NULL){
		if (dev->spi_bus_obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", SPI_BUS_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SPI_BUS_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t spi_bus_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	return No_exp;
}

exception_t xorCheck(char *pData, uint32_t dwLen)
{
	uint8_t byRet = 0;

	while (dwLen--)
		byRet ^= *pData++;

	return byRet;
}

void parse_SPISendData_file(conf_object_t* conf_obj, FILE_ID index)
{
	at697f_spi_device_device *dev = (at697f_spi_device_device *)conf_obj->obj;
	unsigned char buf[SPI_MSGQ_DATA_LEN] = {0};
	unsigned char msgData[SPI_MSGQ_DATA_LEN] = {0};
	int msgLen = 0, msgID = 0;
	char *tmpStr, *dataID, *msgCnt;

	assert(index < FILE_MAX_NUM);
	FILE *fp = dev->fp[index];

	while ((fgets(buf, SPI_MSGQ_DATA_LEN, fp)) != NULL)
	{
		//获取数据包的个数
		if (buf[0] == '@')
		{
			tmpStr = strtok(buf, " ");
			msgCnt = strtok(NULL, " ");
			dev->readMsgCnt[index] = strtol(msgCnt, NULL, 10);
			continue;
		}
		//数据行的开始标志
		if (buf[0] == '#')
		{
			dev->readDataCnt[index] ++;
			continue;
		}

		int i = 0;
		//获取数据行的序号
		dataID = strtok(buf, " ");
		msgID = strtol(dataID, NULL, 10);

		if (dev->readDataCnt[index] == msgID)
		{
			//获取数据行的长度
			tmpStr = strtok(NULL, " ");
			msgLen = strtol(tmpStr, NULL, 10);
			dev->tx_data.buf->wDataLen = bswap_half(msgLen);
			while(tmpStr = strtok(NULL, " "))
			{
				//获取数据行的数据
				msgData[i] = strtol(tmpStr, NULL, 16); //数据
				i++;
				if (msgLen == i)
					break;
			}
			memcpy(dev->tx_data.buf->byData, &msgData[0], msgLen);
			printf("In %s, filename = %s msgLen = %d\n", __func__, dev->file[index+1], msgLen);
			if (msgID == dev->readMsgCnt[index])
			{
				dev->readDataCnt[index] = 0;
				fseek(dev->fp[index], 0, SEEK_SET);
			}
			return;
		}
	}
}

void spi_msg_process(conf_object_t *slave);
void send_spi_data(conf_object_t *slave)
{
	at697f_spi_device_device *dev = slave->obj;

	dev->msgCnt ++;

	switch(dev->msgCnt)
	{
		case 1:
			dev->recvBuf.byDataType = SPI_CHAXUN_CMD;
			spi_msg_process(slave);
			break;
		case 2:
		case 3:
		case 4:
			dev->recvBuf.byDataType = SPI_DOC_CMD;
			spi_msg_process(slave);
			break;
		case 5:
			dev->recvBuf.byDataType = SPI_DIGITAL_CG;
			spi_msg_process(slave);
			break;
		case 6:
			dev->recvBuf.byDataType = SPI_PRJ_DATA_CG;
			spi_msg_process(slave);
			break;
		case 7:
			dev->recvBuf.byDataType = SPI_DIGITAL_JH1;
			spi_msg_process(slave);
			break;
		case 8:
			dev->recvBuf.byDataType = SPI_DIGITAL_YAOCE2;
			spi_msg_process(slave);
			break;
		case 9:
			dev->recvBuf.byDataType = SPI_DIGITAL_YAOCE3;
			spi_msg_process(slave);
			break;
		case 10:
			dev->recvBuf.byDataType = SPI_DIGITAL_YAOCE4;
			spi_msg_process(slave);
			break;
		case 11:
			dev->recvBuf.byDataType = SPI_DIGITAL_OTHER;
			spi_msg_process(slave);
			break;
		case 12:
			dev->recvBuf.byDataType = SPI_ALARM_PARAM;
			spi_msg_process(slave);
			break;
		case 13:
			dev->recvBuf.byDataType = SPI_10GE_INJECT;
			spi_msg_process(slave);
			break;
		default:
			dev->msgCnt = 0;
			break;
	}
}

void spi_msg_process(conf_object_t *slave)
{
	at697f_spi_device_device *dev = slave->obj;

	unsigned char bychecksum = 0;
	uint16_t datalen;
	memset(dev->tx_data.buf->byData, 0, (SPI_MSGQ_DATA_LEN + 6));
	dev->tx_data.buf->bySynData = SPI_COMM_SYNC;
	dev->tx_data.buf->byDevData = ZHUKONG;
	dev->tx_data.buf->byDataType = dev->recvBuf.byDataType;

	OUTPUTINFO info;
	printf("##DataType:0x%x objname = %s\n", dev->tx_data.buf->byDataType, slave->objname);
	switch (dev->tx_data.buf->byDataType)
	{
		case SPI_DIGITAL_YAOCE2:
			parse_SPISendData_file(slave, YAOCE2);
			break;
		case SPI_DIGITAL_YAOCE3:
			parse_SPISendData_file(slave, YAOCE3);
			break;
		case SPI_DIGITAL_YAOCE4:
			parse_SPISendData_file(slave, YAOCE4);
			break;
		case SPI_DIGITAL_OTHER:
			parse_SPISendData_file(slave, OTHER);
			break;
		case SPI_ALARM_PARAM:
			parse_SPISendData_file(slave, ALARM_PARAM);
			break;
		case SPI_10GE_INJECT:
			parse_SPISendData_file(slave, INJECT);
			break;
		case SPI_CHAXUN_CMD:
			parse_SPISendData_file(slave, CHAXUN_CMD);
			if (dev->tx_data.buf->byData[0] != 0 || dev->tx_data.buf->byData[1] != 0)
			{
				dev->start_doc_cmd[0] = dev->tx_data.buf->byData[0];
				dev->start_doc_cmd[1] = dev->tx_data.buf->byData[1];
				dev->count = dev->start_doc_cmd[0];
				dev->count = (dev->count << 8) + dev->start_doc_cmd[1];
			}
			break;
		case SPI_DOC_CMD:
			if (dev->count != 0)
			{
				parse_SPISendData_file(slave, DOC_CMD);
				dev->count--;
			} else
			{
				//未读取到有效数据，设置长度为0
				dev->tx_data.buf->wDataLen = 0;
				dev->readDataCnt[DOC_CMD] = 0;
			}
			break;
		case SPI_DIGITAL_CG:
			parse_SPISendData_file(slave, CG);
			break;
		case SPI_PRJ_DATA_CG:
			parse_SPISendData_file(slave, DATA_CG);
			break;
		case SPI_DIGITAL_JH1:
			parse_SPISendData_file(slave, JH1);
			break;
		default:
			info.type = dev->tx_data.buf->byDataType;

			info.cur_time = SkyEye_GetRunTime("sparc_v8_0", slave);
			//memcpy(info.data, dev->recvBuf.byData, dev->recvDataLen);
			fprintf(dev->fp[STORE], "%x ", info.type);
			int j;
			for (j = 0; j < outnum; j++)
			{
				if (outinfo[j].type == info.type)
				{
					info.type_name = outinfo[j].type_name;
					fprintf(dev->fp[STORE], "%s ", info.type_name);
					break;
				}
			}
			fprintf(dev->fp[STORE], "%lf ", info.cur_time );
			int i;
			for (i = 0; i < dev->recvDataLen; i++)
			{
				fprintf(dev->fp[STORE], "%x ", dev->recvBuf.byData[i]);
			}
			fprintf(dev->fp[STORE], "\n ");
			return;
	}
	datalen = bswap_half(dev->tx_data.buf->wDataLen);
	bychecksum = xorCheck((char*)&dev->tx_data.buf->bySynData, datalen + SPI_MSGQ_HEADER_SIZE);

	dev->tx_data.buf->byData[datalen] = bychecksum;

	printf("SPI dataLen = %d\n", datalen);
	int cnt;
	for(cnt = 0; cnt <= datalen; cnt++)
	{
		printf("%x ", dev->tx_data.buf->byData[cnt]);
	}
	printf("\n\n");

	dev->spi_bus_iface->spi_bus_receive(dev->spi_bus_obj, (char*)&dev->tx_data.buf->bySynData, datalen + SPI_MSGQ_HEADER_SIZE + 1);
}

static void spi_device_receive(conf_object_t *slave, void *buf, int len)
{
	at697f_spi_device_device *dev = slave->obj;
	unsigned char data = *(uint8_t *)buf;
	//printf("recv data from master: 0x%x ,index: %x \n", data, dev->index);
	if (data == SPI_COMM_SYNC && dev->index == 0)
	{
		dev->validFrame = 1; //有效数据包
		dev->recvbuf[dev->index++] = data;
		return;
	}

	if (dev->validFrame)
	{
		dev->recvbuf[dev->index++] = data;

		if (dev->index == 6)
		{
			dev->recvDataLen = (dev->recvbuf[3] << 8)|dev->recvbuf[4];
		}

		if (dev->index == (dev->recvDataLen + 6))
		{
			memcpy(&dev->recvBuf.bySynData, dev->recvbuf, dev->index);
			dev->index = 0;
			dev->validFrame = 0;
			spi_msg_process(slave);
		}
	}

}

static exception_t config_at697f_spi_device(conf_object_t* conf_obj)
{
	at697f_spi_device_device *dev = (at697f_spi_device_device *)conf_obj->obj;
	int i;
	for (i = 0; i < FILE_MAX_NUM; i++)
	{
		dev->fp[i] = fopen(dev->file[i+1], "r+");
		if (dev->fp[i] == NULL)
		{
			printf("[WARING]: In %s, objname = %s Open Filename: %s Failed!\n", __func__, conf_obj->objname, dev->file[i+1]);
		}
	}
#ifdef SPI_TEST
	system_register_timer_handler(conf_obj, 10, (time_sched_t)send_spi_data, SCHED_MS|SCHED_NORMAL);
#endif
	//if (!strcmp(conf_obj->objname, "spi_comm_dev_shuguan"))
	//parse_SPISendData_file(conf_obj, dev->fp_doc_cmd);
	return No_exp;
}

// initializes the module class and executes the following code when the module is loaded by SkyEye
void init_at697f_spi_device(void)
{
	// register a device with SkyEye
	static skyeye_class_t class_data =
	{
		.class_name      = "at697f_spi_device",
		.class_desc      = "at697f_spi_device",
		.new_instance    = new_at697f_spi_device,
		.free_instance   = free_at697f_spi_device,
		.config_instance   = config_at697f_spi_device,
		.module_type     = SKYML_OFFCHIP,
	};
	conf_class_t* dev_class = SKY_register_device_class(class_data.class_name, &class_data);

	// register access interface (for SkyEye graphical control interface)
	static spi_device_intf spi_intf = {
		.spi_device_receive = spi_device_receive,
		//.spi_device_receive_done = spi_device_receive_done,
		//.spi_deassert_cs_line = spi_deassert_cs_line,
		//.spi_assert_cs_line = spi_assert_cs_line,
		//.spi_get_cs_line = spi_get_cs_line,
	};
	SKY_register_iface(dev_class, SPI_DEVICE_INTF_NAME, &spi_intf);
	// list of interface methods
	static const struct InterfaceDescription ifaces[] =
	{
		(struct InterfaceDescription)
		{
			.name = SPI_DEVICE_INTF_NAME,
			.iface = &spi_intf
		}
	};

	// list of interface connection methods
	static const struct ConnectDescription connects[] =
	{
		/* ConnectDescription */
		(struct ConnectDescription) {
			.name = SPI_BUS_INTF_NAME,
			.set = spi_bus_set,
			.get = spi_bus_get,
		},
	};
	class_register_ifaces(dev_class, ifaces);
	class_register_connects(dev_class, connects);
	spi_file_register_attribute(dev_class);
}
