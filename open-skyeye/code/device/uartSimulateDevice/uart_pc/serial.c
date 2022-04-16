#include<stdio.h>      
#include<stdlib.h>     
#include<unistd.h>     
#include<sys/types.h>  
#include<sys/stat.h>   
#include<fcntl.h>      
#include<termios.h>    
#include<errno.h>      
#include<string.h>

#ifndef __MINGW32__
#else
#include <windows.h>
#endif




#ifndef __MINGW32__

#define FALSE  -1
#define TRUE   0
int UART_Open(int fd,char* port)
{
	fd = open( port, O_RDWR|O_NOCTTY|O_NDELAY);
	if (FALSE == fd){
		perror("Can't Open Serial Port");
		return(FALSE);
	}
	if(fcntl(fd, F_SETFL, 0) < 0){
		printf("fcntl failed!\n");
		return(FALSE);
	} else {
		//	printf("fcntl=%d\n",fcntl(fd, F_SETFL,0));
	}
	if(0 == isatty(STDIN_FILENO)){
		printf("standard input is not a terminal device\n");
		return(FALSE);
	}
	return fd;
}
void UART_Close(int fd)
{
	close(fd);
}


int UART_Set(int fd,int speed,int flow_ctrl,int databits,int stopbits,int parity)
{ 

	int   i; 
	int   speed_arr[] = { B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300
	};
	int   name_arr[] = {
		115200, 38400,  19200,  9600,  4800,  2400,  1200,  300  
	};  
	struct termios options; 


	if(tcgetattr( fd,&options)  !=  0){  
		perror("SetupSerial 1");     
		return(FALSE);  
	}
	for(i= 0;i < sizeof(speed_arr) / sizeof(int);i++) {
		if  (speed == name_arr[i]) {        
			cfsetispeed(&options, speed_arr[i]);  
			cfsetospeed(&options, speed_arr[i]);   
		}
	}	 
	options.c_cflag |= CLOCAL;
	options.c_cflag |= CREAD;
	switch(flow_ctrl){
		case 0 :
			options.c_cflag &= ~CRTSCTS;
			break;	
		case 1 :
			options.c_cflag |= CRTSCTS;
			break;
		case 2 :
			options.c_cflag |= IXON | IXOFF | IXANY;
			break;
	}

	options.c_cflag &= ~CSIZE; 
	switch (databits){   
		case 5 :
			options.c_cflag |= CS5;
			break;
		case 6	:
			options.c_cflag |= CS6;
			break;
		case 7	:     
			options.c_cflag |= CS7; 
			break;
		case 8:     
			options.c_cflag |= CS8;
			break;   
		default:    
			fprintf(stderr,"Unsupported data size\n"); 
			return (FALSE);
	}
	switch (parity) {   
		case 'n':
		case 'N': 
			options.c_cflag &= ~PARENB;  
			options.c_iflag &= ~INPCK;     
			break;  
		case 'o':   
		case 'O':    
			options.c_cflag |= (PARODD | PARENB);  
			options.c_iflag |= INPCK;              
			break;  
		case 'e':  
		case 'E':   
			options.c_cflag |= PARENB;        
			options.c_cflag &= ~PARODD;        
			options.c_iflag |= INPCK;       
			break;
		case 's': 
		case 'S': 
			options.c_cflag &= ~PARENB;
			options.c_cflag &= ~CSTOPB;
			break;  
		default:   
			fprintf(stderr,"Unsupported parity\n");    
			return (FALSE); 
	}  
	switch (stopbits){   
		case 1:    
			options.c_cflag &= ~CSTOPB;  
			break;  
		case 2:    
			options.c_cflag |= CSTOPB;  
			break;
		default:    
			fprintf(stderr,"Unsupported stop bits\n");  
			return (FALSE); 
	} 

	options.c_oflag &= ~OPOST; 

	options.c_cc[VTIME] = 1;    
	options.c_cc[VMIN] = 1; 

	tcflush(fd,TCIFLUSH);

	if(tcsetattr(fd,TCSANOW,&options) != 0){ 
		perror("com set error!\n");   
		return (FALSE);  
	} 
	return (TRUE);  
}


