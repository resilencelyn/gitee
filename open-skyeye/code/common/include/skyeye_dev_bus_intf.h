#ifndef __DEV_BUS_INTF_
#define __DEV_BUS_INTF_


/*i2c bus interface*/
typedef  int i2c_device_flag_t;
#define BUS_BUSY       -1
#define ADDR_ERROR     -2
#define SLAVE_ERROR    -3
#define NO_COM         -4

#define MAX_BAR_NUM     6

typedef struct skyeye_i2c_bus_interface{
    conf_object_t *conf_obj;
    int (*start)(conf_object_t* i2c_bus, uint8 address);
    int (*stop)(conf_object_t* i2c_bus);
    uint8 (*read_data)(conf_object_t* i2c_bus);
    void (*write_data)(conf_object_t* i2c_bus, uint8 value);
    int (*register_device)(conf_object_t* i2c_bus, conf_object_t* device, uint8 address, uint8 mask, i2c_device_flag_t flags);
    void (*unregister_device)(conf_object_t* i2c_bus, conf_object_t* device, uint8 address, uint8 mask);
}i2c_bus_intf;

#define I2C_BUS_INTF_NAME "i2c_bus"

typedef struct skyeye_std_1553b_bus_interface{
    conf_object_t *conf_obj;
    int (*start)(conf_object_t* std_1553b_bus, uint8 address);
    int (*stop)(conf_object_t* std_1553b_bus);
    int (*read_data)(conf_object_t* std_1553b_bus, conf_object_t* obj2, void* recv_buf, uint32 length, int sub_address);
    void (*write_data)(conf_object_t* std_1553b_bus, conf_object_t* obj2, void* send_buf, uint32 length, int RT_SubId);
    int (*register_device)(conf_object_t* std_1553b_bus, conf_object_t* device, uint32 dev_id);
    void (*unregister_device)(conf_object_t* std_1553b_bus, conf_object_t* device);
}std_1553b_bus_intf;

#define STD_1553B_BUS_INTF_NAME "std_1553b_bus_intf"

/*spi bus interface*/
typedef struct skyeye_spi_bus_interface{
    conf_object_t *conf_obj;
    int (*spi_slave_request)(conf_object_t *spi_bus, int first, int last, char *buf, int len, char *feedback);
    int (*connect_slave)(conf_object_t *spi_bus, char *slave_name);
    int (*disconnect_slave)(conf_object_t *spi_bus, char *slave_name);
    int  (*register_device)(conf_object_t* spi_bus, conf_object_t* device);
    void (*unregister_device)(conf_object_t* spi_bus, conf_object_t* device);

    void (*spi_bus_receive)(void *spi_bus, char *buf, int len);
    void (*reset_fifo)(void *spi_bus);
}spi_bus_intf;

#define SPI_BUS_INTF_NAME "spi_bus"

/* edma 提供的接口 */
typedef struct esci_sendto_edma_iface
{
    exception_t (*send_flag) (conf_object_t* conf_object, int val);
    exception_t (*recv_length) (conf_object_t* conf_object, int length);
}esci_sendto_edma_intf;

#define ESCI_SENDTO_EDMA "esci_sendto_edma_intf"

/* ESCI 提供的接口 */
typedef struct esci_recvfrom_edma_iface
{
    exception_t (*recv_edma_trans_len) (conf_object_t *conf_object, uint32_t TransLen);
}esci_recvfrom_edma_intf;

#define ESCI_RECVFROM_EDMA "esci_recvfrom_edma_intf"


typedef struct{
    uint32_t ide;
    uint32_t id;
    uint32_t channel_id;
    uint32_t dlc;
    uint8_t data[8];
    uint32_t crc;
    uint32_t ack_slot;
}can_msg_t;

typedef struct{
    int (*send_msg)(conf_object_t *can_linker, conf_object_t *can_device, can_msg_t *msg);
    int (*send_data)(conf_object_t *can_linker, conf_object_t *can_device, void *buf, uint32_t len);
}can_linker_intf;
#define CAN_LINKER_INTF "can_linker_intf"

//double machine communication linker iface 
typedef struct{
    int (*send_data)(conf_object_t *dmc_linker, conf_object_t *dmc_device, void *buf);
    int (*raise_signal)(conf_object_t *dmc_linker, conf_object_t *dmc_device, int irq_number);
    int (*send_linker_backup_data)(conf_object_t *dmc_linker, conf_object_t *dmc_device, uint16_t* buf, int length);
}dmc_linker_intf;
#define DMC_LINKER_INTF "dmc_linker_intf"

