package com.yunze.quartz.task.yunze;

import com.alibaba.fastjson.JSON;
import com.rabbitmq.client.BuiltinExchangeType;
import com.yunze.common.config.RabbitMQConfig;
import com.yunze.common.mapper.yunze.YzOrderMapper;
import com.yunze.common.utils.StringUtils;
import com.yunze.common.utils.yunze.ApiParamMap;
import com.yunze.apiCommon.utils.InternalApiRequest;
import org.springframework.amqp.rabbit.core.RabbitTemplate;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import javax.annotation.Resource;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * 定时任务 未执行加包订单执行 加包任务生产
 * @author root
 */
@Component("orderAddPackageProductionTask")
public class OrderAddPackageProductionTask
{

    @Resource
    private YzOrderMapper yzOrderMapper;
    @Resource
    private InternalApiRequest internalApiRequest;
    @Resource
    private ApiParamMap apiParamMap;
    @Autowired
    private RabbitMQConfig mQConfig;
    @Autowired
    private RabbitTemplate rabbitTemplate;




    //路由队列
    String addPackage_exchangeName = "polling_addPackage_card", addPackage_queueName = "p_addPackage_card", addPackage_routingKey = "p.addPackage.card",
        addPackage_del_exchangeName = "dlx_"+addPackage_exchangeName,addPackage_del_queueName = "dlx_"+addPackage_queueName, addPackage_del_routingKey = "dlx_"+addPackage_routingKey;


    // 除 生效类型为 [激活生效] 外 所有加包 增加到生产任务中去
    public void addPackageProduction(Integer time)
    {

        //0.自动 修改已经加包的订单加包状态 [防止重复加包]
        try {
            yzOrderMapper.updAutomatic();
        }catch (Exception e){
            System.out.println("无 修改已经加包的订单加包状态");
        }

        //1.获取 支付成功 > 用量充值 > 未执行加包的 订单
        Map<String,Object> findAddPackage_Map = new HashMap<>();
        findAddPackage_Map.put("validate_type","4");
        findAddPackage_Map.put("type","notin");
        List<Map<String,Object>> AddPackageArr = yzOrderMapper.findAddPackage(findAddPackage_Map);
        AddPackag(AddPackageArr,time);
    }


    //  生效类型为 [激活生效] 且 已有生效时间 增加到生产任务中去
    public void activationAddPackageProductionAndActivateDate(Integer time)
    {

        //0.自动 修改已经加包的订单加包状态 [防止重复加包]
        try {
            yzOrderMapper.updAutomatic();
        }catch (Exception e){
            System.out.println("无 修改已经加包的订单加包状态");
        }

        //1.获取 支付成功 > 用量充值 > 未执行加包的 [激活生效]  订单
        Map<String,Object> findAddPackage_Map = new HashMap<>();
        findAddPackage_Map.put("validate_type","4");
        findAddPackage_Map.put("type","in");
        findAddPackage_Map.put("activate_date","notnull");
        List<Map<String,Object>> AddPackageArr = yzOrderMapper.findAddPackage(findAddPackage_Map);
        AddPackag(AddPackageArr,time);
    }



    public void AddPackag(List<Map<String,Object>> AddPackageArr,int time){
        if(AddPackageArr!=null && AddPackageArr.size()>0){
            //卡号放入路由
            for (int j = 0; j < AddPackageArr.size(); j++) {
                Map<String, Object> card = AddPackageArr.get(j);
                //生产任务
                try {
                    if (j == 0) {
                        //设置任务 路由器 名称 与队列 名称
                        mQConfig.creatExchangeQueue(addPackage_exchangeName, addPackage_queueName, addPackage_routingKey, addPackage_del_exchangeName, addPackage_del_queueName, addPackage_del_routingKey,null);
                    }
                    rabbitTemplate.convertAndSend(addPackage_exchangeName, addPackage_routingKey, JSON.toJSONString(card), message -> {
                        // 设置消息过期时间 time 分钟 过期
                        message.getMessageProperties().setExpiration("" + (time * 1000 * 60));
                        return message;
                    });
                } catch (Exception e) {
                    System.out.println(e.getMessage().toString());
                }
            }
        }
    }





