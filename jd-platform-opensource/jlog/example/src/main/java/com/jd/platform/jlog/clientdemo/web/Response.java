package com.jd.platform.jlog.clientdemo.web;

import java.io.Serializable;

/**
 * @author tangbohu
 * @version 1.0.0
 * @ClassName Response.java
 * @Description TODO
 * @createTime 2022年03月21日 20:40:00
 */
public class Response implements Serializable {

    private Object data;

    private int errno;

    private String msg;

    public Response(Object data ) {
        this.data = data;
        this.errno = 0;
    }

    public Response(int errno, String msg) {
        this.errno = errno;
        this.msg = msg;
    }

    public Object getData() {
        return data;
    }

    public void setData(Object data) {
        this.data = data;
    }

    public int getErrno() {
        return errno;
    }

    public void setErrno(int errno) {
        this.errno = errno;
    }

    public String getMsg() {
        return msg;
    }

    public void setMsg(String msg) {
        this.msg = msg;
    }
}
