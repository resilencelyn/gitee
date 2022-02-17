package com.dinglz.mariamanager;

public class HttpDataGroupMsg {
    private Long group_id;
    private Long bot_id;
    private Long sender_id;
    private String msg;

    public HttpDataGroupMsg(Long group_id, Long bot_id, Long sender_id, String msg) {
        this.group_id = group_id;
        this.bot_id = bot_id;
        this.sender_id = sender_id;
        this.msg = msg;
    }

    public Long getGroup_id() {
        return group_id;
    }

    public void setGroup_id(Long group_id) {
        this.group_id = group_id;
    }

    public Long getBot_id() {
        return bot_id;
    }

    public void setBot_id(Long bot_id) {
        this.bot_id = bot_id;
    }

    public Long getSender_id() {
        return sender_id;
    }

    public void setSender_id(Long sender_id) {
        this.sender_id = sender_id;
    }

    public String getMsg() {
        return msg;
    }

    public void setMsg(String msg) {
        this.msg = msg;
    }
}
