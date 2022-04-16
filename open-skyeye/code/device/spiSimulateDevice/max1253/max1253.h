#ifndef __MAX1253_
#define __MAX1253_


#define  TEMP  0x0
#define  VDD   0x1
#define  AIN0  0x2
#define  AIN1  0x3
#define  AIN2  0x4
#define  AIN3  0x5
#define  AIN4  0x6
#define  AIN5  0x7
#define  AIN6  0x8
#define  AIN7  0x9


#define Readstatus  1
#define Writestatus 0

typedef  struct MAX1253State{
	conf_object_t *obj;
	struct registers{
		uint16_t temp; 
		uint16_t vdd ;
		uint16_t ain[10];
		uint16_t chanel_upper_thr[8];
		uint16_t chanel_lower_thr[8];
		uint8_t chanel_conf[8];
		uint16_t ch_en;
		uint16_t input_conf;
		uint8_t step;
	}regs;
	uint8_t addr;
	uint8_t command;
	uint8_t write_num;
	uint8_t read_num;
	uint8_t read_flag;
	uint8_t all_channel_data[20];
}max1253_dev;

#endif
