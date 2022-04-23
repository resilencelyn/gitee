#ifndef _OS_SHELL_H__
#define _OS_SHELL_H__

#include "list.h"
#include "type.h"
#include "mkrtos/fs.h"
/**
* @breif shell�İ汾
*/
#define SHELL_VERSION "1"
/**
 * @breif �Ƿ�֧�������
 */
#define SHELL_USING_CMD_EXPORT 1
/**
* @breif �Ƿ����
*/
#define OS_SHELL_DEBUG 0

/**
* @breif ��ʷ��¼����
*/
#define HISTORY_CMD_NUM 10

/**
* @breif shell �����в�������
*/
#define SHELL_CMD_ARG_NUM 8

/**
* @breif ����������ַ�
*/
#define OS_SHELL_NEW_LINE "\r\nOS Shell"

/**
* @breif shell���ջ�������С
*/
#define SHELL_RECV_FIFO_CACHE_SIZE 512

/**
* @breif �������ں�������ں�������Ϊͳһ����ں���
*/
typedef int32_t (*_MainFun)(int32_t argc,const char* args[]);

/**
* @breif ����Ĳ���
*/
typedef struct _InputShellCmd{

    /**
    * @breif ��������
    */
    const char *funName;

    /**
    * @breif �����б�
    */
    const char *strArg[SHELL_CMD_ARG_NUM];

}*PInputShellCmd,InputShellCmd;

/**
* @breif ����
*/
typedef struct _ShellCmd{

    /**
    * @breif shell�����
    */
    void* fun;
    /**
    * @breif ��������
    */
    const char* funName;

}*PShellCmd,ShellCmd;

/**
*	@breif shell��д����
* @param data ��Ҫ���͵�����
* @param len ��Ҫ���͵ĳ���
* @return ��Ҫ���ص�����
*/
typedef int32_t (*_ShellWrite)(uint8_t* data, uint16_t len);
/**
* @breif shell��ȡ����
* @param data ��Ҫ��ȡ�����ݳ���
* @parma len ��ȡ�������ĳ���
* @return ��ȡ�������ݳ���
*/
typedef int32_t (*_ShellRead)(uint8_t* data, uint16_t len);
/**
* @breif ��ʾ���к�������ʾ�����������Ĺ��Ӻ���
*/
typedef char* (*_ShellLRHook)(void);
/**
* @breif ��ȡtickʱ�䣬��λms
* @return ���ػ�ȡ����ʱ��
*/
typedef uint32_t (*_ShellTick)(void);

/**
* @breif �����ڴ�
* @param num ������ڴ�
* @return ���뵽���ڴ�
*/
typedef void* (*_ShellMalloc)(size_t num);

/**
* @breif �ͷ��ڴ�
* @param mem �ͷŵ��ڴ�
*/
typedef void 	(*_ShellFree)(void* mem);

/**
* @breif ����̨��struct
*/
typedef struct _Shell{

    /**
    * @breif �����ڴ�ĺ���
    */
    _ShellMalloc ShellMalloc;

    /**
    * @breif �ͷ��ڴ�ĺ���
    */
    _ShellFree ShellFree;

    /**
    * @breif shellд�ַ�
    */
    _ShellWrite ShellWrite;

    /**
    *	@breif shell���ַ�
    */
    _ShellRead ShellRead;
    /**
    *	@breif �س�����ʾ�Ĺ��Ӻ���
    */
    _ShellLRHook ShellLRHook;


    /**
    *	@breif shell tick
    */
    _ShellTick ShellTick;



    /**
    * @breif shell���ܻ�����
    */
    uint8_t *shellRecvCache;
    /**
    * @breif shell�������ݵĻ�������С
    */
    uint16_t shellRecvCacheSize;
    /**
    * @breif shell�Ѿ����յ����ݴ�С
    */
    uint16_t shellRecvCount;
    /**
    * @breif shellָ����Ƶ�λ��
    */
    int16_t shellDataPosInx;
    /**
    * @breif �ϴΰ������µ�ʱ��
    */
    uint32_t lastKeyPressTick;


    /**
    * @breif �����б�
    */
    OSList ShellCmdList;

    /**
    * @breif ��ֵ�б�
    */
    OSList KeyCmdList;

    /**
    * @breif �洢��ʷ��¼
    */
    const char* cmdList[HISTORY_CMD_NUM];
    /**
    * @breif ��ʷ��¼����
    */
    int16_t cmdCount;
    /**
    * @breif ʹ�õ���һ��inx����Ϊ-1ʱ��ʹ�õĵ�ǰ���뻺����
    */
    int16_t cmdInx;

    /**
    * @breif �����ģʽ��������insert��������ģʽ��һ��ʱ����ģʽ��һ���ǲ���ģʽ��Ĭ��Ϊ����ģʽ
    */
    uint8_t inputMode;

    /**
    * @breif ��ǰ��������Ĳ����洢
    */
    InputShellCmd inputShellCmd;

    /**
    *@brief ctrl c
    */
    uint8_t ctrlCSig;

    int16_t runTaskPid;

}*PShell, Shell;

