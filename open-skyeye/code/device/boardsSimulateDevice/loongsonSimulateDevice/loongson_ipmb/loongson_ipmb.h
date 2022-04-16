#ifndef __LOONGSON_IPMB_H__
#define __LOONGSON_IPMB_H__

// All equipment register names, the list order and struct registers registers inside order is kept consistent

#define AD_CHANNEL			23
#define DA_CHANNEL			14
#define WARN_CHANNEL		39
#define ANA_CHANNEL			34
#define THRESHOD_CHANNEL	33
#define THRE_CHANNEL		36

#define AD_TYPE				0x1
#define DA_TYPE				0x2
#define WARN_TYPE			0x3
#define THRESHOLD_TYPE		0x4
#define ANADATA_GET_TYPE	0x5

#define TYPE_MAX			0x5
static char* regs_name[] =
{
	NULL
};
struct registers
{
	uint32_t ad_data[AD_CHANNEL];
	uint32_t da_data[DA_CHANNEL];
	uint32_t warn_data[WARN_CHANNEL];
	uint32_t thre_data[THRE_CHANNEL];
	uint32_t ana_data[ANA_CHANNEL];
	uint32_t busy_status;
	uint32_t threshod_reg[THRESHOD_CHANNEL];
	uint32_t poweroff_reg;
	uint32_t readFlag_reg;
};

// Device object structure
struct loongson_ipmb_device_t
{
	conf_object_t* obj;
	// Interface objects and methods;
	struct registers regs;
	ad_linker_intf *ad_linker_iface;
	conf_object_t *ad_linker;
	uint32_t dataType;
	uint32_t cnt;
};
typedef struct loongson_ipmb_device_t loongson_ipmb_device;

#define EXTEND(i)					((i - 1) * 2)
#define U8_TO_U32(val,i)			((val[EXTEND(i) + 1] << 24)\
									|(val[EXTEND(i) + 2] << 16)\
									|(val[EXTEND(i) + 3] <<  8)\
									|(val[EXTEND(i) + 4] <<  0))
#define U8_TO_U16(val,i)			((uint16_t)(val[EXTEND(i) + 0] << 8)\
									|(val[EXTEND(i) + 1] << 0))

#define IS_IPMB_RECV_MASK(value)	(!((value >> 8) & 0x1))

#define bitsmask(n, m) ((~(uint32_t)0 ) >> (m) << (31 - (n) + (m)) >> (31 - (n)))
#define clear_bits(x, n, m) ((x) & ~bitsmask((n), (m)))

#define THRESHOD_REG 0x0
#define POWEROFF_REG 0x0
#endif
