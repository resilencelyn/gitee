# FreeMQ

#### 介绍
8051的消息队列组件,使用消息队列机制轻松降低代码复杂度.减少使用延时函数的使用频率,提高CPU利用率.需要适应将自己的程序写成异步的形式.本组件适合flash比较小的场景使用,如果flash比较大建议使用Open-ELL的antos,还有rtos.

#### 软件架构
在FreeMQ.h有个定义

    //空的消息ID,不要在正常业务使用空的消息ID,不然无效
    #define EMPTYMSGID 0xFF
    //最大消息数,如果编译显示内存不够需要减少消息队列容量,如果业务系统复杂需要加到数量,不然满了再加会丢消息
    #define MAXMSGCOUNT 5

原理是有个定时器每1耗秒给一个时间戳给主队列加1,添加消息进队列后根据先后顺序和延迟时间来触发消息,这样几乎不用写delay的函数.并且不用把main函数搞的很复杂,一堆标志位.

默认使用主队列就可以满足大部分需求,还支持创建多个队列.

#### 安装教程

1.  克隆项目
2.  编译运行项目就可看到LED在P12闪烁
3.  如果要移植到别的项目只需要拷贝FreeMQ.h,FreeMQ.h,Timer.h加入到项目

#### 使用说明

1.  在main函数加如下代码

        //初始化消息队列,加如消息处理回调
        FreeMQMainInit(&FreeMQMsgHandle);
        ...其他初始化
        //开始计时器
        Timer0Init();
        //添加一个消息,主循环开始后10耗秒后就会触发,也可不用延时直接触发
        FreeMQSendMessageToMainLater(MSG_START, 10);
        while (1)
        {
            //循环执行如有事件需要触发会触发
            FreeMQMainLoop();
        }

2.  实现定时器初始化与中断函数

        //初始化定时器0
        void Timer0Init(void) // 1毫秒@24.000MHz
        {
            Timer0_Stop();								 //停止计数
            TMOD = (TMOD & ~0x03) | TIM_16BitAutoReload; //工作模式,0: 16位自动重装, 1: 16位定时/计数, 2: 8位自动重装, 3: 不可屏蔽16位自动重装
            Timer0_1T();
            Timer0_CLKO_Disable(); //输出时钟使能
            T0_Load(65536UL - (MAIN_Fosc / 1000UL));
            Timer0_Run();
            Timer0_InterruptEnable();
        }
        //定时器0回调时,让主队列计时器进行累加
        void Timer0IntHandle(void) interrupt TIMER0_VECTOR
        {
            FreeMQMainTick(1);
        }

3.  实现消息处理函数

        void FreeMQMsgHandle(u8 msgid)
        {
            switch (msgid)
            {
            case MSG_MainLoop: //主循环
                MainLoopRun();
                //喂看门狗
                FreeMQSendMessageToMainLater(MSG_MainLoop, 500); //主循环每500ms循环一次
                break;
            case MSG_START: //程序启动
                Startup();
                break;
            case MSG_LEDOn: //打开LED
                LEDOn();
                FreeMQSendMessageToMainLater(MSG_LEDOff, 1000);
                break;
            case MSG_LEDOff: //关闭LED
                LEDOff();
                FreeMQSendMessageToMainLater(MSG_LEDOn, 500);
                break;
            }
        }

#### 参与贡献

1.  Fork 本仓库
2.  新建 Feat_xxx 分支
3.  提交代码
4.  新建 Pull Request


#### 目前存在的问题还在解决欢迎大家一起完善

1.  时间戳是4字节的,需要在累加时发现够大了就复位时间和消息,不然会导致时间爆了就会混乱了
2.  需要在列表处理的时候进行锁定机制,避免主函数在操作列表时中断添加消息导致错乱
