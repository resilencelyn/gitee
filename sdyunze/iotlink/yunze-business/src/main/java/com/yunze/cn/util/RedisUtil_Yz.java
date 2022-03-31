package com.yunze.cn.util;

import org.springframework.data.redis.core.RedisTemplate;
import org.springframework.stereotype.Component;

import javax.annotation.Resource;
import java.util.Map;
import java.util.concurrent.TimeUnit;

@Component
public class RedisUtil_Yz {

    @Resource
    public  RedisTemplate<String,Object> redisTemplate;




    /**
     * 描述： 删除指定的key
     */
    public boolean delKey(String key){
        return redisTemplate.delete(key);
    }

    /**
     * 描述： 设置key
     */
    public void setKey(String key,Object value){
        redisTemplate.opsForValue().set(key, value);
    }

    /**
     * auther:
     * return:
     * 描述： 查询当前key是否已经失效 true:失效
     * 时间： 2020/12/18 15:55
     */
    public boolean ttlKey(String key){
        return isExpire(key);
    }

    /**
     * 描述： 查询key是否过期
     */
    public boolean isExpire(String key){
        long t=redisTemplate.opsForValue().getOperations()
                .getExpire(key);
        if(t>1){
            return false;
        }else{
            return true;
        }

    }

    /**
     * 描述： 查询key是否存在
     */
    public boolean existsKey(String key){
        boolean f=redisTemplate.hasKey(key);
        if(f){
            return true;
        }else{
            return false;
        }

    }

    /**
     * return: 添加过期时间 秒
     */
    public void setExpire(String key,Object
                          value,long time) {
        redisTemplate.opsForValue().set(key,value,time,TimeUnit.SECONDS);
    }
    /**
     * 描述： 获取过期时间
     */
    public long getExpire(String key) {
        return redisTemplate.opsForValue().getOperations()
                .getExpire(key);
    }


    /**
     * map 设置 过期时间
     * @param key
     * @param map
     * @param time
     */
    public void setExpireMap(String key,Map<String,Object> map,long time) {
            redisTemplate.opsForHash().putAll(key,map);
            redisTemplate.expire(key, time, TimeUnit.MINUTES);
    }







    public  Object get(Object o){
        return redisTemplate.opsForValue().get(o);
    }

    public void setAccessToken(Object o){
        redisTemplate.opsForValue().set("accessToken",o,2,TimeUnit.HOURS);
    }

    public void set(String key,Object o){

        redisTemplate.opsForValue().set(key,o,4,TimeUnit.HOURS);
    }



    public static void main(String[] args) {
       RedisTemplate redisTemplate=new RedisTemplate();
       //redisTemplate.expire("",600,TimeUnit.SECONDS);

        System.out.println(redisTemplate.opsForValue().getOperations()
                .getExpire(""));

      // long t=redisTemplate.getExpire("");
        // System.out.println(t);
    }
}
