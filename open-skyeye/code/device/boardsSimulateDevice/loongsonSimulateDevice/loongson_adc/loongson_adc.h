#ifndef __LOONGSON_ADC_H__
#define __LOONGSON_ADC_H__

// All equipment register names, the list order and struct registers registers inside order is kept consistent
static char* regs_name[] =
{
	"ad_base0",
	"ad_base1",
	"ad_base2",
	"ad_base3",
	"ad_base4",
	"ad_base5",
	"ad_base6",
	"ad_base7",
	"ad_base8",
	"ad_base9",
	"ad_base10",
	"ad_base11",
	"ad_base12",
	"ad_base13",
	"ad_base14",
	"ad_base15",
	"ad_base16",
	"ad_base17",
	"ad_base18",
	"ad_base19",
	"ad_base20",
	"ad_base21",
	"ad_base22",
	"ad_base23",
	"ad_base24",
	"ad_base25",
	"ad_base26",
	"ad_base27",
	"ad_base28",
	"ad_base29",
	"ad_base30",
	"ad_base31",
	"ad_base32",
	"ad_base33",
	NULL
};

// Register offset
#define AD_BASE 0x0
#define AD_END  0x88
#define ADC_CHANNELS 34
// device register structure
struct registers
{
	uint32_t ad_base[34];
};


// Device object structure
struct loongson_adc_device_t
{
	
	conf_object_t* obj;
	
	// Interface objects and methods;
	
	struct registers regs;
	FILE* fp;
	int read_flag;
	
};
typedef struct loongson_adc_device_t loongson_adc_device;

#endif
