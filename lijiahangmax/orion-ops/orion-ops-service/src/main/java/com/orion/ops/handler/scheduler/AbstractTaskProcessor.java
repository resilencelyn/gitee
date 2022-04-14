package com.orion.ops.handler.scheduler;

import com.orion.ops.consts.scheduler.SchedulerTaskStatus;
import com.orion.ops.dao.SchedulerTaskDAO;
import com.orion.ops.dao.SchedulerTaskRecordDAO;
import com.orion.ops.entity.domain.SchedulerTaskDO;
import com.orion.ops.entity.domain.SchedulerTaskMachineRecordDO;
import com.orion.ops.entity.domain.SchedulerTaskRecordDO;
import com.orion.ops.handler.scheduler.machine.ITaskMachineHandler;
import com.orion.ops.handler.scheduler.machine.TaskMachineHandler;
import com.orion.ops.service.api.SchedulerTaskMachineRecordService;
import com.orion.spring.SpringHolder;
import com.orion.utils.collect.Maps;
import lombok.extern.slf4j.Slf4j;

import java.util.Date;
import java.util.List;
import java.util.Map;

/**
 * 任务处理器-抽象类
 *
 * @author Jiahang Li
 * @version 1.0.0
 * @since 2022/2/24 21:10
 */
@Slf4j
public abstract class AbstractTaskProcessor implements ITaskProcessor {

    protected static SchedulerTaskDAO schedulerTaskDAO = SpringHolder.getBean(SchedulerTaskDAO.class);

    protected static SchedulerTaskRecordDAO schedulerTaskRecordDAO = SpringHolder.getBean(SchedulerTaskRecordDAO.class);

    protected static SchedulerTaskMachineRecordService schedulerTaskMachineRecordService = SpringHolder.getBean(SchedulerTaskMachineRecordService.class);

    protected static TaskSessionHolder taskSessionHolder = SpringHolder.getBean(TaskSessionHolder.class);

    protected Long recordId;

    protected SchedulerTaskDO task;

    protected SchedulerTaskRecordDO record;

    protected Map<Long, ITaskMachineHandler> handlers;

    protected volatile boolean terminated;

    public AbstractTaskProcessor(Long recordId) {
        this.recordId = recordId;
        this.handlers = Maps.newLinkedMap();
    }

    @Override
    public void run() {
        log.info("开始执行调度任务-recordId: {}", recordId);
        // 初始化
        try {
            // 填充数据
            this.getTaskData();
            // 判断状态
            if (record == null || !SchedulerTaskStatus.WAIT.getStatus().equals(record.getTaskStatus())) {
                return;
            }
            // 修改状态
            this.updateStatus(SchedulerTaskStatus.RUNNABLE);
        } catch (Exception e) {
            log.error("执行调度任务初始化失败-recordId: {}", recordId, e);
            this.updateStatus(SchedulerTaskStatus.FAILURE);
            return;
        }
        // 执行
        Exception ex = null;
        try {
            // 添加会话
            taskSessionHolder.addSession(recordId, this);
            // 处理
            this.handler();
        } catch (Exception e) {
            // 执行失败
            ex = e;
        }
        // 回调
        try {
            if (terminated) {
                // 停止回调
                this.updateStatus(SchedulerTaskStatus.TERMINATED);
                log.info("执行调度任务执行停止-recordId: {}", recordId);
            } else if (ex == null) {
                // 完成回调
                this.updateStatus(SchedulerTaskStatus.SUCCESS);
                log.info("执行调度任务执行成功-recordId: {}", recordId);
            } else {
                // 异常回调
                this.updateStatus(SchedulerTaskStatus.FAILURE);
                log.error("执行调度任务执行失败-recordId: {}", recordId, ex);
            }
        } finally {
            // 释放资源
            this.close();
        }
    }

    /**
     * 处理
     *
     * @throws Exception 处理异常
     */
    protected abstract void handler() throws Exception;

    /**
     * 填充数据
     */
    private void getTaskData() {
        // 查询明细
        this.record = schedulerTaskRecordDAO.selectById(recordId);
        if (record == null || !SchedulerTaskStatus.WAIT.getStatus().equals(record.getTaskStatus())) {
            return;
        }
        // 查询任务
        this.task = schedulerTaskDAO.selectById(record.getTaskId());
        // 查询机器明细
        List<SchedulerTaskMachineRecordDO> machineRecords = schedulerTaskMachineRecordService.selectByRecordId(recordId);
        for (SchedulerTaskMachineRecordDO machineRecord : machineRecords) {
            handlers.put(machineRecord.getId(), new TaskMachineHandler(machineRecord.getId()));
        }
    }

    /**
     * 更新状态
     *
     * @param status status
     */
    protected void updateStatus(SchedulerTaskStatus status) {
        Date now = new Date();
        // 更新任务
        SchedulerTaskDO updateTask = new SchedulerTaskDO();
        updateTask.setId(task.getId());
        updateTask.setUpdateTime(now);
        updateTask.setLatelyStatus(status.getStatus());
        // 更新明细
        SchedulerTaskRecordDO updateRecord = new SchedulerTaskRecordDO();
        updateRecord.setId(recordId);
        updateRecord.setUpdateTime(now);
        updateRecord.setTaskStatus(status.getStatus());
        switch (status) {
            case RUNNABLE:
                updateRecord.setStartTime(now);
                break;
            case SUCCESS:
            case FAILURE:
            case TERMINATED:
            default:
                updateRecord.setEndTime(now);
                break;
        }
        schedulerTaskDAO.updateById(updateTask);
        schedulerTaskRecordDAO.updateById(updateRecord);
    }

    @Override
    public void terminatedAll() {
        this.terminated = true;
    }

    @Override
    public void terminatedMachine(Long recordMachineId) {
        ITaskMachineHandler machineHandler = handlers.get(recordMachineId);
        if (machineHandler != null) {
            machineHandler.terminated();
        }
    }

    @Override
    public void skipMachine(Long recordMachineId) {
        ITaskMachineHandler machineHandler = handlers.get(recordMachineId);
        if (machineHandler != null) {
            machineHandler.skipped();
        }
    }

    @Override
    public void close() {
        // 移除会话
        taskSessionHolder.removeSession(recordId);
    }

}
