package com.larkmidtable.admin.controller;


import com.larkmidtable.admin.core.cron.CronExpression;
import com.larkmidtable.admin.core.thread.JobTriggerPoolHelper;
import com.larkmidtable.admin.core.util.I18nUtil;
import com.larkmidtable.admin.dto.FlinkXBatchJsonBuildDto;
import com.larkmidtable.admin.dto.TriggerJobDto;
import com.larkmidtable.admin.entity.JobInfo;
import com.larkmidtable.admin.service.JobService;
import com.larkmidtable.core.biz.model.ReturnT;
import com.larkmidtable.core.util.DateUtil;
import io.swagger.annotations.Api;
import io.swagger.annotations.ApiOperation;
import org.springframework.web.bind.annotation.*;

import javax.annotation.Resource;
import javax.servlet.http.HttpServletRequest;
import java.io.IOException;
import java.text.ParseException;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Map;



@RestController
@RequestMapping("/api/job")
@Api(tags = "数据集成-任务配置接口")
public class JobInfoController extends BaseController{

    @Resource
    private JobService jobService;


    @GetMapping("/pageList")
    @ApiOperation("任务列表")
    public ReturnT<Map<String, Object>> pageList(@RequestParam(value = "current", required = false, defaultValue = "0") int current,
                                        @RequestParam(value = "size", required = false, defaultValue = "10") int size,
                                                 @RequestParam("jobGroup") int jobGroup, @RequestParam("triggerStatus") int triggerStatus,
                                                 @RequestParam("jobDesc") String jobDesc, @RequestParam("glueType") String glueType,
                                                 @RequestParam(value ="projectIds") Integer[] projectIds) {

        return new ReturnT<>(jobService.pageList((current-1)*size, size, jobGroup, triggerStatus, jobDesc, glueType, 0, projectIds));
    }

    @GetMapping("/list")
    @ApiOperation("全部任务列表")
    public ReturnT<List<JobInfo>> list(){
        return new ReturnT<>(jobService.list());
    }

    @PostMapping("/add")
    @ApiOperation("添加任务")
    public ReturnT<String> add(HttpServletRequest request, @RequestBody JobInfo jobInfo) {
        jobInfo.setUserId(getCurrentUserId(request));
        return jobService.add(jobInfo);
    }

    @PostMapping("/update")
    @ApiOperation("更新任务")
    public ReturnT<String> update(HttpServletRequest request,@RequestBody JobInfo jobInfo) {
        jobInfo.setUserId(getCurrentUserId(request));
        return jobService.update(jobInfo);
    }

    @PostMapping(value = "/remove/{id}")
    @ApiOperation("移除任务")
    public ReturnT<String> remove(@PathVariable(value = "id") int id) {
        return jobService.remove(id);
    }

    @RequestMapping(value = "/stop",method = RequestMethod.POST)
    @ApiOperation("停止任务")
    public ReturnT<String> pause(int id) {
        return jobService.stop(id);
    }

    @RequestMapping(value = "/start",method = RequestMethod.POST)
    @ApiOperation("开启任务")
    public ReturnT<String> start(int id) {
        return jobService.start(id);
    }

    @PostMapping(value = "/trigger")
    @ApiOperation("触发任务")
    public ReturnT<String> triggerJob(@RequestBody TriggerJobDto dto) {
        // force cover job param
		try {
			String executorParam=dto.getExecutorParam();
			if (executorParam == null) {
				executorParam = "";
			}
			JobTriggerPoolHelper jobTriggerPoolHelper =  new JobTriggerPoolHelper();
			jobTriggerPoolHelper.runJob(dto.getJobId());
		} catch (Exception e) {
			return ReturnT.FAIL;
		}
        return ReturnT.SUCCESS;
    }

    @GetMapping("/nextTriggerTime")
    @ApiOperation("获取近5次触发时间")
    public ReturnT<List<String>> nextTriggerTime(String cron) {
        List<String> result = new ArrayList<>();
        try {
            CronExpression cronExpression = new CronExpression(cron);
            Date lastTime = new Date();
            for (int i = 0; i < 5; i++) {
                lastTime = cronExpression.getNextValidTimeAfter(lastTime);
                if (lastTime != null) {
                    result.add(DateUtil.formatDateTime(lastTime));
                } else {
                    break;
                }
            }
        } catch (ParseException e) {
            return new ReturnT<>(ReturnT.FAIL_CODE, I18nUtil.getString("jobinfo_field_cron_invalid"));
        }
        return new ReturnT<>(result);
    }

    @PostMapping("/batchAdd")
    @ApiOperation("批量创建任务")
    public ReturnT<String> batchAdd(@RequestBody FlinkXBatchJsonBuildDto dto) throws IOException {
        if (dto.getTemplateId() ==0) {
            return new ReturnT<>(ReturnT.FAIL_CODE, (I18nUtil.getString("system_please_choose") + I18nUtil.getString("jobinfo_field_temp")));
        }
        return jobService.batchAdd(dto);
    }
}
