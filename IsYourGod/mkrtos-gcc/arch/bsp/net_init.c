#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "netif/ethernetif.h" 
#include "lwip/ip4_addr.h"
#include "lwip/dns.h"
#include "lwip/tcpip.h" 

#include "sys.h"
#include "dm9000.h"
#include "mkrtos/mem.h"
#include <mkrtos/knl_mutex.h>
#include <mkrtos/knl_sem.h>
#include "net_init.h"
#include <arch/isr.h>

__lwip_dev lwipdev;						//lwip���ƽṹ�� 
struct netif lwip_netif;				//����һ��ȫ�ֵ�����ӿ�

volatile struct sem_hdl *dm9000input;					//DM9000���������ź���
volatile struct mutex_hdl *dm9000lock;					//DM9000��д���������ź���

//DM9000���ݽ��մ�������
void lwip_dm9000_input_task(void *pdata)
{

	//�����绺�����ж�ȡ���յ������ݰ������䷢�͸�LWIP���� 
	ethernetif_input(&lwip_netif);
}

//lwip Ĭ��IP����
//lwipx:lwip���ƽṹ��ָ��
void lwip_comm_default_ip_set(__lwip_dev *lwipx)
{
	//Ĭ��Զ��IPΪ:192.168.1.106
	lwipx->remoteip[0]=192;	
	lwipx->remoteip[1]=168;
	lwipx->remoteip[2]=3;
	lwipx->remoteip[3]=1;
	//MAC��ַ����(�����ֽڹ̶�Ϊ:2.0.0,�����ֽ���STM32ΨһID)
	lwipx->mac[0]=dm9000cfg.mac_addr[0];
	lwipx->mac[1]=dm9000cfg.mac_addr[1];
	lwipx->mac[2]=dm9000cfg.mac_addr[2];
	lwipx->mac[3]=dm9000cfg.mac_addr[3];
	lwipx->mac[4]=dm9000cfg.mac_addr[4];
	lwipx->mac[5]=dm9000cfg.mac_addr[5]; 
	//Ĭ�ϱ���IPΪ:192.168.1.30
	lwipx->ip[0]=192;	
	lwipx->ip[1]=168;
	lwipx->ip[2]=3;
	lwipx->ip[3]=120;
	//Ĭ����������:255.255.255.0
	lwipx->netmask[0]=255;	
	lwipx->netmask[1]=255;
	lwipx->netmask[2]=255;
	lwipx->netmask[3]=0;
	//Ĭ������:192.168.1.1
	lwipx->gateway[0]=192;	
	lwipx->gateway[1]=168;
	lwipx->gateway[2]=3;
	lwipx->gateway[3]=1;	
	lwipx->dhcpstatus=0;//û��DHCP	
}



//extern u8 MempPoolsInit(void);
//lwip�ں˲���,�ڴ�����
//����ֵ:0,�ɹ�;
//    ����,ʧ��
u8 lwip_comm_mem_malloc(void)
{
	
    extern const uint32_t lwip_heap_size;
    extern uint8_t *ram_heap;
	ram_heap=OSMalloc(lwip_heap_size);	//������ʱ��OS���ڴ����뺯��,Ϊram_heap�����ڴ�
	if(!ram_heap){//������ʧ�ܵ�
		return 1;
	}
//	if(MempPoolsInit()==0){
//		//�ڴ�����ʧ��
//		return 1;
//	}
	return 0;	
}
int lwip_hard_init(void){
    dm9000input=sem_create(0,1);			//�������ݽ����ź���,������DM9000��ʼ��֮ǰ����
    dm9000lock=mutex_create();			//���������ź���,��ߵ����ȼ�4
    if(DM9000_Init(1)){
        return -1;			//��ʼ��DM9000AEP
    }
    return 0;
}

