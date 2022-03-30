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

import cn.chongho.inf.flink.model.AdminUser;
import cn.chongho.inf.flink.model.UserDataAuthority;
import cn.chongho.inf.flink.model.WebResult;
import cn.chongho.inf.flink.service.*;
import cn.chongho.inf.flink.utils.HttpSessionUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.*;

import javax.servlet.http.HttpSession;
import java.util.List;
import java.util.function.Predicate;
import java.util.stream.Collectors;


/**
 * @author ming
 */
@Controller
@RequestMapping("/data-authority")
public class DataAuthorityController {

    @Autowired
    private DataAuthorityService dataAuthorityService;

    @GetMapping("/listUserDataAuthority")
    public String listUserDataAuthority(ModelMap map, @RequestParam("dataId") Integer dataId, @RequestParam("dataType") Integer dataType, HttpSession httpSession){
        AdminUser loginUser = HttpSessionUtils.getLoginUser(httpSession);
        List<UserDataAuthority> userDataAuthorities = dataAuthorityService.selectAllUser(dataId, dataType);
        userDataAuthorities = userDataAuthorities.stream().filter(new Predicate<UserDataAuthority>() {
            @Override
            public boolean test(UserDataAuthority userDataAuthority) {
                return !userDataAuthority.getId().equals(loginUser.getId());
            }
        }).collect(Collectors.toList());
        map.put("userDataAuthorities", userDataAuthorities);
        map.put("dataId", dataId);
        map.put("dataType", dataType);
        return "dataAuthority/authority";
    }

    @PostMapping("/save")
    @ResponseBody
    public WebResult save(@RequestParam("dataId") Integer dataId, @RequestParam("dataType") Integer dataType, @RequestParam("userIds") String userIds){
        if(dataAuthorityService.update(dataType, dataId, userIds)){
            return WebResult.success();
        }
        return WebResult.error("修改错误");
    }
}
