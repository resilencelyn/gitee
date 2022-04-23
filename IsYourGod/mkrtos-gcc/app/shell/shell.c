//
// Created by Administrator on 2022/3/6.
//

#include "shell.h"
#include "list.h"
//#include "OSShellCmd.h"
#include <stdio.h>
//#include "SysCall.h"
//#include "OSShellNetPort.h"

#include "string.h"

/**
* @breif ��ǰ����ʹ�õ�ShellCmd
*/
PShell curShell=NULL;

/**
* @breif ����list���������ʱ���ݵĴ�����
* @param data ������ڽڵ��е�λ��
* @param newData ��Ҫ��ӵ�����
*/
static void OSListAddNtData(uint8_t** data,void * newData){
    *data=newData;
}
/**
* @breif list������ɾ������
* @param ��ɾ�������ݴ�ŵ�λ��
*/
static void OSListDelNtData(uint8_t** data){
    return;
}

/**
* @breif list����ʱ�����ݱȽϺ���
* @param pOSListNode List�����ݵĽڵ�
* @param toCompareVal �Ƚϵ�����
*/
static int32_t OSListFindCompareDefault(POSListNode pOSListNode,void* toCompareVal){
    PShellCmd pShellCmd=(PShellCmd)pOSListNode->data;
    /**����ָ����������*/
    if(strcmp((char*)(pShellCmd->funName),(const char*)toCompareVal)==0){
        return 0;
    }
    return -1;
}

/**
 * @brief shell��ʼ��
 * @param PShell shell
 * @return 0 �ɹ� <0ʧ��
 */
int32_t OSShellInit(PShell pShell){
//	int32_t res;
//	int32_t i;
    if(pShell == NULL){
        return -1;
    }

    curShell = pShell;

    OSListCreate(&(pShell->ShellCmdList),OSListAddNtData,
                 OSListDelNtData,OSListFindCompareDefault);

    OSListCreate(&(pShell->KeyCmdList),NULL,
                 NULL,NULL);

    OSShellCmdInit(pShell);


    return 0;
}

/**
*	@breif ����̨����ַ���
* @param pShell shell
* @param str ������ַ���
* @return 0�ɹ� <0ʧ��
*/
int32_t OSShellOutput(PShell pShell,const char* str){
    if( pShell == NULL){
        return -1;
    }
    /**������������*/
    pShell->ShellWrite((uint8_t*)str,strlen(str));
    return 0;
}

/**
*	@breif ����̨���һ���ַ�
* @param pShell shellָ��
* @param ch ������ַ�
* @return 0�ɹ� <0ʧ��
*/
int32_t OSShellOutputChar(PShell pShell,char ch){
    if( pShell == NULL){
        return -1;
    }
    /**������������*/
    pShell->ShellWrite((uint8_t*)&ch,1);
    return 0;
}