//struct sem_hdl *sem0;
//struct sem_hdl *sem1;
//pspinlock_handler sh;
struct msg_hdl *msg0;
struct msg_hdl *msg1;
void kel_thread_test(void *arg){
    char data[10];
    while(1){

        msg_put(msg0,"12345678\n",0xffffffff);
        msg_get(msg1,data,0xffffffff);
//        printk("thread 0 try lock.\r\n");
//        spin_lock(sh);
//        printk("thread 0 run\r\n");
//        spin_unlock(sh);
//        printk("thread 0 unlock.\r\n");
//        sem_pend(sem0,0xffffffff);
//        printk("thread 0 sleep\r\n");
//        sleep(1);
//        printk("thread 0 run\r\n");
//        sem_post(sem0);
//        sleep(1);
    }
}
void kel_thread_test1(void *arg){
    char data[10];
    while(1){
        msg_get(msg0,data,0xffffffff);
        msg_put(msg1,"87654321\n",0xffffffff);
        printf(data);
//        printk("thread 1 try lock.\r\n");
//        spin_lock(sh);
//        printk("thread 1 run\r\n");
//        spin_unlock(sh);
//        printk("thread 1 unlock.\r\n");
//        sleep(1);
//        sem_pend(sem0,0xffffffff);
//        printk("thread 1 sleep\r\n");
//        sleep(2);
//        printk("thread 1 run\r\n");
//        sem_post(sem0);
//        sleep(1);
    }
}
//LWIP��ʼ��(LWIP������ʱ��ʹ��)
//����ֵ:0,�ɹ�
//      1,�ڴ����
//      2,DM9000��ʼ��ʧ��
//      3,�������ʧ��.
u8 lwip_comm_init(void)
{
	uint32_t lev;
	u8 err;
	struct netif *Netif_Init_Flag;		//����netif_add()����ʱ�ķ���ֵ,�����ж������ʼ���Ƿ�ɹ�
	struct ip4_addr ipaddr;  			//ip��ַ
	struct ip4_addr netmask; 			//��������
	struct ip4_addr gw;      			//Ĭ������ 

    if(lwip_comm_mem_malloc()){
        return 1;
    }

	tcpip_init(NULL,NULL);				//��ʼ��tcp ip�ں�,�ú�������ᴴ��tcpip_thread�ں�����
	lwip_comm_default_ip_set(&lwipdev);	//����Ĭ��IP����Ϣ
#if LWIP_DHCP		//ʹ�ö�̬IP
	ipaddr.addr = 0;
	netmask.addr = 0;
	gw.addr = 0;
#else
	IP4_ADDR(&ipaddr,lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
	IP4_ADDR(&netmask,lwipdev.netmask[0],lwipdev.netmask[1] ,lwipdev.netmask[2],lwipdev.netmask[3]);
	IP4_ADDR(&gw,lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);

	printk("����en��MAC��ַΪ:................%d.%d.%d.%d.%d.%d\r\n",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);
    printk("��̬IP��ַ........................%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
    printk("��������..........................%d.%d.%d.%d\r\n",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);
    printk("Ĭ������..........................%d.%d.%d.%d\r\n",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);

#endif
	Netif_Init_Flag=netif_add(&lwip_netif,&ipaddr,&netmask,&gw,NULL,&ethernetif_init,&tcpip_input);//�������б������һ������
	if(Netif_Init_Flag != NULL) 	//������ӳɹ���,����netifΪĬ��ֵ,���Ҵ�netif����
	{
		netif_set_default(&lwip_netif); //����netifΪĬ������
		netif_set_up(&lwip_netif);		//��netif����
	}
	
#if	LWIP_DHCP
	lwip_comm_dhcp_creat();			//����DHCP����
#endif	
	
	ip_addr_t dns_addr;
	IP_ADDR4(&dns_addr,223,5,5,5);
	dns_setserver(0, &dns_addr);

    extern sys_thread_t sys_thread_new(const char *name, lwip_thread_fn function, void *arg, int stacksize, int prio);

    sys_thread_new("net recv thread",lwip_dm9000_input_task,0,512*4,6);

//    sem0= sem_create(1,1);
//    sem1= sem_create(1,1);
//    sh=spin_lock_create();
//    msg0= msg_create(32,10);
//    msg1= msg_create(32,10);
//    sys_thread_new("net recv thread",kel_thread_test,0,512*4,6);
//    sys_thread_new("net recv thread",kel_thread_test1,0,512*4,6);
//


	return 0;//����OK.
}   
