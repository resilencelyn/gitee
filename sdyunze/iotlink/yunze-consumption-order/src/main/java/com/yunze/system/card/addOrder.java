package com.yunze.system.card;

import com.alibaba.fastjson.JSON;
import com.rabbitmq.client.Channel;
import com.yunze.common.core.card.AddPacket;
import com.yunze.common.core.redis.RedisCache;
import com.yunze.common.mapper.yunze.YzCardFlowMapper;
import com.yunze.common.mapper.yunze.YzCardMapper;
import com.yunze.common.mapper.yunze.YzExecutionTaskMapper;
import com.yunze.common.mapper.yunze.YzOrderMapper;
import com.yunze.common.utils.yunze.Different;
import com.yunze.common.utils.yunze.ExcelConfig;
import com.yunze.common.utils.yunze.WriteCSV;
import lombok.extern.slf4j.Slf4j;
import org.apache.commons.lang3.StringUtils;
import org.springframework.amqp.rabbit.annotation.RabbitHandler;
import org.springframework.amqp.rabbit.annotation.RabbitListener;
import org.springframework.dao.DuplicateKeyException;
import org.springframework.stereotype.Component;

import javax.annotation.Resource;
import java.io.IOException;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.*;
import java.util.concurrent.TimeUnit;

/**
 * 平台批量充值 消费 者
 */
@Slf4j
@Component
public class addOrder {

    @Resource
    private YzCardMapper yzCardMapper;
    @Resource
    private RedisCache redisCache;
    @Resource
    private YzExecutionTaskMapper yzExecutionTaskMapper;
    @Resource
    private WriteCSV writeCSV;
    @Resource
    private YzOrderMapper yzOrderMapper;


    /**
     * 平台批量充值
     * @param msg
     * @param channel
     * @throws IOException
     */
    @RabbitHandler
    @RabbitListener(queues = "admin_OrderImportRecharge_queue")
    public void createListener(String msg, Channel channel) throws IOException {
        try {
            if (StringUtils.isEmpty(msg)) {
                return;
            }
            Map<String,Object> map = JSON.parseObject(msg);
            String filePath = map.get("filePath").toString();//项目根目录
            String ReadName = map.get("ReadName").toString();//上传新文件名
            String newName = map.get("newName").toString();//输出文件名
            Map<String,Object> Pmap =  ( Map<String,Object>)map.get("map");//参数
            Map<String,Object> User =  ( Map<String,Object>)Pmap.get("User");//登录用户信息
            Map<String,Object> OrderMap =  ( Map<String,Object>)Pmap.get("OrderMap");//添加订单生成参数



            String prefix = "admin_OrderImportRecharge_queue";
            //执行前判断 redis 是否存在 执行数据 存在时 不执行
            Object  isExecute = redisCache.getCacheObject(prefix+":"+ newName);
            if(isExecute==null){
                //System.out.println("SUCCESS");
                redisCache.setCacheObject(prefix+":"+ newName, msg, 30, TimeUnit.SECONDS);//30 秒缓存 避免 重复消费
                AddOrder(filePath,ReadName,newName,Pmap,User,OrderMap);//生成订单
            }
        } catch (Exception e) {
            log.error(">>错误 - 轮序批量加包 消费者:{}<<", e.getMessage().toString());
        }
    }


