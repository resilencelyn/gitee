package com.jd.platform.jlog.clientdemo.appender;

import ch.qos.logback.classic.Level;
import ch.qos.logback.classic.spi.ILoggingEvent;
import ch.qos.logback.classic.spi.ThrowableProxy;
import ch.qos.logback.core.AppenderBase;
import com.jd.platform.jlog.client.log.LogExceptionStackTrace;
import com.jd.platform.jlog.client.tracerholder.TracerHolder;
import com.jd.platform.jlog.client.udp.UdpSender;
import com.jd.platform.jlog.common.model.RunLogMessage;
import com.jd.platform.jlog.core.ClientHandler;
import org.slf4j.helpers.MessageFormatter;

import static com.jd.platform.jlog.common.handler.CompressHandler.*;


/**
 * className：TracerLog4JAppender
 *
 * @author wuweifeng
 * @version 1.0.0
 */
public class TracerLogbackAppender extends AppenderBase<ILoggingEvent> {
    /**
     * 日志的堆栈级别，如直接在类里使用logger.info，则该值为0. 如调用另一个日志类如CommonLogger打印，则该值为1
     */
    private int loggerStage;

    @Override
    protected void append(ILoggingEvent iLoggingEvent) {
        try {
            long tracerId = TracerHolder.getTracerId();
            if (0L == tracerId) {
                return;
            }
            RunLogMessage logMessage = getLogMessage(iLoggingEvent);
            UdpSender.offerLogger(logMessage);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * 转化为对象
     */
    private RunLogMessage getLogMessage(ILoggingEvent loggingEvent) {
        RunLogMessage logMessage = new RunLogMessage();
        //设置链路唯一id
        logMessage.setTracerId(TracerHolder.getTracerId());
        logMessage.setClassName(loggingEvent.getLoggerName());
        logMessage.setThreadName(loggingEvent.getThreadName());

        StackTraceElement stackTraceElement = loggingEvent.getCallerData()[loggerStage];

        String method = stackTraceElement.getMethodName();
        String line = String.valueOf(stackTraceElement.getLineNumber());
        logMessage.setMethodName(method + "(" + stackTraceElement.getFileName() + ":" + line + ")");
        logMessage.setLogLevel(loggingEvent.getLevel().toString());

        logMessage.setCreateTime(loggingEvent.getTimeStamp());

        String formattedMessage = getMessage(loggingEvent);
        Outcome out = ClientHandler.processLog(formattedMessage);
        // 放入标签
        logMessage.setTagMap(out.getTagMap());
        // 放入内容 可能已经被压缩了
        logMessage.setContent(formattedMessage);
        return logMessage;
    }

    /**
     * 日志正文信息
     */
    private String getMessage(ILoggingEvent logEvent) {
        if (logEvent.getLevel() == Level.ERROR) {
            if (logEvent.getThrowableProxy() != null) {
                ThrowableProxy throwableProxy = (ThrowableProxy) logEvent.getThrowableProxy();
                String[] args = new String[]{logEvent.getFormattedMessage() + "\n" + LogExceptionStackTrace.erroStackTrace(throwableProxy.getThrowable()).toString()};
                return packageMessage("{}", args);
            } else {
                Object[] args = logEvent.getArgumentArray();
                if (args != null) {
                    for (int i = 0; i < args.length; i++) {
                        if (args[i] instanceof Throwable) {
                            args[i] = LogExceptionStackTrace.erroStackTrace(args[i]);
                        }
                    }
                    return packageMessage(logEvent.getMessage(), args);
                }
            }
        }
        return logEvent.getFormattedMessage();
    }

    private String packageMessage(String message, Object[] args) {
        if (message != null && message.contains("{}")) {
            return MessageFormatter.arrayFormat(message, args).getMessage();
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


    public int getLoggerStage() {
        return loggerStage;
    }

    public void setLoggerStage(int loggerStage) {
        this.loggerStage = loggerStage;
    }
}
