package com.yunze.system.card.ApiSyn;

import com.alibaba.fastjson.JSON;
import com.rabbitmq.client.Channel;
import com.yunze.apiCommon.utils.InternalApiRequest;
import com.yunze.common.config.RabbitMQConfig;
import com.yunze.common.core.redis.RedisCache;
import com.yunze.common.mapper.yunze.YzCardMapper;
import com.yunze.apiCommon.mapper.YzCardRouteMapper;
import com.yunze.common.mapper.yunze.YzExecutionTaskMapper;
import com.yunze.common.utils.yunze.*;
import lombok.extern.slf4j.Slf4j;
import org.apache.commons.lang3.StringUtils;
import org.springframework.amqp.rabbit.annotation.RabbitHandler;
import org.springframework.amqp.rabbit.annotation.RabbitListener;
import org.springframework.amqp.rabbit.core.RabbitTemplate;
import org.springframework.dao.DuplicateKeyException;
import org.springframework.stereotype.Component;

import javax.annotation.Resource;
import java.io.IOException;
import java.util.*;
import java.util.concurrent.TimeUnit;

/**
 * 特殊操作 查询imie  消费 者
 */
@Slf4j
@Component
public class CardImportSelImei {

    @Resource
    private YzCardMapper yzCardMapper;
    @Resource
    private RedisCache redisCache;
    @Resource
    private YzExecutionTaskMapper yzExecutionTaskMapper;
    @Resource
    private WriteCSV writeCSV;
    @Resource
    private InternalApiRequest internalApiRequest;
    @Resource
    private YzCardRouteMapper yzCardRouteMapper;
    @Resource
    private RabbitMQConfig rabbitMQConfig;
    @Resource
    private RabbitTemplate rabbitTemplate;


    private String Outcolumns[] = {"iccid","imei","执行描述","执行人","执行结果"};
    private String keys[] = {"iccid","imei","message","agentName","result"};
    private int OutSize = 50;//每 50条数据输出一次

    /**
     * 特殊操作 查询imie
     * @param msg
     * @param channel
     * @throws IOException
     */
    @RabbitHandler
    @RabbitListener(queues = "admin_CardImportSelImei_queue")
    public void CardSelImie(String msg, Channel channel) throws IOException {
        try {
            if (StringUtils.isEmpty(msg)) {
                return;
            }
            Map<String,Object> map = JSON.parseObject(msg);
            String filePath = map.get("filePath").toString();//项目根目录
            String ReadName = map.get("ReadName").toString();//上传新文件名
            Map<String,Object> Pmap =  ( Map<String,Object>)map.get("map");//参数
            Map<String,Object> User =  ( Map<String,Object>)Pmap.get("User");//登录用户信息

            String prefix = "admin_CardImportSelImei_queue";
            //执行前判断 redis 是否存在 执行数据 存在时 不执行
            Object  isExecute = redisCache.getCacheObject(prefix+":"+ ReadName);
            if(isExecute==null){
                redisCache.setCacheObject(prefix+":"+ ReadName, msg, 3, TimeUnit.SECONDS);//3 秒缓存 避免 重复消费
                execution(filePath,ReadName,Pmap,User);//执行特殊操作 查询imie
            }
        } catch (Exception e) {
            log.error(">>错误 - 特殊操作 查询imie 消费者:{}<<", e.getMessage().toString());
        }
    }

