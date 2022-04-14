package com.orion.ops.runner;

import com.baomidou.mybatisplus.core.conditions.Wrapper;
import com.baomidou.mybatisplus.core.conditions.query.LambdaQueryWrapper;
import com.orion.ops.consts.app.ActionStatus;
import com.orion.ops.consts.app.ReleaseStatus;
import com.orion.ops.consts.app.StageType;
import com.orion.ops.dao.ApplicationReleaseDAO;
import com.orion.ops.dao.ApplicationReleaseMachineDAO;
import com.orion.ops.entity.domain.ApplicationReleaseDO;
import com.orion.ops.entity.domain.ApplicationReleaseMachineDO;
import com.orion.ops.service.api.ApplicationActionLogService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.boot.CommandLineRunner;
import org.springframework.core.annotation.Order;
import org.springframework.stereotype.Component;

import javax.annotation.Resource;
import java.util.Date;
import java.util.List;

/**
 * 重置应用发布状态
 *
 * @author Jiahang Li
 * @version 1.0.0
 * @since 2021/7/29 17:49
 */
@Component
@Order(2400)
@Slf4j
public class CleanReleaseStatusRunner implements CommandLineRunner {

    @Resource
    private ApplicationReleaseDAO applicationReleaseDAO;

    @Resource
    private ApplicationReleaseMachineDAO applicationReleaseMachineDAO;

    @Resource
    private ApplicationActionLogService applicationActionLogService;

    @Override
    public void run(String... args) {
        log.info("重置应用发布状态-开始");
        Wrapper<ApplicationReleaseDO> wrapper = new LambdaQueryWrapper<ApplicationReleaseDO>()
                .eq(ApplicationReleaseDO::getReleaseStatus, ReleaseStatus.RUNNABLE.getStatus());
        List<ApplicationReleaseDO> releaseList = applicationReleaseDAO.selectList(wrapper);
        for (ApplicationReleaseDO release : releaseList) {
            Long releaseId = release.getId();
            ApplicationReleaseDO update = new ApplicationReleaseDO();
            update.setId(releaseId);
            update.setReleaseStatus(ReleaseStatus.TERMINATED.getStatus());
            update.setUpdateTime(new Date());
            applicationReleaseDAO.updateById(update);
            // 设置机器状态
            this.resetMachineStatus(releaseId);
            log.info("重置应用发布状态-执行 {}", releaseId);
        }

        log.info("重置应用发布状态-结束");
    }

    /**
     * 设置机器状态
     *
     * @param releaseId releaseId
     */
    private void resetMachineStatus(Long releaseId) {
        // 查询机器
        Wrapper<ApplicationReleaseMachineDO> wrapper = new LambdaQueryWrapper<ApplicationReleaseMachineDO>()
                .eq(ApplicationReleaseMachineDO::getReleaseId, releaseId);
        List<ApplicationReleaseMachineDO> machines = applicationReleaseMachineDAO.selectList(wrapper);
        // 修改状态
        for (ApplicationReleaseMachineDO machine : machines) {
            ApplicationReleaseMachineDO update = new ApplicationReleaseMachineDO();
            Long releaseMachineId = machine.getId();
            update.setId(releaseMachineId);
            update.setUpdateTime(new Date());
            switch (ActionStatus.of(machine.getRunStatus())) {
                case WAIT:
                case RUNNABLE:
                    update.setRunStatus(ActionStatus.TERMINATED.getStatus());
                    break;
                default:
                    break;
            }
            applicationReleaseMachineDAO.updateById(update);
            // 设置操作状态
            applicationActionLogService.resetActionStatus(releaseMachineId, StageType.RELEASE);
        }
    }

}
