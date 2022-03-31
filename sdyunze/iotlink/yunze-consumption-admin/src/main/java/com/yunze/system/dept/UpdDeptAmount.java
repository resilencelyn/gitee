package com.yunze.system.dept;

import com.alibaba.fastjson.JSON;
import com.rabbitmq.client.Channel;
import com.yunze.common.mapper.yunze.YzOrderMapper;
import com.yunze.common.mapper.yunze.YzUserMapper;
import com.yunze.common.utils.Arith;
import lombok.extern.slf4j.Slf4j;
import org.apache.commons.lang3.StringUtils;
import org.springframework.amqp.rabbit.annotation.RabbitHandler;
import org.springframework.amqp.rabbit.annotation.RabbitListener;
import org.springframework.amqp.rabbit.core.RabbitTemplate;
import org.springframework.stereotype.Component;

import javax.annotation.Resource;
import java.io.IOException;
import java.util.*;
import java.util.concurrent.TimeUnit;

/**
 * 企业 预存金额 授信额度 授信支付 消费者
 */
@Slf4j
@Component
public class UpdDeptAmount {

    @Resource
    private YzOrderMapper yzOrderMapper;
    @Resource
    private YzUserMapper yzUserMapper;
    @Resource
    private RabbitTemplate rabbitTemplate;


    /**
     * 企业 预存金额 授信额度 授信支付 消费者
     * @param msg
     * @param channel
     * @throws IOException
     */
    @RabbitHandler
    @RabbitListener(queues = "admin_updDeptAmount_queue")
    public void updDeptAmount(String msg, Channel channel){
        try {
            if (StringUtils.isEmpty(msg)) {
                return;
            }
            Map<String,Object> map = JSON.parseObject(msg);
            String type = map.get("type").toString();
            Map<String,Object> Pmap =  ( Map<String,Object>)map.get("Pmap");//请求参数
            Map<String,Object> operateMap =  ( Map<String,Object>)map.get("operateMap");//操作人信息


            switch (type){
                case "updCredit"://变更授信信息
                    updCredit(Pmap,operateMap);//执行连接设置
                    break;
                case "Prestore"://代理 预存金额
                    Prestore(Pmap,operateMap);//执行连接设置
                    break;

                default:
                    log.info(">> switch - default   mytype:{}  msg:{}<<", type,msg);
                    break;
            }


        } catch (Exception e) {
            log.error(">>错误 - 企业 预存金额 授信额度 授信支付 消费者 :{} :{}<<", msg,e.getMessage());
        }
    }





    /**
     * 企业 预存金额 修改 消费者 [充值 增加走这里]
     * @param msg
     * @param channel
     * @throws IOException
     */
    @RabbitHandler
    @RabbitListener(queues = "admin_UpdDepositAmount_queue")
    public void UpdDepositAmount(String msg, Channel channel){
        try {
            if (StringUtils.isEmpty(msg)) {
                return;
            }
            Map<String,Object> map = JSON.parseObject(msg);
            Map<String,Object> Pmap =  ( Map<String,Object>)map.get("Pmap");//请求参数
            Map<String,Object> operateMap =  ( Map<String,Object>)map.get("operateMap");//操作人信息

            UpdDAmount(Pmap,operateMap);

        } catch (Exception e) {
            log.error(">>错误 - 企业 预存金额 授信额度 授信支付 消费者 :{} :{}<<", msg,e.getMessage());
        }
    }