    /**
     * 查询执行
     */
    public void execution(String filePath,String ReadName,Map<String,Object> Pmap,Map<String,Object> User){
        String Message = "";
        //1.读取 上传文件
        String path = filePath + "/upload/importSelImei/" + ReadName;
        ExcelConfig excelConfig = new ExcelConfig();
        String columns[] = {"iccid"};
        List<Map<String, Object>> list = excelConfig.getExcelListMap(path,columns);
        Map<String, String> Dept = (Map<String, String>)User.get("dept");
        String  create_by = " [ "+Dept.get("deptName")+" ] - "+" [ "+User.get("userName")+" ] ";

        String newName = UUID.randomUUID().toString().replace("-","")+"_CardImportSelImei";
        String task_name ="特殊操作 [特殊操作 查询imie] ";
        String SaveUrl = "/getcsv/"+newName+".csv";
        SaveUrl = "/getcsv/"+newName+".csv";
        Map<String, Object> task_map = new HashMap<String, Object>();
        task_map.put("auth",create_by);
        task_map.put("task_name",task_name);
        task_map.put("url",SaveUrl);
        task_map.put("agent_id",User.get("deptId") );
        task_map.put("type", "7");
        
        if(list!=null && list.size()>0) {
            //筛选出  iccid 卡号 重复项
            Map<String, Object> getNotRepeatingMap =  Different.getNotRepeating(list,"iccid");//获取 筛选不重复的某列值 和 重复的
            list = (List<Map<String, Object>>) getNotRepeatingMap.get("Rlist");//更新 查询数据
            List<Map<String, Object>> Repeatlist = (List<Map<String, Object>>) getNotRepeatingMap.get("Repeatlist");
            if(Repeatlist.size()>0){
                Map<String, Object> defOutcolumns = new HashMap<>();
                defOutcolumns.put("message","ICCID重复查询失败！同一ICCID同批次，无需多次查询！");
                defOutcolumns.put("agentName",create_by);
                defOutcolumns.put("result","查询失败");
                writeCSV.OutCSVObj(Repeatlist, newName,Outcolumns, keys,defOutcolumns,OutSize);
            }
            //查询数据库中 匹对iccid 是否存在
            HashMap<String, Object> map = new HashMap<>();
            //添加修改数据
            map.put("card_arrs",list);
            map.put("type","3");
            List<String>  iccidarr = yzCardMapper.isExistence(map);
            yzExecutionTaskMapper.add(task_map);//添加执行 任务表
            if (iccidarr != null && iccidarr.size() > 0) {
                //1.判断 查询卡号是否都在库里
                    //库中查询出卡号与上传卡号数量 不一致 说明有卡号不在数据库中
                    if (!(iccidarr.size() == list.size())) {
                        //上传数据>数据库查询 赛选出
                        List<String> list1 = new ArrayList<>();
                        for (int i = 0; i < list.size(); i++) {
                            list1.add(list.get(i).get("iccid").toString());
                        }
                        // 获取 数组去重数据 和 重复值
                        Map<String, Object> getNotRepeatingMap_DB = Different.getNotRepeating(list, iccidarr, "iccid");//获取 筛选不重复的某列值 和 重复的
                        list = (List<Map<String, Object>>) getNotRepeatingMap_DB.get("Rlist");//更新 查询数据
                        List<Map<String, Object>> Out_list_Different = (List<Map<String, Object>>) getNotRepeatingMap_DB.get("Repeatlist");//更新 查询数据
                        //找出与数据库已存在 相同 ICCID 去除 重复 iccid
                        if (Out_list_Different.size() > 0) {
                            Map<String, Object> defOutcolumns = new HashMap<>();
                            defOutcolumns.put("message","iccid 不在数据库中！请核对iccid卡号！");
                            defOutcolumns.put("agentName",create_by);
                            defOutcolumns.put("result","查询失败");
                            writeCSV.OutCSVObj(Out_list_Different, newName,Outcolumns, keys,defOutcolumns,OutSize);
                        }
                    }
                    map.put("card_arrs", list);//更新 list
                    try {
                        if(list!=null && list.size()>0) {
                            //筛选出未划分通道的
                            map.put("channel_idType","notNull");
                            List<String>  Channel_iccidarr = yzCardMapper.isExistence(map);
                            if (Channel_iccidarr != null && Channel_iccidarr.size() > 0) {
                                if (!(Channel_iccidarr.size() == list.size())) {
                                    //上传数据>数据库查询 赛选出
                                    List<String> list1 = new ArrayList<>();
                                    for (int i = 0; i < list.size(); i++) {
                                        list1.add(list.get(i).get("iccid").toString());
                                    }
                                    // 获取 数组去重数据 和 重复值
                                    Map<String, Object> getNotRepeatingMap_DB = Different.getNotRepeating(list, Channel_iccidarr, "iccid");//获取 筛选不重复的某列值 和 重复的
                                    list = (List<Map<String, Object>>) getNotRepeatingMap_DB.get("Rlist");//更新 查询数据
                                    List<Map<String, Object>> Out_list_Different = (List<Map<String, Object>>) getNotRepeatingMap_DB.get("Repeatlist");//更新 查询数据
                                    //找出与数据库已存在 相同 ICCID 去除 重复 iccid
                                    if (Out_list_Different.size() > 0) {
                                        Map<String, Object> defOutcolumns = new HashMap<>();
                                        defOutcolumns.put("message","未划分通道，取消操作！");
                                        defOutcolumns.put("agentName",create_by);
                                        defOutcolumns.put("result","查询失败");
                                        writeCSV.OutCSVObj(Out_list_Different, newName,Outcolumns, keys,defOutcolumns,OutSize);
                                    }
                                }
                                map.put("card_arrs", list);//更新 list
                                List<Map<String, Object>> find_crArr = yzCardRouteMapper.find_cr();//查询 通道简要信息  状态为 正常
                                List<Object> channel_idArr = new ArrayList<>();
                                if(find_crArr!=null && find_crArr.size()>0){
                                    for (int i = 0; i < find_crArr.size(); i++) {
                                        channel_idArr.add(find_crArr.get(i).get("dictValue"));
                                    }
                                }else{
                                    channel_idArr.add("0");
                                }
                                //筛选出通道正常的进行 查询
                                map.put("channel_idArr",channel_idArr);
                                List<String>  normalChannel_iccidarr = yzCardMapper.isExistence(map);
                                if(normalChannel_iccidarr!=null && normalChannel_iccidarr.size()>0){
                                    if (!(normalChannel_iccidarr.size() == list.size())) {
                                        //上传数据>数据库查询 赛选出
                                        List<String> list1 = new ArrayList<>();
                                        for (int i = 0; i < list.size(); i++) {
                                            list1.add(list.get(i).get("iccid").toString());
                                        }
                                        // 获取 数组去重数据 和 重复值
                                        Map<String, Object> getNotRepeatingMap_DB = Different.getNotRepeating(list, normalChannel_iccidarr, "iccid");//获取 筛选不重复的某列值 和 重复的
                                        list = (List<Map<String, Object>>) getNotRepeatingMap_DB.get("Rlist");//更新 查询数据
                                        List<Map<String, Object>> Out_list_Different = (List<Map<String, Object>>) getNotRepeatingMap_DB.get("Repeatlist");//更新 查询数据
                                        //找出与数据库已存在 相同 ICCID 去除 重复 iccid
                                        if (Out_list_Different.size() > 0) {
                                            Map<String, Object> defOutcolumns = new HashMap<>();
                                            defOutcolumns.put("message","通道状态 为停用或已删除 取消操作");
                                            defOutcolumns.put("agentName",create_by);
                                            defOutcolumns.put("result","查询失败");
                                            writeCSV.OutCSVObj(Out_list_Different, newName,Outcolumns, keys,defOutcolumns,OutSize);
                                        }
                                    }
                                    map.put("card_arrs", list);//更新 list
                                    //查询imei 并同步修改 imei
                                    List<Map<String, Object>> SuccessArr = new ArrayList<>();
                                    List<Map<String, Object>> failArr = new ArrayList<>();
                                    List<Map<String, Object>> findRouteArr = yzCardMapper.findRouteArr(map);//查询需要轮询的卡
                                    for (int i = 0; i <findRouteArr.size() ; i++) {
                                        Map<String, Object> Obj = new HashMap<>();
                                        String iccid = findRouteArr.get(i).get("iccid").toString();
                                        Obj.put("iccid", iccid);
                                        //Obj.put("Is_Stop", "on");
                                        Map<String, Object> Rmap = internalApiRequest.queryCardImei(Obj, findRouteArr.get(i));
                                        String code = Rmap.get("code") != null ? Rmap.get("code").toString() : "500";
                                        if (code.equals("200")) {
                                            //获取 imei  更新 card info
                                            Object imei = Rmap.get("imei");
                                            if (imei != null && imei != "" && imei.toString().trim().length() > 0) {
                                                Obj.put("imei", imei.toString().trim());
                                                SuccessArr.add(Obj);
                                                log.info(">>API - 获取imei statusCode = 0 :{} | {}<<", iccid, Rmap);
                                            }
                                        }else{
                                            Obj.put("message", Message);
                                            failArr.add(Obj);
                                        }
                                        if(SuccessArr.size()>0){
                                            try {
                                                updimei(SuccessArr);//下发变更 imei 队列
                                            }catch (Exception e){
                                                log.info(">>错误 - updimei :{} | {}<<", e.getMessage());
                                            }
                                            Map<String, Object> defOutcolumns = new HashMap<>();
                                            defOutcolumns.put("message","操作成功");
                                            defOutcolumns.put("agentName",create_by);
                                            defOutcolumns.put("result","查询成功");
                                            writeCSV.OutCSVObj(SuccessArr, newName,Outcolumns, keys,defOutcolumns,OutSize);
                                        }
                                        if(failArr.size()>0){
                                            Map<String, Object> defOutcolumns = new HashMap<>();
                                            defOutcolumns.put("agentName",create_by);
                                            defOutcolumns.put("result","查询失败");
                                            writeCSV.OutCSVObj(list, newName,Outcolumns, keys,defOutcolumns,OutSize);
                                        }

                                        //
                                    }
                                }else{
                                    Map<String, Object> defOutcolumns = new HashMap<>();
                                    defOutcolumns.put("message","通道状态 为停用或已删除 取消操作！");
                                    defOutcolumns.put("agentName",create_by);
                                    defOutcolumns.put("result","查询失败");
                                    writeCSV.OutCSVObj(list, newName,Outcolumns, keys,defOutcolumns,OutSize);
                                }
                            }else{
                                Map<String, Object> defOutcolumns = new HashMap<>();
                                defOutcolumns.put("message","未划分通道，取消操作！");
                                defOutcolumns.put("agentName",create_by);
                                defOutcolumns.put("result","查询失败");
                                writeCSV.OutCSVObj(list, newName,Outcolumns, keys,defOutcolumns,OutSize);
                            }
                        }
                    } catch (DuplicateKeyException e) {
                        String[] solit = e.getCause().toString().split("'");
                        Map<String, Object> defOutcolumns = new HashMap<>();
                        defOutcolumns.put("message",e.getCause().toString());
                        defOutcolumns.put("agentName",create_by);
                        defOutcolumns.put("result","查询失败");
                        writeCSV.OutCSVObj(list, newName,Outcolumns, keys,defOutcolumns,OutSize);
                        yzExecutionTaskMapper.set_end_time(task_map);//任务结束
                        log.error(">> cardSet-消费者- 上传excel异常 [插入数据 DuplicateKeyException ] :{}<<", e.getMessage().toString());
                    } catch (Exception e) {
                        Map<String, Object> defOutcolumns = new HashMap<>();
                        defOutcolumns.put("message",e.getCause().toString());
                        defOutcolumns.put("agentName",create_by);
                        defOutcolumns.put("result","查询失败");
                        writeCSV.OutCSVObj(list, newName,Outcolumns, keys,defOutcolumns,OutSize);
                        yzExecutionTaskMapper.set_end_time(task_map);//任务结束
                        log.error(">>cardSet-消费者- 批量查询消费者:{}<<", e.getMessage());
                    }
            } else {
                log.info("上传ICCID卡号不在数据库中！请核对后重试！");
                Map<String, Object> defOutcolumns = new HashMap<>();
                defOutcolumns.put("message","查询iccid卡号平台中未找到！查询取消！");
                defOutcolumns.put("agentName",create_by);
                defOutcolumns.put("result","查询失败");
                writeCSV.OutCSVObj(list, newName,Outcolumns, keys,defOutcolumns,OutSize);
            }
        }else{
            log.error( "admin_CardImportSelImei_queue-消费者 上传表格无数据！无需执行");
        }
    }