/**
* @breif ��ֵ�ṹ��
*/
typedef struct _Key{
    /**
    * @breif �����ص�����
    * @param pShell shell��ָ��
    */
    int32_t (*keyFun)(PShell pShell);
    /**
    * @breif ��ֵ����
    */
    const char *data;
    /**
    * @breif ��ֵ���ݳ���
    */
    uint8_t keyLen;
    /**
    * @breif ƥ���inx
    */
    uint8_t matchInx;
}*PKey, Key;

/**
 * @brief shell��ʼ��
 * @param PShell shell
 * @return 0 �ɹ� <0ʧ��
 */
int32_t OSShellInit(PShell pShell);
/**
* @breif ��������ǰ��shell
* @param pShellCmd ��Ҫ��ӵ�����
*/
int32_t OSShellAddCmd2CurShell(PShellCmd pShellCmd);

/**
 * @brief �������
 * @param pShellCmd ��ӵ�����
 * @return 0 �ɹ� <0ʧ��
 */
int32_t OSShellAddCmd(PShell pShell,PShellCmd pShellCmd);

/**
* @breif ͨ�����������ҵ�ShellCmd
* @param funName ��Ҫ�ҵ��ĺ�������
* @return ���ҵ��Ķ���ָ��
*/
PShellCmd OSShellFindCmdByFunName(PShell pShell,const char* funName);

/**
* @breif Shellִ��һ��cmd����
* @param pShell shellָ��
* @param cmdStr �����ַ�������\n��β
*/
int32_t OSShellExecCmd(PShell pShell);

/**
* @breif OSShell����������
* @param arg0 ����Ĳ���
*/
void OSShellTask(void* arg0,void* arg1);
/**
* @breif ��������ַ�����keyƥ�䣬����ƥ��ɹ���Key
*/
int32_t OSShellKeyMatch(PShell pShell,char inchar);
/**
* @breif shell��Ӽ�ֵ
*/
int32_t OSShellAddKey(PShell pShell,PKey pKey);
/**
* @breif ����һ�����ַ���shell��������β���������л���
* @param pShell shell��ָ��
* @param inChar ������ַ�
*/
int32_t OSShellPutInChar(PShell pShell, uint8_t inChar);
/**
* @breif ����һ���ַ���shell
* @param pShell shell��ָ��
* @param inChar ������ַ�
* @return 0��ȷ <0����
*/
int32_t OSShellInputChar(PShell pShell, uint8_t inChar);
/**
*	@breif ����̨���һ���ַ�
*/
int32_t OSShellOutputChar(PShell pShell,char ch);
/**
*	@breif ����̨����ַ���
*/
int32_t OSShellOutput(PShell pShell,const char* str);
/**
* @breif �����ǰ���������
* @param pShell shellָ��
*/
int32_t OSShellClearCurInput(PShell pShell);

/**
 * @breif shell����������Կ��ٵ�������
 */
#define SHELL_EXPORT_CMD(_fun, _name) \
        const char shellCmd##_##_name[] = #_name; \
        const ShellCmd \
        shellCommand##_name SECTION("shellCmd") =  \
        { \
        	.fun = _fun,\
			.funName = shellCmd##_##_name,\
        }


/**
* @breif ��ǰʹ���е�shell
*/
extern PShell curShell;

#endif
