package com.yunze.web.controller.yunze;

import com.alibaba.fastjson.JSON;
import com.yunze.apiCommon.utils.InternalApiRequest;
import com.yunze.common.annotation.Log;
import com.yunze.common.core.controller.BaseController;
import com.yunze.common.core.domain.AjaxResult;
import com.yunze.common.core.domain.entity.SysUser;
import com.yunze.common.core.domain.entity.YzCard;
import com.yunze.common.core.domain.model.LoginUser;
import com.yunze.common.enums.BusinessType;
import com.yunze.common.utils.ServletUtils;
import com.yunze.common.utils.ip.IpUtils;
import com.yunze.common.utils.poi.ExcelUtil;
import com.yunze.common.utils.spring.SpringUtils;
import com.yunze.common.utils.yunze.*;
import com.yunze.framework.web.service.TokenService;
import com.yunze.system.service.ISysUserService;
import com.yunze.system.service.impl.yunze.YzCardServiceImpl;
import com.yunze.system.service.yunze.IYzCardService;
import io.swagger.annotations.Api;
import io.swagger.annotations.ApiOperation;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.access.prepost.PreAuthorize;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.multipart.MultipartFile;

import javax.annotation.Resource;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * 卡信息
 *  2021-06-05
 * @author root
 */
@Api("卡信息")
@RestController
@RequestMapping("/yunze/card")
public class YzCardController extends BaseController
{
    @Resource
    private YzCardServiceImpl yzCardServiceImpl;
    @Resource
    private ApiParamMap apiParamMap;
    @Autowired
    private ISysUserService userService;
    @Resource
    private TokenService tokenService;
    @Resource
    private CardFlowSyn cardFlowSyn;
    @Resource
    private GetShowStatIdArr getShowStatIdArr;
    @Resource
    private InternalApiRequest internalApiRequest;


