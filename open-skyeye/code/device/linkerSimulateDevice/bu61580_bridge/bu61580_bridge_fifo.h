#ifndef __CAN_FIFO_
#define __CAN_FIFO_
#include "skyeye_dev_bus_intf.h"
#include "skyeye_lock.h"

#ifdef __MINGW32__
#include <windows.h>
#else
#endif

typedef struct
{
    network_mesg_t*     pFirst;
    network_mesg_t*     pLast; 
    network_mesg_t*     pIn;
    network_mesg_t*     pOut;
    unsigned int   Length;
    unsigned int   Enteres;
    char *name;
}FIFO;

FIFO* CreateFIFO(unsigned int  FIFOLength);
int FreeFIFO(FIFO *fifo);
unsigned int  WriteFIFO(FIFO *fifo, network_mesg_t *pSource,unsigned int  WriteLength);
unsigned int  ReadFIFO(FIFO *fifo, network_mesg_t *pAim,unsigned int  ReadLength);
unsigned int  CheckFIFOReadNum(FIFO *fifo);
#endif