typedef struct{
    int (*send_epwm_timer)(conf_object_t *ecap_device, int prd);
}epwm_signal_intf;
#define EPWM_SIGNAL_INTF "epwm_signal_intf"

typedef struct{
    int (*start_adc_seq)(conf_object_t *adc_device, int soca_socb);
}epwm_adc_intf;

#define EPWM_ADC_INTF "epwm_adc_intf"

typedef struct{
    int (*start_sync)(conf_object_t *epwm_device);
}ecap_sync_intf;
#define ECAP_SYNC_INTF "ecap_sync_intf"

typedef struct{
    uint32_t ide;
    uint32_t id;
    uint32_t dlc;
    uint32_t data[4];
    uint32_t crc;
    uint32_t ack_slot;
}tsi578_msg_t;
typedef struct{
    int (*send_msg)(conf_object_t *tsi578_linker, conf_object_t *tsi578_device, tsi578_msg_t *msg);
}tsi578_linker_intf;
#define TSI578_LINKER_INTF "tsi578_linker_intf"

typedef struct{
    uint32_t desc[4];
}atm_msg_t;
typedef struct{
    int (*send_msg)(conf_object_t *atm_linker, conf_object_t *atm_device, atm_msg_t *msg);
    int (*send_data)(conf_object_t *atm_linker, conf_object_t *atm_device, void *buf, uint32_t len);
}atm_linker_intf;
#define ATM_LINKER_INTF "atm_linker_intf"

typedef struct Rs485_bus_interface{
    conf_object_t *conf_obj;
    int (*read_data)(conf_object_t* conf_obj, conf_object_t* device, void* recv_buf, uint32 length);
    void (*write_data)(conf_object_t* conf_obj, conf_object_t* device, void *buf, uint32 length);
    int (*register_device)(conf_object_t* conf_obj, conf_object_t* device);
    void (*unregister_device)(conf_object_t* conf_obj, conf_object_t* device);
}Rs485_bus_intf;

#define RS485_BUS_INTF "Rs485_bus_intf"

typedef struct Rs422_bus_interface{
    conf_object_t *conf_obj;
    int (*read_data)(conf_object_t* conf_obj, conf_object_t* device, void* recv_buf, uint32 length);
    void (*write_data)(conf_object_t* conf_obj, conf_object_t* device, void *buf, uint32 length);
    int (*register_device)(conf_object_t* conf_obj, conf_object_t* device);
    void (*unregister_device)(conf_object_t* conf_obj, conf_object_t* device);
}Rs422_bus_intf;

#define RS422_BUS_INTF "Rs422_bus_intf"

typedef struct io_linker_interface{
    conf_object_t *conf_obj;
    int (*recv_data)(conf_object_t* conf_obj, conf_object_t* device, void* recv_buf, int length, int io_id);
    void (*send_data)(conf_object_t* conf_obj, conf_object_t* device, void *buf, int length, int io_id);
}io_linker_intf;

#define IO_LINKER_INTF "io_linker_intf"

typedef struct ad_linker_interface{
    conf_object_t *conf_obj;
    void (*send_data)(conf_object_t* conf_obj, conf_object_t* device, void *buf, int length);
}ad_linker_intf;

#define AD_LINKER_INTF "ad_linker_intf"

#pragma pack(push,1)
typedef struct usim_pulse_command
{
    unsigned char    ucCmd;            /* 命令，1：初始化， 2：开始，3：停止 */
    unsigned char    ucMode;    /* 1：高电平有效，0：低电平有效 2:状态Io*/
    unsigned char    ucChNo;            /* 通道号: 0~255 */
    float            fPeriod;        /* 脉冲信号周期（s），单脉冲时取值-1 */
    float            fWidth;            /* 脉冲信号宽度（s） */
}pulse_msg_t;
#pragma pack(pop) 

typedef struct pulse_linker_interface{
    conf_object_t *conf_obj;
    void (*send_pulse)(conf_object_t* conf_obj, conf_object_t* device, pulse_msg_t *msg, int io_num, int io_id);
}pulse_linker_intf;

#define PULSE_LINKER_INTF "pulse_linker_intf"

typedef struct rs422_linker_interface{
    conf_object_t *conf_obj;
    void (*send_data)(conf_object_t* conf_obj, conf_object_t* device, void *buf, int length, int port_id);
}rs422_linker_intf;

#define RS422_LINKER_INTF "rs422_linker_intf"

