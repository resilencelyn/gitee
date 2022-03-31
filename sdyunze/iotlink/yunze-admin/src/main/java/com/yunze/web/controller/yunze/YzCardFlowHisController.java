package com.yunze.web.controller.yunze;

import com.alibaba.fastjson.JSON;
import com.yunze.common.annotation.Log;
import com.yunze.common.core.controller.BaseController;
import com.yunze.common.core.domain.entity.SysUser;
import com.yunze.common.core.domain.model.LoginUser;
import com.yunze.common.enums.BusinessType;
import com.yunze.common.utils.ServletUtils;
import com.yunze.common.utils.ip.IpUtils;
import com.yunze.common.utils.spring.SpringUtils;
import com.yunze.common.utils.yunze.AesEncryptUtil;
import com.yunze.framework.web.service.TokenService;
import com.yunze.system.service.yunze.YzCardFlowHisService;
import com.yunze.web.core.config.MyBaseController;
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
import java.util.Map;

/**
 * 用量详情
 * 2021-8-25
 */
@Api("用量详情")
@RestController
@RequestMapping("/yunze/flowhis")
public class YzCardFlowHisController extends MyBaseController {
    @Autowired
    private YzCardFlowHisService yrootlowHisService;

    /**
     * 执行用量详情
     */
    @PreAuthorize("@ss.hasPermi('yunze:flowhis:list')")
    @PostMapping(value = "/list", produces = {"application/json;charset=UTF-8"})
    public String SynFlow(@RequestBody String Pstr) {
        HashMap<String, Object> Parammap = new HashMap<String, Object>();
        if (Pstr != null) {
            Pstr = Pstr.replace("%2F", "/");//转义 /
        }
        try {
            Pstr = AesEncryptUtil.desEncrypt(Pstr);
            //  System.out.println(map);
            Parammap.putAll(JSON.parseObject((String) Pstr));
            LoginUser loginUser = SpringUtils.getBean(TokenService.class).getLoginUser(ServletUtils.getRequest());
            SysUser currentUser = loginUser.getUser();
            List<Integer> agent_id = new ArrayList<>();
            if (currentUser.getDeptId() != 100) {
                if (Parammap.get("agent_id") != null) {
                    List<Integer> P_agent_id = (List<Integer>) Parammap.get("agent_id");
                    agent_id.addAll(P_agent_id);
                } else {
                    agent_id.add(Integer.parseInt(currentUser.getDeptId().toString()));
                    Parammap.put("agent_id", agent_id);
                }
            }
            return MyRetunSuccess(yrootlowHisService.ListHis(Parammap), null);
        } catch (Exception e) {
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> system:flowhis:list  <br/> Pstr = " + Pstr + " <br/> ip =  " + ip + " <br/> ", e.getCause().toString());
        }
        return Myerr("获取卡板信息列表 操作失败！");
    }

    /**
     * 根据id查询
     */
    @PreAuthorize("@ss.hasPermi('/yunze/flowhis/find')")
    @PostMapping(value = "/find", produces = {"application/json;charset=UTF-8"})
    public String find(@RequestBody String Pstr) {
        HashMap<String, Object> Parammap = new HashMap<String, Object>();
        if (Pstr != null) {
            Pstr = Pstr.replace("%2F", "/");//转义 /
        }
        try {
            Pstr = AesEncryptUtil.desEncrypt(Pstr);
            Parammap.putAll(JSON.parseObject((String) Pstr));
            return MyRetunSuccess(yrootlowHisService.getById(Parammap), null);
        } catch (Exception e) {
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:flowhis:D:\\YUNZE\\yun-ze-iot\\YunZeIot\\yunze-admin\\src\\main\\java\\com\\yunze\\web\\controller\\yunze\\YrootlowHisController.java  <br/>   ip =  " + ip + " <br/> ", e.getCause().toString());
        }
        return Myerr("查询 【订单详情】  操作失败！");
    }


    /**
     * 用量详情导出 （同步导出 分组 、备注、发货时间）
     */
    @Log(title = "用量详情导出", businessType = BusinessType.EXPORT)
    @PreAuthorize("@ss.hasPermi('yunze:flowhis:exportflowhis')")
    @PostMapping(value = "/exportflowhis", produces = {"application/json;charset=utf-8"})
    public String exportFlowHis(@RequestBody String Pstr) {
        HashMap<String, Object> Parammap = new HashMap<String, Object>();
        if (Pstr != null) {
            Pstr = Pstr.replace("%2F", "/");//转义 /
        }
        try {
            Pstr = AesEncryptUtil.desEncrypt(Pstr);
            //  System.out.println(map);
            Parammap.putAll(JSON.parseObject((String) Pstr));

            LoginUser loginUser = SpringUtils.getBean(TokenService.class).getLoginUser(ServletUtils.getRequest());
            SysUser currentUser = loginUser.getUser();
            String dept_id = currentUser.getDeptId().toString();
            Parammap.put("dept_id",dept_id );
            return MyRetunSuccess(yrootlowHisService.FlowHis(Parammap,currentUser), null);


        } catch (Exception e) {
            String ip = IpUtils.getIpAddr(ServletUtils.getRequest());
            logger.error("<br/> yunze:flowhis:exportflowhis  <br/> Pstr = " + Pstr + " <br/> ip =  " + ip + " <br/> ", e.getCause().toString());
        }
        return Myerr("用量详情导出 操作失败！");
    }


}