int UART_Init(int fd, int speed,int flow_ctrlint ,int databits,int stopbits,char parity)
{

	if (FALSE == UART_Set(fd,speed,flow_ctrlint,databits,stopbits,parity)) {    	
		return FALSE;
	} else {
		return  TRUE;
	}
}


int UART_Recv(int fd, char *rcv_buf,int data_len)
{
	int len,fs_sel;
	fd_set fs_read;

	struct timeval time;

	FD_ZERO(&fs_read);
	FD_SET(fd,&fs_read);

	time.tv_sec = 1;
	time.tv_usec = 0;

	fs_sel = select(fd+1,&fs_read,NULL,NULL,NULL);
	if(fs_sel){
		len = read(fd,rcv_buf,data_len);	
		return len;
	} else {
		return FALSE;
	}	
}


int UART_Send(int fd, char *send_buf,int data_len)
{
	int ret;

	ret = write(fd,send_buf,data_len);
	if (data_len == ret ){	
		return ret;
	} else {    
		tcflush(fd,TCOFLUSH);    
		return FALSE;

	}

}
#else
HANDLE WIN_UART_Open(char *port){
	HANDLE fd;
	fd =CreateFile("COM7", GENERIC_READ|GENERIC_WRITE,0, NULL, OPEN_EXISTING, 0, NULL);  
	if(fd ==(HANDLE)-1)  
	{  
		printf("Open COM Error\n");  
		return (HANDLE)-1;  
	}  
	return fd; 
}


void WIN_UART_Close(HANDLE fd){
	CloseHandle(fd);
}

int WIN_UART_Set(HANDLE fd, int speed, int flow_ctrl, int databits, int stopbits, int parity)
{
	DCB serial_parameter;
	int i;
	DWORD baudrate_arr[] = {CBR_110, CBR_300, CBR_600, CBR_1200, CBR_2400,CBR_4800, CBR_9600, CBR_19200, CBR_38400, CBR_56000,CBR_57600, CBR_115200, CBR_128000, CBR_256000, CBR_14400};

	int name_arr[] = {110, 300, 600, 1200, 2400,4800, 9600, 19200, 38400, 56000, 57600, 115200, 128000, 256000, 14400};

	GetCommState(fd, &serial_parameter);  
	for(i= 0;i < sizeof(baudrate_arr) / sizeof(DWORD);i++) {
		if  (speed == name_arr[i]) {        
			serial_parameter.BaudRate = baudrate_arr[i];
		}
	}	 

	serial_parameter.ByteSize = databits;


	switch (parity) {   
		case 'n':
		case 'N': 

			serial_parameter.Parity=NOPARITY;   
			break;  case 'o':   
		case 'O':    
				serial_parameter.Parity=ODDPARITY; 
		case 'e':  
		case 'E':   
				serial_parameter.Parity=EVENPARITY; 
				break;
		case 's': 
		case 'S': 
				serial_parameter.Parity=MARKPARITY; 
				break;  

		default:   
				fprintf(stderr,"Unsupported parity\n");    
				return -1; 
	}  


	switch (stopbits){   
		case 1:    
			serial_parameter.StopBits=ONESTOPBIT;   
			break;  
		case 2:    
			serial_parameter.StopBits=TWOSTOPBITS;   
			break;
		default:    
			fprintf(stderr,"Unsupported stop bits\n");  
			return -1; 
	} 
	SetupComm(fd,1024,1024);   
	SetCommState(fd, &serial_parameter);  
	PurgeComm(fd,PURGE_TXCLEAR|PURGE_RXCLEAR);
	return 0;
}


DWORD WIN_UART_Send(HANDLE fd, char *send_buf,int data_len)
{
	DWORD count;
	BOOL ret;
	ret = WriteFile(fd, send_buf, data_len, &count, NULL);  
	if(!ret){
		printf("Write Serial Error\n");
		return -1;
	}
	return count;
}

DWORD WIN_UART_Recv(HANDLE fd, char *rcv_buf,int data_len)
{
	DWORD count;
	BOOL ret;
	ret = ReadFile(fd, rcv_buf,  data_len, &count, NULL);  
	if(!ret){
		printf("Read Serial Error\n");
		return -1;
	}
	return count;
}
#endif
