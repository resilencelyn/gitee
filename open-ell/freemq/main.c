/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* --- Web: www.STCMCUDATA.com  ---------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����            */
/*---------------------------------------------------------------------*/

#include "config.h"
#include "FreeMQ.h"
#include "GPIO.h"
#include "Timer.h"

//�򿪼�����
#define LEDOn() P12 = 1
//�رռ�����
#define LEDOff() P12 = 0

/*************	����˵��	**************

FreePrint3D��ӡ������������

******************************************/

/*************	���س�������	**************/

//�����¼�
typedef enum
{
	MSG_START = 1, //����
	MSG_MainLoop,  //��ѭ��
	MSG_LEDOn,	   //�ƴ�
	MSG_LEDOff,	   //�ƹر�
} MESSAGESLIST;

/******************* IO���ú��� *******************/
void GPIO_config(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.Pin = GPIO_Pin_All;		//ָ��Ҫ��ʼ����IO, GPIO_Pin_0 ~ GPIO_Pin_7
	GPIO_InitStructure.Mode = GPIO_PullUp;		//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P1, &GPIO_InitStructure); //��ʼ��
}

/***************��ʱ����ʼ�����ж�*************************/

void Timer0Init(void) // 1����@24.000MHz
{
	Timer0_Stop();								 //ֹͣ����
	TMOD = (TMOD & ~0x03) | TIM_16BitAutoReload; //����ģʽ,0: 16λ�Զ���װ, 1: 16λ��ʱ/����, 2: 8λ�Զ���װ, 3: ��������16λ�Զ���װ
	Timer0_1T();
	Timer0_CLKO_Disable(); //���ʱ��ʹ��
	T0_Load(65536UL - (MAIN_Fosc / 1000UL));
	Timer0_Run();
	Timer0_InterruptEnable();
}

void Timer0IntHandle(void) interrupt TIMER0_VECTOR
{
	FreeMQMainTick(1);
}

//��ѭ��
void MainLoopRun(void)
{
	//��ҪҪ��������
}

//�������һ������
void Startup(void)
{
	//��ʼ������
	LEDOff();
	FreeMQSendMessageToMainLater(MSG_MainLoop, 500);
}

/**********************��Ϣ���д�����**********************/

void FreeMQMsgHandle(u8 msgid)
{
	switch (msgid)
	{
	case MSG_MainLoop: //��ѭ��
		MainLoopRun();
		//ι���Ź�
		FreeMQSendMessageToMainLater(MSG_MainLoop, 500); //��ѭ��ÿ500msѭ��һ��
		break;
	case MSG_START: //��������
		Startup();
		break;
	case MSG_LEDOn: //��LED
		LEDOn();
		FreeMQSendMessageToMainLater(MSG_LEDOff, 1000);
		break;
	case MSG_LEDOff: //�ر�LED
		LEDOff();
		FreeMQSendMessageToMainLater(MSG_LEDOn, 500);
		break;
	}
}

/********************������*********************/
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