package com.yunze.web.controller.yunze.sysgl;

import com.alibaba.fastjson.JSON;
import com.github.binarywang.wxpay.bean.notify.WxPayNotifyResponse;
import com.yunze.common.core.domain.model.LoginUser;
import com.yunze.common.utils.ServletUtils;
import com.yunze.common.utils.ip.IpUtils;
import com.yunze.common.utils.spring.SpringUtils;
import com.yunze.common.utils.yunze.AesEncryptUtil;
import com.yunze.framework.web.service.TokenService;
import com.yunze.system.service.yunze.sysgl.IMySysDeptService;
import com.yunze.web.core.config.MyBaseController;
import io.swagger.annotations.Api;
import org.springframework.security.access.prepost.PreAuthorize;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import javax.annotation.Resource;
import java.util.HashMap;
import java.util.Map;

/**
 * 部门表预存金额管理
 */
@Api("部门表预存金额管理")
@RestController
@RequestMapping("/yunze/MySysDept")
public class MySysDeptController extends MyBaseController {

    @Resource
    private IMySysDeptService iMySysDeptService;

    /**
     * 授信修改
     */
    @PreAuthorize("@ss.hasPermi('yunze:dept:updCredit')")
    @PostMapping(value = "/updCredit", produces = {"application/json;charset=UTF-8"})
    public String updCredit(@RequestBody String Pstr) {
        HashMap<String, Object> Parammap = new HashMap<String, Object>();
        if (Pstr != null) {
            Pstr = Pstr.replace("%2F", "/");//转义 /
        }
        try {
            Pstr = AesEncryptUtil.desEncrypt(Pstr);
            Parammap.putAll(JSON.parseObject(Pstr));
            LoginUser loginUser = SpringUtils.getBean(TokenService.class).getLoginUser(ServletUtils.getRequest());
            HashMap<String, Object> Pmap = new HashMap<String, Object>();
            return MyRetunSuccess(iMySysDeptService.updCredit(Parammap,loginUser.getUser()), null);
        } catch (Exception e) {
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> /yunze/MySysDept/updCredit  <br/> Pstr = " + Pstr + " <br/> ip =  " + ip + " <br/> ", e.getCause().toString());
        }
        return Myerr("部门表预存金额管理 操作失败！");
    }








    /**
     * 企业预存 【微信支付】
     */
    @PreAuthorize("@ss.hasPermi('yunze:dept:initiatePreSave')")
    @PostMapping(value = "/initiatePreSave", produces = {"application/json;charset=UTF-8"})
    public String initiatePreSave(@RequestBody String Pstr) {
        HashMap<String, Object> Parammap = new HashMap<String, Object>();
        if (Pstr != null) {
            Pstr = Pstr.replace("%2F", "/");//转义 /
            Pstr = Pstr.replace(" ", "+");//转义 ' '
        }
        try {
            Pstr = AesEncryptUtil.desEncrypt(Pstr);
            Parammap.putAll(JSON.parseObject(Pstr));
            Map<String,Object> Rmap = iMySysDeptService.initiatePreSave(Parammap);
            boolean bool = (boolean) Rmap.get("bool");
            String Msg =  Rmap.get("Msg").toString();
            if(bool){
                return MyRetunSuccess(Rmap,Msg);
            }else{
                return Myerr(Msg);
            }
        } catch (Exception e) {
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> /yunze/MySysDept/initiatePreSave  <br/> Pstr = " + Pstr + " <br/> ip =  " + ip + " <br/> ", e.getCause().toString());
        }
        return Myerr("企业预存 【微信支付】 操作失败！");
    }



    /**
     * 企业预存 【微信支付】
     */
    @RequestMapping(value = "/weChatNotify", produces = {"application/json;charset=UTF-8"})
    public String weChatNotify(@RequestBody String xmlData) {
        String ip = "";
        try {
            ip = IpUtils.getIpAddr(ServletUtils.getRequest());
        }catch (Exception e){
            System.out.println("IP 获取操作失败");
        }
        try {
            return iMySysDeptService.weChatNotify(xmlData,ip);
        } catch (Exception e) {

            logger.error("<br/> /yunze/MySysDept/weChatNotify  <br/> xmlData = " + xmlData + " <br/> ip =  " + ip + " <br/> ", e.getCause().toString());
            return WxPayNotifyResponse.fail("回调有误!");
        }
    }


    /**
     * 企业预存 订单查看
     */
    @PreAuthorize("@ss.hasPermi('yunze:dept:initiatePreSave')")
    @PostMapping(value = "/queryWxOrder", produces = {"application/json;charset=UTF-8"})
    public String queryWxOrder(@RequestBody String Pstr) {
        HashMap<String, Object> Parammap = new HashMap<String, Object>();
        if (Pstr != null) {
            Pstr = Pstr.replace("%2F", "/");//转义 /
            Pstr = Pstr.replace(" ", "+");//转义 ' '
        }
        try {
            Pstr = AesEncryptUtil.desEncrypt(Pstr);
            Parammap.putAll(JSON.parseObject(Pstr));
            Map<String,Object> Rmap = iMySysDeptService.queryWxOrder(Parammap);
            boolean bool = (boolean) Rmap.get("bool");
            String Msg =  Rmap.get("Msg").toString();
            if(bool){
                return MyRetunSuccess(Rmap,Msg);
            }else{
                return Myerr(Msg);
            }
        } catch (Exception e) {
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> /yunze/MySysDept/queryWxOrder  <br/> Pstr = " + Pstr + " <br/> ip =  " + ip + " <br/> ", e.getCause().toString());
        }
        return Myerr("企业预存 订单查看 操作失败！");
    }





}