/**
* @breif Shellִ��һ��cmd����
* @param pShell shellָ��
* @param cmdStr �����ַ�������\n��β
* @return 0�ɹ� <0ʧ��
*/
int32_t OSShellExecCmd(PShell pShell){
    uint32_t cmdArgCount=0;
    uint16_t i;
    uint8_t flag=0;
    PInputShellCmd inShellCmd;
    PShellCmd pShellCmd;

    if( pShell == NULL){
        return -1;
    }
    if(pShell->shellRecvCount==0){
        return -2;
    }
    char *cmdStr=(char*)pShell->shellRecvCache;
    cmdStr[pShell->shellRecvCount]=0;
    /**���е��ʷָ�*/
    inShellCmd=&(pShell->inputShellCmd);

    for(i=0;i<pShell->shellRecvCount;i++){

        /**һֱΪ�ո�������*/
        if(flag==1||(cmdStr[i]==' '||cmdStr[i]=='\t')){
            if((cmdStr[i]==' '||cmdStr[i]=='\t')){
                cmdStr[i]=0;
                flag=0;
                continue;
            }
        }
        /**�����ڿո��Ҳ�����\t����Ϊһ������*/
        if((flag==0||i==0)&&cmdStr[i]!=' '
           &&cmdStr[i]!='\t'
                ){
            flag=1;
            if(cmdArgCount==0){
                inShellCmd->funName=cmdStr;
            }else{
                inShellCmd->strArg[cmdArgCount-1]=cmdStr+i;
            }
            cmdArgCount++;
            if(cmdArgCount >= SHELL_CMD_ARG_NUM-1){
                /**������������*/
                OSShellOutput(pShell,"\r\nThe command parameter exceeds the system limit.\r\n");
                break;
            }
        }
    }
    cmdArgCount--;
#if OS_SHELL_DEBUG
    OSShellOutput(pShell,"\r\nfunction name\r\n");
	OSShellOutput(pShell,inShellCmd->funName);
	for(i=0;i<cmdArgCount;i++){
		OSShellOutput(pShell,"\r\n");
		OSShellOutput(pShell,inShellCmd->strArg[i]);
		OSShellOutput(pShell,"\r\n");
	}
#endif

    OSShellOutput(pShell, "\r\n");

    pShellCmd = OSShellFindCmdByFunName(pShell,inShellCmd->funName);
    if(pShellCmd==NULL){
//		if((inShellCmd->funName[0]=='.'&&inShellCmd->funName[1]=='/')
//			||(inShellCmd->funName[0]=='.'&&inShellCmd->funName[1]=='\\')){
//
//				return 0;
//			}
//
        OSShellOutput(pShell,"\r\nThe command could not be found.\r\n");
        return -1;
    }

    _MainFun mainFun=(_MainFun)(pShellCmd->fun);
    if(mainFun==NULL){
        return -1;
    }

    int32_t res;
    res=mainFun(cmdArgCount,inShellCmd->strArg);
    if(res<0){
        printf("\n%s\nreturn is%d\n",inShellCmd->funName,res);

    }
    /**��ӡִ�еĽ��*/
    /*OSShellOutput(pShell,"\r\n");
    OSShellOutput(pShell,inShellCmd->funName);
    OSShellOutput(pShell," command exec succuess.\r\n");*/

    return 0;
}

/**
* @breif ͨ�����������ҵ�ShellCmd
* @param funName ��Ҫ�ҵ��ĺ�������
* @return ���ҵ��Ķ���ָ��
*/
PShellCmd OSShellFindCmdByFunName(PShell pShell,const char* funName){
    if(funName==NULL){
        return NULL;
    }

    return OSListFind(&(pShell->ShellCmdList),(void*)funName);
}
/**
* @breif ��������ǰ��shell
* @param pShellCmd ��Ҫ��ӵ�����
* @return 0�ɹ� <0ʧ��
*/
int32_t OSShellAddCmd2CurShell(PShellCmd pShellCmd){
    if(curShell==NULL||pShellCmd==NULL){
        return -1;
    }
    return OSShellAddCmd(curShell,pShellCmd);
}
/**
 * @brief �������
 * @param pShellCmd ��ӵ�����
 * @return 0 �ɹ� <0ʧ��
 */
int32_t OSShellAddCmd(PShell pShell,PShellCmd pShellCmd){
    int32_t res;
    if(pShellCmd==NULL){
        return -1;
    }

    /*���һ��*/
    res=OSListAddNode(&(pShell->ShellCmdList),pShellCmd);
    if(res!=0){
        return res;
    }

    return 0;
}

/**
* @breif shell��Ӽ�ֵ
* @return 0�ɹ� <0ʧ��
*/
int32_t OSShellAddKey(PShell pShell,PKey pKey){
    int32_t res;
    if( pShell == NULL || pKey==NULL){
        return -1;
    }

    /*���һ��*/
    res=OSListAddNode(&(pShell->KeyCmdList),pKey);
    if(res!=0){
        return res;
    }

    return 0;
}
/**
* @breif ��������ַ�����keyƥ�䣬����ƥ��ɹ���Key
* @return 0�ɹ� <0ʧ��
*/
int32_t OSShellKeyMatch(PShell pShell, char inchar){
    int32_t res=-1;
    if( pShell == NULL){
        goto end;
    }
    OSListIterator osLI;

    OSListIteratorStart(&osLI,&(pShell->KeyCmdList));
    uint8_t matchFlag=0;
    PKey pkey;
    while((pkey = OSListIteratorGetNext(&osLI)) != NULL){
        if(
                pkey->data[pkey->matchInx] == inchar /**�ַ�ƥ��*/
                ){
            pkey->matchInx++;
            if(pkey->matchInx >= pkey->keyLen){
                PKey tKey;
                OSListIteratorStop(&osLI);

                OSListIteratorStart(&osLI,&(pShell->KeyCmdList));
                while((tKey = OSListIteratorGetNext(&osLI)) != NULL){
                    /**����ƥ������*/
                    tKey->matchInx=0;
                }
                OSListIteratorStop(&osLI);

                /**ƥ��ɹ�*/
                if(pkey->keyFun!=NULL){
                    pkey->keyFun(pShell);
                }
                pkey->matchInx=0;
                res = 0;
                goto end;
            }

            /**����ƥ��*/
            matchFlag=1;
        }
    }
    if(matchFlag){
        res=1;
    }
    /**û��ƥ�䵽*/
    OSListIteratorStop(&osLI);
    end:
    return res;
}
/**
* @breif ������ƶ�һ���������ֱ���ӵ�
* @param data ��Ҫ�ƶ���ָ��
* @param inx ���Ǹ�λ�ÿ�ʼ�ƶ�
* @param len ���ݳ���
* @return 0�ɹ� <0ʧ��
*/
int32_t OSShellMoveCache(uint8_t *data,int16_t inx,uint16_t dataLen,uint16_t chLen){
    if( data == NULL ){
        return -1;
    }
    if(dataLen==0){
        return -2;
    }
    int16_t i;
    for(i=dataLen-1;i>=inx;i--){
        if( dataLen == chLen && i == dataLen-1){
            continue;
        }
        data[i+1]=data[i];
    }
    return 0;
}

