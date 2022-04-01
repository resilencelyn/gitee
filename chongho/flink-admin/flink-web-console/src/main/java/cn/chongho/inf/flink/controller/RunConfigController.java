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

import cn.chongho.inf.flink.constants.Constant;
import cn.chongho.inf.flink.model.Jar;
import cn.chongho.inf.flink.model.JobRunConfig;
import cn.chongho.inf.flink.model.WebResult;
import cn.chongho.inf.flink.service.JobRunConfigService;
import cn.chongho.inf.flink.service.JarService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.*;

import javax.annotation.Resource;
import javax.servlet.http.HttpSession;

/**
 * @author ming
 */
@Controller
@RequestMapping("/runConfig")
@Slf4j
public class RunConfigController {

    @Resource
    private JobRunConfigService jobRunConfigService;

    @Resource
    private JarService jarService;

    @GetMapping("/list")
    public String list(ModelMap map) {

        map.put("runConfigs", jobRunConfigService.selectAll());
        return "runconfig/index";
    }

    @GetMapping(value = "edit")
    public String edit(@RequestParam(defaultValue = "0") int id, ModelMap map){
        if(id != 0){
            map.put("runConfig", jobRunConfigService.selectJobById(id));
        }
        Jar jar = new Jar();
        jar.setEnableFlag(Constant.EnableFlag.ENABLE.ordinal());
        map.put("jarList", jarService.getAllJars(jar));
        return "runconfig/edit";
    }

    @RequestMapping("update")
    @ResponseBody
    public WebResult update(JobRunConfig jobRunConfig, HttpSession httpSession){
        jobRunConfigService.update(jobRunConfig);
        return WebResult.success();
    }
}
