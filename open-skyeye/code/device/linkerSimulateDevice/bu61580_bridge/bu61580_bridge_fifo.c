#include <skyeye_types.h>
#include <skyeye_obj.h>
#include "skyeye_mm.h"
#include "bu61580_bridge_fifo.h"
#include <stdio.h>
#include <malloc.h>

FIFO* CreateFIFO(unsigned int  FIFOLength)
{
    FIFO *fifo = skyeye_mm_zero(sizeof(FIFO));
    if(fifo == NULL)
	    return NULL;
    network_mesg_t *pfifo;
    pfifo = skyeye_mm_zero(FIFOLength*sizeof(network_mesg_t));
    memset(pfifo, 0, FIFOLength*sizeof(network_mesg_t));
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

int  FreeFIFO(FIFO *fifo)
{
	skyeye_free(fifo->pFirst);
	skyeye_free(fifo);
    	return 1;
}



unsigned int  WriteFIFO(FIFO *fifo, network_mesg_t *pSource,unsigned int  WriteLength)
{
    	unsigned int  i;
	for (i = 0; i < WriteLength; i++){
		if (fifo->Enteres >= fifo->Length){
			return i;
		}
		memcpy(fifo->pIn, pSource, sizeof(network_mesg_t));
		pSource++;
		if (fifo->pIn == fifo->pLast)
		{
			fifo->pIn = fifo->pFirst;
		}
		else
		{
			fifo->pIn++;
		}
		__sync_fetch_and_add(&(fifo->Enteres),1);
	}
    	return i;
}

unsigned int  ReadFIFO(FIFO *fifo, network_mesg_t *pAim,unsigned int  ReadLength)
{
    	unsigned int  i;
	for (i = 0; i < ReadLength; i++){
		if (fifo->Enteres <= 0){
			return i; 
		}
		memcpy(pAim, fifo->pOut, sizeof(network_mesg_t));
		pAim++;
		if (fifo->pOut == fifo->pLast)
		{
			fifo->pOut = fifo->pFirst;
		}
		else
		{
			fifo->pOut++;
		}
		__sync_fetch_and_sub(&(fifo->Enteres),1);
		
	}
	
	return i;
}

unsigned int  CheckFIFOReadNum(FIFO *fifo)
 {
    	return fifo->Enteres;
 }

