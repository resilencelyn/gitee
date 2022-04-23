
#include "list.h"
#include <stdlib.h>
#include <ipc/spin_lock.h>
/*Ĭ�ϵ����ݴ�����*/
static void OSListAddNtDataDefault(uint8_t**data,void *newData){
    *data=newData;
}
/*Ĭ�ϵ����ݴ�����*/
static void OSListDelNtDataDefault(uint8_t**data){
    return ;
}
/*Ĭ�ϵ����ݴ�����*/
static int32_t OSListFindCompareDefault(POSListNode pOSListNode,void* toCompareVal){
    return pOSListNode==toCompareVal;
}
POSList OSListCreate(
        POSList pOSLs
        ,_OSListAddNtData addNtDataFun
        ,_OSListDelNtData delNetDataFun
        ,_OSListFindCompare findComFun
){
    if(pOSLs==NULL){return NULL;}

    spin_lock_init(&(pOSLs->lock));

    pOSLs->osListAddNtDataFn=addNtDataFun;
    pOSLs->osListDelNtDataFn=delNetDataFun;
    pOSLs->osListFindCompareFn=findComFun;
    /*����Ĭ�ϵ����ݴ�����*/
    if(pOSLs->osListAddNtDataFn==NULL){
        pOSLs->osListAddNtDataFn=OSListAddNtDataDefault;
    }
    if(pOSLs->osListDelNtDataFn==NULL){
        pOSLs->osListDelNtDataFn=OSListDelNtDataDefault;
    }
    if(pOSLs->osListFindCompareFn==NULL){
        pOSLs->osListFindCompareFn=OSListFindCompareDefault;
    }
    pOSLs->len=0;
    pOSLs->osLsIm=NULL;
    return pOSLs;
}
int32_t OSListDelNodeByData(POSList pOSLs,void* data ){
    if(pOSLs==NULL){return -1;}

    POSListNode pOsLastLtNd=NULL;
    POSListNode pOsLtNd;
    spin_lock(&(pOSLs->lock));
    pOsLtNd=pOSLs->osLsIm;
    /*��һ���ڵ�*/
    pOsLastLtNd=pOsLtNd;

    while(pOsLtNd){

        /*�ҵ��ڵ���*/
        if(pOSLs->osListFindCompareFn(pOsLtNd,data)==0){/*�ҵ�������ͬ��*/
            if(pOsLastLtNd==pOSLs->osLsIm){/*��һ���ڵ���ǵ�һ���ڵ�*/

                /*�û�����ɾ���ڵ�����*/
                pOSLs->osListDelNtDataFn(&(pOsLtNd->data));
                /*�ڵ�����*/
                pOsLastLtNd=pOsLtNd->next;

                pOSLs->len--;
                spin_unlock(&(pOSLs->lock));
                /*�ͷŽڵ��ڴ�*/
                free(pOsLtNd);
                return 0;
            }else{/*���ǵ�һ���ڵ�*/

                /*�û�����ɾ���ڵ�����*/
                pOSLs->osListDelNtDataFn(&(pOsLtNd->data));
                /*�ڵ�����*/
                pOsLastLtNd->next=pOsLtNd->next;

                pOSLs->len--;
                spin_unlock(&(pOSLs->lock));
                /*�ͷŽڵ��ڴ�*/
                free(pOsLtNd);
                return 0;
            }
        }

        pOsLastLtNd=pOsLtNd;
        pOsLtNd=pOsLtNd->next;
    }
    spin_unlock(&(pOSLs->lock));
    return -1;
}
/*listɾ������*/
int32_t OSListDelNode(POSList pOSLs,POSListNode pOsLtDelNd){
    if(pOSLs==NULL || pOsLtDelNd==NULL){return -1;}

    POSListNode pOsLastLtNd=NULL;
    POSListNode pOsLtNd;
    spin_lock(&(pOSLs->lock));
    pOsLtNd=pOSLs->osLsIm;
    /*��һ���ڵ�*/
    pOsLastLtNd=pOsLtNd;

    while(pOsLtNd){

        /*�ҵ��ڵ���*/
        if(pOsLtDelNd==pOsLtNd){
            if(pOsLastLtNd==pOSLs->osLsIm){/*��һ���ڵ���ǵ�һ���ڵ�*/

                /*�û�����ɾ���ڵ�����*/
                pOSLs->osListDelNtDataFn(&(pOsLtNd->data));
                /*�ڵ�����*/
                pOsLastLtNd=pOsLtNd->next;

                pOSLs->len--;
                spin_unlock(&(pOSLs->lock));
                /*�ͷŽڵ��ڴ�*/
                free(pOsLtNd);
                return 0;
            }else{/*���ǵ�һ���ڵ�*/

                /*�û�����ɾ���ڵ�����*/
                pOSLs->osListDelNtDataFn(&(pOsLtNd->data));
                /*�ڵ�����*/
                pOsLastLtNd->next=pOsLtNd->next;

                pOSLs->len--;
                spin_unlock(&(pOSLs->lock));
                /*�ͷŽڵ��ڴ�*/
                free(pOsLtNd);
                return 0;
            }
        }

        pOsLastLtNd=pOsLtNd;
        pOsLtNd=pOsLtNd->next;
    }
    spin_unlock(&(pOSLs->lock));
    return -1;
}

