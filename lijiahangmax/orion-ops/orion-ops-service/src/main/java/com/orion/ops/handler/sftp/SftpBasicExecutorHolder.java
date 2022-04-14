package com.orion.ops.handler.sftp;

import com.orion.ops.consts.Const;
import com.orion.ops.consts.event.EventKeys;
import com.orion.ops.consts.event.EventParamsHolder;
import com.orion.ops.service.api.MachineEnvService;
import com.orion.ops.service.api.MachineInfoService;
import com.orion.ops.service.api.SftpService;
import com.orion.remote.channel.SessionStore;
import com.orion.remote.channel.sftp.SftpExecutor;
import com.orion.utils.collect.Maps;
import lombok.extern.slf4j.Slf4j;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Component;

import javax.annotation.Resource;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

/**
 * sftp基本操作执行holder
 *
 * @author Jiahang Li
 * @version 1.0.0
 * @since 2021/11/9 22:30
 */
@Slf4j
@Component
public class SftpBasicExecutorHolder {

    @Resource
    private MachineInfoService machineInfoService;

    @Resource
    private MachineEnvService machineEnvService;

    @Resource
    private SftpService sftpService;

    /**
     * 基本操作的executor 不包含(upload, download)
     * machineId: executor
     */
    private final Map<Long, SftpExecutor> basicExecutorHolder = Maps.newCurrentHashMap();

    /**
     * 基本操作executor 最后使用时间
     */
    private final Map<Long, Long> executorUsing = Maps.newCurrentHashMap();

    /**
     * 获取 sftp 基本操作 executor
     *
     * @param sessionToken sessionToken
     * @return SftpExecutor
     */
    public SftpExecutor getBasicExecutor(String sessionToken) {
        // 获取executor
        Long machineId = sftpService.getMachineId(sessionToken);
        EventParamsHolder.addParam(EventKeys.MACHINE_ID, machineId);
        return this.getBasicExecutor(machineId);
    }

    /**
     * 获取 sftp 基本操作 executor
     *
     * @param machineId machineId
     * @return SftpExecutor
     */
    public SftpExecutor getBasicExecutor(Long machineId) {
        SftpExecutor executor = basicExecutorHolder.get(machineId);
        if (executor != null) {
            if (!executor.isConnected()) {
                try {
                    executor.connect();
                } catch (Exception e) {
                    // 无法连接则重新创建实例
                    executor = null;
                }
            }
        }
        // 如果没有重新建立连接
        if (executor == null) {
            // 获取charset
            String charset = machineEnvService.getSftpCharset(machineId);
            // 打开sftp连接
            SessionStore sessionStore = machineInfoService.openSessionStore(machineId);
            executor = sessionStore.getSftpExecutor(charset);
            executor.connect();
            basicExecutorHolder.put(machineId, executor);
        }
        executorUsing.put(machineId, System.currentTimeMillis());
        return executor;
    }

    /**
     * 无效化一段时间(5分钟)未使用的执行器
     */
    @Scheduled(cron = "0 */1 * * * ?")
    private void invalidationUnusedExecutor() {
        long curr = System.currentTimeMillis();
        // 查询需要淘汰的executor的key
        List<Long> expireKeys = basicExecutorHolder.keySet().stream()
                .filter(key -> curr > executorUsing.get(key) + Const.MS_S_60 * 5)
                .collect(Collectors.toList());
        // 移除
        expireKeys.forEach(key -> {
            SftpExecutor sftpExecutor = basicExecutorHolder.get(key);
            if (sftpExecutor == null) {
                return;
            }
            log.info("淘汰sftp执行器: {}", key);
            basicExecutorHolder.remove(key);
            sftpExecutor.disconnect();
        });
    }

}
