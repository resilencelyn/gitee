package com.yunze.quartz.task.yunze.Automation;

import com.alibaba.fastjson.JSON;
import com.yunze.common.mapper.yunze.automationCC.YzAutomationCcHisMapper;
import com.yunze.common.mapper.yunze.commodity.YzWxByOrderMapper;
import com.yunze.common.utils.Email.EmailCc;
import org.springframework.stereotype.Component;

import javax.annotation.Resource;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * 定时任务 自动化 邮件抄送 [商品购物]
 *
 * @author root
 */
@Component("emailCCShopping")
public class EmailCCShopping {

    @Resource
    private YzAutomationCcHisMapper yzAutomationCcHisMapper;
    @Resource
    private EmailCc emailCc;
    @Resource
    private YzWxByOrderMapper yzWxByOrderMapper;

    /**
     * 邮件抄送
     * @param cc_count 重试次数
     */
    public void CCShopping(Integer cc_count) {
        Map<String, Object> findNotPerformed_Map = new HashMap<>();
        findNotPerformed_Map.put("trigger_type","99");
        findNotPerformed_Map.put("cc_count",cc_count);//重试次数
        List<Map<String, Object>> CcArr = yzAutomationCcHisMapper.findNotPerformed(findNotPerformed_Map);
        if (CcArr != null && CcArr.size() > 0) {
            //2.获取 通道下卡号
            for (int i = 0; i < CcArr.size(); i++) {
                Map<String, Object> Cc_Map = CcArr.get(i);
                Map<String,Object> MsgMap = JSON.parseObject(Cc_Map.get("cc_parameter").toString());
                MsgMap.put("Aid",MsgMap.get("ord_no"));
                Map<String,Object> OrderMap = yzWxByOrderMapper.findConcise(MsgMap);
                if(OrderMap!=null){
                    String SendEmail = Cc_Map.get("cc_email").toString();
                    Map<String,Object> updHisMap = new HashMap<>();
                    updHisMap.put("id",Cc_Map.get("id"));
                    Map<String,Object> Rmap = null;
                    Boolean bool = false;
                    String remark = "",cc_result = "0";
                    try {
                        Rmap = emailCc.ShoppingDelivery_default(OrderMap,SendEmail);
                        bool = (Boolean) Rmap.get("bool");
                        if(bool){
                            cc_result = "1";
                        }
                    }catch (Exception e){
                        remark = e.getMessage();
                        remark = remark.length()>240?remark.substring(0,240):remark;
                    }
                    updHisMap.put("remark",remark);
                    updHisMap.put("cc_result",cc_result);
                    boolean updHis =  yzAutomationCcHisMapper.updCcCount(updHisMap)>0;
                    System.out.println(">>自动化 已抄送邮箱"+SendEmail+"  - 抄送 bool   {"+bool+"}  修改 bool {"+updHis+"}<<");
                }
            }
        }
    }




}
