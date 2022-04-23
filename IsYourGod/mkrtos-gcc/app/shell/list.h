

#ifndef _OS_LIST_H__
#define _OS_LIST_H__
#include "type.h"
#include "ipc/spin_lock.h"
/*����ڵ�*/
typedef struct _OSListNode{
    uint8_t* data;
    struct _OSListNode* next;
}*POSListNode,OSListNode;

typedef void (*_OSListAddNtData)(uint8_t**data, void *newData);
typedef void (*_OSListDelNtData)(uint8_t**data);
typedef int32_t (*_OSListFindCompare)(POSListNode pOSListNode,void* toCompareVal);

/*list����*/
typedef struct _OSList{

    /*ͷ�ڵ�*/
    POSListNode osLsIm;

    /*��ӽڵ�ʱ����δ������ݣ��û������Զ���*/
    _OSListAddNtData osListAddNtDataFn;
    /*ɾ���ڵ�ʱ����δ������ݣ��û������Զ���*/
    _OSListDelNtData osListDelNtDataFn;
    /*����ʱ�ıȽϺ���*/
    _OSListFindCompare  osListFindCompareFn;

    /*����*/
    uint32_t len;

    /*������*/
    spinlock_handler lock;
}*POSList,OSList;

/*List������*/
typedef struct _OSListIterator{
    POSList pOSLs;
    POSListNode newLsNode;
}*POSListIterator,OSListIterator;

POSList OSListCreate(
        POSList pOSLs
        ,_OSListAddNtData addNtDataFun
        ,_OSListDelNtData delNetDataFun
        ,_OSListFindCompare findComFun
);

/*���ݡ���������*/
int32_t OSListDelNodeByData(POSList pOSLs,void* data );
int32_t OSListDelNode(POSList pOSLs,POSListNode pOsLtDelNd);
int32_t OSListAddNode(POSList pOSLs,void *data);
void* OSListFind(POSList pOSLs,void *findVal);

/*������������ջ��ʵ��*/
int32_t OSListAddNode2Top(POSList pOSLs,void* data);
void* OSListGetTopData(POSList pOSLs);
int32_t OSListPopData(POSList pOSLs);

/*������list�ı�����*/
int32_t OSListIteratorStart(POSListIterator pOsLsItr,POSList pOSLs);
void* OSListIteratorGetNext(POSListIterator pOsLsItr);
int32_t OSListIteratorStop(POSListIterator pOsLsItr);

#endif
