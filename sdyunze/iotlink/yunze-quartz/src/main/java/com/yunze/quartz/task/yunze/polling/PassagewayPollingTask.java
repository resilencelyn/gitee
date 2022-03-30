package com.yunze.quartz.task.yunze.polling;

import com.alibaba.fastjson.JSON;
import com.yunze.common.config.RabbitMQConfig;
import com.yunze.common.core.redis.RedisCache;

import com.yunze.common.mapper.yunze.YzPassagewayPollingMapper;

import org.springframework.stereotype.Component;

import javax.annotation.Resource;
import java.util.*;

/**
 * 定时任务 通道轮序进度同步任务
 * @author root
 */
@Component("passagewayPollingTask")
public class PassagewayPollingTask
{

    @Resource
    private YzPassagewayPollingMapper yzPassagewayPollingMapper;
    @Resource
    private RedisCache redisCache;

    @Resource
    private RabbitMQConfig rabbitMQConfig;


    /**
     * 获取正在执行的轮序任务进度获取  通过 redis 模糊key 查询 获取 轮序任务编号下的卡号
     */
    public void SynChannel()
    {
        //1. 查询  近三天 正在执行的轮序任务 并且 当前执行任务数 小于 任务总数 （三天以上的可能读取数据误差或其他原因不在同步数据了）
        List<Map<String, Object>> channelArr = yzPassagewayPollingMapper.find_execute();
        if(channelArr!=null && channelArr.size()>0){

            for (int i = 0; i < channelArr.size(); i++) {
                Map<String, Object> obj = channelArr.get(i);
                String polling_id = obj.get("polling_id").toString();
                Integer cd_current = Integer.parseInt(obj.get("cd_current").toString());

                Collection<String> keys =  redisCache.scanKeys(polling_id);
                 if(keys!=null && keys.size()>0){
                    //redis 缓存数据大于数据库当前轮询数量时 修改
                    if(keys.size()>cd_current){
                        Map<String, Object> updMap = new HashMap<String, Object>();
                        updMap.put("polling_id",polling_id);
                        updMap.put("cd_current",keys.size());
                        yzPassagewayPollingMapper.update_cd_current(updMap);
                    }
                }
            }

        }
    }






}
