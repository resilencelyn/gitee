package com.orion.ops.handler.scheduler;

import com.orion.ops.consts.ExceptionHandlerType;
import com.orion.ops.consts.scheduler.SchedulerTaskMachineStatus;
import com.orion.ops.handler.scheduler.machine.ITaskMachineHandler;

import java.util.Collection;

/**
 * 串行任务处理器
 *
 * @author Jiahang Li
 * @version 1.0.0
 * @since 2022/2/24 22:58
 */
public class SerialTaskProcessor extends AbstractTaskProcessor {

    public SerialTaskProcessor(Long recordId) {
        super(recordId);
    }

    @Override
    protected void handler() throws Exception {
        // 异常处理策略
        final boolean errorSkipAll = ExceptionHandlerType.SKIP_ALL.getType().equals(task.getExceptionHandler());
        // 串行执行
        Exception ex = null;
        Collection<ITaskMachineHandler> handlers = this.handlers.values();
        for (ITaskMachineHandler handler : handlers) {
            // 停止跳过
            if (terminated) {
                handler.skipped();
                continue;
            }
            // 发生异常并且异常处理策略是跳过所有则跳过
            if (ex != null && errorSkipAll) {
                handler.skipped();
                continue;
            }
            // 执行
            try {
                handler.run();
            } catch (Exception e) {
                ex = e;
            }
        }
        // 异常返回
        if (ex != null) {
            throw ex;
        }
        // 全部停止
        final boolean allTerminated = handlers.stream()
                .map(ITaskMachineHandler::getStatus)
                .filter(s -> !SchedulerTaskMachineStatus.SKIPPED.equals(s))
                .allMatch(SchedulerTaskMachineStatus.TERMINATED::equals);
        if (allTerminated) {
            this.terminated = true;
        }
    }

    @Override
    public void terminatedAll() {
        super.terminatedAll();
        // 获取当前执行中的机器执行器
        handlers.values().stream()
                .filter(s -> s.getStatus().equals(SchedulerTaskMachineStatus.RUNNABLE))
                .forEach(ITaskMachineHandler::terminated);
    }

}
