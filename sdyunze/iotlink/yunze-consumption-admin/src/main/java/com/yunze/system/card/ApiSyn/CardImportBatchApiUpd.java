package com.yunze.system.card.ApiSyn;

import com.alibaba.fastjson.JSON;
import com.rabbitmq.client.Channel;
import com.yunze.apiCommon.utils.InternalApiRequest;
import com.yunze.common.config.MyDictionary;
import com.yunze.common.core.redis.RedisCache;
import com.yunze.common.mapper.yunze.YzCardInfoChangeMapper;
import com.yunze.common.mapper.yunze.YzCardMapper;
import com.yunze.apiCommon.mapper.YzCardRouteMapper;
import com.yunze.common.mapper.yunze.YzExecutionTaskMapper;
import com.yunze.common.utils.yunze.*;
import lombok.extern.slf4j.Slf4j;
import org.apache.commons.lang3.StringUtils;
import org.springframework.amqp.rabbit.annotation.RabbitHandler;
import org.springframework.amqp.rabbit.annotation.RabbitListener;
import org.springframework.dao.DuplicateKeyException;
import org.springframework.stereotype.Component;

import javax.annotation.Resource;
import java.io.IOException;
import java.util.*;
import java.util.concurrent.TimeUnit;

/**
 * 特殊操作 批量变更停复机操作
 */
@Slf4j
@Component
public class CardImportBatchApiUpd {

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
    private YzCardInfoChangeMapper yzCardInfoChangeMapper;
    @Resource
    private MyDictionary myDictionary;
    @Resource
    private GetShowStatIdArr getShowStatIdArr;

    private String Outcolumns[] = {"iccid", "返回消息", "执行描述", "执行人", "执行结果"};
    private String keys[] = {"iccid", "Message", "describe", "agentName", "result"};
    private int OutSize = 50;//每 50条数据输出一次

    /**
     * 批量停复机 断开网 操作
     */
    @RabbitHandler
    @RabbitListener(queues = "admin_CardImportBatch_queue")
    public void CardImportBatch(String msg, Channel channel) throws IOException {
        try {
            if (StringUtils.isEmpty(msg)) {
                return;
            }
            Map<String, Object> map = JSON.parseObject(msg);
            String filePath = map.get("filePath").toString();//项目根目录
            String ReadName = map.get("ReadName").toString();//上传新文件名
            Map<String, Object> Pmap = (Map<String, Object>) map.get("map");//参数
            Map<String, Object> User = (Map<String, Object>) Pmap.get("User");//登录用户信息

            String prefix = "admin_CardImportBatch_queue";
            //执行前判断 redis 是否存在 执行数据 存在时 不执行
            Object isExecute = redisCache.getCacheObject(prefix + ":" + ReadName);
            if (isExecute == null) {
                redisCache.setCacheObject(prefix + ":" + ReadName, msg, 3, TimeUnit.SECONDS);//3 秒缓存 避免 重复消费
                execution(filePath, ReadName, Pmap, User);//执行特殊操作 查询imie
            }
        } catch (Exception e) {
            log.error(">>错误 - 特殊操作 批量停复机 断开网 消费者:{}<<", e.getMessage().toString());
        }
    }

