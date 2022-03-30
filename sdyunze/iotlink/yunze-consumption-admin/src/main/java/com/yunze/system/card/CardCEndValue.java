package com.yunze.system.card;
import com.yunze.common.config.MyDictionary;
import com.yunze.common.mapper.yunze.YzCardInfoChangeMapper;
import com.yunze.common.mapper.yunze.YzCardIsStopMapper;
import com.yunze.common.mapper.yunze.YzCardMapper;
import com.yunze.common.mapper.yunze.YzExecutionTaskMapper;
import com.yunze.common.mapper.yunze.commodity.YzWxByProductAgentMapper;
import com.yunze.apiCommon.utils.InternalApiRequest;
import com.yunze.common.utils.yunze.GetShowStatIdArr;
import com.yunze.common.utils.yunze.WriteCSV;
import lombok.extern.slf4j.Slf4j;
import org.springframework.amqp.rabbit.annotation.RabbitHandler;
import org.springframework.amqp.rabbit.annotation.RabbitListener;
import org.springframework.dao.DuplicateKeyException;
import org.springframework.stereotype.Component;
import javax.annotation.Resource;
import java.io.IOException;
import java.util.*;

@Slf4j
@Component
public class CardCEndValue {

    @Resource
    private YzCardMapper yzCardMapper;
    @Resource
    private YzCardIsStopMapper yzCardIsStopMapper;
    @Resource
    private YzExecutionTaskMapper yzExecutionTaskMapper;
    @Resource
    private WriteCSV writeCSV;
    @Resource
    private InternalApiRequest internalApiRequest;
    @Resource
    private YzCardInfoChangeMapper yzCardInfoChangeMapper;
    @Resource
    private MyDictionary myDictionary;
    @Resource
    private YzWxByProductAgentMapper yzWxByProductAgentMapper;
    @Resource
    private GetShowStatIdArr getShowStatIdArr;

    private String Outcolumns[] = {"iccid", "返回消息", "执行描述", "执行人", "执行结果"};
    private String keys[] = {"iccid", "Message", "describe", "agentName", "result"};
    private int OutSize = 50;//每 50条数据输出一次

    /**
     * C端充值 轮询 卡状态变更失败的 进行再次 复机；（前提是没用完用量）
     */
    @RabbitHandler
    @RabbitListener(queues = "admin_CEndValue_queue")
    public void CardMonth(String msg) throws IOException {
        try {
            execution();//C端充值 轮询 卡状态变更失败的 进行再次 复机
        } catch (Exception e) {
            log.error(">>错误 - C端充值 轮询 卡状态变更失败的 进行再次 复机 消费者:{}<<", e.getMessage());
        }
    }