/**
* @brief ��ȡlist�Ķ�������
* @return ��ȡ�������ݣ�����NULL����û������
*/
void* OSListGetTopData(POSList pOSLs){
    if(pOSLs==NULL){return NULL;}
    POSListNode pOsLtNd;
    pOsLtNd=pOSLs->osLsIm;

    if(pOsLtNd==NULL){
        return NULL;
    }
    return pOsLtNd->data;
}
/**
* @brief ɾ�������Ľڵ�
*	@return 0�ɹ� -1ʧ��
*/
int32_t OSListPopData(POSList pOSLs){
    if(pOSLs==NULL){
        return -1;
    }
    POSListNode pOsLtNd;
    pOsLtNd=pOSLs->osLsIm;

    return OSListDelNode(pOSLs,pOsLtNd);
}
/**
* @brief ������ݵ�����
* @param ��Ҫ��ӵ�����
*/
int32_t OSListAddNode2Top(POSList pOSLs,void* data){
    if(pOSLs==NULL){return -1;}

    POSListNode pOSltNtNew;
    POSListNode pOsLtNd;

    pOSltNtNew=malloc(sizeof(OSListNode));
    if(pOSltNtNew==NULL){
        return -1;
    }
    pOSltNtNew->next=NULL;

    spin_lock(&(pOSLs->lock));
    pOsLtNd=pOSLs->osLsIm;
    /*�û��������ݴ���*/
    pOSLs->osListAddNtDataFn(&(pOSltNtNew->data),data);
    if(pOsLtNd==NULL){
        pOsLtNd=pOSltNtNew;
    }else{
        /*����һ��*/
        POSListNode pTemp;
        pTemp=pOsLtNd->next;
        pOsLtNd=pOSltNtNew;
        pOSltNtNew->next=pTemp;
    }
    spin_unlock(&(pOSLs->lock));
    return 0;
}
/**
* @brief List������ݵ�β��
* @param pOSLs listָ��
* @param data ��Ҫ��ӵ�����
*/
int32_t OSListAddNode(POSList pOSLs,void *data){
    if(pOSLs==NULL){return -1;}
    POSListNode pOSltNtNew;
    POSListNode pOsLtNd;

    pOSltNtNew=malloc(sizeof(OSListNode));
    if(pOSltNtNew==NULL){
        return -1;
    }
    pOSltNtNew->next=NULL;

    spin_lock(&(pOSLs->lock));
    pOsLtNd	=pOSLs->osLsIm;
    if(pOsLtNd==NULL){
        /*�û��������ݴ���*/
        pOSLs->osListAddNtDataFn(&(pOSltNtNew->data),data);
        /*��ֵ��һ���ڵ�*/
        pOSLs->osLsIm=pOSltNtNew;
        /*���ӳ���*/
        pOSLs->len++;
    }else{
        while(pOsLtNd->next){
            pOsLtNd=pOsLtNd->next;
        }
        /*�û��������ݴ���*/
        pOSLs->osListAddNtDataFn(&(pOSltNtNew->data),data);
        /*��ֵ��һ���ڵ�*/
        pOsLtNd->next=pOSltNtNew;
        /*���ӳ���*/
        pOSLs->len++;
    }
    spin_unlock(&(pOSLs->lock));
    return 0;
}
/*����ָ��node*/
void* OSListFind(POSList pOSLs,void *findVal){
    if(pOSLs==NULL){return NULL;}
    spin_lock(&(pOSLs->lock));
    POSListNode pOsLtNd=pOSLs->osLsIm;

    while(pOsLtNd){
        if(pOSLs->osListFindCompareFn(pOsLtNd,findVal)==0){
            spin_unlock(&(pOSLs->lock));
            return pOsLtNd->data;
        }
        pOsLtNd=pOsLtNd->next;
    }
    spin_unlock(&(pOSLs->lock));
    return NULL;
}

/*��������ʼ��*/
int32_t OSListIteratorStart(POSListIterator pOsLsItr,POSList pOSLs){
    if(pOsLsItr==NULL || pOSLs==NULL){
        return -1;
    }
    spin_lock(&(pOSLs->lock));
    pOsLsItr->pOSLs = pOSLs;
    pOsLsItr->newLsNode=pOSLs->osLsIm;

    return 0;
}
/*��ȡdata*/
void* OSListIteratorGetNext(POSListIterator pOsLsItr){
    POSListNode tempNode;
    if(pOsLsItr==NULL){
        return NULL;
    }
    tempNode=pOsLsItr->newLsNode;
    if(pOsLsItr->newLsNode!=NULL){
        pOsLsItr->newLsNode=pOsLsItr->newLsNode->next;

        return tempNode->data;
    }
    return NULL;
}
int32_t OSListIteratorStop(POSListIterator pOsLsItr){
    if(pOsLsItr==NULL){
        return -1;
    }

    pOsLsItr->newLsNode=NULL;
    spin_unlock(&(pOsLsItr->pOSLs->lock));
    return 0;
}



