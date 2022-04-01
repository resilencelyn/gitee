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
import cn.chongho.inf.flink.model.DbSource;
import cn.chongho.inf.flink.model.Jar;
import cn.chongho.inf.flink.model.Job;
import cn.chongho.inf.flink.model.WebResult;
import cn.chongho.inf.flink.request.JobListRequest;
import cn.chongho.inf.flink.utils.HttpSessionUtils;
import cn.chongho.inf.flink.utils.StringUtils;
import cn.chongho.inf.flink.service.*;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;

import javax.servlet.http.HttpSession;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

/**
 * @author ming
 */
@Controller
@RequestMapping("/job")
public class JobController {

    @Autowired
    private JobService jobService;

    @Autowired
    private JarService jarService;

    @Autowired
    private DbSourceService dbSourceService;

    @Autowired
    private CheckPointInfoService checkPointInfoService;

    @Autowired
    private ClusterService clusterService;

    @GetMapping("/list")
    public String list(JobListRequest jobListRequest, ModelMap map, HttpSession httpSession){
        Job job = new Job();
        if(!StringUtils.isEmpty(jobListRequest.getJobName())){
            job.setJobName(jobListRequest.getJobName());
        }
        if(!StringUtils.isEmpty(jobListRequest.getEntryClass())){
            job.setEntryClass(jobListRequest.getEntryClass());
        }
        if(jobListRequest.getStatus() != -1){
            job.setStatus(jobListRequest.getStatus());
        }
        if(jobListRequest.getJarId() != -1){
            job.setJarId(jobListRequest.getJarId());
        }
        if(jobListRequest.getFlinkColonyId() != -1){
            job.setFlinkColonyId(jobListRequest.getFlinkColonyId());
        }
        job.setJobType(Constant.JobType.JAR_JOB.getValue());
        Integer loginUserId = HttpSessionUtils.getLoginUser(httpSession).getId();
        int page = jobListRequest.getPage();
        int pageSize = jobListRequest.getPageSize();
        List<Job> jobs = jobService.selectByPage(page, pageSize, job, loginUserId);

        int count = jobService.selectCount(job);
        map.put("jobs",jobs);
        map.put("count",count);
        map.put("maxPage",count/10);
        map.put("page",page);
        map.put("pageSize",pageSize);
        map.put("nowBegin",pageSize * (page - 1 )+1);
        map.put("nowEnd",pageSize * (page - 1 )+jobs.size());

        Jar jar = new Jar();
        jar.setEnableFlag(Constant.EnableFlag.ENABLE.ordinal());
        map.put("jarList", jarService.getAllJars(jar));

        map.put("clusterList", clusterService.getAllCluster());

        map.put("jobListRequest",jobListRequest);

        return "job/index";
    }

    @GetMapping(value = "edit")
    public String edit(@RequestParam(defaultValue = "0") int id, ModelMap map){
        if(id != 0){
            map.put("job", jobService.selectById(id));
            map.put("checkPointList",checkPointInfoService.getCheckPointInfoByJobId(id, Constant.CheckJobForm.JOB.getValue()));
        }else{
            map.put("checkPointList",new ArrayList<>());
        }

        DbSource dbSource = new DbSource();
        dbSource.setEnableFlag(Constant.EnableFlag.ENABLE.ordinal());
        map.put("dbSourceList", dbSourceService.getAllDbSource(dbSource));

        map.put("clusterList", clusterService.getAllCluster());

        Jar jar = new Jar();
        jar.setEnableFlag(Constant.EnableFlag.ENABLE.ordinal());
        map.put("jarList", jarService.getAllJars(jar));
        return "job/edit";
    }

    @RequestMapping("delete")
    @ResponseBody
    public WebResult del(int id , HttpSession httpSession){
        if(jobService.delete(id , HttpSessionUtils.getLoginUser(httpSession).getId())){
            return WebResult.success();
        }
        return WebResult.unKnown();
    }


    @RequestMapping("update")
    @ResponseBody
    public WebResult update(Job job ,HttpSession httpSession){
        job.setUpdateTime(new Date());
        job.setUpdateUserId(HttpSessionUtils.getLoginUser(httpSession).getId());
        if (jobService.update(job)) {
            return WebResult.success();
        }
        return WebResult.unKnown();
    }

    @RequestMapping("run")
    @ResponseBody
    public WebResult run(Integer id ,HttpSession httpSession){
        if (jobService.runJob(id ,HttpSessionUtils.getLoginUser(httpSession).getId())) {
            return WebResult.success();
        }
        return WebResult.unKnown();
    }


    @RequestMapping("savepoint")
    @ResponseBody
    public WebResult savepoint(Integer id ,HttpSession httpSession){
        if (jobService.savepoint(id ,HttpSessionUtils.getLoginUser(httpSession).getId())) {
            return WebResult.success();
        }
        return WebResult.unKnown();
    }

    @RequestMapping("stop")
    @ResponseBody
    public WebResult stop(Integer id ,HttpSession httpSession){
        try {
            if (jobService.stopJob(id ,HttpSessionUtils.getLoginUser(httpSession).getId())) {
                return WebResult.success();
            }
        }catch (Exception e){
            return WebResult.error(e.getMessage());
        }
        return WebResult.unKnown();
    }
}
