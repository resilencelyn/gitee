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
    rs422_msg_t*     pFirst;
    rs422_msg_t*     pLast; 
    rs422_msg_t*     pIn;
    rs422_msg_t*     pOut;
    unsigned int   Length;
    unsigned int   Enteres;
    RWLOCK_T lock;
    char *name;
}FIFO;

FIFO* CreateFIFO(unsigned int  FIFOLength);
int FreeFIFO(FIFO *fifo);
unsigned int  WriteFIFO(FIFO *fifo, rs422_msg_t *pSource,unsigned int  WriteLength);
unsigned int  ReadFIFO(FIFO *fifo, rs422_msg_t *pAim,unsigned int  ReadLength);
void FifoReadOK(FIFO *fifo);
unsigned int  CheckFIFOLength(FIFO *fifo);
rs422_msg_t *CheckCurrentWritePoint(FIFO *fifo);
rs422_msg_t* CheckCurrentReadPoint(FIFO *fifo);
unsigned int  CheckCanWriteNum(FIFO *fifo);
unsigned int  CheckCanReadNum(FIFO *fifo);
#endif
