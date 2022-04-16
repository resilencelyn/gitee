#include <stdio.h>
#include <malloc.h>
#include "intc_fifo.h"

FIFO* CreateFIFO(unsigned int  FIFOLength)
{
	FIFO *fifo = malloc(sizeof(FIFO));
	if(fifo == NULL)
		return NULL;
	unsigned int *pfifo;
	pfifo = (unsigned int *)malloc(FIFOLength * sizeof(int));
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

unsigned int WriteFIFO(FIFO *fifo, unsigned int *pSource,unsigned int  WriteLength)
{
	unsigned int  i;

	for (i = 0; i < WriteLength; i++){
		if (fifo->Enteres >= fifo->Length){
			return i;//如果已经写入FIFO的数据两超过或者等于FIFO的长度，就返回实际写入FIFO的数据个数
		}
		*(fifo->pIn) = *(pSource ++);
		if (fifo->pIn == fifo->pLast)
		{
			fifo->pIn = fifo->pFirst;
		}
		else
		{
			fifo->pIn ++;
		}
		fifo->Enteres ++;
	}
	return i;
}

unsigned int ReadFIFO(FIFO *fifo, unsigned int *pAim,unsigned int ReadLength)
{
	unsigned int  i;
	for (i = 0; i < ReadLength; i++){
		if (fifo->Enteres <= 0){
			return i;//返回从FIFO中读取到的数据个数
		}
		*(pAim ++) = *(fifo->pOut);
		if (fifo->pOut == fifo->pLast)
		{
			fifo->pOut = fifo->pFirst;
		}
		else
		{
			fifo->pOut ++;
		}
		fifo->Enteres -- ;
	}
	return i;
}

unsigned int CheckFIFOLength(FIFO *fifo)
{
	return fifo->Length;
}

unsigned int *CheckCurrentWritePoint(FIFO *fifo)
{
	return (fifo->pIn);
}

unsigned int *CheckCurrentReadPoint(FIFO *fifo)
{
	return (fifo->pOut);
}

void FreeFIFO(FIFO *fifo)
{
	free(fifo->pFirst);
	free(fifo);
}

unsigned int CheckIntcWriteNum(FIFO *fifo)
{
	return (fifo->Length - fifo->Enteres);
}

unsigned int CheckIntcReadNum(FIFO *fifo)
{
	return fifo->Enteres;
}


