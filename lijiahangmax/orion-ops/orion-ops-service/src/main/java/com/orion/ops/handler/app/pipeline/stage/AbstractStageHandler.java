package com.orion.ops.handler.app.pipeline.stage;

import com.orion.ops.consts.app.PipelineDetailStatus;
import com.orion.ops.consts.app.PipelineLogStatus;
import com.orion.ops.consts.app.StageType;
import com.orion.ops.consts.user.RoleType;
import com.orion.ops.consts.user.UserHolder;
import com.orion.ops.dao.ApplicationPipelineDetailRecordDAO;
import com.orion.ops.dao.ApplicationPipelineRecordLogDAO;
import com.orion.ops.dao.UserInfoDAO;
import com.orion.ops.entity.domain.ApplicationPipelineDetailRecordDO;
import com.orion.ops.entity.domain.ApplicationPipelineRecordDO;
import com.orion.ops.entity.domain.ApplicationPipelineRecordLogDO;
import com.orion.ops.entity.domain.UserInfoDO;
import com.orion.ops.entity.dto.UserDTO;
import com.orion.spring.SpringHolder;
import com.orion.utils.Exceptions;
import lombok.Getter;
import lombok.extern.slf4j.Slf4j;

import java.util.Date;
import java.util.Optional;

/**
 * 流水线阶段处理器 基类
 *
 * @author Jiahang Li
 * @version 1.0.0
 * @since 2022/4/15 17:03
 */
@Slf4j
public abstract class AbstractStageHandler implements IStageHandler {

    protected static ApplicationPipelineDetailRecordDAO applicationPipelineDetailRecordDAO = SpringHolder.getBean(ApplicationPipelineDetailRecordDAO.class);

    protected static ApplicationPipelineRecordLogDAO applicationPipelineRecordLogDAO = SpringHolder.getBean(ApplicationPipelineRecordLogDAO.class);

    protected static UserInfoDAO userInfoDAO = SpringHolder.getBean(UserInfoDAO.class);

    protected Long id;

    protected ApplicationPipelineRecordDO record;

    protected ApplicationPipelineDetailRecordDO detail;

    protected StageType stageType;

    @Getter
    protected volatile PipelineDetailStatus status;

    protected volatile boolean terminated;

    public AbstractStageHandler(ApplicationPipelineRecordDO record, ApplicationPipelineDetailRecordDO detail) {
        this.record = record;
        this.detail = detail;
        this.id = detail.getId();
        this.status = PipelineDetailStatus.WAIT;
    }

    @Override
    public void exec() {
        log.info("流水线阶段操作-开始执行 id: {}", id);
        // 状态检查
        if (!PipelineDetailStatus.WAIT.equals(status)) {
            return;
        }
        Exception ex = null;
        // 执行
        try {
            // 更新状态
            this.updateStatus(PipelineDetailStatus.RUNNABLE);
            // 执行操作
            this.execStageTask();
        } catch (Exception e) {
            ex = e;
        }
        // 回调
        try {
            if (terminated) {
                // 停止回调
                this.terminatedCallback();
            } else if (ex == null) {
                // 成功回调
                this.successCallback();
            } else {
                // 异常回调
                this.exceptionCallback(ex);
                throw Exceptions.runtime(ex.getMessage(), ex);
            }
        } finally {
            this.close();
        }
    }

    /**
     * 执行操作任务
     */
    protected abstract void execStageTask();

    /**
     * 停止回调
     */
    protected void terminatedCallback() {
        log.info("流水线阶段操作-终止回调 id: {}", id);
        // 修改状态
        this.updateStatus(PipelineDetailStatus.TERMINATED);
    }

    /**
     * 成功回调
     */
    protected void successCallback() {
        log.info("流水线阶段操作-成功回调 id: {}", id);
        // 修改状态
        this.updateStatus(PipelineDetailStatus.FINISH);
    }

    /**
     * 异常回调
     *
     * @param ex ex
     */
    protected void exceptionCallback(Exception ex) {
        log.error("流水线阶段操作-异常回调 id: {}", id, ex);
        // 修改状态
        this.updateStatus(PipelineDetailStatus.FAILURE);
    }

    /**
     * 更新状态
     *
     * @param status 状态
     */
    protected void updateStatus(PipelineDetailStatus status) {
        Date now = new Date();
        ApplicationPipelineDetailRecordDO update = new ApplicationPipelineDetailRecordDO();
        update.setId(id);
        update.setExecStatus(status.getStatus());
        update.setUpdateTime(now);
        switch (status) {
            case RUNNABLE:
                update.setExecStartTime(now);
                break;
            case FINISH:
            case FAILURE:
            case TERMINATED:
                update.setExecEndTime(now);
                break;
            default:
                break;
        }
        // 更新
        applicationPipelineDetailRecordDAO.updateById(update);
        // 插入日志
        String appName = detail.getAppName();
        switch (status) {
            case FINISH:
                this.addLog(PipelineLogStatus.SUCCESS, appName);
                break;
            case FAILURE:
                this.addLog(PipelineLogStatus.FAILURE, appName);
                break;
            case SKIPPED:
                this.addLog(PipelineLogStatus.SKIP, appName);
                break;
            case TERMINATED:
                this.addLog(PipelineLogStatus.TERMINATED, appName);
                break;
            default:
                break;
        }
    }

    @Override
    public void terminated() {
        log.info("流水线阶段操作-终止 id: {}", id);
        this.terminated = true;
    }

    @Override
    public void skipped() {
        log.info("流水线阶段操作-跳过 id: {}", id);
        if (PipelineDetailStatus.WAIT.equals(status)) {
            // 只能跳过等待中的任务
            this.updateStatus(PipelineDetailStatus.SKIPPED);
        }
    }

    /**
     * 添加日志
     *
     * @param logStatus logStatus
     * @param params    日志参数
     */
    protected void addLog(PipelineLogStatus logStatus, Object... params) {
        ApplicationPipelineRecordLogDO log = new ApplicationPipelineRecordLogDO();
        log.setRecordId(record.getId());
        log.setRecordDetailId(detail.getId());
        log.setLogStatus(logStatus.getStatus());
        log.setStageType(stageType.getType());
        log.setLogInfo(logStatus.format(stageType, params));
        applicationPipelineRecordLogDAO.insert(log);
    }

    /**
     * 设置执行人用户上下文
     */
    protected void setExecuteUserContext() {
        UserInfoDO userInfo = userInfoDAO.selectById(record.getExecUserId());
        UserDTO user = new UserDTO();
        user.setId(record.getExecUserId());
        user.setUsername(record.getExecUserName());
        Integer roleType = Optional.ofNullable(userInfo)
                .map(UserInfoDO::getRoleType)
                .orElseGet(() -> record.getExecUserId().equals(record.getAuditUserId())
                        ? RoleType.ADMINISTRATOR.getType()
                        : RoleType.DEVELOPER.getType());
        user.setRoleType(roleType);
        UserHolder.set(user);
    }

    /**
     * 释放资源
     */
    protected void close() {
        UserHolder.remove();
    }

}
