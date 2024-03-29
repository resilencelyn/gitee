package com.dtstack.taier.develop.service.template.kafka;



import com.dtstack.taier.develop.service.template.DaPluginParam;

import java.util.Map;

/**
 * Date: 2020/3/5
 * Company: www.dtstack.com
 *
 * @author xiaochen
 */
public class KafkaReaderParam extends DaPluginParam {
    private String topic;

    /**
     * json、text
      */
    private String codec;
    /**
     * 可选值：
     * group-offsets：      从ZK / Kafka brokers中指定的消费组已经提交的offset开始消费
     * earliest-offset：    从最早的偏移量开始(如果可能)
     * latest-offset：      从最新的偏移量开始(如果可能)
     * timestamp：          从每个分区的指定的时间戳开始
     * specific-offsets：   从每个分区的指定的特定偏移量开始
     * 必选：否
     * 字段类型：String
     * 默认值：group-offsets
     */
    private String mode;

    /**
     * 必选：当mode为timestamp时必选
     * 字段类型：Long
     * 默认值：无
     */
    private Long timestamp;

    /**
     * 必选：当mode为specific-offsets时必选
     * 字段类型：String
     * 格式：partition:0,offset:42;partition:1,offset:300;partition:2,offset:300
     * 默认值：无
     */
    private String offset;

    /**
     * doris配置规则
     * "mapper": {
     *               "databaseMapper": {
     *                 "prefix": "ods",
     *                 "suffix": "",
     *                 "connectStr": "_",
     *                 "customize": {
     *                   "test": "ods_tiezhu"
     *                 }
     *               },
     *               "tableMapper": {
     *                 "prefix": "ods",
     *                 "suffix": "test",
     *                 "connectStr": "_",
     *                 "customize": {
     *                   "doris_1": "ods_testOne",
     *                   "doris_2": "ods_testOne",
     *                   "doris_3": "ods_testOne",
     *                   "doris_4": "ods_testOne"
     *                 }
     *               },
     *               "fieldMapper": {
     *                 "prefix": "ods",
     *                 "suffix": "test",
     *                 "connectStr": "_",
     *                 "customize": {
     *                   "id": "ods_id_test"
     *                 }
     *               }
     *             }
     */
    private Map<String,Object> mapper;

    public Map<String, Object> getMapper() {
        return mapper;
    }

    public void setMapper(Map<String, Object> mapper) {
        this.mapper = mapper;
    }

    public String getMode() {
        return mode;
    }

    public void setMode(String mode) {
        this.mode = mode;
    }

    public Long getTimestamp() {
        return timestamp;
    }

    public void setTimestamp(Long timestamp) {
        this.timestamp = timestamp;
    }

    public String getOffset() {
        return offset;
    }

    public void setOffset(String offset) {
        this.offset = offset;
    }
    public String getCodec() {
        return codec;
    }

    public void setCodec(String codec) {
        this.codec = codec;
    }

    public String getTopic() {
        return topic;
    }

    public void setTopic(String topic) {
        this.topic = topic;
    }
}
