package com.jd.platform.jlog.common.handler;

import com.alibaba.fastjson.JSON;
import com.jd.platform.jlog.common.utils.FastJsonUtils;
import com.jd.platform.jlog.common.utils.StringUtil;

import java.util.List;
import java.util.Properties;

import static com.jd.platform.jlog.common.utils.ConfigUtil.invoke;

/**
 * @author tangbohu
 * @version 1.0.0
 * @ClassName JcProperties.java
 * @createTime 2022年03月07日 12:43:00
 */
public class JcProperties extends Properties {

    public JcProperties() {}

    public String getString(String key) {
        Object val = get(key);
        if(val == null){
            return null;
        }
        return String.valueOf(val);
    }

    public Long getLong(String key) {
        String val = getString(key);
        if(val == null){
            return null;
        }
        return Long.parseLong(val);
    }


    public List<String> getStrList(String key) {
        if(StringUtil.isEmpty(key)){
            return null;
        }
        String val = getString(key);
        if(StringUtil.isNotEmpty(val)){
            return FastJsonUtils.toList(val,String.class);
        }
        return null;
    }

   /* public List<String> getStrList(String key) {
        if(StringUtil.isEmpty(key)){
            return null;
        }
        int index = 0;
        String suffix;
        String fastSuffix;
        List<String> list = new ArrayList<>();
        do{
            suffix = "["+index+"]";
            fastSuffix = "["+(index+1)+"]";
            list.add(getString(key+suffix));
            index ++;
        }while (getString(key + fastSuffix) != null);
        return list;
    }*/

    public <T> T getBean(String key, Class<T> clz) {
        T bean = FastJsonUtils.toBean(JSON.toJSONString(get(key)), clz);
        if(bean != null){
            return bean;
        }
        try {
            T instance = clz.newInstance();
            invoke(instance, this, "");
            return instance;
        }catch (Exception e){
            e.printStackTrace();
        }

        return null;
    }



}
