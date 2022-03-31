package com.yunze.cn.util;

import com.alibaba.fastjson.JSON;

import java.util.HashMap;
import java.util.Map;

/**
 * @author
 * @title: ResponseJson
 * @description: TODO
 */
public class ResponseJsonMap extends HashMap<String ,Object>  {

    public static String SUCCESS = "200";
    public static String SUCCESSMessage = "操作成功！";

    /**
     * @Description:成功且带数据
     **/
    public static Map<String,Object> success(Object object){
        Map<String,Object> map =new HashMap<String,Object>();
        map.put("code",SUCCESS);
        map.put("msg",SUCCESSMessage);
        map.put("data",object);
        return map;
    }

    /**
     * @Description:成功不带数据
     **/
    public static Map<String,Object> success(){
        return success(null);
    }

    /**
     **/
    public static Map<String,Object> error(String code, String msg){
        Map<String,Object> map =new HashMap<String,Object>();
        map.put("code",code);
        map.put("msg",msg);
        return map;
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
