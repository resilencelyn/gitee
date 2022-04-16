#ifndef __X86_CAN_FIFO_
#define __X86_CAN_FIFO_
#include "skyeye_dev_bus_intf.h"
#include "skyeye_lock.h"

#ifdef __MINGW32__
#include <windows.h>
#else
#endif

typedef struct
{
    can_msg_t*     pFirst;
    can_msg_t*     pLast; 
    can_msg_t*     pIn;
    can_msg_t*     pOut;
    unsigned int   Length;
    unsigned int   Enteres;
    RWLOCK_T lock;
    char *name;
}CAN_FIFO;

CAN_FIFO* CreateCanFIFO(unsigned int  FIFOLength);
unsigned int  WriteCanFIFO(CAN_FIFO *fifo, can_msg_t *pSource,unsigned int  WriteLength);
unsigned int  ReadCanFIFO(CAN_FIFO *fifo, can_msg_t *pAim,unsigned int  ReadLength);
void CanFifoReadOK(CAN_FIFO *fifo);
unsigned int  CheckCanFIFOLength(CAN_FIFO *fifo);
can_msg_t *CheckCurrentCanWritePoint(CAN_FIFO *fifo);
can_msg_t* CheckCurrentCanReadPoint(CAN_FIFO *fifo);
void FreeCanFIFO(CAN_FIFO *fifo);
unsigned int  CanFIFOCheckCanWriteNum(CAN_FIFO *fifo);
unsigned int  CanFIFOCheckCanReadNum(CAN_FIFO *fifo);
#endif
