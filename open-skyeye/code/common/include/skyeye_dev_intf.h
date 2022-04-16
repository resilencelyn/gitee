#ifndef __SKYEYE_DEV_INTF_
#define __SKYEYE_DEV_INTF_

#include "skyeye_dev_bus_intf.h"
typedef int i2c_device_state_t;
#define I2C_IDLE           -1
#define I2C_SLAVE_RECEIVE  0
#define I2C_SLAVE_TRANSMIT 1

typedef struct i2c_device_interface{
	conf_object_t *obj;
	int (*set_state)(conf_object_t* device, i2c_device_state_t state, uint8_t address);
	uint8_t (*read_data)(conf_object_t*device);
	void (*write_data)(conf_object_t*device, uint8_t value);
	uint8_t (*get_address)(conf_object_t*device);
}i2c_device_intf;

#define I2C_DEVICE_INTF_NAME "i2c_device"



/*SPI SLAVE DEVICE INTERFACE*/
typedef struct spi_device_interface{
	conf_object_t *obj;
	void (*spi_request)(conf_object_t *slave, int first, int last, char *buf, int len, char *feedback);
	int (*connect_master)(conf_object_t *slave);
	int (*disconnect_master)(conf_object_t *slave);

	void (*spi_device_receive)(conf_object_t *slave, char *buf, int len);
	void (*spi_device_receive_done)(conf_object_t *slave);
	void (*spi_deassert_cs_line)(conf_object_t *slave);
	void (*spi_assert_cs_line)(conf_object_t *slave);
	int (*spi_get_cs_line)(conf_object_t *slave);
}spi_device_intf;

#define SPI_DEVICE_INTF_NAME "spi_device"

/*GDB INTERFACE*/
#define  GDB_SERVER_ERROR -1
#define  GDB_SERVER_SUCCESS 0
typedef struct skyeye_gdbserver_interface{
	int (*GdbServerStart)(conf_object_t *obj, char *cpuname, int port, char *ip);
	int (*GdbServerIsConnected)(conf_object_t *obj);
	char* (*GdbServerGetClientIp)(conf_object_t *obj);
}skyeye_gdbserver_intf;
#define SKYEYE_GDBSERVER_INTF_NAME "skyeye_gdbserver"


/*GPIO connector interface*/
typedef struct skyeye_gpio_connector_interface{
	exception_t (*update)(conf_object_t *obj, uint32_t regvalue);
}skyeye_gpio_connector_intf;
#define SKYEYE_GPIO_CONNECTOR "gpio_connect"

typedef struct skyeye_gate_a_interface{
	int (*update)(conf_object_t *obj, uint32_t regvalue);
}skyeye_gate_a_intf;
#define LOGIC_GATE_A "gate_input_a"


typedef struct skyeye_gate_b_interface{
	int (*update)(conf_object_t *obj, uint32_t regvalue);
}skyeye_gate_b_intf;
#define LOGIC_GATE_B "gate_input_b"

typedef struct gpio_conncet_device_interface{
	int (*connect_device)(conf_object_t *gpio_obj, conf_object_t *device_obj, uint8_t channel, uint32_t value);
	int (*connect_gpio)(conf_object_t *device_obj, conf_object_t *gpio_obj, uint8_t channel, uint32_t value);
}gpio_connect_device_intf;
#define GPIO_CONNECT_DEVICE_INTF "gpio_connect_device_intf"

typedef struct image_info{
	uint32_t (*get_page_size)(conf_object_t *obj);
	uint32_t (*get_page_count)(conf_object_t *obj);
	int32_t (*get_page_index_by_id)(conf_object_t *obj, uint32_t id);
	char *(*get_page_content)(conf_object_t *obj, uint32_t index);
	exception_t (*set_page_content)(conf_object_t *obj, void *buf, uint32_t index);
	exception_t (*clear_all_pages)(conf_object_t *obj);
	void (*set_image_size)(conf_object_t *obj, uint32_t size);
}skyeye_image_info;
#define SKYEYE_IMAGE_INFO "image_info"

/*BU61580 interface*/
typedef struct bu61580_dev_iface{
	conf_object_t* conf_obj;
	exception_t (*read) (conf_object_t *conf_obj, void *buf, size_t length);
	exception_t (*write) (conf_object_t *conf_obj, void *buf, size_t length);
	exception_t (*set_state) (conf_object_t *conf_obj, int length);
}bu61580_dev_intf;
#define BU61580_DEV_INTF_NAME "bu61580_dev_intf"


