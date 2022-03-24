package com.jd.platform.jlog.nacos;

import com.alibaba.nacos.api.NacosFactory;
import com.alibaba.nacos.api.config.ConfigService;
import com.alibaba.nacos.api.exception.NacosException;
import com.alibaba.nacos.api.naming.NamingFactory;
import com.alibaba.nacos.api.naming.NamingService;
import com.alibaba.nacos.api.naming.listener.NamingEvent;

import static com.jd.platform.jlog.nacos.NacosConstant.DEFAULT_DATA_ID;
import static com.jd.platform.jlog.nacos.NacosConstant.JLOG_GROUP;

/**
 * @author tangbohu
 * @version 1.0.0
 * @ClassName NacosTest.java
 * @Description TODO
 * @createTime 2022年02月22日 22:04:00
 */
public class NacosTest {

    public static void main(String[] args) throws NacosException, InterruptedException {
        String serverAddr = "101.42.242.201:8848";
        String dataId = DEFAULT_DATA_ID;
        String group = JLOG_GROUP;
        ConfigService configService = NacosFactory.createConfigService(serverAddr);

        NamingService naming = NamingFactory.createNamingService(serverAddr);
        naming.registerInstance("nacos.test1", JLOG_GROUP,"172.22.216.105", 8888, "TEST1");
        Thread.sleep(3000);
        NacosListener server = new NacosListener();
        naming.subscribe("nacos.test1", JLOG_GROUP,server);
        String content = configService.getConfig(dataId, group, 2000L);
        System.out.println("content: "+content);
        NacosListener nL = new NacosListener();
        configService.addListener(dataId, group, nL);

        System.out.println("新增完成");
        Thread.sleep(20000);
        System.out.println("移除开始");
        configService.removeListener(dataId, group, nL);
        System.out.println("移除结束");

        naming.deregisterInstance("nacos.test1", JLOG_GROUP,"172.22.216.105", 8888, "TEST1");
        Thread.sleep(5000);

        System.out.println("睡眠结束");

       /* System.out.println("移除开始222");
        configService.removeListener(dataId, group, new Listener() {
            @Override
            public Executor getExecutor() {
                return Executors.newSingleThreadExecutor();
            }

            @Override
            public void receiveConfigInfo(String s) {
                System.out.println("#### 移除开始222");
            }
        });*/

        Thread.sleep(90000);
        System.out.println("---------------------------睡眠结束");

    }


}