    //  生效类型为 [激活生效] 且 无生效时间 API更新激活时间 【不生产】让 activationAddPackageProductionAndActivateDate 去生产
    public void activationAddPackageProduction()
    {

        //0.自动 修改已经加包的订单加包状态 [防止重复加包]
        try {
            yzOrderMapper.updAutomatic();
        }catch (Exception e){
            System.out.println("无 修改已经加包的订单加包状态");
        }


        //1.获取 支付成功 > 用量充值 > 未执行加包的 [激活生效]  订单
        Map<String,Object> findAddPackage_Map = new HashMap<>();
        findAddPackage_Map.put("validate_type","4");
        findAddPackage_Map.put("type","in");
        findAddPackage_Map.put("activate_date","null");
        findAddPackage_Map.put("channel_id","notnull");


        List<Map<String,Object>> AddPackageArr = yzOrderMapper.findAddPackage(findAddPackage_Map);

        if(AddPackageArr!=null && AddPackageArr.size()>0){
            //2.筛选出 无 激活日期的卡信息 调用API接口请求 获取 激活信息
            for (int i = 0; i < AddPackageArr.size(); i++) {
                Map<String,Object> map = AddPackageArr.get(i);
                Map<String,Object> Parammap=new HashMap<>();
                Parammap.put("iccid",map.get("iccid"));
                Map<String,Object> apiMapNet=apiParamMap.getApiParams(Parammap);
                Map<String,Object> Rmap =  null;//apiUtil.queryCardActiveTime(apiMapNet,);
                String statusCode =  Rmap.get("statusCode")!=null?Rmap.get("statusCode").toString():"500";
                if(statusCode.equals("200")){
                    //获取 激活日期 开卡日期 更新 card info
                    if(Rmap.get("activateDate")!=null &&  Rmap.get("activateDate")!="" &&  Rmap.get("activateDate").toString().trim().length()>0){
                        String activateDate = Rmap.get("activateDate").toString().trim();
                        Object openDate = Rmap.get("openDate")!=null &&  Rmap.get("openDate")!="" &&  Rmap.get("openDate").toString().trim().length()>0?
                                Rmap.get("openDate").toString().trim():null;
                        Map<String,Object> Upd_Map = new HashMap<>();
                        Upd_Map.put("activate_date",activateDate);
                        if(openDate!=null){
                            Upd_Map.put("open_date",openDate);
                        }
                        Upd_Map.put("id",map.get("Bid"));
                        boolean bool = yzOrderMapper.updActivate(Upd_Map)>0;//变更激活时间
                        System.out.println(map.get("iccid")+" 修改激活时间 bool ： "+bool);
                    }
                }else{
                    System.out.println(map.get("iccid")+" 未获取到激活日期 ！");
                }
            }
        }
    }


    /**
     * 创建 生产者路由队列
     * @param time
     * @param size
     * @param polling_id
     * @param p_Map
     * @param type
     * @return
     */
    public boolean CreateExchange(int time, int size, String polling_id,Map<String, Object> p_Map,String type) {
        boolean bool = true;
        //卡状态  轮询
        try {
            mQConfig.creatExchangeQueue(addPackage_exchangeName, addPackage_queueName, addPackage_routingKey, null, null, null, BuiltinExchangeType.FANOUT);
            Map<String, Object> start_type = new HashMap<>();
            start_type.putAll(p_Map);
            start_type.put("type", type);
            rabbitTemplate.convertAndSend(addPackage_exchangeName, addPackage_queueName, JSON.toJSONString(start_type), message -> {
                // 设置消息过期时间 time 分钟 过期
                message.getMessageProperties().setExpiration("" + (time * 1000 * 60));
                return message;
            });
        } catch (Exception e) {
            bool = false;
            System.out.println("轮询 生产激活生效包  [CreateExchange] 启动类型 失败 " + e.getMessage().toString());
        }
        return bool;
    }



}
