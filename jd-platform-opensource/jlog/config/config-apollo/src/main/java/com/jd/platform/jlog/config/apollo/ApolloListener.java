package com.jd.platform.jlog.config.apollo;

import com.jd.platform.jlog.core.ClientHandlerBuilder;
import com.jd.platform.jlog.core.ConfigChangeEvent;
import com.jd.platform.jlog.core.ConfigChangeListener;

/**
 * @author tangbohu
 * @version 1.0.0
 * @ClassName ApolloListener.java
 * @createTime 2022年02月22日 19:18:00
 */
public class ApolloListener implements ConfigChangeListener {

    @Override
    public void onChangeEvent(ConfigChangeEvent event) {
        ClientHandlerBuilder.refresh();
    }
}
