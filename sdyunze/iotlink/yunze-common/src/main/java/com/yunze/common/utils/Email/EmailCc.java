package com.yunze.common.utils.Email;

import com.yunze.common.core.redis.RedisCache;
import com.yunze.common.mapper.yunze.automationCC.YzAutomationCcMapper;
import com.yunze.common.mapper.yunze.commodity.YzWxByProductAgentMapper;
import com.yunze.common.utils.yunze.VeDate;
import org.springframework.stereotype.Component;

import javax.annotation.Resource;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.TimeUnit;

/**
 * 邮件抄送
 * @Auther: zhang feng
 * @Date: 2021/09/26/14:16
 * @Description:
 */
@Component
public class EmailCc {
    @Resource
    private YzWxByProductAgentMapper yzWxByProductAgentMapper;
    @Resource
    private RedisCache redisCache;
    //findConfig



    /**
     * 下单发货通知 抄送 默认模板
     * @param CCMap
     * @return
     */
    public  Map<String,Object> ShoppingDelivery_default(Map<String,Object> CCMap,String SendEmail) {
        String userName = "",password = "";
        Map<String,String> configMap = getConfig();
        userName = configMap.get("userName");
        password = configMap.get("password");

        Map<String, Object> rmap = new HashMap<String, Object>();
        boolean bool = true;
        String info = CCMap.get("info")!=null?CCMap.get("info").toString():"";
        Map<String, Object> Text = EmailSend.Shopping_text(CCMap.get("ord_no").toString(),CCMap.get("create_time").toString(),CCMap.get("shipping_Phone").toString(),
                CCMap.get("shipping_user").toString(),CCMap.get("province").toString(),CCMap.get("city").toString(),CCMap.get("district").toString(),
                CCMap.get("address").toString(),CCMap.get("by_count").toString(),CCMap.get("product_name").toString(),info);

        try {
            MimeMessageDTO mimeDTO = new MimeMessageDTO();
            mimeDTO.setSentDate(new Date());
            mimeDTO.setSubject("您有新的发货订单");
            mimeDTO.setText(Text.get("template").toString());
            bool = MailUtil.sendEmail(userName, password, SendEmail, mimeDTO);
        }catch(Exception e) {
            System.out.println("ShoppingDelivery_default 异常 "+e.getMessage());
            bool=false;
        }
        rmap.put("bool", bool);
        return rmap;
    }



    /**
     * 物联网卡 已订购有用量状态异常 抄送 默认模板
     * @param CCMap
     * @return
     */
    public  Map<String,Object> CardStatusAbnormal_default(Map<String,Object> CCMap,String SendEmail) {
        String userName = "",password = "";
        Map<String,String> configMap = getConfig();
        userName = configMap.get("userName");
        password = configMap.get("password");

        Map<String, Object> rmap = new HashMap<String, Object>();
        boolean bool = true;
        String Count = CCMap.get("Count").toString();
        Map<String, Object> Text = EmailSend.CardUsefulAmountDowntime_text(CCMap.get("taskId").toString(), VeDate.getStringDateSSS(),Count);
        try {
            MimeMessageDTO mimeDTO = new MimeMessageDTO();
            mimeDTO.setSentDate(new Date());
            mimeDTO.setSubject("已订购用量状态已停机数量["+Count+"]");
            mimeDTO.setText(Text.get("template").toString());
            bool = MailUtil.sendEmail(userName, password, SendEmail, mimeDTO);
        }catch(Exception e) {
            System.out.println("CardStatusAbnormal_default 异常 "+e.getMessage());
            bool=false;
        }
        rmap.put("bool", bool);
        return rmap;
    }


    /**
     * 物联网卡 未划分资费组状态异常 抄送 默认模板
     * @param CCMap
     * @return
     */
    public  Map<String,Object> NotDividedIntoTariffGroups_default(Map<String,Object> CCMap,String SendEmail) {
        String userName = "",password = "";
        Map<String,String> configMap = getConfig();
        userName = configMap.get("userName");
        password = configMap.get("password");

        Map<String, Object> rmap = new HashMap<String, Object>();
        boolean bool = true;
        String Count = CCMap.get("Count").toString();

        Map<String,Object> emailMap = (Map<String, Object>) CCMap.get("emailMap");
        Map<String, Object> Text = EmailSend.NotDividedIntoTariffGroups_text(emailMap, VeDate.getStringDateSSS(),Count);
        try {
            MimeMessageDTO mimeDTO = new MimeMessageDTO();
            mimeDTO.setSentDate(new Date());
            mimeDTO.setSubject("未划分资费组数量["+Count+"]");
            mimeDTO.setText(Text.get("template").toString());
            bool = MailUtil.sendEmail(userName, password, SendEmail, mimeDTO);
        }catch(Exception e) {
            System.out.println("NotDividedIntoTariffGroups_default 异常 "+e.getMessage());
            bool=false;
        }
        rmap.put("bool", bool);
        return rmap;
    }

