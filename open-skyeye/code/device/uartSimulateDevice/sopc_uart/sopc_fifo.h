#ifndef __FIFO__
#define __FIFO__

typedef struct _FIFO
{
    unsigned char     *pFirst;
    unsigned char     *pLast;
    unsigned char     *pIn;
    unsigned char     *pOut;
    unsigned int   Length;
    unsigned int   Enteres;

}FIFO;



FIFO* CreateFIFO(unsigned int  FIFOLength);
unsigned int  WriteFIFO(FIFO *fifo, unsigned char  *pSource,unsigned int  WriteLength);
unsigned int  ReadFIFO(FIFO *fifo, unsigned char  *pAim,unsigned int  ReadLength);
unsigned int  CheckFIFOLength(FIFO *fifo);
unsigned char * CheckCurrentWritePoint(FIFO *fifo);
unsigned char * CheckCurrentReadPoint(FIFO *fifo);
void FreeFIFO(FIFO *fifo);
unsigned int  CheckFIFOWriteNum(FIFO *fifo);
unsigned int  CheckFIFOReadNum(FIFO *fifo);

#endif