typedef struct Rs232_bus_interface{
    conf_object_t *conf_obj;
    int (*read_data)(conf_object_t* conf_obj, conf_object_t* device, void* recv_buf, uint32 dev_id, uint32 length);
    void (*write_data)(conf_object_t* conf_obj, conf_object_t* device, void *buf, uint32 dev_id, uint32 length);
    int (*register_device)(conf_object_t* conf_obj, conf_object_t* device, uint8 dev_id);
    void (*unregister_device)(conf_object_t* conf_obj, conf_object_t* device, uint8 dev_id);
}Rs232_bus_intf;

#define RS232_BUS_INTF "Rs232_bus_intf"

typedef struct Lvds_bus_interface{
    conf_object_t *conf_obj;
    int (*read_data)(conf_object_t* conf_obj, conf_object_t* device, void* recv_buf, uint32 dev_id, uint32 length);
    void (*write_data)(conf_object_t* conf_obj, conf_object_t* device, void *buf, uint32 dev_id, uint32 length);
    int (*register_device)(conf_object_t* conf_obj, conf_object_t* device, uint8 dev_id);
    void (*unregister_device)(conf_object_t* conf_obj, conf_object_t* device, uint8 dev_id);
}Lvds_bus_intf;

#define LVDS_BUS_INTF "Lvds_bus_intf"

typedef struct arinc_bus_interface{
    conf_object_t *conf_obj;
    int (*read_data)(conf_object_t* conf_obj, conf_object_t* device, void* recv_buf, uint32 dev_id, uint32 length);
    void (*write_data)(conf_object_t* conf_obj, conf_object_t* device, void *buf, uint32 dev_id, uint32 length);
    int (*register_device)(conf_object_t* conf_obj, conf_object_t* device, uint8 dev_id);
    void (*unregister_device)(conf_object_t* conf_obj, conf_object_t* device, uint8 dev_id);
}arinc_bus_intf;

#define ARINC_BUS_INTF "arinc_bus_intf"

#include "skyeye_types.h"
typedef struct spi_dpram_linker_interface{
    int (*send_data)(conf_object_t *linker_obj, conf_object_t *dev_obj, void *buf, int len, int ID);
    int (*recv_data)(conf_object_t *obj, void *buf, int len, int ID);
}spi_dpram_linker_intf;
#define SPI_DPRAM_LINKER "spi_dpram_linker"

typedef struct fpga_gpio_connect_interface{
    int (*send_data)(conf_object_t *obj, void *buf, int length);
}fpga_gpio_connect_intf;
#define FPGA_GPIO_CONNECT "fpga_gpio_connect"

typedef enum{
    BC_TO_RT,  //0
    RT_TO_RT,  //1
    RT_TO_BC,  //2
    RT_TO_RT_BROADCAST, //3
    BROADCAST, //4
    MODE_CODE_NO_DATA, //5
    TX_MODE_CODE_WITH_DATA, //6
    RX_MODE_CODE_WITH_DATA, //7
    BROADCAST_MODE_CODE_NO_DATA, //8
    BROADCAST_MODE_CODE_WITH_DATA,//9
    //MESG_TYPE_CNT is not mesg type
    MESG_TYPE_CNT,
}mesg_type_t;

typedef enum{
    BC_MODE,
    MT_MODE,
    RT_MODE, 
}std_1553b_mode_t;

typedef enum{
    COMMAND_WORD, //0
    DATA_WORD,    //1
    LAST_DATA_WORD,//2
    LAST_DATA_STATUS_WORD,//3
    DATA_WORD_LOOPED_BACK,//4
    LAST_DATA_WORD_LOOPED_BACK,//5
    TRANSMIT_COMMAND_WORD_LOOPED_BACK,//6
    BROADCAST_MODE_COMMAND_LOOPED_BACK,//7
    MODE_COMMAND_WORD_LOOPED_BACK,//8
    STATUS_WORD,//9
    TX_RT_STATUS_WORD,//10
    RX_RT_STATUS_WORD,//11
    //WORD_TYPE_CNT is not word type
    WORD_TYPE_CNT,
    NLL,
}word_type_t;


typedef struct{
    word_type_t type;
    mesg_type_t mesg_type;
    uint16_t value;
}std_1553b_word_t;

typedef enum{
    RT_TO_RT_TRANSMIT,
    RT_TO_RT_RECEIVE,
}std_1553b_mesg_status_t;

typedef struct{
    mesg_type_t type;
    uint32_t word_cnt;
    uint32_t rtr_status;//RT->RT, rt use this flag to identification data from BC or another RT
    uint32_t bc_mark; // bc used this value to mark the location of the data block
    uint32_t rt_mark; // rt used this value to mark the location of the data block
    std_1553b_word_t words[64]; //use up to 37 words, RT-to-RT transfer of 32 data words: 2 commands + loopback + \
                                //2 status words + 32 data words = 37
}std_1553b_mesg_t;

