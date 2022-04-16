package com.orion.ops.handler.app.pipeline;

import com.orion.able.Executable;

/**
 * 流水线处理器
 *
 * @author Jiahang Li
 * @version 1.0.0
 * @since 2022/4/15 14:18
 */
public interface IPipelineProcessor extends Executable, Runnable {

    /**
     * 获取明细id
     *
     * @return recordId
     */
    Long getRecordId();

    /**
     * 停止执行
     */
    void terminated();

    /**
     * 停止执行详情
     *
     * @param id id
     */
    void terminatedDetail(Long id);

    /**
     * 跳过执行详情
     *
     * @param id id
     */
    void skippedDetail(Long id);

    /**
     * 获取流水线执行器
     *
     * @param id id
     * @return 流水线执行器
     */
    static IPipelineProcessor with(Long id) {
        return new PipelineProcessor(id);
    }

}
