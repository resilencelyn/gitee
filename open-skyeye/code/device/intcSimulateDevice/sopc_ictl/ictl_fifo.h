#ifndef __FIFO__
#define __FIFO__

struct intc_type_t
{
	unsigned int irq_num;
	unsigned int isIrq;
};

typedef struct _FIFO
{
    struct intc_type_t     *pFirst;
    struct intc_type_t     *pLast;
    struct intc_type_t     *pIn;
    struct intc_type_t     *pOut;
    unsigned int   Length;
    unsigned int   Enteres;

}FIFO;

FIFO* CreateFIFO(unsigned int  FIFOLength);
unsigned int  WriteFIFO(FIFO *fifo, struct intc_type_t  *pSource,unsigned int  WriteLength);
unsigned int  ReadFIFO(FIFO *fifo, struct intc_type_t  *pAim,unsigned int  ReadLength);
unsigned int  CheckFIFOLength(FIFO *fifo);
void FreeFIFO(FIFO *fifo);
unsigned int  CheckIntWriteNum(FIFO *fifo);
unsigned int  CheckIntReadNum(FIFO *fifo);

#endif
