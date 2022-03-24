package com.jd.platform.jlog.core;


import static com.jd.platform.jlog.core.Constant.DEFAULT_NAMESPACE;

/**
 * @author tangbohu
 * @version 1.0.0
 * @ClassName ConfigChangeEvent.java
 * @createTime 2022年02月16日 22:13:00
 */
public class ConfigChangeEvent {

    private String key;
    private String oldValue;
    private String newValue;
    private String namespace;
    private ConfigChangeType changeType;


    public ConfigChangeEvent(){

    }

    public ConfigChangeEvent(String key, String newValue) {
        this(key, DEFAULT_NAMESPACE, null, newValue, ConfigChangeType.MODIFY);
    }

    public ConfigChangeEvent(String key, String oldValue, String newValue){
        this(key, DEFAULT_NAMESPACE, oldValue, newValue, ConfigChangeType.MODIFY);
    }

    public ConfigChangeEvent(String key, String namespace, String oldValue, String newValue,
                             ConfigChangeType type) {
        this.key = key;
        this.namespace = namespace;
        this.oldValue = oldValue;
        this.newValue = newValue;
        this.changeType = type;
    }


    public String getKey() {
        return key;
    }


    public ConfigChangeEvent setKey(String key) {
        this.key = key;
        return this;
    }


    public String getOldValue() {
        return oldValue;
    }


    public ConfigChangeEvent setOldValue(String oldValue) {
        this.oldValue = oldValue;
        return this;
    }


    public String getNewValue() {
        return newValue;
    }


    public ConfigChangeEvent setNewValue(String newValue) {
        this.newValue = newValue;
        return this;
    }


    public ConfigChangeType getChangeType() {
        return changeType;
    }


    public ConfigChangeEvent setChangeType(ConfigChangeType changeType) {
        this.changeType = changeType;
        return this;
    }


    public String getNamespace() {
        return namespace;
    }


    public ConfigChangeEvent setNamespace(String namespace) {
        this.namespace = namespace;
        return this;
    }

    @Override
    public String toString() {
        return "ConfigChangeEvent{" +
                "key='" + key + '\'' +
                ", oldValue='" + oldValue + '\'' +
                ", newValue='" + newValue + '\'' +
                ", namespace='" + namespace + '\'' +
                ", changeType=" + changeType +
                '}';
    }
}
