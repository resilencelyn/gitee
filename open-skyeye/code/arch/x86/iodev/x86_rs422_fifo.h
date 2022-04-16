#ifndef __X86_RS422_FIFO_
#define __X86_RS422_FIFO_
#include "skyeye_lock.h"

#ifdef __MINGW32__
#include <windows.h>
#else
#endif

#define RECV_RS422_MSG_LENGTH 128
typedef struct{
	int com_id;
	unsigned char uc_buf[RECV_RS422_MSG_LENGTH];	
	int length;
}x86_rs422_msg_t;

typedef struct
{
    x86_rs422_msg_t*     pFirst;
    x86_rs422_msg_t*     pLast; 
    x86_rs422_msg_t*     pIn;
    x86_rs422_msg_t*     pOut;
    unsigned int   Length;
    unsigned int   Enteres;
    RWLOCK_T lock;
    char *name;
}RS422_FIFO;

RS422_FIFO* CreateRs422FIFO(unsigned int  FIFOLength);
unsigned int  WriteRs422FIFO(RS422_FIFO *fifo, x86_rs422_msg_t *pSource,unsigned int  WriteLength);
unsigned int  ReadRs422FIFO(RS422_FIFO *fifo, x86_rs422_msg_t *pAim,unsigned int  ReadLength);
void Rs422FifoReadOK(RS422_FIFO *fifo);
unsigned int  CheckRs422FIFOLength(RS422_FIFO *fifo);
x86_rs422_msg_t *CheckCurrentRs422WritePoint(RS422_FIFO *fifo);
x86_rs422_msg_t* CheckCurrentRs422ReadPoint(RS422_FIFO *fifo);
void FreeRs422FIFO(RS422_FIFO *fifo);
unsigned int  FIFOCheckRs422WriteNum(RS422_FIFO *fifo);
unsigned int  FIFOCheckRs422ReadNum(RS422_FIFO *fifo);
#endif
