package com.yunze.common.utils.yunze;

import com.alibaba.fastjson.JSONObject;

public class ResponseJson extends JSONObject {

    private String code;
    private String msg;
    private Object data;


    public ResponseJson(String code, String msg, Object data) {
        this.code = code;
        this.msg = msg;
        this.data = data;

    }

    public ResponseJson() {

    }

    /**
     * 开放接口返回格式
     * @param data
     * @return
     */
    public ResponseJson successOpen(Object data){
        this.put("status","0");
        this.put("message","success");
        this.put("result",data);
        return this;
    }


    /**
     * 开放接口返回格式
     * @param code
     * @param msg
     * @return
     */
    public ResponseJson errorOpen(String code,String msg){
        this.put("status",code);
        this.put("message",msg);
        this.put("result",null);
        return this;
    }



    public ResponseJson success(Object data){
        this.put("code","0");
        this.put("msg","success");

        this.put("data",data);
        return this;
    }

    public ResponseJson error(String code,String msg){
        this.put("code",code);
        this.put("msg",msg);
        this.put("data","");
        return this;
    }
}
