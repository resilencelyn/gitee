/*
 * @file: uart_com_serial_win.c
 * @brief: The implementation of set rs232 function for windows
 * @author: liubo
 * @version: 1.0
 * @date; 2016-08-25
 */
#ifdef __MINGW32__

#include "uart_com_serial_win.h"

/*******************************************************************
* function:  set_opt
* desc:      com port setting
* in argument:  hFile  handle
                nSpeed BaudRate
                nBits  ByteSize
                nEvent Parity
                nStop  StopBit
* out argument: none
* return value: 0  set success
                -1 set failure
* other:        none
***********************************************************************/
int set_opt(HANDLE hFile, int nSpeed, int nBits, char nEvent, int nStop, unsigned int buffersize)
{
    DCB dcb;
    COMMTIMEOUTS timeouts;

    //set input buf and output buf
    SetupComm(hFile, buffersize, buffersize);
/*
    //set read timeout
    timeouts.ReadIntervalTimeout = 1000;
    timeouts.ReadTotalTimeoutMultiplier = 500;
    timeouts.ReadTotalTimeoutConstant = 5000;
    //set write timeout
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 2000;
    SetCommTimeouts(hFile, &timeouts);
*/

    //store current setting
    if (0 == GetCommState(hFile, &dcb)){
        return -1;
    }

    //set BaudRate
    switch(nSpeed){
        case 2400:
            dcb.BaudRate = CBR_2400;
            break;
        case 4800:
            dcb.BaudRate = CBR_4800;
            break;
        case 115200:
            dcb.BaudRate = CBR_115200;
            break;
        default:
            dcb.BaudRate = CBR_9600;
            break;
    }

    //set ByteSize
    switch(nBits){
        case 7:
            dcb.ByteSize = 7;
            break;
        case 8:
            dcb.ByteSize = 8;
            break;
        default:
            dcb.ByteSize = 8;
            break;
    }

    //set Parity
    switch(nEvent){
        case 'N':
            dcb.Parity = NOPARITY;
            break;
        case 'O':
            dcb.Parity = ODDPARITY;
            break;
        case 'E':
            dcb.Parity = EVENPARITY;
            break;
    }

    //set StopBit
    switch(nStop){
        case 1:
            dcb.StopBits = ONESTOPBIT;
            break;
        case 2:
            dcb.StopBits = TWOSTOPBITS;
            break;
    }

    if(0 == SetCommState(hFile, &dcb)){
        return -1;
    }

    return 0;
}

#endif  /* __MINGW32__  */