    /**
     * 查询执行
     */
    public void execution(String filePath, String ReadName, Map<String, Object> Pmap, Map<String, Object> User) {
        String Message = "";
        String cbefore = "";//变更前
        String cafterward = "";//变更后
        String OPtionType = "";//操作类型
        String describe = "";

        //1.读取 上传文件
        String path = filePath + "/upload/importSelImei/" + ReadName;
        ExcelConfig excelConfig = new ExcelConfig();
        String columns[] = {"iccid"};
        List<Map<String, Object>> list = excelConfig.getExcelListMap(path, columns);
        Map<String, String> Dept = (Map<String, String>) User.get("dept");
        String create_by = " [ " + Dept.get("deptName") + " ] - " + " [ " + User.get("userName") + " ] ";

        String newName = UUID.randomUUID().toString().replace("-", "") + "_CardChangeInfo";
        String task_name = "-批量变更 [批量变更 输出] ";
        String SaveUrl = "/getcsv/" + newName + ".csv";
        SaveUrl = "/getcsv/" + newName + ".csv";
        Map<String, Object> task_map = new HashMap<String, Object>();
        task_map.put("auth", create_by);
        task_map.put("task_name", task_name);
        task_map.put("url", SaveUrl);
        task_map.put("agent_id", User.get("deptId"));
        task_map.put("type", "8");

        if (list != null && list.size() > 0) {
            //筛选出  iccid 卡号 重复项
            Map<String, Object> getNotRepeatingMap = Different.getNotRepeating(list, "iccid");//获取 筛选不重复的某列值 和 重复的
            list = (List<Map<String, Object>>) getNotRepeatingMap.get("Rlist");//更新 查询数据
            List<Map<String, Object>> Repeatlist = (List<Map<String, Object>>) getNotRepeatingMap.get("Repeatlist");
            if (Repeatlist.size() > 0) {
                Map<String, Object> defOutcolumns = new HashMap<>();
                defOutcolumns.put("describe", "ICCID重复查询失败！同一ICCID同批次，无需多次查询！");
                defOutcolumns.put("agentName", create_by);
                defOutcolumns.put("result", "查询失败");
                writeCSV.OutCSVObj(Repeatlist, newName, Outcolumns, keys, defOutcolumns, OutSize);
            }
            //查询数据库中 匹对iccid 是否存在
            HashMap<String, Object> map = new HashMap<>();
            //添加修改数据
            map.put("card_arrs", list);
            map.put("type", "3");
            List<String> iccidarr = yzCardMapper.isExistence(map);
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
                        defOutcolumns.put("describe", "iccid 不在数据库中！请核对iccid卡号！");
                        defOutcolumns.put("agentName", create_by);
                        defOutcolumns.put("result", "查询失败");
                        writeCSV.OutCSVObj(Out_list_Different, newName, Outcolumns, keys, defOutcolumns, OutSize);
                    }
                }
                map.put("card_arrs", list);//更新 list
                try {
                    if (list != null && list.size() > 0) {
                        //筛选出未划分通道的
                        map.put("channel_idType", "notNull");
                        List<String> Channel_iccidarr = yzCardMapper.isExistence(map);
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
                                    defOutcolumns.put("describe", "未划分通道，取消操作！");
                                    defOutcolumns.put("agentName", create_by);
                                    defOutcolumns.put("result", "查询失败");
                                    writeCSV.OutCSVObj(Out_list_Different, newName, Outcolumns, keys, defOutcolumns, OutSize);
                                }
                            }
                            map.put("card_arrs", list);//更新 list
                            List<Map<String, Object>> find_crArr = yzCardRouteMapper.find_cr();//查询 通道简要信息  状态为 正常
                            List<Object> channel_idArr = new ArrayList<>();
                            if (find_crArr != null && find_crArr.size() > 0) {
                                for (int i = 0; i < find_crArr.size(); i++) {
                                    channel_idArr.add(find_crArr.get(i).get("dictValue"));
                                }
                            } else {
                                channel_idArr.add("0");
                            }
                            //筛选出通道正常的进行 查询
                            map.put("channel_idArr", channel_idArr);
                            List<String> normalChannel_iccidarr = yzCardMapper.isExistence(map);
                            if (normalChannel_iccidarr != null && normalChannel_iccidarr.size() > 0) {
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
                                        defOutcolumns.put("describe", "通道状态 为停用或已删除 取消操作");
                                        defOutcolumns.put("agentName", create_by);
                                        defOutcolumns.put("result", "查询失败");
                                        writeCSV.OutCSVObj(Out_list_Different, newName, Outcolumns, keys, defOutcolumns, OutSize);
                                    }
                                }
                                map.put("card_arrs", list);//更新 list

                                List<Map<String, Object>> SuccessArr = new ArrayList<>();
                                List<Map<String, Object>> failArr = new ArrayList<>();

                                String Is_remind_ratio = Pmap.get("Is_remind_ratio").toString();
                                System.out.println(Is_remind_ratio.toString());
                                String Switch_network = Pmap.get("Switch_network").toString();
                                List<Map<String, Object>> findRouteArr = yzCardMapper.findRouteArr(map);//查询 卡号 分配 通道 及变更前 卡状态 断开网状态
                                //判断 取值 key 字典key  ctype
                                String ctype = "";
                                String dict_type = "";
                                String key = "";
                                if (Is_remind_ratio.equals("3") || Is_remind_ratio.equals("2")) {//停复机
                                    key = "Sel_status_ShowId";
                                    dict_type = "yunze_card_status_ShowId";
                                    ctype = "1";
                                }else if (Switch_network.equals("3") || Switch_network.equals("2")) {//断开网
                                    key = "Sel_connection_status";
                                    dict_type = "yz_cardConnection_type";
                                    ctype = "2";
                                }


                                for (int i = 0; i < findRouteArr.size(); i++) {
                                    Map<String, Object> Obj = findRouteArr.get(i);
                                    String iccid = Obj.get("iccid").toString();
                                    Obj.put("iccid", iccid);

                                    //设置 变更前后 数据
                                    Obj.put("ctype", ctype);//变更 类型
                                    String value = Obj.get(key) != null && Obj.get(key).toString().length() > 0 ? Obj.get(key).toString() : "未知";
                                    if (!value.equals("未知")) {
                                        cbefore = myDictionary.getdictLabel(dict_type, value);
                                    } else {
                                        cbefore = value;
                                    }
                                    Obj.put("cbefore", cbefore);//变更前


                                    //【是否停复机】  on 开机 off 停机
                                    System.out.println(Switch_network.toString());
                                    if (Is_remind_ratio.equals("1") && Switch_network.equals("1")) {
                                        System.out.println("不操作");
                                    } else if (!Is_remind_ratio.equals("1") && !Switch_network.equals("1")) {
                                        System.out.println("多项操作不允许");
                                    } else if (!Is_remind_ratio.equals("1")) {
                                        System.out.println("批量停复机操作");
                                        describe = "批量停复机操作-";
                                        OPtionType = "changeCardStatus";
                                        if (Is_remind_ratio.equals("3")) {
                                            describe += "【复机】";
                                            cafterward = "已激活";
                                            Obj.put("Is_Stop", "on");
                                        } else if (Is_remind_ratio.equals("2")) {
                                            describe += "【停机】";
                                            cafterward = "已停机";
                                            Obj.put("Is_Stop", "off");
                                        }
                                        Obj.put("cafterward", cafterward);//变更后
                                        Map<String, Object> Rmap = internalApiRequest.changeCardStatus(Obj, findRouteArr.get(i));
                                        String code = Rmap.get("code") != null ? Rmap.get("code").toString() : "500";

                                        if (code.equals("200")) {
                                            //停复机操作
                                            Object status = Rmap.get("status");
                                            if (status != null && status != "" && status.toString().trim().length() > 0) {
                                                Obj.put("status", status.toString().trim());
                                                String RMessage = Rmap.get("Message") != null ? Rmap.get("Message").toString() : "操作成功！";
                                                Obj.put("Message", RMessage);
                                                Obj.put("agentName", create_by);
                                                Obj.put("result", "操作成功");
                                                SuccessArr.add(Obj);//输出 csv 文间
                                                log.info(">>API - 变更操作  statusCode = 0 :{} | {}<<", iccid, Rmap);
                                            }
                                        } else {
                                            Obj.put("result", "操作失败");
                                            Message = Rmap.get("Message") != null ? Rmap.get("Message").toString() : "API异常！";
                                            Obj.put("Message", Message);
                                            failArr.add(Obj);
                                        }
                                    } else if (!Switch_network.equals("1")) {
                                        System.out.println("允许操作");
                                        describe = "断开网操作";
                                        OPtionType = "FunctionApnStatus";
                                        if (Switch_network.equals("3")) {
                                            describe += "【开网】";
                                            cafterward = "开网";
                                            Obj.put("Is_Break", "0");
                                        } else if (Switch_network.equals("2")) {
                                            describe += "【断网】";
                                            cafterward = "断网";
                                            Obj.put("Is_Break", "1");
                                        }
                                        Obj.put("cafterward", cafterward);//变更后
                                        Map<String, Object> Rmap = internalApiRequest.FunctionApnStatus(Obj, findRouteArr.get(i));
                                        String code = Rmap.get("code") != null ? Rmap.get("code").toString() : "500";
                                        if (code.equals("200")) {
                                            //断开网操作
                                            String RMessage = Rmap.get("Message") != null ? Rmap.get("Message").toString() : "操作成功！";
                                            Obj.put("Message", RMessage);
                                            Obj.put("describe", describe);
                                            Obj.put("agentName", create_by);
                                            Obj.put("result", "操作成功");
                                            SuccessArr.add(Obj);//输出 csv 文间
                                            log.info(">>API - 变更操作  statusCode = 0 :{} | {}<<", iccid, Rmap);
                                        } else {
                                            Obj.put("result", "操作失败");
                                            Message = Rmap.get("Message") != null ? Rmap.get("Message").toString() : "API异常！";
                                            Obj.put("Message", Message);
                                            failArr.add(Obj);
                                        }
                                    }
                                }

                                //2. 卡信息 变更表 插入数据
                                if (SuccessArr.size() > 0) {
                                    writeCSV.OutCSVObj(SuccessArr, newName, Outcolumns, keys, null, OutSize);
                                    //变更主表信息
                                    try {
                                        if (!Is_remind_ratio.equals("1")) {//停复机
                                            String statusCode = Is_remind_ratio.equals("2") ? "2" : Is_remind_ratio.equals("3") ? "1" : "";
                                            if (statusCode != "") {
                                                updStatus(SuccessArr, statusCode);//下发变更 卡状态 队列
                                            } else {
                                                log.info(">>错误 - updStatus :{}<<", "变更状态不在规定数据内");
                                            }
                                        } else if (!Switch_network.equals("1")) {//断开网
                                            String Connection = Switch_network.equals("2") ? "2" : Switch_network.equals("3") ? "1" : "";
                                            if (Connection != "") {
                                                updConnectionStatus(SuccessArr, Connection);//下发变更 断开网 队列
                                            } else {
                                                log.info(">>错误 - updConnectionStatus :{}<<", "变更状态不在规定数据内");
                                            }
                                        }
                                    } catch (Exception e) {
                                        log.info(">>错误 - 变更主表信息 :{} | {}<<", e.getMessage());
                                    }

                                    List<Map<String, Object>> AddArr = new ArrayList<>();
                                    for (int i = 0; i < SuccessArr.size(); i++) {
                                        Map<String, Object> Add_Map = SuccessArr.get(i);

                                        Add_Map.put("cafterward", cafterward);
                                        Add_Map.put("remark", "执行成功！");
                                        Add_Map.put("source_type", "5");// 来源 类型 ：5 批量操作
                                        Add_Map.put("execution_status", "1");
                                        AddArr.add(Add_Map);
                                    }
                                    Map<String, Object> map1 = new HashMap<>();
                                    map1.put("CardInfoMapList", AddArr);
                                    int addBool = yzCardInfoChangeMapper.addinfo(map1);
                                    log.info("插入成功");
                                    log.info("Dlx - " + map.get("iccid") + " 卡信息变更表 插入 ： " + OPtionType + " 新增卡信息变更记录 addBool :" + addBool);

                                }
                                if (failArr.size() > 0) {
                                    writeCSV.OutCSVObj(failArr, newName, Outcolumns, keys, null, OutSize);
                                    List<Map<String, Object>> AddArr = new ArrayList<>();
                                    for (int j = 0; j < failArr.size(); j++) {
                                        Map<String, Object> Add_Map = failArr.get(j);
                                        Add_Map.put("remark", "执行失败！");
                                        Add_Map.put("source_type", "5");// 来源 类型 ：5 批量操作
                                        Add_Map.put("execution_status", "2");
                                        AddArr.add(Add_Map);
                                    }
                                    Map<String, Object> map2 = new HashMap<>();
                                    map2.put("CardInfoMapList", AddArr);
                                    int addBool = yzCardInfoChangeMapper.addinfo(map2);
                                    log.info("Dlx - " + map.get("iccid") + " 卡信息变更表 插入 ： " + SuccessArr + " 新增卡信息变更记录 addBool :" + addBool);
                                }


                            } else {
                                Map<String, Object> defOutcolumns = new HashMap<>();
                                defOutcolumns.put("describe", "通道状态 为停用或已删除 取消操作！");
                                defOutcolumns.put("agentName", create_by);
                                defOutcolumns.put("result", "查询失败");
                                writeCSV.OutCSVObj(list, newName, Outcolumns, keys, defOutcolumns, OutSize);
                            }
                        } else {
                            Map<String, Object> defOutcolumns = new HashMap<>();
                            defOutcolumns.put("describe", "未划分通道，取消操作！");
                            defOutcolumns.put("agentName", create_by);
                            defOutcolumns.put("result", "查询失败");
                            writeCSV.OutCSVObj(list, newName, Outcolumns, keys, defOutcolumns, OutSize);
                        }
                    }
                } catch (DuplicateKeyException e) {
                    String[] solit = e.getCause().toString().split("'");
                    Map<String, Object> defOutcolumns = new HashMap<>();
                    defOutcolumns.put("describe", e.getCause().toString());
                    defOutcolumns.put("agentName", create_by);
                    defOutcolumns.put("result", "查询失败");
                    writeCSV.OutCSVObj(list, newName, Outcolumns, keys, defOutcolumns, OutSize);
                    yzExecutionTaskMapper.set_end_time(task_map);//任务结束
                    log.error(">> cardSet-消费者- 上传excel异常 [插入数据 DuplicateKeyException ] :{}<<", e.getMessage().toString());
                } catch (Exception e) {
                    Map<String, Object> defOutcolumns = new HashMap<>();
                    defOutcolumns.put("describe", e.getCause().toString());
                    defOutcolumns.put("agentName", create_by);
                    defOutcolumns.put("result", "查询失败");
                    writeCSV.OutCSVObj(list, newName, Outcolumns, keys, defOutcolumns, OutSize);
                    yzExecutionTaskMapper.set_end_time(task_map);//任务结束
                    log.error(">>cardSet-消费者- 批量查询消费者:{}<<", e.getMessage());
                }
            } else {
                log.info("上传ICCID卡号不在数据库中！请核对后重试！");
                Map<String, Object> defOutcolumns = new HashMap<>();
                defOutcolumns.put("describe", "查询iccid卡号平台中未找到！查询取消！");
                defOutcolumns.put("agentName", create_by);
                defOutcolumns.put("result", "查询失败");
                writeCSV.OutCSVObj(list, newName, Outcolumns, keys, defOutcolumns, OutSize);
            }
        } else {
            log.error("admin_CardImportBatch_queue`-消费者 上传表格无数据！无需执行");
        }
        yzExecutionTaskMapper.set_end_time(task_map);//需改任务 结束 时间
    }


    /**
     * 修改卡状态
     *
     * @param SuccessArr
     */
    public void updStatus(List<Map<String, Object>> SuccessArr, String statusCode) {
        //1. 修改卡状态 || 修改停复机状态
        List<String> iccidArr = new ArrayList<String>();
        Map<String, Object> Upd_Map = new HashMap<>();
        for (int i = 0; i < SuccessArr.size(); i++) {
            iccidArr.add(SuccessArr.get(i).get("iccid").toString());
        }
        Upd_Map.put("iccidArr", iccidArr);
        Upd_Map.put("status_id", statusCode);
        Upd_Map.put("status_ShowId", getShowStatIdArr.GetShowStatId(statusCode));

        yzCardMapper.updStatusIdArr(Upd_Map);
    }

    public void updConnectionStatus(List<Map<String, Object>> SuccessArr, String Connection) {
        //1. 修改卡状态 || 修改断开网状态
        List<String> iccidArr = new ArrayList<String>();
        Map<String, Object> Upd_Map = new HashMap<>();
        for (int i = 0; i < SuccessArr.size(); i++) {
            iccidArr.add(SuccessArr.get(i).get("iccid").toString());
        }
        Upd_Map.put("iccidArr", iccidArr);
        Upd_Map.put("connection_status", Connection);
        yzCardMapper.updction(Upd_Map);
    }


}