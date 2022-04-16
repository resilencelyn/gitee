#ifndef __BU61580_FIFO_
#define __BU61580_FIFO_
#include "skyeye_dev_bus_intf.h"
#include "skyeye_lock.h"
#include "skyeye_signal.h"

#ifdef __MINGW32__
#include <windows.h>
#else
#endif

typedef struct {
    int pie_x;
    int pie_y;
    int pie_group;
    int type;
}irq_mesg_t;

typedef struct
{
    irq_mesg_t*     pFirst;
    irq_mesg_t*     pLast; 
    irq_mesg_t*     pIn;
    irq_mesg_t*     pOut;
    unsigned int   Length;
    unsigned int   Enteres;
    RWLOCK_T lock;
    char *name;
}FIFO;

FIFO* CreateFIFO(unsigned int  FIFOLength);
unsigned int  WriteFIFO(FIFO *fifo, irq_mesg_t *pSource,unsigned int  WriteLength);
unsigned int  ReadFIFO(FIFO *fifo, irq_mesg_t *pAim,unsigned int  ReadLength);
void FifoReadOK(FIFO *fifo);
unsigned int  CheckFIFOLength(FIFO *fifo);
irq_mesg_t *CheckCurrentWritePoint(FIFO *fifo);
irq_mesg_t* CheckCurrentReadPoint(FIFO *fifo);
void FreeFIFO(FIFO *fifo);
unsigned int  CheckCanWriteNum(FIFO *fifo);
unsigned int  CheckCanReadNum(FIFO *fifo);
#endif
