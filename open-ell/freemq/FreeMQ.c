#include "FreeMQ.h"

//#define FR_DEBUG_EN

#ifdef FR_DEBUG_EN

#include "STDIO.H"

#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

struct FreeMQ FreeMainMQ; //主任务

//空的列表
static u8 FreeMQIsEmpty(struct FreeMQ *mq)
{
    if (mq->firstmsg == NULL)
    {
        return 1;
    }
    return 0;
}

//查找下一个存放消息的索引
static struct FreeMQkMessage *FreeMQNewMessage(struct FreeMQ *mq)
{
    u8 msgindex;
    for (msgindex = 0; msgindex < MAXMSGCOUNT; msgindex++)
    {
        if (mq->msglist[msgindex].messageId == EMPTYMSGID)
        {
            return &(mq->msglist[msgindex]);
        }
    }
    return NULL;
}

//删除某个消息
static void FreeMQRemoveMessage(struct FreeMQ *mq, struct FreeMQMessage *prevmsg, struct FreeMQMessage *currmsg)
{
    if (mq->lastmsg == currmsg)
    {
        mq->lastmsg = prevmsg;
    }
    if (mq->firstmsg == currmsg)
    {
        mq->firstmsg = currmsg->nextMsg;
    }
    if (prevmsg != NULL)
    {
        prevmsg->nextMsg = currmsg->nextMsg;
    }
    currmsg->messageId = EMPTYMSGID;
}

//初始化任务
void FreeMQInit(struct FreeMQ *mq, void (*dispatchfun)(u8 messageId))
{
    mq->dispatchhandler = (FreeMQProcessHandle)dispatchfun;
    FreeMQClearMessages(mq);
}

//任务时钟
void FreeMQTick(struct FreeMQ *mq, u32 tick)
{
    mq->timeTick = mq->timeTick + tick;
}

//添加延迟处理消息
void FreeMQSendMessageLater(struct FreeMQ *mq, u8 messageId, u32 delayms)
{
    struct FreeMQMessage *newmsg = FreeMQNewMessage(mq);
    if (newmsg == NULL)
    {
#ifdef FR_DEBUG_EN
        printf("Pool full,send cancel!\n");
#endif
        return; //消息池已满无法添加
    }
    newmsg->messageId = messageId;
    newmsg->timeStamp = mq->timeTick + delayms;
    newmsg->nextMsg = NULL;
    if (FreeMQIsEmpty(mq))
    { //空的

        mq->firstmsg = newmsg;
        mq->lastmsg = newmsg;
    }
    else
    {
        mq->lastmsg->nextMsg = newmsg;
        mq->lastmsg = newmsg;
    }
}

//取消未执行的消息
u8 FreeMQCancelMessages(struct FreeMQ *mq, u8 messageId)
{
    struct FreeMQMessage *prevmsg;
    struct FreeMQMessage *currmsg;
    u8 cancelcount;
    if (FreeMQIsEmpty(mq))
    {
        return 0;
    }
    cancelcount = 0;
    prevmsg = NULL;
    currmsg = mq->firstmsg;
    while (currmsg != NULL)
    {
        if (currmsg->messageId == messageId)
        {
            //删除掉
            FreeMQRemoveMessage(mq, prevmsg, currmsg);
            currmsg = currmsg->nextMsg;
            cancelcount++;
            continue;
        }
        prevmsg = currmsg;
        currmsg = currmsg->nextMsg;
    }
    return cancelcount;
}

//清除所有消息
void FreeMQClearMessages(struct FreeMQ *mq)
{
    u8 msgindex;
    mq->firstmsg = NULL;
    mq->lastmsg = NULL;
    mq->timeTick = 0;
    for (msgindex = 0; msgindex < MAXMSGCOUNT; msgindex++)
    {
        mq->msglist[msgindex].messageId = EMPTYMSGID;
    }
}

//所有空闲时间时触发
void FreeMQLoop(struct FreeMQ *mq)
{
    struct FreeMQMessage *prevmsg;
    struct FreeMQMessage *currmsg;
    if (FreeMQIsEmpty(mq))
    {
        return;
    }
    prevmsg = NULL;
    currmsg = mq->firstmsg;
    while (currmsg != NULL)
    {
        if (currmsg->timeStamp <= mq->timeTick)
        {
            u8 handlemessageid = currmsg->messageId;
            FreeMQRemoveMessage(mq, prevmsg, currmsg);
            //处理到期的消息
            mq->dispatchhandler(handlemessageid);
            //匹配到了

            return;
        }
        prevmsg = currmsg;
        currmsg = currmsg->nextMsg;
    }
}
