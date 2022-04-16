#ifndef __FT2000A_SPI_H__
#define __FT2000A_SPI_H__

#define MAX_SLAVES 8

struct spi_registers
{
    uint32_t spi_conf_reg;					//配置访问数据空间时产生的读写命令、SCK 分频等信息
    uint32_t wp_reg;						//控制写保护位的输出
    uint32_t cs_delay_reg;					//CS 延迟寄存器
    uint32_t set_flash_capacity_reg;		//Flash 容量设置寄存器
    uint32_t flush_fifo_reg;				//写缓冲 flush 寄存器
    uint32_t cmd_port_reg;					//命令端口寄存器
    uint32_t addr_port_reg;					//地址端口寄存器
    uint32_t high_data_reg;					//高位数据寄存器
    uint32_t low_data_reg;					//低位数据寄存器
    uint32_t cs0_cfg_addr;					//CS0配置地址寄存器
};

struct ft2000a_spi_device_t
{
    conf_object_t* obj;
    struct spi_registers regs;
    conf_object_t* image;
    memory_space_intf* image_space;
    struct
    {
        conf_object_t *slave_obj;
        spi_device_intf *spi_dev_iface;
    }slaves[MAX_SLAVES];
    char *filename;
    FILE* wr_fp;
    FILE* rd_fp;
    int skyeye_debug;
};

typedef struct ft2000a_spi_device_t ft2000a_spi_device;

static char* regs_name[] =
{
    "spi_conf_reg",
    "wp_reg",
    "cs_delay_reg",
    "set_flash_capacity_reg",
    "flush_fifo_reg",
    "cmd_port_reg",
    "addr_port_reg",
    "high_data_reg",
    "low_data_reg",
    "cs0_cfg_addr",
    NULL
};

#define SPI_CONFIG     0x1fffff00
#define WP_REG         0x1fffff0c
#define CS_DELAY       0x1fffff10
#define SFC_REG        0x1fffff14
#define FLUSH_FIFO     0x1fffff18
#define CMD_PORT       0x1fffff20
#define ADDR_PORT      0x1fffff24
#define HIGH_DATA_PORT 0x1fffff28
#define LOW_DATA_PORT  0x1fffff2c
#define CS0_CFG_ADDR   0x1fffff30

static uint32_t regs_offset[] =
{
    SPI_CONFIG,
    WP_REG,
    CS_DELAY,
    SFC_REG,
    FLUSH_FIFO,
    CMD_PORT,
    ADDR_PORT,
    HIGH_DATA_PORT,
    LOW_DATA_PORT,
    CS0_CFG_ADDR,
};

#define SPI_CFG_CMD_PP              (0x02 << 8)
#define SPI_CFG_CMD_READ_ID         (0x90 << 8)
#define SPI_CFG_CMD_RDID            (0x9F << 8)
#define SPI_CFG_CMD_RSSR            (0x05 << 8)
#define SPI_CFG_CMD_SE_64KB         (0xD8 << 8)
#define SPI_CFG_CMD_WREN            (0x06 << 8)
#define SPI_CFG_CMD_READ            (0x03 << 8)
#define SPI_CFG_ADDR_MODE           (0x01 << 6)
#define SPI_CFG_ADDR_CNT_3          (0x00 << 5)
#define SPI_CFG_ADDR_CNT_4          (0x01 << 5)
#define SPI_CFG_DUMMY_SEND          (0x01 << 4)
#define SPI_CFG_RW_NUM_0            (0x00)
#define SPI_CFG_RW_NUM_1            (0x01)
#define SPI_CFG_RW_NUM_2            (0x02)
#define SPI_CFG_RW_NUM_4            (0x04)
#define SPI_CFG_RW_NUM_8            (0x08)
#define SPI_ACCESS_CMD_MODE_MASK    (0x03 << 6)
#define SPI_ACCESS_ADDR_MODE_MASK   (0x01 << 5)

#define LEN_64K         (64*1024)

#define SKYEYE_DEBUG(args...) {if (dev->skyeye_debug) {printf(args);fflush(stdout);}}
#endif
