package com.yunze.cn.util;

import com.alibaba.fastjson.JSON;

import java.util.HashMap;
import java.util.Map;

/**
 * @author
 * @title: ResponseJson
 * @description: TODO
 */
public class ResponseJson extends HashMap<String ,Object>  {

    public static String SUCCESS = "200";
    public static String SUCCESSMessage = "操作成功！";

    /**
     * @Description:成功且带数据
     **/
    public static Result success(Object object){
        Result response=new Result();
        response.setCode(SUCCESS);
        response.setMsg(SUCCESSMessage);
        response.setData(object);
        return response;
    }

    /**
     * @Description:成功不带数据
     **/
    public static Result success(){
        return success(null);
    }

    /**
     * @Description:   失败
     **/
    public static Result error(String code, String msg){
        Result response=new Result();
        response.setCode(code);
        response.setMsg(msg);
        return response;
    }


    /**
     * my 成功返回函数
     */
    public static String MyRetunSuccess(Object obj,String msg)
    {
        msg = msg!=null?msg:"操作成功";
        Map<String, Object> Rmap =  new HashMap<String, Object>();
        Rmap.put("code",200);
        Rmap.put("msg",msg);
        Rmap.put("Data",obj);
        try {
            return AesEncryptUtil.encrypt(JSON.toJSONString(Rmap));
        }catch (Exception e){
            System.err.println(e);
            return "数据返回加密失败……";
        }
    }



    public static String Myerr(String Msg)
    {
        Map<String ,Object> map = new HashMap<String,Object>();
        map.put("code",500);
        map.put("msg",Msg);
        try {
            return AesEncryptUtil.encrypt(JSON.toJSONString(map));
            //return JSON.toJSONString(map);
        }catch (Exception e){
            System.err.println(e);
            return "数据返回加密失败……";
        }
    }


}