    /**
     * 创建队列 变更 imei
     * @param SuccessArr
     */
    public void updimei(List<Map<String, Object>> SuccessArr){
        //同步主表数据 如果开始 时间 和 当前 月 时间 一致 剩余用量 += 包用量
        Map<String,Object> UpdMap = new HashMap<>();
        //1.创建路由 绑定 生产队列 发送消息
        String card_exchangeName = "admin_card_exchange", card_queueName = "admin_CardUpd_queue", card_routingKey = "admin.CardUpd.queue",
                card_del_exchangeName = "dlx_"+card_exchangeName,card_del_queueName = "dlx_"+card_queueName, card_del_routingKey = "dlx_"+card_routingKey;
        try {
            rabbitMQConfig.creatExchangeQueue(card_exchangeName, card_queueName, card_routingKey, card_del_exchangeName, card_del_queueName, card_del_routingKey,null);
            for (int i = 0; i < SuccessArr.size(); i++) {
                Map<String, Object> Obj = SuccessArr.get(i);
                UpdMap.putAll(Obj);
                UpdMap.put("queueTypeName","admin_CardUpdimei_queue");
                rabbitTemplate.convertAndSend(card_exchangeName, card_routingKey, JSON.toJSONString(UpdMap), message -> {
                    // 设置消息过期时间 30 分钟 过期
                    message.getMessageProperties().setExpiration("" + (30 * 1000 * 60));
                    return message;
                });
            }
            System.out.println("发送 修改剩余 指令成功 ！");
        } catch (Exception e) {
            System.out.println("发送 修改剩余 创建 失败 " + e.getMessage().toString());
        }
    }







}
