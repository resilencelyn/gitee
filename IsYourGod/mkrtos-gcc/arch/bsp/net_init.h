#ifndef _NET_INIT_H__
#define _NET_INIT_H__
#include "sys.h"
//lwip���ƽṹ��
typedef struct  
{
	u8 mac[6];      //MAC��ַ
	u8 remoteip[4];	//Զ������IP��ַ 
	u8 ip[4];       //����IP��ַ
	u8 netmask[4]; 	//��������
	u8 gateway[4]; 	//Ĭ�����ص�IP��ַ
	
	vu8 dhcpstatus;	//dhcp״̬ 
					//0,δ��ȡDHCP��ַ;
					//1,����DHCP��ȡ״̬
					//2,�ɹ���ȡDHCP��ַ
					//0XFF,��ȡʧ��.
}__lwip_dev;
extern __lwip_dev lwipdev;	//lwip���ƽṹ��

u8 lwip_comm_init(void);

#endif
