#ifndef __SERIAL_LINUX__
#define __SERIAL_LINUX__

#ifndef __MINGW32__
int UART_Open(int fd,char* port);
void UART_Close(int fd);
int UART_Set(int fd,int speed,int flow_ctrl,int databits,int stopbits,int parity);
int UART_Init(int fd, int speed,int flow_ctrlint ,int databits,int stopbits,char parity);
int UART_Recv(int fd, char *rcv_buf,int data_len);
int UART_Send(int fd, char *send_buf,int data_len);
#else
HANDLE  WIN_UART_Open(char* port);
void WIN_UART_Close(HANDLE fd);
int  WIN_UART_Set(HANDLE fd, int speed,int flow_ctrlint ,int databits,int stopbits,char parity);
DWORD  WIN_UART_Recv(HANDLE fd, char *rcv_buf,int data_len);
DWORD  WIN_UART_Send(HANDLE fd, char *send_buf,int data_len);
#endif
#endif
