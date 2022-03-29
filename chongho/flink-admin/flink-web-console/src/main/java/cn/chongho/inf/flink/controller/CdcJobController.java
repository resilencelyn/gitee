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
import cn.chongho.inf.flink.request.CdcJobListRequest;
import cn.chongho.inf.flink.service.CdcJobService;
import cn.chongho.inf.flink.service.DbSourceService;
import cn.chongho.inf.flink.service.ClusterService;
import cn.chongho.inf.flink.utils.HttpSessionUtils;
import cn.chongho.inf.flink.utils.StringUtils;
import cn.chongho.inf.flink.model.*;
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
@RequestMapping("/cdcjob")
public class CdcJobController {

    @Autowired
    private CdcJobService cdcJobService;

    @Autowired
    private DbSourceService dbSourceService;

    @Autowired
    private ClusterService clusterService;

    @GetMapping("/list")
    public String list(CdcJobListRequest cdcJobListRequest, ModelMap map){
        int page = cdcJobListRequest.getPage();
        int pageSize = cdcJobListRequest.getPageSize();

        CdcJob cdcJob = new CdcJob();
        if(!StringUtils.isEmpty(cdcJobListRequest.getJobName())){
            cdcJob.setJobName(cdcJobListRequest.getJobName());
        }
        if(cdcJobListRequest.getJobType() != -1){
            cdcJob.setJobType(cdcJobListRequest.getJobType());
        }
        if(cdcJobListRequest.getTargetDbId()  != -1){
            cdcJob.setTargetDbId(cdcJobListRequest.getTargetDbId());
        }
        if(cdcJobListRequest.getStatus()  != -1){
            cdcJob.setStatus(cdcJobListRequest.getStatus());
        }

        if(cdcJobListRequest.getFlinkColonyId() != -1){
            cdcJob.setFlinkColonyId(cdcJobListRequest.getFlinkColonyId());
        }

        List<CdcJob> cdcJobList = cdcJobService.selectByPage(page,  pageSize ,cdcJob);

        int count = cdcJobService.selectCount(cdcJob);
        map.put("cdcJobList",cdcJobList);
        map.put("count",count);
        map.put("maxPage",count/10);
        map.put("page",page);
        map.put("pageSize",pageSize);
        map.put("nowBegin",pageSize * (page - 1 )+1);
        map.put("nowEnd",pageSize * (page - 1 ) + cdcJobList.size());

        DbSource dbSource = new DbSource();
        dbSource.setEnableFlag(Constant.EnableFlag.ENABLE.ordinal());
        map.put("dbSourceList", dbSourceService.getAllDbSource(dbSource));

        map.put("clusterList", clusterService.getAllCluster());

        map.put("cdcJobListRequest", cdcJobListRequest);
        return "cdcjob/index";
    }

    @GetMapping(value = "edit")
    public String edit(@RequestParam(defaultValue = "0") int id, ModelMap map){
        if(id != 0){
            map.putAll(cdcJobService.editShow(id));
        }else{
            List<SourceInfo> sourceDbList = new ArrayList<>();
            sourceDbList.add(new SourceInfo());
            map.put("sourceDbList", sourceDbList);
            map.put("targetTableList", new ArrayList<>());
            map.put("columnAssociationList", new ArrayList<>());
            map.put("checkPointList", new ArrayList<>());
            map.put("jobType", 1);
        }

        map.put("clusterList", clusterService.getAllCluster());

        DbSource dbSource = new DbSource();
        dbSource.setEnableFlag(Constant.EnableFlag.ENABLE.ordinal());
        map.put("dbSourceList", dbSourceService.getAllDbSource(dbSource));
        return "cdcjob/edit";
    }

    @RequestMapping("delete")
    @ResponseBody
    public WebResult del(int id , HttpSession httpSession){
        if(cdcJobService.delete(id , HttpSessionUtils.getLoginUser(httpSession).getId())){
            return WebResult.success();
        }
        return WebResult.unKnown();
    }


    @RequestMapping("update")
    @ResponseBody
    public WebResult update(CdcJob cdcJob ,HttpSession httpSession){
        cdcJob.setUpdateTime(new Date());
        cdcJob.setUpdateUserId(HttpSessionUtils.getLoginUser(httpSession).getId());
        if (cdcJobService.update(cdcJob)) {
            return WebResult.success();
        }
        return WebResult.unKnown();
    }

    @RequestMapping("run")
    @ResponseBody
    public WebResult run(Integer id ,HttpSession httpSession){
        if (cdcJobService.runJob(id ,HttpSessionUtils.getLoginUser(httpSession).getId())) {
            return WebResult.success();
        }
        return WebResult.unKnown();
    }

    @RequestMapping("savepoint")
    @ResponseBody
    public WebResult savepoint(Integer id ,HttpSession httpSession){
        if (cdcJobService.savepoint(id ,HttpSessionUtils.getLoginUser(httpSession).getId())) {
            return WebResult.success();
        }
        return WebResult.unKnown();
    }
}
