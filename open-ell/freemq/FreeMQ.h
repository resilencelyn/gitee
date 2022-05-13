#ifndef FREEMQ_H
#define FREEMQ_H

#include	"config.h"

//空的消息ID
#define EMPTYMSGID 0xFF
//最大消息数
#define MAXMSGCOUNT 5

//消息体
struct FreeMQMessage{
	u8 messageId;
  u32 timeStamp;
  struct FreeMQMessage *nextMsg;
};

struct FreeMQ;

//任务回调函数
typedef void (*FreeMQProcessHandle)(u8 messageId);

//任务结构体
struct FreeMQ{
  struct FreeMQMessage *firstmsg;//链表头
  struct FreeMQMessage *lastmsg;//链表尾
	u32 timeTick;//当前时间戳
	struct FreeMQMessage msglist[MAXMSGCOUNT];//消息存储的地方
    FreeMQProcessHandle dispatchhandler;//触发器
};

extern struct FreeMQ FreeMainMQ;//主任务

//时钟追加时间戳,1ms一次
void FreeMQTick(struct FreeMQ *mq,u32 tick);

//初始化任务
void FreeMQInit(struct FreeMQ *mq,void (*dispatchfun)(u8 messageId));

//添加马上要处理的消息
void FreeMQSendMessage(struct FreeMQ *mq,u8 messageId);

//添加延迟处理消息
void FreeMQSendMessageLater(struct FreeMQ *mq,u8 messageId,u32 delayms);

//取消未执行的消息
u8 FreeMQCancelMessages(struct FreeMQ *mq,u8 messageId);

//清除所有消息
void FreeMQClearMessages(struct FreeMQ *mq);

//所有空闲时间时触发,放main函数循环调用
void FreeMQLoop(struct FreeMQ *mq);

//时钟追加时间戳,1ms调用一次
#define FreeMQMainTick(tick) FreeMQTick(&FreeMainMQ,tick)

//初始化主要队列
#define FreeMQMainInit(fun) FreeMQInit(&FreeMainMQ,fun)

//添加马上要处理的消息到主要队列
#define FreeMQSendMessageToMain(messageId) FreeMQSendMessageLater(&FreeMainMQ,messageId,0)

//添加延迟处理消息到主要队列
#define FreeMQSendMessageToMainLater(messageId,delayms) FreeMQSendMessageLater(&FreeMainMQ,messageId,delayms)

//取消未执行的消息到主要队列
#define FreeMQCancelMessagesFromMain(messageId) FreeMQCancelMessages(&FreeMainMQ,messageId)

//清除所有消息到主要队列
#define FreeMQClearMessagesFromMain() FreeMQClearMessages(&FreeMainMQ)

//主任务循环,放到main函数
#define FreeMQMainLoop() FreeMQLoop(&FreeMainMQ)


#endif
