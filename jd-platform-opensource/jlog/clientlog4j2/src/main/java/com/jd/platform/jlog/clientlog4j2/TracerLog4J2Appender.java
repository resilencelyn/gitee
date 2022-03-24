package com.jd.platform.jlog.clientlog4j2;

import com.jd.platform.jlog.client.log.LogExceptionStackTrace;
import com.jd.platform.jlog.client.tracerholder.TracerHolder;
import com.jd.platform.jlog.client.udp.UdpSender;
import com.jd.platform.jlog.common.model.RunLogMessage;
import org.apache.logging.log4j.Level;
import org.apache.logging.log4j.core.Filter;
import org.apache.logging.log4j.core.Layout;
import org.apache.logging.log4j.core.LogEvent;
import org.apache.logging.log4j.core.appender.AbstractAppender;
import org.apache.logging.log4j.core.config.plugins.Plugin;
import org.apache.logging.log4j.core.config.plugins.PluginAttribute;
import org.apache.logging.log4j.core.config.plugins.PluginElement;
import org.apache.logging.log4j.core.config.plugins.PluginFactory;
import org.apache.logging.log4j.core.layout.PatternLayout;

import java.io.Serializable;

import static org.apache.logging.log4j.message.ParameterizedMessageFactory.INSTANCE;

/**
 * className：TracerLog4JAppender
 *
 * @author wuweifeng
 * @version 1.0.0
 */
@Plugin(name = "tracer", category = "Core", elementType = "appender", printObject = true)
public class TracerLog4J2Appender extends AbstractAppender {

    @PluginFactory
    public static TracerLog4J2Appender createAppender(@PluginAttribute("name") String name,
                                                    @PluginElement("Filter") final Filter filter,
                                                    @PluginElement("Layout") Layout<? extends Serializable> layout) {
        if (name == null) {
            LOGGER.error("No name provided for TracerLog4J2Appender");
            return null;
        }
        if (layout == null) {
            layout = PatternLayout.createDefaultLayout();
        }
        return new TracerLog4J2Appender(name, filter, layout);
    }

    public TracerLog4J2Appender(String name, Filter filter, Layout<? extends Serializable> layout) {
        super(name, filter, layout);
    }

    @Override
    public void append(LogEvent loggingEvent) {
        try {
            long tracerId = TracerHolder.getTracerId();
            if (0L == tracerId) {
                return;
            }
            RunLogMessage logMessage = getLogMessage(loggingEvent);
            UdpSender.offerLogger(logMessage);
        } catch (Exception e) {
            //doNothing
        }
    }

    /**
     * 转化为对象
     */
    private RunLogMessage getLogMessage(LogEvent loggingEvent) {
        RunLogMessage logMessage = new RunLogMessage();
        //设置链路唯一id
        logMessage.setTracerId(TracerHolder.getTracerId());
        logMessage.setClassName(loggingEvent.getLoggerName());
        logMessage.setThreadName(loggingEvent.getThreadName());

        StackTraceElement stackTraceElement = loggingEvent.getSource();
        if (stackTraceElement != null) {
            String method = stackTraceElement.getMethodName();
            String line = String.valueOf(stackTraceElement.getLineNumber());
            logMessage.setMethodName(method + "(" + stackTraceElement.getFileName() + ":" + line + ")");
        } else {
            logMessage.setMethodName(loggingEvent.getThreadName());
        }
        logMessage.setLogLevel(loggingEvent.getLevel().toString());

        logMessage.setCreateTime(loggingEvent.getTimeMillis());

        String formattedMessage = getMessage(loggingEvent);
        logMessage.setContent(formattedMessage);

        return logMessage;
    }

    /**
     * 日志正文信息
     */
    private String getMessage(LogEvent logEvent) {
        if (logEvent.getLevel().equals(Level.ERROR)) {
            // 如果占位符个数与参数个数相同,即使最后一个参数为Throwable类型,logEvent.getThrown()也会为null
            Throwable thrown = logEvent.getThrown();
            String formatMessage = logEvent.getMessage().getFormat();
            Object[] args = logEvent.getMessage().getParameters();
            if (args != null) {
                for (int i = 0, l = args.length; i < l; i++) {
                    // 当最后一个参数与thrown是同一个对象时,表示logEvent.getThrown()不为null,并且占位符个数与参数个数不相同,则将该thrown留后处理
                    if ((i != l - 1 || args[i] != thrown) && args[i] instanceof Throwable) {
                        args[i] = LogExceptionStackTrace.erroStackTrace(args[i]);
                    }
                }
                formatMessage = packageMessage(formatMessage, args);
            }
            if (thrown != null) {
                return packageMessage(formatMessage,
                        new String[]{LogExceptionStackTrace.erroStackTrace(thrown).toString()});
            }
            return formatMessage;
        }
        return logEvent.getMessage().getFormattedMessage();
    }

    private String packageMessage(String message, Object[] args) {
        if (message != null && message.contains("{}")) {
            return INSTANCE.newMessage(message, args).getFormattedMessage();
        }
        return packageMsg(message, args);
    }

    private String packageMsg(String message, Object[] args) {
        StringBuilder builder = new StringBuilder(128);
        builder.append(message);
        for (Object arg : args) {
            builder.append("\n").append(arg);
        }
        return builder.toString();
    }
}
