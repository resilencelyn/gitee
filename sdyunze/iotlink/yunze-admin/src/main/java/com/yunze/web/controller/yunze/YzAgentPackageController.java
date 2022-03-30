package com.yunze.web.controller.yunze;

import com.alibaba.fastjson.JSON;
import com.yunze.common.core.controller.BaseController;
import com.yunze.common.core.domain.entity.SysUser;
import com.yunze.common.core.domain.model.LoginUser;
import com.yunze.common.utils.ServletUtils;
import com.yunze.common.utils.ip.IpUtils;
import com.yunze.common.utils.spring.SpringUtils;
import com.yunze.common.utils.yunze.AesEncryptUtil;
import com.yunze.framework.web.service.TokenService;
import com.yunze.system.service.yunze.IYzAgentPackageService;
import io.swagger.annotations.Api;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.access.prepost.PreAuthorize;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

/**
 *  代理资费组 管理
 *  2021-06-22
 * @author root
 */
@Api("代理资费组管理")
@RestController
@RequestMapping("/yunze/agentTariffGroup")
public class YzAgentPackageController extends BaseController
{

    @Autowired
    private IYzAgentPackageService agentPackageService;



    /**
     * 代理资费组 列表
     */
    @PreAuthorize("@ss.hasPermi('yunze:agentTariffGroup:list')")
    @PostMapping(value = "/list", produces = { "application/json;charset=UTF-8" })
    public String list(@RequestBody String Pstr)
    {
        HashMap<String, Object> Parammap = new HashMap<String, Object>();
        if(Pstr!=null){
            Pstr = Pstr.replace("%2F", "/");//转义 /
        }
        try {
            Pstr =  AesEncryptUtil.desEncrypt(Pstr);
            Parammap.putAll(JSON.parseObject((String) Pstr));
            Object agent_idObj = Parammap.get("agent_id");
            if(agent_idObj==null){
                LoginUser loginUser = SpringUtils.getBean(TokenService.class).getLoginUser(ServletUtils.getRequest());
                SysUser currentUser = loginUser.getUser();
                List<String> agent_id = new ArrayList<>();
                agent_id.add(currentUser.getDeptId().toString());
                Parammap.put("agent_id",agent_id);
            }
            return MyRetunSuccess(agentPackageService.selMap(Parammap),null);
        }catch (Exception e){
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:agentTariffGroup:list  <br/> Pstr = " + Pstr + " <br/> ip =  " + ip + " <br/> ",e.getCause().toString());
        }
        return Myerr("获取代理资费组列表 异常！");
    }


   /* *//**
     * 代理资费组 新增
     *//*
    @Log(title = "代理资费组", businessType = BusinessType.INSERT)
    @PreAuthorize("@ss.hasPermi('yunze:agentTariffGroup:add')")
    @PostMapping(value = "/add", produces = { "application/json;charset=UTF-8" })
    public String add(@RequestBody String Pstr)
    {
        HashMap<String, Object> Parammap = new HashMap<String, Object>();
        if(Pstr!=null){
            Pstr = Pstr.replace("%2F", "/");//转义 /
        }
        try {
            Pstr =  AesEncryptUtil.desEncrypt(Pstr);
            Parammap.putAll(JSON.parseObject((String) Pstr));
            return MyRetunSuccess(agentPackageService.add(Parammap),null);
        }catch (Exception e){
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:agentTariffGroup:add  <br/> Pstr = " + Pstr + " <br/> ip =  " + ip + " <br/> ",e.toString());
        }
        return Myerr("代理资费组 新增 异常！");
    }


    *//**
     * 代理资费组 详情
     *//*
    @PreAuthorize("@ss.hasPermi('yunze:agentTariffGroup:find')")
    @PostMapping(value = "/find", produces = { "application/json;charset=UTF-8" })
    public String find(@RequestBody String Pstr)
    {
        HashMap<String, Object> Parammap = new HashMap<String, Object>();
        if(Pstr!=null){
            Pstr = Pstr.replace("%2F", "/");//转义 /
        }
        try {
            Pstr =  AesEncryptUtil.desEncrypt(Pstr);
            Parammap.putAll(JSON.parseObject((String) Pstr));
            return MyRetunSuccess(agentPackageService.find(Parammap),null);
        }catch (Exception e){
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:agentTariffGroup:find  <br/> Pstr = " + Pstr + " <br/> ip =  " + ip + " <br/> ",e.toString());
        }
        return Myerr("代理资费组 详情 异常！");
    }


    *//**
     * 代理资费组 修改
     *//*
    @Log(title = "代理资费组", businessType = BusinessType.UPDATE)
    @PreAuthorize("@ss.hasPermi('yunze:agentTariffGroup:edit')")
    @PostMapping(value = "/edit", produces = { "application/json;charset=UTF-8" })
    public String edit(@RequestBody String Pstr)
    {
        HashMap<String, Object> Parammap = new HashMap<String, Object>();
        if(Pstr!=null){
            Pstr = Pstr.replace("%2F", "/");//转义 /
        }
        try {
            Pstr =  AesEncryptUtil.desEncrypt(Pstr);
            Parammap.putAll(JSON.parseObject((String) Pstr));
            boolean bool = agentPackageService.update(Parammap);
            String msg = bool?"修改 代理资费组成功！":"网络异常请稍后重试！！！";
            return MyRetunSuccess(bool,msg);
        }catch (Exception e){
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:agentTariffGroup:edit  <br/> Pstr = " + Pstr + " <br/> ip =  " + ip + " <br/> ",e.toString());
        }
        return Myerr("代理资费组 修改 异常！");
    }
*/










}