/*uart file interface*/
typedef struct uart_file_iface{
	exception_t (*read) (conf_object_t *conf_obj, void *buf, size_t length);
	exception_t (*write) (conf_object_t *conf_obj, void *buf, size_t length);
}uart_file_intf;
#define UART_FILE_INTF "uart_file_intf"

/*uart com interface*/
typedef struct uart_com_iface{
	exception_t (*read) (conf_object_t *conf_obj, void *buf, size_t length);
	exception_t (*write) (conf_object_t *conf_obj, void *buf, size_t length);
}uart_com_intf;
#define UART_COM_INTF "uart_com_intf"

/* fb interface */
typedef struct skyeye_fb_iface{
	uint32_t (*read) (conf_object_t *conf_obj, void *buf, size_t length);
	uint32_t (*write) (conf_object_t *conf_obj, void *buf, size_t length);
} skyeye_fb_intf;
#define SKYEYE_FB_INTF "skyeye_fb_intf"

/* pin interface */
typedef struct skyeye_pin_info_iface {
	uint32_t (*get_pin_num) (conf_object_t *conf_obj);
	exception_t (*get_pin_type) (conf_object_t *conf_obj, uint32_t index, void *type);
	exception_t (*read_pin_state) (conf_object_t *conf_obj, uint32_t index, void *state);
	exception_t (*write_pin_state) (conf_object_t *conf_obj, uint32_t index, void *state);
} skyeye_pin_info_intf;
#define SKYEYE_PIN_INFO_INTF "skyeye_pin_info_intf"

typedef struct  instr_process_iface{
	int (*get_dif_pc_cache)(conf_object_t *obj, char *fname);
	int (*get_pc_record_size)(conf_object_t *obj);
	int (*set_pc_record_size)(conf_object_t *obj, int size);
	int (*get_pc_record_nums)(conf_object_t *obj);
	int (*get_pc_record_index)(conf_object_t *obj);
	int (*get_pc_record_overflow)(conf_object_t *obj);
	generic_address_t (*get_pc_by_index)(conf_object_t *obj, int n);
}instr_process_intf;
#define INSTR_PROCESS_INTF "instr_process_intf"

typedef struct{
	int (*receive_msg)(conf_object_t *can_device, can_msg_t *msg);
}can_dev_intf;
#define CAN_DEV_INTF "can_dev_intf"

typedef struct{
	int (*receive_data) (conf_object_t *io_device, void *buf, int length, int io_id);
}io_dev_intf;
#define IO_DEV_INTF "io_dev_intf"

typedef struct{
	int (*receive_pulse) (conf_object_t *pulse_device, pulse_msg_t *msg, int io_num, int io_id);
}pulse_dev_intf;
#define PULSE_DEV_INTF "pulse_dev_intf"

typedef struct{
	int (*receive_data) (conf_object_t *rs422_device, void *buf, int length, int port_id);
}rs422_dev_intf;
#define RS422_DEV_INTF "rs422_dev_intf"

typedef struct{
	int (*receive_data) (conf_object_t *ad_device, void *buf, int length);
}ad_dev_intf;
#define AD_DEV_INTF "ad_dev_intf"

//double machine communication device
typedef struct{
	void (*receive_data)(conf_object_t *dmc_device, void *buf);
	void (*raise_signal)(conf_object_t *dmc_device, int irq_number);
    void (*send_backup_data)(conf_object_t *obj, uint16_t *buffer, int length);
}dmc_dev_intf;
#define DMC_DEV_INTF "dmc_dev_intf"

/* 获取A机或B机接口 */
typedef struct{
	int (*get_aocs_status)(conf_object_t *device);
}get_aocs_intf;
#define GET_AOCS_INTF "get_aocs_intf"

typedef struct{
	int (*receive_msg) (conf_object_t *device, network_mesg_t *msg, int count); //count=1??ʾ???ֽڴ??䣬count=2??ʾ˫?ֽڴ???
}bu61580_bridge_intf;
#define BU61580_BRIDGE_INTF "bu61580_bridge_intf"

typedef struct{
	int (*receive_msg)(conf_object_t *atm_device, atm_msg_t *msg);
}atm_dev_intf;
#define ATM_DEV_INTF "atm_dev_intf"

typedef struct{
	int (*receive_msg)(conf_object_t *tsi578_device, tsi578_msg_t *msg);
}tsi578_dev_intf;
#define TSI578_DEV_INTF "tsi578_dev_intf"

typedef struct{
	exception_t (*write)(conf_object_t *emif_device, generic_address_t addr, void *buf, size_t count);
	exception_t (*read)(conf_object_t *emif_device, generic_address_t addr, void *buf, size_t count);
}emif_memory_intf;
#define EMIF_MEMORY_INTF "emif_memory_intf"

