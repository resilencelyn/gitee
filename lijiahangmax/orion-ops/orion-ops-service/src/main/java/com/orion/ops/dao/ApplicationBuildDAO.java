package com.orion.ops.dao;

import com.baomidou.mybatisplus.core.mapper.BaseMapper;
import com.orion.ops.entity.domain.ApplicationBuildDO;
import com.orion.ops.entity.dto.ApplicationBuildStatisticsDTO;
import org.apache.ibatis.annotations.Param;

import java.util.Date;
import java.util.List;

/**
 * <p>
 * 应用构建 Mapper 接口
 * </p>
 *
 * @author Jiahang Li
 * @since 2021-11-26
 */
public interface ApplicationBuildDAO extends BaseMapper<ApplicationBuildDO> {

    /**
     * 通过 id 查询状态
     *
     * @param id id
     * @return status
     */
    Integer selectStatusById(@Param("id") Long id);

    /**
     * 通过 id 查询状态信息
     *
     * @param id id
     * @return row
     */
    ApplicationBuildDO selectStatusInfoById(@Param("id") Long id);

    /**
     * 通过 id 查询状态信息
     *
     * @param idList idList
     * @return rows
     */
    List<ApplicationBuildDO> selectStatusInfoByIdList(@Param("idList") List<Long> idList);

    /**
     * 查询上一次构建分支
     *
     * @param appId     appId
     * @param profileId profileId
     * @param vcsId     vcsId
     * @return branch
     */
    String selectLastBuildBranch(@Param("appId") Long appId, @Param("profileId") Long profileId, @Param("vcsId") Long vcsId);

    /**
     * 查询构建序列
     *
     * @param id id
     * @return seq
     */
    Integer selectBuildSeq(@Param("id") Long id);

    /**
     * 查询构建发布列表
     *
     * @param appId     appId
     * @param profileId profileId
     * @param limit     limit
     * @return rows
     */
    List<ApplicationBuildDO> selectBuildReleaseList(@Param("appId") Long appId, @Param("profileId") Long profileId, @Param("limit") Integer limit);

    /**
     * 查询构建发布列表
     *
     * @param appIdList appIdList
     * @param profileId profileId
     * @return rows
     */
    List<ApplicationBuildDO> selectBuildReleaseGroupList(@Param("appIdList") List<Long> appIdList, @Param("profileId") Long profileId);

    /**
     * 获取构建统计
     *
     * @param appId          appId
     * @param profileId      profileId
     * @param rangeStartDate rangeStartDate
     * @return 统计信息
     */
    ApplicationBuildStatisticsDTO getBuildStatistics(@Param("appId") Long appId, @Param("profileId") Long profileId, @Param("rangeStartDate") Date rangeStartDate);

    /**
     * 获取构建时间线统计
     *
     * @param appId          appId
     * @param profileId      profileId
     * @param rangeStartDate rangeStartDate
     * @return 时间线统计信息
     */
    List<ApplicationBuildStatisticsDTO> getBuildDateStatistics(@Param("appId") Long appId, @Param("profileId") Long profileId, @Param("rangeStartDate") Date rangeStartDate);

}
