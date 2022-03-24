package com.jd.platform.jlog.common.handler;

import java.io.Serializable;
import java.util.List;

import static com.jd.platform.jlog.common.handler.CollectMode.COMPRESS_ALL;
import static com.jd.platform.jlog.common.handler.CollectMode.EXTRACT_ALL;

/**
 * @author tangbohu
 * @version 1.0.0
 * @ClassName TagConfig.java
 * @createTime 2022年02月13日 22:35:00
 */
public class TagConfig implements Serializable {

    /**
     * 入参的tag
     */
    private List<String> reqTags;

    /**
     * 普通日志的tag
     */
    private List<String> logTags;

    /**
     * 返参的tag
     */
    private List<String> respTags;

    /**
     * 自定义正则 可为空
     */
    private String regex;

    /**
     * 分隔符 默认｜
     */
    private String delimiter = "|";

    /**
     * 连接符 默认=
     */
    private String join = "=";

    /**
     * 提取策略
     */
    private long extract = EXTRACT_ALL;



    public List<String> getReqTags() {
        return reqTags;
    }

    public void setReqTags(List<String> reqTags) {
        this.reqTags = reqTags;
    }

    public List<String> getLogTags() {
        return logTags;
    }

    public void setLogTags(List<String> logTags) {
        this.logTags = logTags;
    }

    public List<String> getRespTags() {
        return respTags;
    }

    public void setRespTags(List<String> respTags) {
        this.respTags = respTags;
    }

    public String getRegex() {
        return regex;
    }

    public void setRegex(String regex) {
        this.regex = regex;
    }

    public String getDelimiter() {
        return delimiter;
    }

    public void setDelimiter(String delimiter) {
        this.delimiter = delimiter;
    }

    public String getJoin() {
        return join;
    }

    public void setJoin(String join) {
        this.join = join;
    }

    public long getExtract() {
        return extract;
    }

    public void setExtract(long extract) {
        this.extract = extract;
    }


    @Override
    public String toString() {
        return "TagConfig{" +
                "reqTags=" + reqTags +
                ", logTags=" + logTags +
                ", respTags=" + respTags +
                ", regex='" + regex + '\'' +
                ", delimiter='" + delimiter + '\'' +
                ", join='" + join + '\'' +
                ", extract='" + extract + '\'' +
                '}';
    }
}
