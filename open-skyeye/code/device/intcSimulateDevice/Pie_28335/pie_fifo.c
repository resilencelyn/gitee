#include <skyeye_types.h>
#include <skyeye_obj.h>
#include "pie_fifo.h"
#include <stdio.h>
#include <malloc.h>

FIFO* CreateFIFO(unsigned int  FIFOLength)
{
    FIFO *fifo = malloc(sizeof(FIFO));
    if(fifo == NULL)
	    return NULL;
    irq_mesg_t *pfifo;
    pfifo = malloc(FIFOLength*sizeof(irq_mesg_t));
    memset(pfifo, 0, FIFOLength*sizeof(irq_mesg_t));
    if(pfifo == NULL)
	    return NULL;
    fifo->pFirst = pfifo;
    fifo->pLast = pfifo + FIFOLength-1;
    fifo->Length = FIFOLength;
    fifo->pIn     = fifo->pFirst;
    fifo->pOut    = fifo->pFirst;
    fifo->Enteres = 0;  
    return fifo;
}

unsigned int  WriteFIFO(FIFO *fifo, irq_mesg_t *pSource,unsigned int  WriteLength)
{
    unsigned int  i;
	for (i = 0; i < WriteLength; i++){
		if (fifo->Enteres >= fifo->Length){
			return i;
		}
		memcpy(fifo->pIn, pSource, sizeof(irq_mesg_t));
		pSource++;
		if (fifo->pIn == fifo->pLast)
		{
			fifo->pIn = fifo->pFirst;
		}
		else
		{
			fifo->pIn++;
		}
		//fifo->Enteres ++;
        __sync_fetch_and_add(&(fifo->Enteres),1);
	}
    return i;
}

unsigned int  ReadFIFO(FIFO *fifo, irq_mesg_t *pAim,unsigned int  ReadLength)
{
    	unsigned int  i;
	for (i = 0; i < ReadLength; i++){
		if (fifo->Enteres <= 0){
			return i; 
		}
		memcpy(pAim, fifo->pOut, sizeof(irq_mesg_t));
		pAim++;
		if (fifo->pOut == fifo->pLast)
		{
			fifo->pOut = fifo->pFirst;
		}
		else
		{
			fifo->pOut++;
		}
		//fifo->Enteres -- ;
        	__sync_fetch_and_sub(&(fifo->Enteres),1);
	}
	
	return i;
}


unsigned int  CheckFIFOLength(FIFO *fifo)
{
    return fifo->Length;
}


irq_mesg_t *CheckCurrentWritePoint(FIFO *fifo)
{
    return (fifo->pIn);
}

irq_mesg_t* CheckCurrentReadPoint(FIFO *fifo)
{
    return (fifo->pOut);
}


void FreeFIFO(FIFO *fifo)
{
    free(fifo->pFirst);
    free(fifo);
}

unsigned int  CheckCanWriteNum(FIFO *fifo)
{
    return fifo->Enteres;
}

unsigned int  CheckCanReadNum(FIFO *fifo)
{
    return fifo->Enteres;
}
