#ifndef __FIFO__
#define __FIFO__

typedef struct _FIFO
{
    unsigned int     *pFirst;
    unsigned int     *pLast;
    unsigned int     *pIn;
    unsigned int     *pOut;
    unsigned int   Length;
    unsigned int   Enteres;

}FIFO;



FIFO* CreateFIFO(unsigned int  FIFOLength);
unsigned int  WriteFIFO(FIFO *fifo, unsigned int  *pSource,unsigned int  WriteLength);
unsigned int  ReadFIFO(FIFO *fifo, unsigned int  *pAim,unsigned int  ReadLength);
unsigned int  CheckFIFOLength(FIFO *fifo);
void FreeFIFO(FIFO *fifo);
unsigned int  CheckIntWriteNum(FIFO *fifo);
unsigned int  CheckIntReadNum(FIFO *fifo);

#endif
