/**
 *    Copyright (c) [2022] [中和农信]
 *    [Software Name] is licensed under Mulan PSL v2.
 *    You can use this software according to the terms and conditions of the Mulan PSL v2.
 *    You may obtain a copy of Mulan PSL v2 at:
 *             http://license.coscl.org.cn/MulanPSL2
 *    THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *    See the Mulan PSL v2 for more details.
 *
 */
package cn.chongho.inf.flink.controller;

import cn.chongho.inf.flink.model.*;
import cn.chongho.inf.flink.service.*;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;

import javax.servlet.http.HttpSession;
import java.util.List;

/**
 * 后台管理主控制器
 *  * @author feihu.wang
 *  * @since 2022-02-14
 */
@Controller
@Slf4j
public class AdminController {

    @Autowired
    private MenuService menuService;
    @Autowired
    private AdminUserService adminUserService;

    @Autowired
    private ClusterService clusterService;

    /**
     * 进入系统管理首页
     * @param map
     * @return
     */
    @GetMapping(value={"/admin/index","/"})
    public String index(ModelMap map, HttpSession session){

        AdminUser user = (AdminUser) session.getAttribute("loginUser");
        List<Menu> menus = menuService.selectByUser(user.getId());
        map.put("treeMenu",menus);

        map.put("flinkList", clusterService.getColonyInfo());
//        map.put("flinkList", flinkList);
        return "index";
    }

    /**
     * 进入登录页面
     * @return
     */
    @GetMapping(value = "/admin/login")
    public String login(@RequestParam(defaultValue = "0") int type){
        if(type == 1){
            log.info("nologin");
            return "nologin";
        }
        return "login";
    }


    @PostMapping(value = "/admin/login")
    public String login(@RequestParam(defaultValue = "") String name, @RequestParam(defaultValue = "")  String pass,
                        ModelMap map, HttpSession session){
        AdminUser user = adminUserService.login(name,pass);
        if(user == null){
            map.put("error","用户名或密码错误");
            return "login";
        }
        session.setAttribute("loginUser",user);
        session.setAttribute("authorities",menuService.selectAuthorities(user.getId()));
        return "redirect:/admin/index";
    }

    /**
     * 注销登录
     * @param session
     * @return
     */
    @RequestMapping("/admin/loginout")
    public String loginOut(HttpSession session){
        session.setAttribute("loginUser",null);
        session.setAttribute("authorities",null);
        return "login";
    }
}
