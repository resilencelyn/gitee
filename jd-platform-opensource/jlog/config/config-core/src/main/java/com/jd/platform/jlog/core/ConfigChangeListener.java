/*
 *  Copyright 1999-2019 Seata.io Group.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
package com.jd.platform.jlog.core;


import io.netty.util.concurrent.DefaultThreadFactory;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.concurrent.*;

/**
 * @author tangbohu
 * @version 1.0.0
 * @ClassName ConfigChangeListener.java
 * @createTime 2022年02月16日 12:06:00
 */
public interface ConfigChangeListener {

    Logger LOGGER = LoggerFactory.getLogger(ConfigChangeListener.class);


    ExecutorService EXECUTOR_SERVICE = new ThreadPoolExecutor(1, 1,
            Integer.MAX_VALUE, TimeUnit.MILLISECONDS, new LinkedBlockingQueue<>(), new DefaultThreadFactory("configListener", 1));


    /**
     * 事件具体处理
     * @param event event
     */
    void onChangeEvent(ConfigChangeEvent event);


    /**
     * 处理变更事件
     * @param event 事件
     */
    default void onProcessEvent(ConfigChangeEvent event) {
        LOGGER.info("通用监听器配置处理器", event.toString());
        getExecutorService().submit(() -> {
            beforeEvent();
            try {
                onChangeEvent(event);
            } catch (Exception e) {
                e.printStackTrace();
            }
            afterEvent();
        });
    }

    default void onShutDown() {
        getExecutorService().shutdown();
    }


    default ExecutorService getExecutorService() {
        return EXECUTOR_SERVICE;
    }


    default void beforeEvent() {

    }


    default void afterEvent() {

    }
}