    private void execution() {
        String Message = "";
        HashMap<String, Object> map = new HashMap<>();

        List<Map<String, Object>> card_arrs = new ArrayList<Map<String, Object>>();
        map.put("config_key", "yunze.card.IsStop");//参数键名 config_key
        String MaixCount = yzWxByProductAgentMapper.findConfig(map);//查询系统配置
        MaixCount = MaixCount != null ? MaixCount : "10";
        map.put("MaixCount", MaixCount);

        List<Map<String, Object>> SleOrcount = yzCardIsStopMapper.SleOrcount(map);//数据库 查询
        List<Map<String, Object>> SleOrderno = yzCardIsStopMapper.SleOrderno(map);//数据库 查询
        card_arrs.addAll(0, SleOrcount);
        int index = card_arrs.size() - 1;
        index = index>=0?index:0;
        card_arrs.addAll(index, SleOrderno);//添加到一个List
        map.put("cardArrs", card_arrs);

        List<Map<String, Object>> SleMulti = yzCardMapper.SleMulti(map);//数据库 查询



        String create_by = " [总平台] - " + " [管理员] ";
        //1.读取 上传文件
        String newName = UUID.randomUUID().toString().replace("-", "") + "_CardAgain";//这里
        String task_name = create_by + "-C端充值 [变更失败再次 复机] ";
        String SaveUrl = "/getcsv/" + newName + ".csv";
        SaveUrl = "/getcsv/" + newName + ".csv";
        Map<String, Object> task_map = new HashMap<String, Object>();
        task_map.put("auth", create_by);
        task_map.put("task_name", task_name);
        task_map.put("url", SaveUrl);
        task_map.put("agent_id", "100");
        task_map.put("type", "10"); //对应的任务类别
        if(SleMulti!=null && SleMulti.size()>0){
            yzExecutionTaskMapper.add(task_map);//添加执行 任务表
        }else{
            System.out.println("C端充值 失败再次 未找到需要，需要操作的数据！ ");
        }

        map.put("card_arrs", SleMulti);//更新 list

        try {

            List<Map<String, Object>> SuccessArr = new ArrayList<>();
            String describe = "";
            List<Map<String, Object>> failArr = new ArrayList<>();
            String cafterward = "正常";//变更后状太
            String OPtionType = "";//操作类型
            for (int i = 0; i < SleMulti.size(); i++) {

                Map<String, Object> Obj = new HashMap<>();
                String iccid = SleMulti.get(i).get("iccid").toString();
                String ord_no = SleMulti.get(i).get("ord_no").toString();
                Obj.put("iccid", iccid);
                Obj.put("Is_Stop", "on");
                Obj.put("ord_no", ord_no);

                Map<String, Object> Rmap = internalApiRequest.changeCardStatus(Obj, SleMulti.get(i));
                String code = Rmap.get("code") != null ? Rmap.get("code").toString() : "500";
                if (code.equals("200")) {
                    //复机操作
                    Object status = Rmap.get("status");
                    if (status != null && status != "" && status.toString().trim().length() > 0) {
                        Obj.put("status", status.toString().trim());
                        String RMessage = Rmap.get("Message") != null ? Rmap.get("Message").toString() : "操作成功！";
                        Obj.put("Message", RMessage);
                        Obj.put("describe", describe);
                        Obj.put("agentName", create_by);
                        Obj.put("result", "操作成功");
                        SuccessArr.add(Obj);
                        log.info(">>API - 变更操作  成功 = 0 :{} | {}<<", iccid, Rmap);
                    }
                } else {
                    Obj.put("agentName", create_by);
                    Obj.put("result", "操作失败");
                    Message = Rmap.get("Message") != null ? Rmap.get("Message").toString() : "API异常！";
                    Obj.put("Message", Message);
                    failArr.add(Obj);
                }
                String ctype = "";
                String dict_type = "";
                String key = "";
                key = "status_id";
                dict_type = "yunze_card_status_ShowId";
                ctype = "1";

                List<Map<String, Object>> lmArr = yzCardMapper.addsel(map);//这里是 查询 出 数据库 操作 前 卡状态

                List<Map<String, Object>> AddArr = new ArrayList<>();
                List<Map<String, Object>> AddStop = new ArrayList<>();
                //2. 卡信息 变更表 插入数据
                if (SuccessArr.size() > 0) {
                    writeCSV.OutCSVObj(SuccessArr, newName, Outcolumns, keys, null, OutSize);
                    yzExecutionTaskMapper.set_end_time(task_map);//任务结束
                    //变更主表信息
                    try {
                        updStatus(SuccessArr, "1");//下发变更 卡状态 队列
                    } catch (Exception e) {
                        log.info(">>错误 - 变更主表信息 :{} | {}<<", e.getMessage());
                    }
                    String cbefore = "正常";//变更后
                    for (int j = 0; j < lmArr.size(); j++) {
                        Map<String, Object> ObjMap = lmArr.get(j);
                        Map<String, Object> Add_Map = new HashMap<>();
                        String value = ObjMap.get(key) != null && ObjMap.get(key).toString().length() > 0 ? ObjMap.get(key).toString() : "未知";

                        if (!value.equals("未知")) {
                            cbefore = myDictionary.getdictLabel(dict_type, value);
                        } else {
                            cbefore = value;
                        }
                        Add_Map.put("iccid", ObjMap.get("iccid"));
                        Add_Map.put("ctype", ctype);//生命周期
                        Add_Map.put("cbefore", cbefore);
                        Add_Map.put("cafterward", cafterward);
                        Add_Map.put("remark", "执行成功！");
                        Add_Map.put("source_type", "7");// 来源 类型
                        Add_Map.put("execution_status", "1");
                        AddArr.add(Add_Map);
                    }
                    Map<String, Object> map1 = new HashMap<>();
                    map1.put("CardInfoMapList", AddArr);
                    map1.put("ctype", lmArr);
                    int addBool = yzCardInfoChangeMapper.addinfo(map1);
                    log.info("插入成功");
                    log.info("Dlx - " + map.get("iccid") + " 卡信息变更表 插入 ： " + OPtionType + " 新增卡信息变更记录 addBool :" + addBool);


                    for (int j = 0; j < SuccessArr.size(); j++) {
                        Map<String, Object> ObjMap = SuccessArr.get(j);
                        Map<String, Object> Add_Map = new HashMap<>();
                        Add_Map.put("iccid", ObjMap.get("iccid"));
                        Add_Map.put("status", "1");
                        Add_Map.put("order_no", ObjMap.get("ord_no"));
                        AddStop.add(Add_Map);
                    }
                    Map<String, Object> map3 = new HashMap<>();
                    map3.put("Stoploop", AddStop);
                    int AddStopBool = yzCardIsStopMapper.AddStop(map3);
                    log.info("Dlx - " + map.get("iccid") + " 卡信息 插入 ： " + SuccessArr + " 新增月初复机有用量的卡 addBool :" + AddStopBool);
                    SuccessArr = new ArrayList<>();

                }
                if (failArr.size() > 0) {

                    writeCSV.OutCSVObj(failArr, newName, Outcolumns, keys, null, OutSize);
                    yzExecutionTaskMapper.set_end_time(task_map);//任务结束
                    for (int j = 0; j < lmArr.size(); j++) {
                        Map<String, Object> ObjMap = lmArr.get(j);
                        Map<String, Object> Add_Map = new HashMap<>();
                        String value = ObjMap.get(key) != null && ObjMap.get(key).toString().length() > 0 ? ObjMap.get(key).toString() : "未知";
                        String cbefore = "";//变更后
                        if (!value.equals("未知")) {
                            cbefore = myDictionary.getdictLabel(dict_type, value);
                        } else {
                            cbefore = value;
                        }
                        Add_Map.put("iccid", ObjMap.get("iccid"));
                        Add_Map.put("ctype", ctype);//生命周期
                        Add_Map.put("cbefore", cbefore);
                        Add_Map.put("cafterward", cafterward);
                        Add_Map.put("describe", describe);
                        Add_Map.put("agentName", create_by);
                        Add_Map.put("remark", "执行失败！");
                        Add_Map.put("source_type", "7");// 来源 类型
                        Add_Map.put("execution_status", "2");
                        AddArr.add(Add_Map);
                    }
                    Map<String, Object> map2 = new HashMap<>();
                    map2.put("CardInfoMapList", AddArr);
                    int addBool = yzCardInfoChangeMapper.addinfo(map2);
                    log.info("Dlx - " + map.get("iccid") + " 卡信息变更表 插入 ： " + SuccessArr + " 新增卡信息变更记录 addBool :" + addBool);


                    for (int j = 0; j < failArr.size(); j++) {
                        Map<String, Object> ObjMap = failArr.get(j);
                        Map<String, Object> Add_Map = new HashMap<>();
                        Add_Map.put("iccid", ObjMap.get("iccid"));
                        Add_Map.put("status", "2");
                        Add_Map.put("order_no", ObjMap.get("ord_no"));
                        AddStop.add(Add_Map);
                    }
                    Map<String, Object> map3 = new HashMap<>();
                    map3.put("Stoploop", AddStop);
                    int AddStopBool = yzCardIsStopMapper.AddStop(map3);
                    log.info("Dlx - " + map.get("iccid") + " 卡信息 插入 ： " + SuccessArr + " 新增月初复机有用量的卡 addBool :" + AddStopBool);
                    failArr = new ArrayList<>();
                }

            }

        } catch (DuplicateKeyException e) {
            String[] solit = e.getCause().toString().split("'");
            Map<String, Object> defOutcolumns = new HashMap<>();
            defOutcolumns.put("describe", e.getCause().toString());
            defOutcolumns.put("agentName", create_by);
            defOutcolumns.put("result", "查询失败");
            writeCSV.OutCSVObj(SleMulti, newName, Outcolumns, keys, defOutcolumns, OutSize);
            yzExecutionTaskMapper.set_end_time(task_map);//任务结束
            log.error(">> cardSet-消费者- 上传excel异常 [插入数据 DuplicateKeyException ] :{}<<", e.getMessage().toString());
        } catch (Exception e) {
            Map<String, Object> defOutcolumns = new HashMap<>();
            defOutcolumns.put("describe", e.getCause().toString());
            defOutcolumns.put("agentName", create_by);
            defOutcolumns.put("result", "查询失败");
            writeCSV.OutCSVObj(SleMulti, newName, Outcolumns, keys, defOutcolumns, OutSize);
            yzExecutionTaskMapper.set_end_time(task_map);//任务结束
            log.error(">>cardSet-消费者- 批量查询消费者:{}<<", e.getMessage());
        }
    }
    /**
     * 修改卡状态
     * @param SuccessArr
     */
    public void updStatus(List<Map<String, Object>> SuccessArr, String statusCode) {
        //1. 修改卡状态 || 修改停机状态
        List<String> iccidArr = new ArrayList<String>();
        Map<String, Object> Upd_Map = new HashMap<>();
        for (int i = 0; i < SuccessArr.size(); i++) {
            iccidArr.add(SuccessArr.get(i).get("iccid").toString());
        }
        Upd_Map.put("iccidArr", iccidArr);
        Upd_Map.put("status_id", statusCode);
        Upd_Map.put("status_ShowId",getShowStatIdArr.GetShowStatId(statusCode));

        yzCardMapper.updStatusIdArr(Upd_Map);
    }
}