typedef struct{
	exception_t (*read) (conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t length);
	exception_t (*write)(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t length);
}bu61580_ram_intf;
#define BU61580_RAM_INTF "bu61580_ram_intf"

typedef enum{
	FIFOC_CMD = 0,
	FIFOC_INT = 0x4,
	FIFOC_DMA = 0x8,
	FIFOC_AXE = 0xc,
	FIFOC_DEBUG = 0x10
}mpc5121_sfifoc_reg_t;

typedef struct{
	void (*write_register)(conf_object_t *obj, mpc5121_sfifoc_reg_t reg, uint32_t value, reg_op_t op);
	void (*read_register)(conf_object_t *obj, mpc5121_sfifoc_reg_t reg, uint32_t *value, reg_op_t op);
}mpc5121_sfifoc_intf;
#define MPC5121_SFIFOC_INTF "mpc5121_sfifoc_intf"

typedef struct{
	exception_t (*transmit_complete)(conf_object_t *conf_obj, size_t length);
	exception_t (*start_dma_transfer)(conf_object_t *conf_obj);
}edma_dev_intf;
#define EDMA_DEV_INTF "edma_dev_intf"

typedef struct{
	exception_t (*transmit_complete)(conf_object_t *conf_obj, size_t length);
	exception_t (*start_dma_transfer)(conf_object_t *conf_obj);
}fifo_dev_intf;
#define FIFO_DEV_INTF "fifo_dev_intf"

typedef struct{
	exception_t (*write_sound)(conf_object_t *conf_obj, void *buf, size_t length);
	exception_t (*read_sound)(conf_object_t *conf_obj, void *buf, size_t length);
}sound_intf;
#define SOUND_INTF "sound_intf"

typedef struct{
	int32_t (*dma_write)(conf_object_t *master, uint32_t addr, void *buf, size_t count); //implemented by dma master
	int32_t (*dma_read)(conf_object_t *master, uint32_t addr, void *buf, size_t count); //implemented by dma master
	exception_t (*register_write_event)(conf_object_t *master, conf_object_t *device, uint32_t start, uint32_t end); //implemented by dma master
	int32_t (*event_write)(conf_object_t *device, uint32_t addr, void *buf, size_t count); //implemented by device
}dma_intf;
#define DMA_INTF "dma_intf"

typedef struct{
		int (*kgl_ctrl)(conf_object_t *obj, int cmd, int argc, int*argv);
}htey_kgl_intf;
/*-------cmd list*******/
#define TUOLUO 0
/*--------------*/
#define HTEY_KGL_INTF "htey_kgl_intf"

typedef struct{
		int (*client_send)(conf_object_t *obj, void *buf, int len);
}htey_client_intf;
#define HTEY_CLIENT_INTF "htey_client_intf"


/*stm32 RCC clock select*/
#define HSE_CLOCK 0
#define HSI_CLOCK 1
#define HSI48_CLOCK 2
#define LSE_CLOCK 3
#define LSI_CLOCK 4
#define SYSCLOCK 5
#define ADC_CLOCK 6
#define RTC_CLOCK 7
typedef struct{
    int (*read)(conf_object_t *obj,uint32_t clock);
}get_clock_frequency_t;
#define CLOCK_FREQUENCY_INTERFACE "clock_frequency_intf"

typedef struct{
    exception_t (*write)(conf_object_t *obj, uint32_t channel,uint32_t value);
    int (*get_event_info)(conf_object_t *obj, char *event_name, int *edmacc_n, int *event_num);
    // get_event_info
    // return 1: is successful
    // return 0: is failed
}edma_device_event_t;
#define EDMA_DEVICE_EVENT_INTERFACE "edma_device_event_intf"

typedef struct{
    exception_t (*write)(conf_object_t *obj, uint32_t channel,uint32_t value);
}fifo_device_event_t;
#define FIFO_DEVICE_EVENT_INTERFACE "fifo_device_event_intf"

/********************************/
#define PLL0_C6748_SYSCLK1 1
#define PLL1_C6748_SYSCLK1 1
#define PLL0_C6748_SYSCLK2 2
#define PLL1_C6748_SYSCLK2 2
#define PLL0_C6748_SYSCLK3 3
#define PLL1_C6748_SYSCLK3 3
#define PLL0_C6748_SYSCLK4 4
#define PLL0_C6748_SYSCLK5 5
#define PLL0_C6748_SYSCLK6 6
#define PLL0_C6748_SYSCLK7 7
#define PLL0_C6748_AUXCLK 8
#define PLL0_C6748_OBSCLK 9
#define PLL1_C6748_OBSCLK 4

