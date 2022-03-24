package com.jd.platform.jlog.clientlog4j;

import com.jd.platform.jlog.client.constant.TracerConstant;
import com.jd.platform.jlog.client.log.LogExceptionStackTrace;
import com.jd.platform.jlog.client.tracerholder.TracerHolder;
import com.jd.platform.jlog.client.udp.UdpSender;
import com.jd.platform.jlog.common.model.RunLogMessage;
import org.apache.log4j.AppenderSkeleton;
import org.apache.log4j.Priority;
import org.apache.log4j.spi.LocationInfo;
import org.apache.log4j.spi.LoggingEvent;
import org.slf4j.helpers.FormattingTuple;
import org.slf4j.helpers.MessageFormatter;

/**
 * className：TracerLog4JAppender
 *
 * @author wuweifeng
 * @version 1.0.0
 */
public class TracerLog4JAppender extends AppenderSkeleton {

    @Override
    protected void append(LoggingEvent loggingEvent) {
        try {
            long tracerId = TracerHolder.getTracerId();
            if (0L == tracerId) {
                return;
            }
            RunLogMessage logMessage = getLogMessage(loggingEvent);
            UdpSender.offerLogger(logMessage);
        } catch (Exception e) {

        }
    }

    /**
     * 转化为对象
     */
    private RunLogMessage getLogMessage(LoggingEvent loggingEvent) {
        RunLogMessage logMessage = new RunLogMessage();
        //设置链路唯一id
        logMessage.setTracerId(TracerHolder.getTracerId());
        logMessage.setClassName(loggingEvent.getLoggerName());
        logMessage.setThreadName(loggingEvent.getThreadName());

        LocationInfo locationInfo = loggingEvent.getLocationInformation();
        String method = locationInfo.getMethodName();
        String line = locationInfo.getLineNumber();

        logMessage.setMethodName(method + "(" + locationInfo.getFileName() + ":" + line + ")");
        logMessage.setLogLevel(loggingEvent.getLevel().toString());

        logMessage.setCreateTime(loggingEvent.timeStamp);

        String formattedMessage = getMessage(loggingEvent);
        logMessage.setContent(formattedMessage);

        return logMessage;
    }

    /**
     * 日志正文信息
     */
    private String getMessage(LoggingEvent logEvent) {
        if (logEvent.getLevel().toInt() == Priority.ERROR_INT) {
            String msg = "";
            if (logEvent.getThrowableInformation() != null) {
                msg = LogExceptionStackTrace.erroStackTrace(
                        logEvent.getThrowableInformation().getThrowable()).toString();
            }
            if (logEvent.getRenderedMessage() != null && logEvent.getRenderedMessage()
                    .contains(TracerConstant.DELIM_STR)) {
                FormattingTuple format = MessageFormatter.format(logEvent.getRenderedMessage(), msg);
                return format.getMessage();
            }
            return logEvent.getRenderedMessage() + "\n" + msg;
        }
        return logEvent.getRenderedMessage();
    }

    @Override
    public void close() {

    }

    @Override
    public boolean requiresLayout() {
        return false;
    }
}
