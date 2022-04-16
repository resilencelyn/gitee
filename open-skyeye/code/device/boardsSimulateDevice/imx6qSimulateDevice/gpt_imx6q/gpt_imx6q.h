#ifndef __GPT_IMX6Q_H__ 
#define __GPT_IMX6Q_H__ 
struct registers 
{
	
	uint32_t gpt_cr;
	
	uint32_t gpt_pr;
	
	uint32_t gpt_sr;
	
	uint32_t gpt_ir;
	
	uint32_t gpt_ocr1;
	
	uint32_t gpt_ocr2;
	
	uint32_t gpt_ocr3;
	
	uint32_t gpt_icr1;
	
	uint32_t gpt_icr2;
	
	uint32_t gpt_cnt;
	
};


struct imx6q_device 
{
	
	conf_object_t* obj;
	
	core_signal_intf* signal_iface;

	conf_object_t* general_core;
	general_signal_intf* general_signal;
	
	conf_object_t* signal;
	
	struct registers regs;

	time_handle_t cnt_timer;
	
};
typedef struct imx6q_device gpt_imx6q_device;
static char* regs_name[] = 
{
	"GPT_CR",
	"GPT_PR",
	"GPT_SR",
	"GPT_IR",
	"GPT_OCR1",
	"GPT_OCR2",
	"GPT_OCR3",
	"GPT_ICR1",
	"GPT_ICR2",
	"GPT_CNT",
	NULL
};
#define GPT_CR 0x0
#define GPT_PR 0x4
#define GPT_SR 0x8
#define GPT_IR 0xC
#define GPT_OCR1 0x10
#define GPT_OCR2 0x14
#define GPT_OCR3 0x18
#define GPT_ICR1 0x1C
#define GPT_ICR2 0x20
#define GPT_CNT 0x24
#endif  