    /**
     * 物联网卡 未划分通道 抄送 默认模板
     * @param CCMap
     * @return
     */
    public  Map<String,Object> UndividedChannel_default(Map<String,Object> CCMap,String SendEmail) {
        String userName = "",password = "";
        Map<String,String> configMap = getConfig();
        userName = configMap.get("userName");
        password = configMap.get("password");

        Map<String, Object> rmap = new HashMap<String, Object>();
        boolean bool = true;
        String Count = CCMap.get("Count").toString();

        Map<String,Object> emailMap = (Map<String, Object>) CCMap.get("emailMap");
        Map<String, Object> Text = EmailSend.UndividedChannel_text(emailMap, VeDate.getStringDateSSS(),Count);
        try {
            MimeMessageDTO mimeDTO = new MimeMessageDTO();
            mimeDTO.setSentDate(new Date());
            mimeDTO.setSubject("未划分通道["+Count+"]");
            mimeDTO.setText(Text.get("template").toString());
            bool = MailUtil.sendEmail(userName, password, SendEmail, mimeDTO);
        }catch(Exception e) {
            System.out.println("UndividedChannel_default 异常 "+e.getMessage());
            bool=false;
        }
        rmap.put("bool", bool);
        return rmap;
    }


    /**
     * 物联网卡 归属为总平台 抄送 默认模板
     * @param CCMap
     * @return
     */
    public  Map<String,Object> CardConsumption_default(Map<String,Object> CCMap,String SendEmail) {
        String userName = "",password = "";
        Map<String,String> configMap = getConfig();
        userName = configMap.get("userName");
        password = configMap.get("password");

        Map<String, Object> rmap = new HashMap<String, Object>();
        boolean bool = true;
        String Count = CCMap.get("Count").toString();
        Map<String,Object> findCardMap = (Map<String, Object>) CCMap.get("findCardMap");
        Map<String, Object> Text = EmailSend.CardConsumption_text(findCardMap, VeDate.getStringDateSSS(),Count);
        try {
            MimeMessageDTO mimeDTO = new MimeMessageDTO();
            mimeDTO.setSentDate(new Date());
            mimeDTO.setSubject("归属为总平台["+Count+"]");
            mimeDTO.setText(Text.get("template").toString());
            bool = MailUtil.sendEmail(userName, password, SendEmail, mimeDTO);
        }catch(Exception e) {
            System.out.println("CardConsumption_default 异常 "+e.getMessage());
            bool=false;
        }
        rmap.put("bool", bool);
        return rmap;
    }


    /**
     * 获取配置的邮箱 账号 密码
     * @return
     */
    public  Map<String,String> getConfig(){
        String userName = "",password = "";
        Map<String, String> configMap = new HashMap<String, String>();
        Map<String,Object> userNameMap = new HashMap<>();
        Map<String,Object> passwordMap = new HashMap<>(userNameMap);
        Object  Email = redisCache.getCacheObject("ccConfig.Email");
        Object  EmailPwd = redisCache.getCacheObject("ccConfig.EmailPwd");
        if(Email!=null && Email.toString().length()>0){
            userName = Email.toString();
        }else{
            userNameMap.put("config_key","yunze.ccConfig.Email");
            userName = yzWxByProductAgentMapper.findConfig(userNameMap);
            redisCache.setCacheObject("ccConfig.Email",userName,6,TimeUnit.HOURS);//6 小时 缓存
        }
        if(EmailPwd!=null && EmailPwd.toString().length()>0){
            password = EmailPwd.toString();
        }else{
            passwordMap.put("config_key","yunze.ccConfig.EmailPwd");
            password = yzWxByProductAgentMapper.findConfig(passwordMap);
            redisCache.setCacheObject("ccConfig.EmailPwd",password,6,TimeUnit.HOURS);//6 小时 缓存
        }
        configMap.put("userName",userName);
        configMap.put("password",password);
        return configMap;
    }



}
