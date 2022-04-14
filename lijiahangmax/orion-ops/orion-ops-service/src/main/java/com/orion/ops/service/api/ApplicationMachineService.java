package com.orion.ops.service.api;

import com.orion.ops.entity.domain.ApplicationMachineDO;
import com.orion.ops.entity.request.ApplicationConfigRequest;
import com.orion.ops.entity.vo.ApplicationMachineVO;

import java.util.List;

/**
 * 应用机器服务
 *
 * @author Jiahang Li
 * @version 1.0.0
 * @since 2021/7/9 18:24
 */
public interface ApplicationMachineService {

    /**
     * 获取应用环境的机器
     *
     * @param machineId machineId
     * @param appId     appId
     * @param profileId profileId
     * @return machineId
     */
    Long getAppProfileMachineId(Long machineId, Long appId, Long profileId);

    /**
     * 获取应用环境的机器id
     *
     * @param appId               appId
     * @param profileId           profileId
     * @param filterMachineStatus 是否过滤已禁用的机器状态
     * @return machineIdList
     */
    List<Long> getAppProfileMachineIdList(Long appId, Long profileId, boolean filterMachineStatus);

    /**
     * 获取应用环境的机器数量
     *
     * @param appId     appId
     * @param profileId profileId
     * @return count
     */
    Integer getAppProfileMachineCount(Long appId, Long profileId);

    /**
     * 获取应用环境的机器
     *
     * @param appId     appId
     * @param profileId profileId
     * @return machineList
     */
    List<ApplicationMachineVO> getAppProfileMachineDetail(Long appId, Long profileId);

    /**
     * 获取应用环境的机器
     *
     * @param appId     appId
     * @param profileId profileId
     * @return machineList
     */
    List<ApplicationMachineDO> getAppProfileMachineList(Long appId, Long profileId);

    /**
     * 通过机器 id 删除应用机器
     *
     * @param machineId machineId
     * @return effect
     */
    Integer deleteAppMachineByMachineId(Long machineId);

    /**
     * 通过 appId profileId 删除应用机器
     *
     * @param appId     appId
     * @param profileId profileId
     * @return effect
     */
    Integer deleteAppMachineByAppProfileId(Long appId, Long profileId);

    /**
     * 通过 appId profileId machineId 删除应用机器
     *
     * @param appId     appId
     * @param profileId profileId
     * @param machineId machineId
     * @return effect
     */
    Integer deleteAppMachineByAppProfileMachineId(Long appId, Long profileId, Long machineId);

    /**
     * 通过 id 删除
     *
     * @param id id
     * @return effect
     */
    Integer deleteById(Long id);

    /**
     * 通过 appId profileId 查询应用机器id列表
     *
     * @param appId     appId
     * @param profileId profileId
     * @return 机器id列表
     */
    List<Long> selectAppProfileMachineIdList(Long appId, Long profileId);

    /**
     * 配置app机器
     *
     * @param request request
     */
    void configAppMachines(ApplicationConfigRequest request);

    /**
     * 同步 app 机器
     *
     * @param appId         appId
     * @param profileId     profileId
     * @param syncProfileId 需要同步的profileId
     */
    void syncAppProfileMachine(Long appId, Long profileId, Long syncProfileId);

    /**
     * 复制 app 机器
     *
     * @param appId       appId
     * @param targetAppId targetAppId
     */
    void copyAppMachine(Long appId, Long targetAppId);

    /**
     * 更新 app releaseId
     *
     * @param appId         appId
     * @param profileId     profileId
     * @param releaseId     releaseId
     * @param machineIdList machineIdList
     */
    void updateAppMachineReleaseId(Long appId, Long profileId, Long releaseId, List<Long> machineIdList);

}
