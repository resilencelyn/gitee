/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* --- Web: www.STCMCUDATA.com  ---------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序            */
/*---------------------------------------------------------------------*/

#include "config.h"
#include "FreeMQ.h"
#include "GPIO.h"
#include "Timer.h"

//打开加热器
#define LEDOn() P12 = 1
//关闭加热器
#define LEDOff() P12 = 0

/*************	功能说明	**************

FreePrint3D打印机空气管理器

******************************************/

/*************	本地常量声明	**************/

//所有事件
typedef enum
{
	MSG_START = 1, //启动
	MSG_MainLoop,  //主循环
	MSG_LEDOn,	   //灯打开
	MSG_LEDOff,	   //灯关闭
} MESSAGESLIST;

/******************* IO配置函数 *******************/
void GPIO_config(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.Pin = GPIO_Pin_All;		//指定要初始化的IO, GPIO_Pin_0 ~ GPIO_Pin_7
	GPIO_InitStructure.Mode = GPIO_PullUp;		//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P1, &GPIO_InitStructure); //初始化
}

/***************定时器初始化与中断*************************/

void Timer0Init(void) // 1毫秒@24.000MHz
{
	Timer0_Stop();								 //停止计数
	TMOD = (TMOD & ~0x03) | TIM_16BitAutoReload; //工作模式,0: 16位自动重装, 1: 16位定时/计数, 2: 8位自动重装, 3: 不可屏蔽16位自动重装
	Timer0_1T();
	Timer0_CLKO_Disable(); //输出时钟使能
	T0_Load(65536UL - (MAIN_Fosc / 1000UL));
	Timer0_Run();
	Timer0_InterruptEnable();
}

void Timer0IntHandle(void) interrupt TIMER0_VECTOR
{
	FreeMQMainTick(1);
}

//主循环
void MainLoopRun(void)
{
	//主要要做的事情
}

//启动后第一次运行
void Startup(void)
{
	//初始化过程
	LEDOff();
	FreeMQSendMessageToMainLater(MSG_MainLoop, 500);
}

/**********************消息队列处理器**********************/

void FreeMQMsgHandle(u8 msgid)
{
	switch (msgid)
	{
	case MSG_MainLoop: //主循环
		MainLoopRun();
		//喂看门狗
		FreeMQSendMessageToMainLater(MSG_MainLoop, 500); //主循环每500ms循环一次
		break;
	case MSG_START: //程序启动
		Startup();
		break;
	case MSG_LEDOn: //打开LED
		LEDOn();
		FreeMQSendMessageToMainLater(MSG_LEDOff, 1000);
		break;
	case MSG_LEDOff: //关闭LED
		LEDOff();
		FreeMQSendMessageToMainLater(MSG_LEDOn, 500);
		break;
	}
}

/********************主函数*********************/
void main(void)
{
	FreeMQMainInit(&FreeMQMsgHandle);
	GPIO_config();
	Timer0Init();
	EA = 1;

	FreeMQSendMessageToMainLater(MSG_START, 10);
	while (1)
	{
		FreeMQMainLoop();
	}
}