typedef struct{
    double (*get_device_clock)(conf_object_t *obj, uint32_t clock_type);
}device_clock_intf;
#define DEVICE_CLOCK_INTF "device_clock_intf"


typedef struct{
    exception_t (*set_gpio)(conf_object_t *obj, uint32_t pin_num, int status);
}gpio_pin_set;
#define GPIO_PIN_SET "gpio_pin_set"

#define PIN_HIGH 1
#define PIN_LOW 0
typedef struct{
    exception_t (*linker_pin_change_state)(conf_object_t *obj,conf_object_t *gpio,uint32_t pin_num, uint32_t status);
}gpio_linker_pin_intf;
#define GPIO_LINKER_PIN_INTF "gpio_linker_pin_intf"

typedef struct{
    int (*send_data)(conf_object_t *obj, uint32_t channel, char *data,uint32_t datalength);
}upp_data_signal_intf;
#define UPP_DATA_SIGNAL_INTF "upp_data_signal_intf"

typedef struct{
    int (*upp_linker_data_send)(conf_object_t *obj, conf_object_t *upp, uint32_t channle, char *data, uint32_t datalength);
}upp_linker_data_tranf_intf;
#define UPP_LINKER_DATA_TRANF_INTF "upp_linker_data_tranf_intf"

typedef struct{
    exception_t (*receive_data)(conf_object_t *obj,char *data, uint32_t datalength);
}McBSP_data_receive_intf;
#define McBSP_DATA_RECEIVE_INTF "McBSP_data_receive_intf"

typedef struct{
    exception_t (*McBSP_linker_data_send)(conf_object_t *obj,conf_object_t *McBSP,char *data, uint32_t datalength);
}McBSP_linker_data_connect_intf;
#define McBSP_LINKER_DATA_CONNECT_INTF "McBSP_linker_data_connect_intf"

typedef struct{
	exception_t (*write_fifo)(conf_object_t *conf_obj, void *buf, size_t length);
	exception_t (*read_fifo)(conf_object_t *conf_obj, void *buf, size_t length);
}fifo_data_intf;
#define FIFO_DATA_INTF "fifo_data_intf"

typedef struct{
	exception_t (*write_gic_interface)(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count, int cpu_id);
	exception_t (*read_gic_interface)(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count, int cpu_id);
}gic_interface_intf;
#define GIC_INTERFACE_INTF "gic_interface_intf"

typedef struct{
	void (*set_skyeye_debug)(conf_object_t *conf_obj, int isEnable);
}skyeye_debug_intf;
#define SKYEYE_DEBUG_INTF "skyeye_debug_intf"

typedef struct
{
    exception_t (*write_data)(conf_object_t *obj, void *data, uint32_t datalength, int intd_trigger);
} skyeye_srio_device_t;
#define SRIO_DEVICE_INTERFACE "srio_device_intf"

typedef struct
{
    exception_t (*power_linker)(conf_object_t *obj, conf_object_t *device, uint32_t power_flag);
    exception_t (*duty_linker)(conf_object_t *obj, conf_object_t *device, uint32_t duty);
    int (*get_signal_linker)(conf_object_t *obj, conf_object_t *device);
}power_linker_intf;
#define POWER_LINKER_INTF "power_linker_intf"

typedef struct
{
    exception_t (*power_mach)(conf_object_t *obj, uint32_t power_flag);
    exception_t (*set_duty)(conf_object_t *obj, uint32_t duty);
    exception_t (*get_signal)(conf_object_t *obj);
}power_dev_intf;
#define POWER_DEV_INTF "power_dev_intf"

typedef struct
{
    exception_t (*send_ctrl_cmd)(conf_object_t *obj, uint32_t cmd_type, uint32_t cmd);
    exception_t (*send_power_flag)(conf_object_t *obj, uint32_t flag);
}sparc_ctrl_intf;
#define SPARC_CTRL_INTF "sparc_ctrl_intf"

typedef struct
{
    int (*send_data)(conf_object_t *obj, unsigned char *buf, int length);
}send_file_data_intf;
#define SEND_FILE_DATA_INTF "send_file_data_intf"

typedef struct dpram_get_smc_reg_interface
{
    exception_t (*dpram_get_smc)(conf_object_t *target, generic_address_t addr, void *buf, size_t count);
    exception_t (*dpram_set_smc)(conf_object_t *target, generic_address_t addr, void *buf, size_t count);
}dpram_get_smc_reg_intf;
#define DPRAM_GET_SMC_REG "dpram_get_smc_reg"
#endif


