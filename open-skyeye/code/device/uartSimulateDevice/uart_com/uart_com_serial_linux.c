/*
 * @file: uart_com_serial_linux.c
 * @brief: The implementation of set- and open- rs232 port function
 * @author: liubo
 * @version: 1.0
 * @date: 2016-08-25
 */

#ifndef __MINGW32__
#include "uart_com_serial_linux.h"
/*
 * return value: FAILURE -1
 *               SUCCESS  0
 */
#define FAILURE (-1)
#define SUCCESS   0

/*******************************************************************
* function:    set_opt
* desc:        set rs232
* in argument: fd file descriptor(by open_port function return)
               nSpeed  BaudRate 2400\4800\9600\115200
               nBits   ByteSize 7\8
               nEvent  Parity 'O'\'N'\'E'
               nStop   StopBit 1\1.5\2
* out argument:none
* return value:SUCCESS set ok
               FAILURE set error
* other:       none
***********************************************************************/
int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop)
{
    struct termios newtio,oldtio;

    //store current setting
    if (0 != tcgetattr( fd,&oldtio)){
        return FAILURE;
    }
    memset(&newtio, 0, sizeof(newtio));

    //set character size
    newtio.c_cflag |= CLOCAL | CREAD;
    newtio.c_cflag &= ~CSIZE;

    //set ByteSize
    switch(nBits){
        case 7:
            newtio.c_cflag |= CS7;
            break;
        default:  //8
            newtio.c_cflag |= CS8;
            break;
    }

    //set Parity
    switch(nEvent){
        case 'O':  //ODDPARITY
            newtio.c_cflag |= PARENB;
            newtio.c_cflag |= PARODD;
            newtio.c_iflag |= (INPCK | ISTRIP);
            break;
        case 'E':  //EVENTPARITY
            newtio.c_iflag |= (INPCK | ISTRIP);
            newtio.c_cflag |= PARENB;
            newtio.c_cflag &= ~PARODD;
            break;
        default:  //NOPARITY
            newtio.c_cflag &= ~PARENB;
            break;
    }

    //set BaudRate
    switch(nSpeed){
        case 2400:
            cfsetispeed(&newtio, B2400);
            cfsetospeed(&newtio, B2400);
            break;
        case 4800:
            cfsetispeed(&newtio, B4800);
            cfsetospeed(&newtio, B4800);
            break;
        case 9600:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
            break;
        case 115200:
            cfsetispeed(&newtio, B115200);
            cfsetospeed(&newtio, B115200);
            break;
        default:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
            break;
    }

    //set StopBit
    if(1 == nStop)
        newtio.c_cflag &= ~CSTOPB;
    else if (2 == nStop)
        newtio.c_cflag |= CSTOPB;

    //set wait time and min receive character
    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 0;
    //set untreated character
    tcflush(fd,TCIFLUSH);
    //set rs232 setting
    if(0 != (tcsetattr(fd,TCSANOW,&newtio))){
        return FAILURE;
    }

    return SUCCESS;
}


/**********************************************************************
* function:     open_port
* desc:         open rs232
* in argument:  fd file descriptor
* out argument: none
* return value: FAILURE -1
                fd set success return fd
* other:        none
***********************************************************************/
int open_port(const char *filename){
    int fd;
    //open rs232
    fd = open(filename, O_RDWR|O_NOCTTY|O_NDELAY);
    if(-1 == fd)
    {
        return FAILURE;
    }
    //rs232 block state
    if(fcntl(fd, F_SETFL, 0) < 0){
        return FAILURE;
    }
    //test fd
    if(0 == isatty(STDIN_FILENO)){
        return FAILURE;
    }

    return fd;
}

#endif  /* __MINGW32__ */
