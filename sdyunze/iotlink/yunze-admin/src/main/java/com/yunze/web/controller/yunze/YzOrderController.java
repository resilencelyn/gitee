package com.yunze.web.controller.yunze;

import com.alibaba.fastjson.JSON;
import com.yunze.common.annotation.Log;
import com.yunze.common.core.controller.BaseController;
import com.yunze.common.core.domain.AjaxResult;
import com.yunze.common.core.domain.entity.SysUser;
import com.yunze.common.core.domain.model.LoginUser;
import com.yunze.common.enums.BusinessType;
import com.yunze.common.utils.ServletUtils;
import com.yunze.common.utils.ip.IpUtils;
import com.yunze.common.utils.spring.SpringUtils;
import com.yunze.common.utils.yunze.AesEncryptUtil;
import com.yunze.framework.web.service.TokenService;
import com.yunze.system.service.yunze.IYzCardFlowService;
import com.yunze.system.service.yunze.IYzOrderService;
import io.swagger.annotations.Api;
import org.springframework.security.access.prepost.PreAuthorize;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.multipart.MultipartFile;

import javax.annotation.Resource;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 *  通道 管理
 *  2021-06-19
 * @author root
 */
@Api("订单管理")
@RestController
@RequestMapping("/yunze/order")
public class YzOrderController extends BaseController
{

    @Resource
    private IYzOrderService iYzOrderService;
    @Resource
    private IYzCardFlowService iYzCardFlowService;


    /**
     * 订单 列表
     */
    @PreAuthorize("@ss.hasPermi('yunze:order:list')")
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
            LoginUser loginUser = SpringUtils.getBean(TokenService.class).getLoginUser(ServletUtils.getRequest());
            SysUser currentUser = loginUser.getUser();
            List<Integer> agent_id = new ArrayList<>();
            if(currentUser.getDeptId()!=100){
                if(Parammap.get("agent_id")!=null){
                    List<Integer> P_agent_id = (List<Integer>) Parammap.get("agent_id");
                    agent_id.addAll(P_agent_id);
                }else{
                    agent_id.add(Integer.parseInt(currentUser.getDeptId().toString()));
                    Parammap.put("agent_id",agent_id);
                }
            }
            return MyRetunSuccess(iYzOrderService.selMap(Parammap),null);
        }catch (Exception e){
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:order:list  <br/> Pstr = " + Pstr + " <br/> ip =  " + ip + " <br/> ",e.getCause().toString());
        }
        return Myerr("获取订单 列表信息列表 异常！");
    }



    /**
     * 查询 资费计划 信息
     */
    @PreAuthorize("@ss.hasPermi('yunze:order:list')")
    @PostMapping(value = "/queryPacketSimple", produces = { "application/json;charset=UTF-8" })
    public String queryPacketSimple()
    {
        HashMap<String, Object> Parammap = new HashMap<String, Object>();
        try {
            LoginUser loginUser = SpringUtils.getBean(TokenService.class).getLoginUser(ServletUtils.getRequest());
            SysUser currentUser = loginUser.getUser();
            List<String> agent_id = new ArrayList<>();
            if(currentUser.getDeptId()!=100){
                    agent_id.add(currentUser.getDeptId().toString());
            }else{
                agent_id.add(currentUser.getDeptId().toString());
            }

            Parammap.put("agent_id",agent_id);
            return MyRetunSuccess(iYzCardFlowService.queryPacket_simple(Parammap),null);
        }catch (Exception e){
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:order:list  <br/>   ip =  " + ip + " <br/> ",e.getCause().toString());
        }
        return Myerr("获取 【订单】 查询 资费计划 异常！");
    }


    /**
     * 查询 订单详情
     */
    @PreAuthorize("@ss.hasPermi('yunze:order:findOrder')")
    @PostMapping(value = "/findOrder", produces = { "application/json;charset=UTF-8" })
    public String findOrder(@RequestBody String Pstr)
    {
        if(Pstr!=null){
            Pstr = Pstr.replace("%2F", "/");//转义 /
        }
        HashMap<String, Object> Parammap = new HashMap<String, Object>();
        try {
            Pstr =  AesEncryptUtil.desEncrypt(Pstr);
            Parammap.putAll(JSON.parseObject((String) Pstr));
            return MyRetunSuccess(iYzOrderService.findOrder(Parammap),null);
        }catch (Exception e){
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:order:findOrder  <br/>   ip =  " + ip + " <br/> ",e.getCause().toString());
        }
        return Myerr("查询 【订单详情】  异常！");
    }


    /**
     * 平台导入充值
     * @param file
     * @return
     */
    @Log(title = "订单管理", businessType = BusinessType.IMPORT)
    @PreAuthorize("@ss.hasPermi('yunze:order:importRecharge')")
    @PostMapping(value = "/importRecharge", produces = { "application/json;charset=utf-8" })
    public AjaxResult importRecharge(MultipartFile file,@RequestParam Map<String,String> map)
    {

        String Pstr = map.get("Pstr").toString();
        if(Pstr!=null){
            Pstr = Pstr.replace("%2F", "/");//转义 /
        }
        HashMap<String, Object> Parammap = new HashMap<String, Object>();
        try {
            Pstr =  AesEncryptUtil.desEncrypt(Pstr);
            Parammap.putAll(JSON.parseObject((String) Pstr));
            LoginUser loginUser = SpringUtils.getBean(TokenService.class).getLoginUser(ServletUtils.getRequest());
            SysUser User = loginUser.getUser();
            Parammap.put("User",User);
            Parammap.put("agent_id",User.getDeptId());
            return AjaxResult.success(iYzOrderService.importRecharge(file,Parammap));
        }catch (Exception e){
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:order:importRecharge  <br/> ip =  " + ip + " <br/> ",e.getCause().toString());
        }
        return AjaxResult.error("平台导入充值 异常！");
    }

    /**
     * 导出
     * @return
     */
    @Log(title = "全部订单导出", businessType = BusinessType.EXPORT)
    @PreAuthorize("@ss.hasPermi('yunze:order:outOrder')")
    @PostMapping(value = "/outOrder", produces = { "application/json;charset=utf-8" })
    public String exportData(@RequestBody String Pstr)
    {
        HashMap<String, Object> Parammap = new HashMap<String, Object>();
        if(Pstr!=null){
            Pstr = Pstr.replace("%2F", "/");//转义 /
        }
        try {

            Pstr =  AesEncryptUtil.desEncrypt(Pstr);
            //  System.out.println(map);
            Parammap.putAll(JSON.parseObject((String) Pstr));

            LoginUser loginUser = SpringUtils.getBean(TokenService.class).getLoginUser(ServletUtils.getRequest());
            SysUser currentUser = loginUser.getUser();

            return MyRetunSuccess(iYzOrderService.exportallorders(Parammap,currentUser),null);
        }catch (Exception e){
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:order:outOrder  <br/> Pstr = " + Pstr + " <br/> ip =  " + ip + " <br/> ",e.getCause().toString());
        }
        return Myerr("全部订单导出 异常！");
    }





}
