    /**
     * 生成订单
     * @param filePath
     * @param ReadName
     * @param newName
     * @param Pmap
     * @param User
     * @param OrderMap
     */
    public void AddOrder(String filePath,String ReadName,String newName,Map<String,Object> Pmap,Map<String,Object> User,Map<String,Object> OrderMap){

        //1.读取 上传文件
        String path = filePath + "/upload/importRecharge/" + ReadName;
        ExcelConfig excelConfig = new ExcelConfig();
        String columns[] = {"iccid"};

        List<Map<String, Object>> list = excelConfig.getExcelListMap(path,columns,OrderMap,"order");
        Map<String, String> Dept = (Map<String, String>)User.get("dept");
        String  create_by = " [ "+Dept.get("deptName")+" ] - "+" [ "+User.get("userName")+" ] ";
        String Outcolumns[] = {"iccid","操作描述","执行时间","执行结果","执行人"};
        String keys[] = {"iccid","description","time","result","agentName"};

        if(list!=null && list.size()>0){
            //筛选出  iccid 卡号 重复项
            Map<String, Object> getNotRepeatingMap =  Different.getNotRepeating(list,"iccid");//获取 筛选不重复的某列值 和 重复的
            list = (List<Map<String, Object>>) getNotRepeatingMap.get("Rlist");//更新 充值数据
            List<Map<String, Object>> Repeatlist = (List<Map<String, Object>>) getNotRepeatingMap.get("Repeatlist");
            if(Repeatlist.size()>0){
                writeCSV.OutCSVObj(Repeatlist,newName,"ICCID重复充值失败！同一ICCID同批次，多次充值不允许！",create_by,"充值失败",Outcolumns,keys);
            }
            //查询数据库中 匹对iccid 是否存在
            HashMap<String, Object> map = new HashMap<>();
            map.put("order_arrs",list);
            map.put("card_arrs",list);
            map.put("type","3");
            List<String>  iccidarr = yzCardMapper.isExistence(map);

            String SaveUrl = "/getcsv/"+newName+".csv";
            String task_name = create_by+"-平台批量充值 [充值] ";
            Map<String, Object> task_map = new HashMap<String, Object>();
            task_map.put("auth",create_by);
            task_map.put("task_name",task_name);
            task_map.put("url",SaveUrl);
            task_map.put("agent_id", Dept.get("deptId"));
            task_map.put("type","5");
            yzExecutionTaskMapper.add(task_map);//添加执行 任务表
            //1.判断 充值卡号是否都在库里
            if(iccidarr.size()>0){
                //库中查询出卡号与上传卡号数量 不一致 说明有卡号不在数据库中
                if(!(iccidarr.size()==list.size())){
                    //上传数据>数据库查询 赛选出
                    List<String> list1 = new ArrayList<>();
                    for (int i = 0; i <list.size() ; i++) {
                        list1.add(list.get(i).get("iccid").toString());
                    }
                    // 获取 数组去重数据 和 重复值
                    Map<String, Object> getNotRepeatingMap_DB =  Different.getNotRepeating(list,iccidarr,"iccid");//获取 筛选不重复的某列值 和 重复的
                    list = (List<Map<String, Object>>) getNotRepeatingMap_DB.get("Rlist");//更新 充值数据
                    List<Map<String, Object>> Out_list_Different = (List<Map<String, Object>>) getNotRepeatingMap_DB.get("Repeatlist");//更新 充值数据
                    //找出与数据库已存在 相同 ICCID 去除 重复 iccid
                    if(Out_list_Different.size()>0){
                        writeCSV.OutCSVObj(Out_list_Different,newName,"iccid 不在数据库中！请核对iccid卡号！",create_by,"充值失败",Outcolumns,keys);
                    }
                }
                map.put("order_arrs",list);//更新 list
                map.put("create_by",create_by);
                try {
                    if(list.size()>0){
                        int  sInt = yzOrderMapper.importOrder(map);
                        if(sInt>0){
                            writeCSV.OutCSVObj(list,newName,"平台批量成功 充值 数据 ["+sInt+"] 条",create_by,"充值成功",Outcolumns,keys);
                            yzExecutionTaskMapper.set_end_time(task_map);//任务结束
                        }
                    }
                }catch (DuplicateKeyException e){
                    String[] solit=e.getCause().toString().split("'");
                    writeCSV.OutCSVObj(list,newName,e.getCause().toString(),create_by,"充值失败",Outcolumns,keys);
                    yzExecutionTaskMapper.set_end_time(task_map);//任务结束
                    log.error(">> order-消费者- 上传excel异常 [插入数据 DuplicateKeyException ] :{}<<", e.getMessage().toString());
                }catch (Exception e){
                    writeCSV.OutCSVObj(list,newName,e.getCause().toString(),create_by,"充值失败",Outcolumns,keys);
                    yzExecutionTaskMapper.set_end_time(task_map);//任务结束
                    log.error(">>order-消费者- 批量充值消费者:{}<<", e.getMessage().toString());
                }
            }else{
                //上传卡号不在数据库中
                writeCSV.OutCSVObj(list,newName,"充值iccid卡号平台中未找到！充值取消！",create_by,"充值失败",Outcolumns,keys);
            }
        }else{
            log.error( "admin-消费者 上传表格无数据！无需执行充值");
        }
    }










}