    /**
     * 卡板信息列表
     */
    @PreAuthorize("@ss.hasPermi('yunze:card:list')")
   /* @PostMapping("/list", produces = { "application/json;charset=UTF-8" })*/
    @PostMapping(value = "/list", produces = { "application/json;charset=UTF-8" })
    public String list(@RequestBody String Pstr)
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
            return MyRetunSuccess(yzCardServiceImpl.selMap(Parammap),null);
        }catch (Exception e){
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:card:list  <br/> Pstr = " + Pstr + " <br/> ip =  " + ip + " <br/> ",e.getCause().toString());
        }
        return Myerr("获取卡板信息列表 异常！");
    }





    /**
     * 获取 所属代理下 卡分组
     */
    @PreAuthorize("@ss.hasPermi('yunze:card:list')")
    @PostMapping(value = "/getCardGrouping", produces = { "application/json;charset=UTF-8" })
    public String getCardGrouping(@RequestBody String Pstr)
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
            List<Integer> agent_id = new ArrayList<>();
            String StrAgent_id = currentUser.getDeptId().toString()+",";
            if(Parammap.get("agent_id")!=null){
                List<Integer> P_agent_id = (List<Integer>) Parammap.get("agent_id");
                for (int i = 0; i < P_agent_id.size(); i++) {
                    StrAgent_id += ""+P_agent_id.get(i)+",";
                }
            }
            if(currentUser.getDeptId()==100){
                if(Parammap.get("agent_id")!=null){
                    Parammap.put("agent_id",StrAgent_id);
                }
            }else{
                Parammap.put("agent_id",StrAgent_id);
            }
            return MyRetunSuccess(yzCardServiceImpl.getCardGrouping(Parammap),null);
        }catch (Exception e){
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> /yunze/card/getCardGrouping  <br/> Pstr = " + Pstr + " <br/> ip =  " + ip + " <br/> ",e.getCause().toString());
        }
        return Myerr("获取 所属代理下 卡分组 异常！");
    }










    /**
     * 查询卡板详情
     */
    @PreAuthorize("@ss.hasPermi('yunze:card:find')")
    @PostMapping(value = "/find", produces = { "application/json;charset=utf-8" })//
    public String find(@RequestBody String Pstr)
    {
        HashMap<String, Object> Parammap = new HashMap<String, Object>();
        if(Pstr!=null){
            Pstr = Pstr.replace("%2F", "/");//转义 /
        }
        try {
            Pstr =  AesEncryptUtil.desEncrypt(Pstr);
            //  System.out.println(map);
            Parammap.putAll(JSON.parseObject((String) Pstr));
            return MyRetunSuccess(yzCardServiceImpl.find(Parammap),null);
        }catch (Exception e){
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:card:find  <br/> Pstr = " + Pstr + " <br/> ip =  " + ip + " <br/> ",e.getCause().toString());
        }
        return Myerr("查询卡板详情 异常！");
    }


    /**
     * 导入
     * @param file
     * @param updateSupport
     * @return
     */
    @Log(title = "卡列表", businessType = BusinessType.IMPORT)
    @PreAuthorize("@ss.hasPermi('yunze:card:import')")
    @PostMapping(value = "/importData", produces = { "application/json;charset=utf-8" })
    public AjaxResult importData(MultipartFile file, boolean updateSupport)
    {
        try {
            LoginUser loginUser = SpringUtils.getBean(TokenService.class).getLoginUser(ServletUtils.getRequest());
            SysUser User = loginUser.getUser();
            return AjaxResult.success(yzCardServiceImpl.uploadCard(file,updateSupport,User));
        }catch (Exception e){
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:card:import  <br/> ip =  " + ip + " <br/> ",e.getCause().toString());
        }
        return AjaxResult.error("导入卡列表 异常！");
    }



    /**
     * 下载导入模板
     * @return
     */
    @GetMapping("/importTemplate")
    public AjaxResult importTemplate()
    {
        ExcelUtil<YzCard> util = new ExcelUtil<YzCard>(YzCard.class);
        return util.importTemplateExcel("卡列表数据");
    }


    /**
     * 连接管理获取部门名称
     * @return
     */
    @PreAuthorize("@ss.hasPermi('yunze:card:list')")
    @PostMapping(value = "/getDeptName", produces = { "application/json;charset=utf-8" })
    public String getDeptName()
    {
        try {
            return MyRetunSuccess(yzCardServiceImpl.getDeptName(),null);
        }catch (Exception e){
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:card:list  " + " <br/> ip =  " + ip + " <br/> ",e.getCause().toString());
        }
        return Myerr("连接管理获取部门名称 异常！");
    }





    /**
     * 导出
     * @return
     */
    @Log(title = "卡列表", businessType = BusinessType.EXPORT)
    @PreAuthorize("@ss.hasPermi('yunze:card:exportData')")
    @PostMapping(value = "/exportData", produces = { "application/json;charset=utf-8" })
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


            return MyRetunSuccess(yzCardServiceImpl.exportData(Parammap,currentUser),null);
        }catch (Exception e){
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:card:exportData  <br/> Pstr = " + Pstr + " <br/> ip =  " + ip + " <br/> ",e.getCause().toString());
        }
        return Myerr("导出卡列表 异常！");
    }



    /**
     * 查询 企业下所属人员
     * @return
     */
    @PreAuthorize("@ss.hasPermi('yunze:card:findDeptUser')")
    @PostMapping(value = "/findDeptUser", produces = { "application/json;charset=utf-8" })
    public String findDeptUser(@RequestBody String Pstr)
    {
        HashMap<String, Object> Parammap = new HashMap<String, Object>();
        if(Pstr!=null){
            Pstr = Pstr.replace("%2F", "/");//转义 /
        }
        try {
            Pstr =  AesEncryptUtil.desEncrypt(Pstr);
            Parammap.putAll(JSON.parseObject((String) Pstr));
            String dept_id = Parammap.get("dept_id").toString();
            if(dept_id!=null && dept_id!=""){
                return MyRetunSuccess(userService.findDeptUser(Parammap),null);
            }else{
                return Myerr("参数不全请核对后重试！");
            }
        }catch (Exception e){
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:card:findDeptUser  " + " <br/> ip =  " + ip + " <br/> ",e.getCause().toString());
        }
        return Myerr("查询 企业下所属人员 异常！");
    }



    /**
     * 划分卡信息
     * @return
     */
    @Log(title = "划卡操作", businessType = BusinessType.UPDATE)
    @PreAuthorize("@ss.hasPermi('yunze:card:divide')")
    @PostMapping(value = "/divide", produces = { "application/json;charset=utf-8" })
    public String divide(@RequestBody String Pstr)
    {
        HashMap<String, Object> Parammap = new HashMap<String, Object>();
        if(Pstr!=null){
            Pstr = Pstr.replace("%2F", "/");//转义 /
        }
        try {
            Pstr =  AesEncryptUtil.desEncrypt(Pstr);
            Parammap.putAll(JSON.parseObject((String) Pstr));
            Object set_dept_id = Parammap.get("set_dept_id");
            Object set_user_id = Parammap.get("set_user_id");
            Object set_dept_name = Parammap.get("set_dept_name");
            Object set_user_name = Parammap.get("set_user_name");
            if(set_dept_id!=null && set_dept_id.toString()!="" && set_user_id!=null && set_user_id.toString()!=""
                    && set_dept_name!=null && set_dept_name.toString()!=""&& set_user_name!=null && set_user_name.toString()!=""){
                LoginUser loginUser = SpringUtils.getBean(TokenService.class).getLoginUser(ServletUtils.getRequest());
                SysUser currentUser = loginUser.getUser();
                String agent_id = currentUser.getDeptId().toString();
                // if(!set_dept_id.equals(agent_id)){
                //权限 管理 操作必须传入当前操作人 id
                List<Integer> agent_idArr = new ArrayList<>();
                if(currentUser.getDeptId()!=100){
                    if(Parammap.get("agent_id")!=null){
                        List<Integer> P_agent_id = (List<Integer>) Parammap.get("agent_id");
                        agent_idArr.addAll(P_agent_id);
                    }else{
                        agent_idArr.add(Integer.parseInt(currentUser.getDeptId().toString()));
                        Parammap.put("agent_id",agent_idArr);
                    }
                }
                Parammap.put("TaskAgent_id",agent_id);
                return MyRetunSuccess(yzCardServiceImpl.dividCard(Parammap),null);
                /*}else{
                    return Myerr("不能操作！");
                }*/
            }else{
                return Myerr("参数不全请核对后重试！");
            }
        }catch (Exception e){
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:card:divide  " + " <br/> ip =  " + ip + " <br/> ",e.getCause().toString());
        }
        return Myerr("划分卡信息 异常！");
    }



    /**
     * 同步用量
     * @param Pstr
     * @return
     */
    @PreAuthorize("@ss.hasPermi('yunze:card:SynFlow')")
    @PostMapping(value = "/SynFlow", produces = { "application/json;charset=utf-8" })
    public String SynFlow(@RequestBody String Pstr)
    {
        HashMap<String, Object> Parammap = new HashMap<String, Object>();
        if(Pstr!=null){
            Pstr = Pstr.replace("%2F", "/");//转义 /
        }
        try {
            Pstr =  AesEncryptUtil.desEncrypt(Pstr);
            //  System.out.println(map);
            Parammap.putAll(JSON.parseObject((String) Pstr));
            Map<String, Object> Route  = yzCardServiceImpl.findRoute(Parammap);
            if(Route!=null ){
                String cd_status = Route.get("cd_status").toString();
                if(cd_status!=null && cd_status!="" && cd_status.equals("1")){

                    Map<String,Object> Rmap =  internalApiRequest.queryFlow(Parammap,Route);
                    String code =  Rmap.get("code")!=null?Rmap.get("code").toString():"500";
                    if(code.equals("200")){
                        //获取 卡用量 开卡日期 更新 card info
                        if(Rmap.get("Use")!=null &&  Rmap.get("Use")!="" &&  Rmap.get("Use").toString().trim().length()>0){
                            Double Use = Double.parseDouble(Rmap.get("Use").toString());
                            if(Use>=0){
                                try {
                                    Map<String,Object> RMap = cardFlowSyn.CalculationFlow(Parammap.get("iccid").toString(),Use);
                                    return MyRetunSuccess(RMap,"已成功同步用量！");
                                }catch (Exception e){
                                    return Myerr("用量内部计算错误！"+e.getMessage().toString());
                                }
                            }else{
                                return Myerr("接口超频返回暂无数据返回，请稍后重试！");
                            }
                        }
                    }else{
                        return Myerr("网络繁忙稍后重试！"+Rmap.get("Message").toString());
                    }
                }else{
                    String statusVal = cd_status.equals("2")?"已停用":cd_status.equals("3")?"已删除":"状态未知";
                    return Myerr("同步用量 异常！"+" 通道 ["+statusVal+"]");
                }
            }else{
                return Myerr(" iccid ["+Parammap.get("iccid")+"] 未划分 API通道 ！请划分通道后重试！");
            }
        }catch (Exception e){
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:card:SynFlow  " + " <br/> ip =  " + ip + " <br/> ",e.getCause().toString());
        }
        return Myerr("同步用量 异常！");
    }



    /**
     * 同步卡状态
     * @param Pstr
     * @return
     */
    @PreAuthorize("@ss.hasPermi('yunze:card:SynStatus')")
    @PostMapping(value = "/SynStatus", produces = { "application/json;charset=utf-8" })
    public String SynStatus(@RequestBody String Pstr)
    {
        HashMap<String, Object> Parammap = new HashMap<String, Object>();
        if(Pstr!=null){
            Pstr = Pstr.replace("%2F", "/");//转义 /
        }
        try {
            Pstr =  AesEncryptUtil.desEncrypt(Pstr);
            //  System.out.println(map);
            Parammap.putAll(JSON.parseObject((String) Pstr));
            Map<String, Object> Route  = yzCardServiceImpl.findRoute(Parammap);
            if(Route!=null ){
                String cd_status = Route.get("cd_status").toString();
                if(cd_status!=null && cd_status!="" && cd_status.equals("1")){
                    Map<String,Object> Rmap =  internalApiRequest.queryCardStatus(Parammap,Route);

                    String code =  Rmap.get("code")!=null?Rmap.get("code").toString():"500";
                    if(code.equals("200")){
                        //获取 卡状态 开卡日期 更新 card info
                        if(Rmap.get("statusCode")!=null &&  Rmap.get("statusCode")!="" &&  Rmap.get("statusCode").toString().trim().length()>0){
                            String statusCode = Rmap.get("statusCode").toString().trim();
                            if(!statusCode.equals("0")){
                                Map<String,Object> Upd_Map = new HashMap<>();
                                Upd_Map.put("status_id",statusCode);
                                Upd_Map.put("status_ShowId",getShowStatIdArr.GetShowStatId(statusCode));
                                Upd_Map.put("iccid",Parammap.get("iccid").toString());
                                try {
                                    boolean bool = yzCardServiceImpl.updStatusId(Upd_Map);//变更卡状态
                                    if(bool){
                                        return MyRetunSuccess("","已成功同步卡状态！");
                                    }else{
                                        return Myerr("保存状态异常:False！");
                                    }
                                }catch (Exception e){
                                    return Myerr("DB保存状态异常！"+e.getMessage().toString());
                                }
                            }else{
                                return Myerr("接口超频返回暂无数据返回，请稍后重试！");
                            }
                        }
                    }else{
                        return Myerr("网络繁忙稍后重试！"+Rmap.get("Message").toString());
                    }

                }else{
                    String statusVal = cd_status.equals("2")?"已停用":cd_status.equals("3")?"已删除":"状态未知";
                    return Myerr("同步卡状态 异常！"+" 通道 ["+statusVal+"]");
                }
            }else{
                return Myerr(" iccid ["+Parammap.get("iccid")+"] 未划分 API通道 ！请划分通道后重试！");
            }
        }catch (Exception e){
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:card:SynStatus  " + " <br/> ip =  " + ip + " <br/> ",e.getCause().toString());
        }
        return Myerr("同步卡状态 异常！");
    }




    /**
     * 同步 激活时间
     * @param Pstr
     * @return
     */
    @PreAuthorize("@ss.hasPermi('yunze:card:SynActivateDate')")
    @PostMapping(value = "/SynActivateDate", produces = { "application/json;charset=utf-8" })
    public String SynActivateDate(@RequestBody String Pstr)
    {
        HashMap<String, Object> Parammap = new HashMap<String, Object>();
        if(Pstr!=null){
            Pstr = Pstr.replace("%2F", "/");//转义 /
        }
        try {
            Pstr =  AesEncryptUtil.desEncrypt(Pstr);
            //  System.out.println(map);
            Parammap.putAll(JSON.parseObject((String) Pstr));
            Map<String, Object> Route  = yzCardServiceImpl.findRoute(Parammap);
            if(Route!=null ){
                String cd_status = Route.get("cd_status").toString();
                if(cd_status!=null && cd_status!="" && cd_status.equals("1")){
                    Map<String,Object> Rmap =  internalApiRequest.queryCardActiveTime(Parammap,Route);
                    String code =  Rmap.get("code")!=null?Rmap.get("code").toString():"500";
                    if(code.equals("200")){
                        //获取 激活时间
                        if(Rmap.get("activateDate")!=null &&  Rmap.get("activateDate")!="" &&  Rmap.get("activateDate").toString().trim().length()>0){
                            String activateDate = Rmap.get("activateDate").toString().trim();
                            activateDate = activateDate.length()>10?activateDate.substring(0,10):activateDate;
                                Map<String,Object> Upd_Map = new HashMap<>();
                                Upd_Map.put("activate_date",activateDate);
                                Upd_Map.put("open_date",null);
                                Upd_Map.put("iccid",Parammap.get("iccid").toString());
                                try {
                                    boolean bool = yzCardServiceImpl.updActivate(Upd_Map);//变更卡状态
                                    if(bool){
                                        return MyRetunSuccess("","已成功同步激活时间！");
                                    }else{
                                        return Myerr("保存激活时间异常:False！");
                                    }
                                }catch (Exception e){
                                    return Myerr("DB保存状态异常！"+e.getMessage().toString());
                                }
                        }else{
                            System.out.println(Rmap);
                            return Myerr("上游接口返回数据异常,稍后重试！");
                        }
                    }else{
                        return Myerr("网络繁忙稍后重试！"+Rmap.get("Message").toString());
                    }
                }else{
                    String statusVal = cd_status.equals("2")?"已停用":cd_status.equals("3")?"已删除":"状态未知";
                    return Myerr("同步激活时间 异常！"+" 通道 ["+statusVal+"]");
                }
            }else{
                return Myerr(" iccid ["+Parammap.get("iccid")+"] 未划分 API通道 ！请划分通道后重试！");
            }
        }catch (Exception e){
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> SynActivateDate  " + " <br/> ip =  " + ip + " <br/> ",e.getCause().toString());
        }
        return Myerr("同步激活时间 异常！");
    }





    /**
     * 查询在线信息
     * @param Pstr
     * @return
     */
    @ApiOperation("查询在线信息")
    @PreAuthorize("@ss.hasPermi('yunze:card:queryOnlineStatus')")
    @PostMapping(value = "/queryOnlineStatus", produces = { "application/json;charset=utf-8" })
    public String queryOnlineStatus(@RequestBody String Pstr)
    {
        HashMap<String, Object> Parammap = new HashMap<String, Object>();
        if(Pstr!=null){
            Pstr = Pstr.replace("%2F", "/");//转义 /
        }
        try {
            Pstr =  AesEncryptUtil.desEncrypt(Pstr);
            //  System.out.println(map);
            Parammap.putAll(JSON.parseObject((String) Pstr));
            Map<String, Object> Route  = yzCardServiceImpl.findRoute(Parammap);
            if(Route!=null ){
                String cd_status = Route.get("cd_status").toString();
                if(cd_status!=null && cd_status!="" && cd_status.equals("1")){
                    Map<String, Object> Rmap = internalApiRequest.queryOnlineStatus(Parammap,Route);
                    String code = Rmap.get("code")!=null?Rmap.get("code").toString():"500";
                    if(code.equals("200")){
                        return MyRetunSuccess(Rmap,"查询在线信息成功！");
                    }else{
                        return Myerr("暂未查询到在线信息！");
                    }
                }else{
                    String statusVal = cd_status.equals("2")?"已停用":cd_status.equals("3")?"已删除":"状态未知";
                    return Myerr("查询在线信息 异常！"+" 通道 ["+statusVal+"]");
                }
            }else{
                return Myerr(" iccid ["+Parammap.get("iccid")+"] 未划分 API通道 ！请划分通道后重试！");
            }
        }catch (Exception e){
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:card:queryOnlineStatus  " + " <br/> ip =  " + ip + " <br/> ",e.getCause().toString());
        }
        return Myerr("查询在线信息 异常！");
    }



    /**
     * 机卡重绑
     * @param Pstr
     * @return
     */
    @ApiOperation("机卡重绑")
    @PreAuthorize("@ss.hasPermi('yunze:card:CardBinding')")
    @PostMapping(value = "/CardBinding", produces = { "application/json;charset=utf-8" })
    public String CardBinding(@RequestBody String Pstr)
    {
        HashMap<String, Object> Parammap = new HashMap<String, Object>();
        if(Pstr!=null){
            Pstr = Pstr.replace("%2F", "/");//转义 /
        }
        try {
            Pstr =  AesEncryptUtil.desEncrypt(Pstr);
            //  System.out.println(map);
            Parammap.putAll(JSON.parseObject((String) Pstr));
            Map<String, Object> Route  = yzCardServiceImpl.findRoute(Parammap);
            if(Route!=null ){
                String cd_status = Route.get("cd_status").toString();
                if(cd_status!=null && cd_status!="" && cd_status.equals("1")){
                    Map<String, Object> Rmap = internalApiRequest.unbundling(Parammap,Route);
                    String code = Rmap.get("code").toString();
                    if(code.equals("200")){
                        return MyRetunSuccess(Rmap.get("Message"),"操作完成！");
                    }else{
                        return Myerr(Rmap.get("Message").toString());
                    }
                }else{
                    String statusVal = cd_status.equals("2")?"已停用":cd_status.equals("3")?"已删除":"状态未知";
                    return Myerr("机卡重绑 异常！"+" 通道 ["+statusVal+"]");
                }
            }else{
                return Myerr(" iccid ["+Parammap.get("iccid")+"] 未划分 API通道 ！请划分通道后重试！");
            }
        }catch (Exception e){
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:card:CardBinding  " + " <br/> ip =  " + ip + " <br/> ",e.getCause().toString());
        }
        return Myerr("机卡重绑 异常！");
    }



    /**
     * 连接管理设置
     * @param file
     * @return
     */
    @Log(title = "连接管理设置", businessType = BusinessType.UPDATE)
    @PreAuthorize("@ss.hasPermi('yunze:card:importSet')")
    @PostMapping(value = "/importSet", produces = { "application/json;charset=utf-8" })
    public AjaxResult importSet(MultipartFile file, @RequestParam Map<String,String> map)
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
            return AjaxResult.success(yzCardServiceImpl.importSet(file,Parammap));
        }catch (Exception e){
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:card:import  <br/> ip =  " + ip + " <br/> ",e.getCause().toString());
        }
        return AjaxResult.error("连接管理设置 异常！");
    }



    /**
     * 特殊操作查询IMEI
     * @param file
     * @return
     */
    @Log(title = "特殊操作查询IMEI", businessType = BusinessType.IMPORT)
    @PreAuthorize("@ss.hasPermi('yunze:card:importSelImei')")
    @PostMapping(value = "/importSelImei", produces = { "application/json;charset=utf-8" })
    public AjaxResult importSelImei(MultipartFile file)
    {
        HashMap<String, Object> Parammap = new HashMap<String, Object>();
        try {
            LoginUser loginUser = SpringUtils.getBean(TokenService.class).getLoginUser(ServletUtils.getRequest());
            SysUser User = loginUser.getUser();
            Parammap.put("User",User);
            Parammap.put("agent_id",User.getDeptId());
            return AjaxResult.success(yzCardServiceImpl.importSelImei(file,Parammap));
        }catch (Exception e){
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:card:importSelImei  <br/> ip =  " + ip + " <br/> ",e.getCause().toString());
        }
        return AjaxResult.error("特殊操作查询IMEI 异常！");
    }



    /**
     * 特殊操作 变更卡分组、备注
     * @param file
     * @return
     */
    @Log(title = "特殊操作变更卡分组、备注", businessType = BusinessType.IMPORT)
    @PreAuthorize("@ss.hasPermi('yunze:card:importSetCardInfo')")
    @PostMapping(value = "/importSetCardInfo", produces = { "application/json;charset=utf-8" })
    public AjaxResult importSetCardInfo(MultipartFile file)
    {
        HashMap<String, Object> Parammap = new HashMap<String, Object>();
        try {
            LoginUser loginUser = SpringUtils.getBean(TokenService.class).getLoginUser(ServletUtils.getRequest());
            SysUser User = loginUser.getUser();
            Parammap.put("User",User);
            Parammap.put("agent_id",User.getDeptId());
            return AjaxResult.success(yzCardServiceImpl.importSetCardInfo(file,Parammap));
        }catch (Exception e){
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:card:importSetCardInfo  <br/> ip =  " + ip + " <br/> ",e.getCause().toString());
        }
        return AjaxResult.error("特殊操作 变更卡分组、备注 异常！");
    }


    /**
     * 导入查询、卡号类别
     * @param file
     * @return
     */
    @Log(title = "卡号导入查询", businessType = BusinessType.IMPORT)
    @PreAuthorize("@ss.hasPermi('yunze:card:ImportQuery')")
    @PostMapping(value = "/cardNumber", produces = { "application/json;charset=utf-8" })
    public AjaxResult CardNumberImport(MultipartFile file, @RequestParam Map<String,String> map)
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
            List<Integer> agent_id = new ArrayList<>();
            if(User.getDeptId()!=100){
                if(Parammap.get("agent_id")!=null){
                    List<Integer> P_agent_id = (List<Integer>) Parammap.get("agent_id");
                    agent_id.addAll(P_agent_id);
                }else{
                    agent_id.add(Integer.parseInt(User.getDeptId().toString()));
                    Parammap.put("agent_id",agent_id);
                }
            }
            try {
                return AjaxResult.success(AesEncryptUtil.encrypt(JSON.toJSONString(yzCardServiceImpl.CardNumberImport(file,Parammap))));
            }catch (Exception e){
                return AjaxResult.error(AesEncryptUtil.encrypt(JSON.toJSONString("返回数据加密异常")));
            }
        }catch (Exception e){
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:card:ImportQuery  <br/> ip =  " + ip + " <br/> ",e.getCause().toString());
        }
        return AjaxResult.error("卡号导入查询 异常！");
    }




    /**
     * 批量停复机、断开网
     * @param file
     * @return
     */
    @Log(title = "批量停复机、断开网 ", businessType = BusinessType.IMPORT)
    @PreAuthorize("@ss.hasPermi('yunze:card:ApiUpdBatch')")
    @PostMapping(value = "/status", produces = { "application/json;charset=utf-8" })
    public AjaxResult importSelImei(MultipartFile file,@RequestParam Map<String,String> map)
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

            return AjaxResult.success(yzCardServiceImpl.status(file,Parammap));
        }catch (Exception e){
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:card:ApiUpdBatch  <br/> ip =  " + ip + " <br/> ",e.getCause().toString());
        }
        return AjaxResult.error("批量停复机、断开网  异常！");
    }

    /**修改 备注 分组*/

    @PreAuthorize("@ss.hasPermi('yunze:card:importSetCardInfo')")
    @PostMapping(value = "/updatefill", produces = {"application/json;charset=UTF-8"})
    public String Update(@RequestBody String Pstr) {
        HashMap<String, Object> Parammap = new HashMap<String, Object>();
        if (Pstr != null) {
            Pstr = Pstr.replace("%2F", "/");//转义 /
        }
        try {
            Pstr = AesEncryptUtil.desEncrypt(Pstr);
            //  System.out.println(map);
            Parammap.putAll(JSON.parseObject(Pstr));
            return MyRetunSuccess(yzCardServiceImpl.UpdateFill(Parammap), null);
        } catch (Exception e) {
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:card:updatefill  <br/> Pstr = " + Pstr + " <br/> ip =  " + ip + " <br/> ", e.getCause().toString());
        }
        return Myerr("修改 备注 分组 异常！");
    }

    /**
     * 下面是 操作
     * */
    /**停机*/
    @PreAuthorize("@ss.hasPermi('yunze:card:Stopped')")
    @PostMapping(value = "/stopped", produces = {"application/json;charset=UTF-8"})
    public String StoppedArr(@RequestBody String Pstr) {
        HashMap<String, Object> Parammap = new HashMap<String, Object>();
        if (Pstr != null) {
            Pstr = Pstr.replace("%2F", "/");//转义 /
        }
        try {
            Pstr = AesEncryptUtil.desEncrypt(Pstr);
            Parammap.putAll(JSON.parseObject(Pstr));
            return MyRetunSuccess(yzCardServiceImpl.stoppedarr(Parammap), null);
        } catch (Exception e) {
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:card:Stopped  <br/> Pstr = " + Pstr + " <br/> ip =  " + ip + " <br/> ", e.getCause().toString());
        }
        return Myerr("批量 【停机】 异常！");
    }

    /**复机*/
    @PreAuthorize("@ss.hasPermi('yunze:card:MachineArr')")
    @PostMapping(value = "/machine", produces = {"application/json;charset=UTF-8"})
    public String MachineArr(@RequestBody String Pstr) {
        HashMap<String, Object> Parammap = new HashMap<String, Object>();
        if (Pstr != null) {
            Pstr = Pstr.replace("%2F", "/");//转义 /
        }
        try {
            Pstr = AesEncryptUtil.desEncrypt(Pstr);
            Parammap.putAll(JSON.parseObject(Pstr));
            return MyRetunSuccess(yzCardServiceImpl.machinearr(Parammap), null);
        } catch (Exception e) {
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:card:machine  <br/> Pstr = " + Pstr + " <br/> ip =  " + ip + " <br/> ", e.getCause().toString());
        }
        return Myerr("批量 【复机】 异常！");
    }

    /**断网*/
    @PreAuthorize("@ss.hasPermi('yunze:card:DisconnectNetworkArr')")
    @PostMapping(value = "/disconnectNetwork", produces = {"application/json;charset=UTF-8"})
    public String DisconnectNetworkArr(@RequestBody String Pstr) {
        HashMap<String, Object> Parammap = new HashMap<String, Object>();
        if (Pstr != null) {
            Pstr = Pstr.replace("%2F", "/");//转义 /
        }
        try {
            Pstr = AesEncryptUtil.desEncrypt(Pstr);
            Parammap.putAll(JSON.parseObject(Pstr));
            return MyRetunSuccess(yzCardServiceImpl.disconnectNetworkarr(Parammap), null);
        } catch (Exception e) {
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:card:disconnectNetwork  <br/> Pstr = " + Pstr + " <br/> ip =  " + ip + " <br/> ", e.getCause().toString());
        }
        return Myerr("批量 【断网】 异常！");
    }

    /**开网*/
    @PreAuthorize("@ss.hasPermi('yunze:card:OpenNetworkArr')")
    @PostMapping(value = "/openNetwork", produces = {"application/json;charset=UTF-8"})
    public String OpenNetworkArr(@RequestBody String Pstr) {
        HashMap<String, Object> Parammap = new HashMap<String, Object>();
        if (Pstr != null) {
            Pstr = Pstr.replace("%2F", "/");//转义 /
        }
        try {
            Pstr = AesEncryptUtil.desEncrypt(Pstr);
            Parammap.putAll(JSON.parseObject(Pstr));
            return MyRetunSuccess(yzCardServiceImpl.openNetworkarr(Parammap), null);
        } catch (Exception e) {
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:card:OpenNetworkArr  <br/> Pstr = " + Pstr + " <br/> ip =  " + ip + " <br/> ", e.getCause().toString());
        }
        return Myerr("批量 【开网】 异常！");
    }

    /**同步流量*/
    @PreAuthorize("@ss.hasPermi('yunze:card:ConsumptionArr')")
    @PostMapping(value = "/consumption", produces = {"application/json;charset=UTF-8"})
    public String ConsumptionArr(@RequestBody String Pstr) {
        HashMap<String, Object> Parammap = new HashMap<String, Object>();
        if (Pstr != null) {
            Pstr = Pstr.replace("%2F", "/");//转义 /
        }
        try {
            Pstr = AesEncryptUtil.desEncrypt(Pstr);
            Parammap.putAll(JSON.parseObject(Pstr));
            return MyRetunSuccess(yzCardServiceImpl.consumptionarr(Parammap), null);
        } catch (Exception e) {
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:card:ConsumptionArr  <br/> Pstr = " + Pstr + " <br/> ip =  " + ip + " <br/> ", e.getCause().toString());
        }
        return Myerr("批量 【同步流量】 异常！");
    }

    /**同步状态*/
    @PreAuthorize("@ss.hasPermi('yunze:card:PublicMethodArr')")
    @PostMapping(value = "/publicmethod", produces = {"application/json;charset=UTF-8"})
    public String PublicMethodArr(@RequestBody String Pstr) {
        HashMap<String, Object> Parammap = new HashMap<String, Object>();
        if (Pstr != null) {
            Pstr = Pstr.replace("%2F", "/");//转义 /
        }
        try {
            Pstr = AesEncryptUtil.desEncrypt(Pstr);
            Parammap.putAll(JSON.parseObject(Pstr));
            return MyRetunSuccess(yzCardServiceImpl.publicmethodarr(Parammap), null);
        } catch (Exception e) {
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:card:PublicMethodArr  <br/> Pstr = " + Pstr + " <br/> ip =  " + ip + " <br/> ", e.getCause().toString());
        }
        return Myerr("批量 【同步状态】 异常！");
    }

    /**同步状态和用量*/
    @PreAuthorize("@ss.hasPermi('yunze:card:ConsumptionAndStateArr')")
    @PostMapping(value = "/consumptionandstate", produces = {"application/json;charset=UTF-8"})
    public String ConsumptionAndStateArr(@RequestBody String Pstr) {
        HashMap<String, Object> Parammap = new HashMap<String, Object>();
        if (Pstr != null) {
            Pstr = Pstr.replace("%2F", "/");//转义 /
        }
        try {
            Pstr = AesEncryptUtil.desEncrypt(Pstr);
            Parammap.putAll(JSON.parseObject(Pstr));
            return MyRetunSuccess(yzCardServiceImpl.consumptionandstatearr(Parammap), null);
        } catch (Exception e) {
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:card:ConsumptionAndStateArr  <br/> Pstr = " + Pstr + " <br/> ip =  " + ip + " <br/> ", e.getCause().toString());
        }
        return Myerr("批量 【同步状态和用量】 异常！");
    }


}































