
///************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

#define SYS_LIGHTWEIGHT_PROT    		1							//Ϊ1ʱʹ��ʵʱ����ϵͳ������������,�����ؼ����벻���жϴ��
#define NO_SYS                  		0  							//ʹ��UCOS����ϵͳ
#define MEM_ALIGNMENT           		4  							//ʹ��4�ֽڶ���ģʽ
#define MEM_SIZE                		(64*1024)						//�ڴ��heap��С
#define MEMP_NUM_PBUF           		16							//MEMP_NUM_PBUF:memp�ṹ��pbuf����,���Ӧ�ô�ROM���߾�̬�洢�����ʹ�������ʱ,���ֵӦ�����ô�һ��
#define MEMP_NUM_UDP_PCB        		5							//MEMP_NUM_UDP_PCB:UDPЭ����ƿ�(PCB)����.ÿ�����UDP"����"��Ҫһ��PCB.
#define MEMP_NUM_TCP_PCB        		8							//MEMP_NUM_TCP_PCB:ͬʱ���������TCP����
#define MEMP_NUM_TCP_PCB_LISTEN 		7							//MEMP_NUM_TCP_PCB_LISTEN:�ܹ�������TCP��������
#define MEMP_NUM_TCP_SEG        		20							//MEMP_NUM_TCP_SEG:���ͬʱ�ڶ����е�TCP������
#define MEMP_NUM_SYS_TIMEOUT    		8							//MEMP_NUM_SYS_TIMEOUT:�ܹ�ͬʱ�����timeout����

//pbufѡ��
#define PBUF_POOL_SIZE          		25							//PBUF_POOL_SIZE:pbuf�ڴ�ظ���
#define PBUF_POOL_BUFSIZE     			1528						//PBUF_POOL_BUFSIZE:ÿ��pbuf�ڴ�ش�С

#define LWIP_TCP                		1  							//ʹ��TCP
#define TCP_TTL                			255							//����ʱ��

#undef TCP_QUEUE_OOSEQ
#define TCP_QUEUE_OOSEQ         		0 							//��TCP�����ݶγ�������ʱ�Ŀ���λ,���豸���ڴ��С��ʱ�����ӦΪ0

#undef TCPIP_MBOX_SIZE
#define TCPIP_MBOX_SIZE         		64   		//tcpip�������߳�ʱ����Ϣ�����С

#undef DEFAULT_TCP_RECVMBOX_SIZE
#define DEFAULT_TCP_RECVMBOX_SIZE       64

#undef DEFAULT_ACCEPTMBOX_SIZE
#define DEFAULT_ACCEPTMBOX_SIZE         64


#define TCP_MSS                			(1500 - 40)	  				//���TCP�ֶ�,TCP_MSS = (MTU - IP��ͷ��С - TCP��ͷ��С
#define TCP_SND_BUF            		 	(2*TCP_MSS)					//TCP���ͻ�������С(bytes).
#define TCP_SND_QUEUELEN       		 	(4* TCP_SND_BUF/TCP_MSS)	//TCP_SND_QUEUELEN: TCP���ͻ�������С(pbuf).���ֵ��СΪ(2 * TCP_SND_BUF/TCP_MSS)
#define TCP_WND               		  	(4*TCP_MSS)					//TCP���ʹ���
#define LWIP_ICMP             		  	1 							//ʹ��ICMPЭ��
#define LWIP_DHCP             		  	0							//ʹ��DHCP
#define LWIP_DNS                 			1
#define LWIP_UDP              		  	1 							//ʹ��UDP����
#define UDP_TTL               		  	255 						//UDP���ݰ�����ʱ��
#define LWIP_STATS 						0
#define LWIP_PROVIDE_ERRNO 				1


#define LWIP_NETCONN                    1 							//LWIP_NETCONN==1:ʹ��NETCON����(Ҫ��ʹ��api_lib.c)
#define LWIP_SOCKET                     1							//LWIP_SOCKET==1:ʹ��Sicket API(Ҫ��ʹ��sockets.c)
#define LWIP_COMPAT_MUTEX               1
#define LWIP_SO_RCVTIMEO                1 							//ͨ������LWIP_SO_RCVTIMEOʹ��netconn�ṹ����recv_timeout,ʹ��recv_timeout���Ա��������߳�
#define LWIP_SO_SNDTIMEO                1
//�й�ϵͳ��ѡ��
#define TCPIP_THREAD_PRIO								6							//�����ں���������ȼ�Ϊ5
#define TCPIP_THREAD_STACKSIZE          638						//�ں������ջ��С
#define DEFAULT_UDP_RECVMBOX_SIZE       2000
#define DEFAULT_THREAD_STACKSIZE        512

//LWIP����ѡ��
#define LWIP_DEBUG                    	0 						//�ر�DEBUGѡ��
#define SOCKETS_DEBUG                   0


#define LWIP_COMPAT_MUTEX 1
#define LWIP_COMPAT_MUTEX_ALLOWED
//#define LWIP_SKIP_CONST_CHECK

#define ETHARP_TRUST_IP_MAC     1
#define IP_REASSEMBLY           1
#define IP_FRAG                 1
#define ARP_QUEUEING            1
#define LWIP_DNS_SECURE					0

#define LWIP_COMPAT_SOCKETS             0
#define LWIP_POSIX_SOCKETS_IO_NAMES     0

#define LWIP_TCPIP_CORE_LOCKING_INPUT   1

#define LWIP_NETCONN_FULLDUPLEX         0
#define LWIP_NETCONN_SEM_PER_THREAD     0
#define TCP_LISTEN_BACKLOG              1
#define LWIP_TIMEVAL_PRIVATE 0

#define MEMP_MEM_MALLOC                 1

//#define LWIP_TCPIP_CORE_LOCKING 0
#endif /* __LWIPOPTS_H__ */