typedef int position_t;
typedef struct{
    int (*bc_send_message)(conf_object_t *linker, std_1553b_mesg_t *message);  //LINKER implement the function
    int (*rt_receive_message)(conf_object_t *rt, std_1553b_mesg_t *message);   //RT implement the function
    int (*reponse_word)(conf_object_t *linker, conf_object_t *dev, std_1553b_word_t *words, uint32_t subaddress); //LINKER implement the function,it mark use subaddress 
    position_t (*bc_receive_word)(conf_object_t *bc, std_1553b_word_t *words, uint32_t bc_mark); //BC implement the function
    int (*register_address)(conf_object_t *linker, conf_object_t *dev, uint32_t address); //LINKER implement the function
    int (*set_mode)(conf_object_t *linker, conf_object_t *dev, std_1553b_mode_t mode); //LINKER implement the function
    int (*set_address)(conf_object_t *linker, conf_object_t *dev, uint16_t address);  //LINKER implement the function
    int (*send_external_message)(conf_object_t *linker, conf_object_t *dev, std_1553b_mesg_t *message);  //LINKER implement the function
    int (*bc_receive_message)(conf_object_t *rt, std_1553b_mesg_t *message);   //BC implement the function
}std_1553b_intf;
#define STD_1553B_INTF  "std_1553b_intf"

typedef struct{
    int (*send_message)(conf_object_t *obj, uint16_t *message, int count);
    int (*receive_message)(conf_object_t *obj, uint16_t *message, int count);
}external_1553b_intf;
#define EXTERNAL_1553B_INTF  "external_1553b_intf"

typedef struct{
    int (*write) (conf_object_t *obj, char* buf, size_t count);
    int (*read) (conf_object_t *obj, char* buf);
}data_inject_intf;
#define DATA_INJECT_INTF "data_inject_intf"

typedef struct{
    char* buffer;
    uint32_t length;
}rs422_msg_t;

#define DATA_LENGTH 1024
typedef struct{
    uint32_t data_length;
    uint32_t sub_addr;
    uint32_t rt_addr;
    uint32_t tx_rx_flag; //tx = 1, rx = 0
    uint32_t rx_bx_flag; //bx = 1, rx = 0
    uint16_t buffer[DATA_LENGTH];
    int count;
}network_mesg_t;

typedef struct{
    int (*receive_network_mesg)(conf_object_t *opaque, network_mesg_t* message);
    int (*send_network_mesg)(conf_object_t *opaque, network_mesg_t* message);
}network_bus_intf;
#define NETWORK_BUS_INTF  "network_bus_intf"

typedef struct{
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t command;
    uint16_t status;
    uint32_t revision_id : 8;
    uint32_t class_code : 24;
    uint8_t cacheline_size;
    uint8_t latency_timer;
    uint8_t header_type;
    uint8_t bist;
    uint32_t bar[MAX_BAR_NUM];
    uint32_t cardbus_cis_ptr;
    uint16_t sub_vendor_id;
    uint16_t sub_system_id;
    uint32_t expan_rom_base;
    uint32_t  capabilities_ptr : 8;
    uint32_t reserved0 : 24;
    uint32_t reserved1;
    uint8_t inter_line;
    uint8_t inter_pin;
    uint8_t min_gnt;
    uint8_t max_lat;
    //uint8_t local_config_mem[192];//256B- 64B
    //uint8_t extend_config_space[3840];//4k - 256B
}pcie_config_space_t;

typedef struct{
    conf_object_t *pcie_ep_obj;
    uint32_t bus_num;
    uint32_t dev_num;
    uint32_t fun_num;
    pcie_config_space_t ep_config_space;
    uint32_t mem_length[MAX_BAR_NUM];
}pcie_msg_t;

typedef struct
{
    int (*get_config_info)(conf_object_t *obj, pcie_msg_t * pcie_dev_info, int dev_cnt);
}pcie_config_intf;
#define PCIE_CONFIG_INTF "pcie_config_intf"

typedef struct
{
    int (*write_pcie_data)(conf_object_t *obj, int dev_index, int bar_index, generic_address_t offset, void *buf, size_t count);
    int (*read_pcie_data)(conf_object_t *obj, int dev_index, int bar_index, generic_address_t offset, void *buf, size_t count);
}pcie_bus_intf;
#define PCIE_BUS_INTF "pcie_bus_intf"
#endif
