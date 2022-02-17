package com.dinglz.mariamanager;

public class HttpData{
    private String  msg;
    private Long bot_id;
    private Long user_id;

    public HttpData(String msg, Long bot_id, Long user_id) {
        this.msg = msg;
        this.bot_id = bot_id;
        this.user_id = user_id;
    }

    public String getMsg() {
        return msg;
    }

    public void setMsg(String msg) {
        this.msg = msg;
    }

    public Long getBot_id() {
        return bot_id;
    }

    public void setBot_id(Long bot_id) {
        this.bot_id = bot_id;
    }

    public Long getUser_id() {
        return user_id;
    }

    public void setUser_id(Long user_id) {
        this.user_id = user_id;
    }
}