/**
* @breif ����һ�����ַ���shell��������β���������л���
* @param pShell shell��ָ��
* @param inChar ������ַ�
* @return 0�ɹ� <0ʧ��
*/
int32_t OSShellPutInChar(PShell pShell, uint8_t inChar){
    if( pShell == NULL){
        return -1;
    }
    if(pShell->inputMode==0){
        if(pShell->shellDataPosInx==pShell->shellRecvCount){
            goto next;
        }
        /**��������ƶ�n��*/
        OSShellMoveCache(pShell->shellRecvCache
                ,pShell->shellDataPosInx-1
                ,pShell->shellRecvCount
                ,pShell->shellRecvCacheSize
        );
        pShell->shellRecvCache[pShell->shellDataPosInx]=inChar;
        pShell->shellRecvCount++;
        uint16_t i;
        for(i=pShell->shellDataPosInx;i<pShell->shellRecvCount;i++){
            OSShellOutputChar(pShell,pShell->shellRecvCache[i]);
        }
        for(i=pShell->shellDataPosInx;i<pShell->shellRecvCount;i++){
            OSShellOutputChar(pShell,'\b');
        }

    }else{
        next:
        pShell->shellRecvCache[pShell->shellRecvCount]=inChar;
        pShell->shellRecvCount++;
        if(pShell->shellRecvCount >= pShell->shellRecvCacheSize-1){
            return -2;
        }
        pShell->shellDataPosInx++;
        OSShellOutputChar(pShell,inChar);
    }
    return 0;
}
/**
* @breif ����һ���ַ���shell
* @param pShell shell��ָ��
* @param inChar ������ַ�
* @return 0��ȷ <0����
*/
int32_t OSShellInputChar(PShell pShell, uint8_t inChar){

    if( pShell == NULL){
        return -1;
    }

    if(OSShellKeyMatch(pShell,inChar)>=0){
        /**˵����ƥ��*/
        return -2;
    }

    if(inChar>=32 && inChar<=126){
        /**������ַ�*/
        OSShellPutInChar(pShell,inChar);
    }

    return 0;
}
/**
* @breif OSShell����������
* @param arg0 ����Ĳ���
*/
void OSShellTask(void* arg0,void* arg1){
    PShell pShell=arg0;

//    if(arg1){
//
//        int32_t netFd;
//        netFd=ShellNetPortInit();
//        if(netFd>0){
//            int32_t save_fd = dup(0);
//            dup2(netFd, 0);
//        }
//    }
    OSInfo(NULL,NULL);
    while(1){
        int32_t rLen;
        uint8_t rData[6];
        if( pShell->ShellRead!=NULL &&
            (rLen=pShell->ShellRead(rData,sizeof(rData)))>0
                ){
            for(int32_t i=0;i<rLen;i++){
                //if(rData[i]!='\n'){
                if(OSShellInputChar(pShell, rData[i])!=0){
                }
                //}
            }
        }

//        if(pShell->runTaskPid>0){
//            if(waitpid(pShell->runTaskPid,NULL,NULL)<0){
//                SVCOSTaskDelay(-1,50);
//            }else{
//                pShell->runTaskPid=-1;
//            }
//        }
//        SVCOSTaskDelay(-1,10);
    }

}

