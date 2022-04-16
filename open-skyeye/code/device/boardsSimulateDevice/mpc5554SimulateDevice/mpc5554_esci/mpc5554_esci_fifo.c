#include <stdio.h>
#include <malloc.h>
#include "mpc5554_esci_fifo.h"

FIFO* CreateFIFO(unsigned int  FIFOLength)
{
    FIFO *fifo = malloc(sizeof(FIFO));
    unsigned int *pfifo;
    pfifo = (unsigned int *)malloc(FIFOLength * sizeof(int));

    if(fifo == NULL)
    {
        return NULL;
    }

    if(pfifo == NULL)
    {
        return NULL;
    }

    fifo->pFirst = pfifo;
    fifo->pLast = pfifo + FIFOLength - 1;
    fifo->Length = FIFOLength;
    fifo->pIn = fifo->pFirst;
    fifo->pOut = fifo->pFirst;
    fifo->Enteres = 0;

    return fifo;
}

unsigned int WriteFIFO(FIFO *fifo, unsigned int *pSource,unsigned int  WriteLength)
{
    unsigned int  i;

    for (i = 0; i < WriteLength; i++)
    {
        if (fifo->Enteres >= fifo->Length)
        {
            return i;
        }
        *(fifo->pIn ++ ) = *(pSource ++);
        if (fifo->pIn == fifo->pLast)
        {
            fifo->pIn = fifo->pFirst;
        }
        fifo->Enteres ++;
    }

    return i;
}

unsigned int ReadFIFO(FIFO *fifo, unsigned int *pAim,unsigned int ReadLength)
{
    unsigned int  i;
    for (i = 0; i < ReadLength; i++)
    {
        if (fifo->Enteres <= 0)
        {
            return i;
        }
        *(pAim ++) = *(fifo->pOut ++);
        if (fifo->pOut == fifo->pLast)
        {
            fifo->pOut = fifo->pFirst;
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


