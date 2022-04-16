#include <skyeye_types.h>
#include <skyeye_obj.h>
#include "skyeye_mm.h"
#include "can_bridge_fifo.h"
#include <stdio.h>
#include <malloc.h>

FIFO* CreateFIFO(unsigned int  FIFOLength)
{
    FIFO *fifo = skyeye_mm_zero(sizeof(FIFO));
    if(fifo == NULL)
	    return NULL;
    can_msg_t *pfifo;
    pfifo = skyeye_mm_zero(FIFOLength*sizeof(can_msg_t));
    memset(pfifo, 0, FIFOLength*sizeof(can_msg_t));
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

int  FreeFIFO(FIFO *fifo)
{
	skyeye_free(fifo->pFirst);
	skyeye_free(fifo);
    	return 1;
}



unsigned int  WriteFIFO(FIFO *fifo, can_msg_t *pSource,unsigned int  WriteLength)
{
    	unsigned int  i;
	RW_WRLOCK(fifo->lock);
	for (i = 0; i < WriteLength; i++){
		if (fifo->Enteres >= fifo->Length){
			return i;
		}
		memcpy(fifo->pIn, pSource, sizeof(can_msg_t));
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

unsigned int  ReadFIFO(FIFO *fifo, can_msg_t *pAim,unsigned int  ReadLength)
{
    	unsigned int  i;
	RW_WRLOCK(fifo->lock);
	for (i = 0; i < ReadLength; i++){
		if (fifo->Enteres <= 0){
			return i; 
		}
		memcpy(pAim, fifo->pOut, sizeof(can_msg_t));
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


unsigned int  CheckFIFOLength(FIFO *fifo)
{
    return fifo->Length;
}


can_msg_t *CheckCurrentWritePoint(FIFO *fifo)
{
    return (fifo->pIn);
}

can_msg_t* CheckCurrentReadPoint(FIFO *fifo)
{
    return (fifo->pOut);
}


unsigned int  CheckCanWriteNum(FIFO *fifo)
{
    return (fifo->Length - fifo->Enteres);
}

unsigned int  CheckCanReadNum(FIFO *fifo)
{
    return fifo->Enteres;
}