    /**
     * 修改授信额度
     * @param Pmap
     * @param operateMap
     */
    public void updCredit( Map<String,Object> Pmap ,Map<String,Object> operateMap){


       if(Pmap.get("agent_id")!=null && Pmap.get("agent_id").toString().length()>0 && Pmap.get("line_of_credit")!=null && Pmap.get("line_of_credit").toString().length()>0){
           String agent_id = Pmap.get("agent_id").toString();
           Double line_of_credit = Double.parseDouble(Pmap.get("line_of_credit").toString());

        //1. 授信额度 - （ 获取 订单 SUM(交易类型为 [用量充值] & 支付方式[授信支付]) - SUM(预存抵扣) ） = 已使用额度

        Double D_CreditPrice = 0.0;
        Double D_DeductPrice = 0.0;

        List<String> agent_idArr =  new ArrayList<>();
        agent_idArr.add(agent_id);

        Map<String,Object> OrderpMap = new HashMap<>();
        OrderpMap.put("ord_mytype","2");
        OrderpMap.put("pay_mytype","ct");
        OrderpMap.put("agent_id",agent_idArr);
        String SumCreditPrice = yzOrderMapper.getSumPrice(OrderpMap);
        D_CreditPrice = SumCreditPrice!=null?Double.parseDouble(SumCreditPrice):D_CreditPrice;

        Map<String,Object> deductMap = new HashMap<>();
        deductMap.put("ord_mytype","6");
        deductMap.put("agent_id",agent_idArr);
        String SumDeductPrice = yzOrderMapper.getSumPrice(deductMap);
        D_DeductPrice = SumDeductPrice!=null?Double.parseDouble(SumDeductPrice):D_DeductPrice;


         Double used_line_of_credit = Arith.sub(line_of_credit,Arith.sub(D_CreditPrice,D_DeductPrice));//已使用授信额度
         Double be_usable_line_of_credit = Arith.sub(line_of_credit,used_line_of_credit);//可使用授信额度

           String result = "1";
           String remarks = "";


           //2.准备   金额变更记录表 新增数据
           Map<String,Object> FindDeptAmountMap = new HashMap<>();
           FindDeptAmountMap.put("dept_id",agent_id);
           Map<String,Object> DeptAmountMap =  yzUserMapper.findDeptAmount(FindDeptAmountMap);

           try {
               Map<String,Object> updCreditMap = new HashMap<>();
               updCreditMap.put("used_line_of_credit",used_line_of_credit);
               updCreditMap.put("be_usable_line_of_credit",be_usable_line_of_credit);
               updCreditMap.put("line_of_credit",line_of_credit);
               updCreditMap.put("dept_id",agent_id);
               int updCOunt = yzUserMapper.updCredit(updCreditMap);
               remarks = "操作成功 updCOunt "+updCOunt;
           }catch (Exception e){
               result = "2";
               remarks = e.getMessage()!=null?e.getMessage():remarks;
               remarks = remarks.length()>150?remarks.substring(0,150):remarks;
               log.error("yzUserMapper.updCredit Exception {}",e.getMessage());

           }




           Map<String,Object> par_valMap = new HashMap<>();
           par_valMap.put("Pmap",Pmap);
           par_valMap.put("operateMap",operateMap);


         String mytable_name = "sys_dept";
         String mytype = "2";
         String myfunction_name = "updCredit";
         String par_val = JSON.toJSONString(par_valMap);
         String sel_key = "dept_id";
         String sel_val = agent_id;


           Map<String,Object> add_common = new HashMap<>();
           add_common.put("mytable_name",mytable_name);
           add_common.put("sel_key",sel_key);
           add_common.put("sel_val",sel_val);
           add_common.put("result",result);
           add_common.put("par_val",par_val);
           add_common.put("remarks",remarks);
           add_common.put("myfunction_name",myfunction_name);
           add_common.put("mytype",mytype);


           Map<String,Object> add_row1 = new HashMap<>();
           add_row1.put("mybefore",DeptAmountMap.get("line_of_credit")!=null?DeptAmountMap.get("line_of_credit").toString():"");
           add_row1.put("change_value",line_of_credit.toString());
           add_row1.put("field_key","line_of_credit");
           add_row1.put("field_val","已授信额度");

           Map<String,Object> add_row2 = new HashMap<>();
           add_row2.put("mybefore",DeptAmountMap.get("used_line_of_credit")!=null?DeptAmountMap.get("used_line_of_credit").toString():"");
           add_row2.put("change_value",used_line_of_credit.toString());
           add_row2.put("field_key","used_line_of_credit");
           add_row2.put("field_val","已使用授信额度");

           Map<String,Object> add_row3 = new HashMap<>();
           add_row3.put("mybefore",DeptAmountMap.get("be_usable_line_of_credit")!=null?DeptAmountMap.get("be_usable_line_of_credit").toString():"");
           add_row3.put("change_value",be_usable_line_of_credit.toString());
           add_row3.put("field_key","be_usable_line_of_credit");
           add_row3.put("field_val","可使用授信额度");


           List<Map<String,Object>> insertArr = new ArrayList<>();

            insertArr.add(add_row1);
            insertArr.add(add_row2);
            insertArr.add(add_row3);

           add_common.put("saveArr",insertArr);

           //修改企业金额信息 路由队列
           String polling_routingKey = "admin.insertMoneyChangeRecord.queue";
           String polling_exchangeName = "admin_exchange";//路由
           try {
               Map<String, Object> start_mytype = new HashMap<>();
               rabbitTemplate.convertAndSend(polling_exchangeName, polling_routingKey, JSON.toJSONString(add_common), message -> {
                   // 设置消息过期时间 30 分钟 过期
                   message.getMessageProperties().setExpiration("" + (30 * 1000 * 60));
                   return message;
               });
           } catch (Exception e) {
               log.error("admin_insertMoneyChangeRecord_queue 发送 失败 {} {}",add_common,e.getMessage() );
           }



       }else{
           log.error(">>updCredit 参数不全取消操作 Pmap:{} operateMap:{}<<", Pmap,operateMap);
       }


    }

