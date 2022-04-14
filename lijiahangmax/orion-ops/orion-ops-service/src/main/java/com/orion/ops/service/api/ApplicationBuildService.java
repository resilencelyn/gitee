package com.orion.ops.service.api;

import com.orion.lang.wrapper.DataGrid;
import com.orion.ops.entity.domain.ApplicationBuildDO;
import com.orion.ops.entity.request.ApplicationBuildRequest;
import com.orion.ops.entity.vo.ApplicationBuildReleaseListVO;
import com.orion.ops.entity.vo.ApplicationBuildStatusVO;
import com.orion.ops.entity.vo.ApplicationBuildVO;

import java.util.List;

/**
 * 应用构建服务
 *
 * @author Jiahang Li
 * @version 1.0.0
 * @since 2021/11/26 23:23
 */
public interface ApplicationBuildService {

    /**
     * 提交执行
     *
     * @param request request
     * @return id
     */
    Long submitBuildTask(ApplicationBuildRequest request);

    /**
     * 获取构建列表
     *
     * @param request request
     * @return rows
     */
    DataGrid<ApplicationBuildVO> getBuildList(ApplicationBuildRequest request);

    /**
     * 获取构建详情
     *
     * @param id id
     * @return row
     */
    ApplicationBuildVO getBuildDetail(Long id);

    /**
     * 查询构建状态
     *
     * @param id id
     * @return status
     */
    ApplicationBuildStatusVO getBuildStatus(Long id);

    /**
     * 查询构建状态列表
     *
     * @param buildIdList id
     * @return key: id  value: status
     */
    List<ApplicationBuildStatusVO> getBuildStatusList(List<Long> buildIdList);

    /**
     * 停止构建
     *
     * @param id id
     */
    void terminatedBuildTask(Long id);

    /**
     * 删除构建
     *
     * @param idList idList
     * @return effect
     */
    Integer deleteBuildTask(List<Long> idList);

    /**
     * 重新构建
     *
     * @param id id
     * @return id
     */
    Long rebuild(Long id);

    /**
     * 通过id查询
     *
     * @param id id
     * @return row
     */
    ApplicationBuildDO selectById(Long id);

    /**
     * 获取构建日志路径
     *
     * @param id id
     * @return path
     */
    String getBuildLogPath(Long id);

    /**
     * 获取构建产物路径
     *
     * @param id id
     * @return path
     */
    String getBuildBundlePath(Long id);

    /**
     * 检查并且获取构建目录
     *
     * @param build build
     * @return 构建产物路径
     */
    String checkBuildBundlePath(ApplicationBuildDO build);

    /**
     * 获取构建发布序列
     *
     * @param appId     appId
     * @param profileId profileId
     * @return rows
     */
    List<ApplicationBuildReleaseListVO> getBuildReleaseList(Long appId, Long profileId);

}
