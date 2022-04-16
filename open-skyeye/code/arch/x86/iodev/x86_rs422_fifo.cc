#include <skyeye_types.h>
#include <skyeye_obj.h>
#include "x86_rs422_fifo.h"
#include <stdio.h>
#include <malloc.h>

RS422_FIFO* CreateRs422FIFO(unsigned int  FIFOLength)
{
    RS422_FIFO *fifo = malloc(sizeof(RS422_FIFO));
    if(fifo == NULL)
	    return NULL;
    x86_rs422_msg_t *pfifo;
    pfifo = malloc(FIFOLength*sizeof(x86_rs422_msg_t));
    memset(pfifo, 0, FIFOLength*sizeof(x86_rs422_msg_t));
    if(pfifo == NULL)
	    return NULL;
    fifo->pFirst = pfifo;
    fifo->pLast = pfifo + FIFOLength-1;
    fifo->Length = FIFOLength;
    fifo->pIn     = fifo->pFirst;
    fifo->pOut    = fifo->pFirst;
    fifo->Enteres = 0;  
    RWLOCK_INIT(fifo->lock);
    return fifo;
}

unsigned int  WriteRs422FIFO(RS422_FIFO *fifo, x86_rs422_msg_t *pSource,unsigned int  WriteLength)
{
    	unsigned int  i;
	RW_WRLOCK(fifo->lock);
	for (i = 0; i < WriteLength; i++){
		if (fifo->Enteres >= fifo->Length){
			return i;
		}
		memcpy(fifo->pIn, pSource, sizeof(x86_rs422_msg_t));
		pSource++;
		if (fifo->pIn == fifo->pLast)
		{
			fifo->pIn = fifo->pFirst;
		}
		else
		{
			fifo->pIn++;
		}
		fifo->Enteres ++;
	}
	RW_UNLOCK(fifo->lock);
    return i;
}

unsigned int  ReadRs422FIFO(RS422_FIFO *fifo, x86_rs422_msg_t *pAim,unsigned int  ReadLength)
{
    	unsigned int  i;
	RW_WRLOCK(fifo->lock);
	for (i = 0; i < ReadLength; i++){
		if (fifo->Enteres <= 0){
			return i; 
		}
		memcpy(pAim, fifo->pOut, sizeof(x86_rs422_msg_t));
		pAim++;
		if (fifo->pOut == fifo->pLast)
		{
			fifo->pOut = fifo->pFirst;
		}
		else
		{
			fifo->pOut++;
		}
		fifo->Enteres -- ;
	}
	
	RW_UNLOCK(fifo->lock);
	return i;
}


unsigned int  CheckRs422FIFOLength(RS422_FIFO *fifo)
{
    return fifo->Length;
}


x86_rs422_msg_t *CheckCurrentRs422WritePoint(RS422_FIFO *fifo)
{
    return (fifo->pIn);
}

x86_rs422_msg_t* CheckCurrentRs422ReadPoint(RS422_FIFO *fifo)
{
    return (fifo->pOut);
}


void FreeRs422FIFO(RS422_FIFO *fifo)
{
    RWLOCK_DESTROY(fifo->lock);
    free(fifo->pFirst);
    free(fifo);
}

unsigned int  FIFOCheckRs422WriteNum(RS422_FIFO *fifo)
{
    return (fifo->Length - fifo->Enteres);
}

unsigned int  FIFOCheckRs422ReadNum(RS422_FIFO *fifo)
{
    return fifo->Enteres;
}