    /**
     * 修改 预存金额
     * @param Pmap
     * @param operateMap
     */
     public void UpdDAmount( Map<String,Object> Pmap ,Map<String,Object> operateMap){
         if(Pmap.get("agent_id")!=null && Pmap.get("agent_id").toString().length()>0 && Pmap.get("deposit_amount")!=null && Pmap.get("deposit_amount").toString().length()>0
                 && Pmap.get("operateType")!=null && Pmap.get("operateType").toString().length()>0) {
             String agent_id = Pmap.get("agent_id").toString();
             Double deposit_amount = Double.parseDouble(Pmap.get("deposit_amount").toString());
             String operateType = Pmap.get("operateType").toString();


             //准备   金额变更记录表 新增数据
             Map<String,Object> FindDeptAmountMap = new HashMap<>();
             FindDeptAmountMap.put("dept_id",agent_id);
             Map<String,Object> DeptAmountMap =  yzUserMapper.findDeptAmount(FindDeptAmountMap);
             String result = "1";
             String remarks = "";

             try {
                 int updCOunt =  yzUserMapper.updDepositAmount(Pmap);;
                 remarks = ""+operateType+" 操作成功 updCOunt "+updCOunt;
             }catch (Exception e){
                 result = "2";
                 remarks = e.getMessage()!=null?e.getMessage():remarks;
                 remarks = remarks.length()>150?remarks.substring(0,150):remarks;
                 remarks = ""+operateType+" "+remarks;
                 log.error("yzUserMapper.updCredit Exception {}",e.getMessage());

             }

             Map<String,Object> par_valMap = new HashMap<>();
             par_valMap.put("Pmap",Pmap);
             par_valMap.put("operateMap",operateMap);


             String mytable_name = "sys_dept";
             String mytype = "2";
             String myfunction_name = "updDepositAmount";
             String par_val = JSON.toJSONString(par_valMap);
             String sel_key = "dept_id";
             String sel_val = agent_id;


             Map<String,Object> add_common = new HashMap<>();
             add_common.put("mytable_name",mytable_name);
             add_common.put("sel_key",sel_key);
             add_common.put("sel_val",sel_val);
             add_common.put("result",result);
             add_common.put("par_val",par_val);
             add_common.put("remarks",remarks);
             add_common.put("myfunction_name",myfunction_name);
             add_common.put("mytype",mytype);


             Map<String,Object> add_row1 = new HashMap<>();
             add_row1.put("mybefore",DeptAmountMap.get("deposit_amount")!=null?DeptAmountMap.get("deposit_amount").toString():"");
             add_row1.put("change_value",deposit_amount.toString());
             add_row1.put("field_key","deposit_amount");
             add_row1.put("field_val","预存金额");

             List<Map<String,Object>> insertArr = new ArrayList<>();

             insertArr.add(add_row1);

             add_common.put("saveArr",insertArr);

             //修改企业金额信息 路由队列
             String polling_routingKey = "admin.insertMoneyChangeRecord.queue";
             String polling_exchangeName = "admin_exchange";//路由
             try {
                 rabbitTemplate.convertAndSend(polling_exchangeName, polling_routingKey, JSON.toJSONString(add_common), message -> {
                     // 设置消息过期时间 30 分钟 过期
                     message.getMessageProperties().setExpiration("" + (30 * 1000 * 60));
                     return message;
                 });
             } catch (Exception e) {
                 log.error("admin_insertMoneyChangeRecord_queue 发送 失败 {} {}",add_common,e.getMessage() );
             }



         }


     }



    /**
     * 代理 预存金额
     * @param Pmap
     * @param operateMap
     */
    public void Prestore( Map<String,Object> Pmap ,Map<String,Object> operateMap){


        if(Pmap.get("agent_id")!=null && Pmap.get("agent_id").toString().length()>0 && Pmap.get("deposit_amount")!=null && Pmap.get("deposit_amount").toString().length()>0){
            String agent_id = Pmap.get("agent_id").toString();
            Double deposit_amount = Double.parseDouble(Pmap.get("deposit_amount").toString());

            //1. 授信额度 - （ 获取 订单 SUM(交易类型为 [用量充值] & 支付方式[授信支付]) - SUM(预存抵扣) ） = 已使用额度

            Double D_CreditPrice = 0.0;
            Double D_DeductPrice = 0.0;

            List<String> agent_idArr =  new ArrayList<>();
            agent_idArr.add(agent_id);

            Map<String,Object> OrderpMap = new HashMap<>();
            OrderpMap.put("ord_mytype","2");
            OrderpMap.put("pay_mytype","ct");
            OrderpMap.put("agent_id",agent_idArr);
            String SumCreditPrice = yzOrderMapper.getSumPrice(OrderpMap);
            D_CreditPrice = SumCreditPrice!=null?Double.parseDouble(SumCreditPrice):D_CreditPrice;

            Map<String,Object> deductMap = new HashMap<>();
            deductMap.put("ord_mytype","6");
            deductMap.put("agent_id",agent_idArr);
            String SumDeductPrice = yzOrderMapper.getSumPrice(deductMap);
            D_DeductPrice = SumDeductPrice!=null?Double.parseDouble(SumDeductPrice):D_DeductPrice;




        }else{
            log.error(">>updCredit 参数不全取消操作 Pmap:{} operateMap:{}<<", Pmap,operateMap);
        }


    }






}
