package com.jd.platform.jlog.client;

import com.jd.platform.jlog.client.mdc.Mdc;
import io.netty.channel.Channel;

/**
 * Context
 * @author wuweifeng wrote on 2019-12-05
 * @version 1.0
 */
public class Context {
    /**
     * APP_NAME
     */
    public static String APP_NAME;

    /**
     * 客户端发消息用的channel
     */
    public static Channel CHANNEL;
    /**
     * 机房
     */
    public static Mdc MDC;

}
