package com.orion.ops.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.LambdaQueryWrapper;
import com.orion.lang.collect.MutableLinkedHashMap;
import com.orion.lang.wrapper.DataGrid;
import com.orion.ops.consts.Const;
import com.orion.ops.consts.ExceptionHandlerType;
import com.orion.ops.consts.MessageConst;
import com.orion.ops.consts.SerialType;
import com.orion.ops.consts.app.ApplicationEnvAttr;
import com.orion.ops.consts.app.StageType;
import com.orion.ops.consts.app.TransferDirType;
import com.orion.ops.consts.env.EnvConst;
import com.orion.ops.consts.event.EventKeys;
import com.orion.ops.consts.event.EventParamsHolder;
import com.orion.ops.consts.history.HistoryOperator;
import com.orion.ops.consts.history.HistoryValueType;
import com.orion.ops.dao.ApplicationEnvDAO;
import com.orion.ops.dao.ApplicationInfoDAO;
import com.orion.ops.dao.ApplicationProfileDAO;
import com.orion.ops.entity.domain.ApplicationEnvDO;
import com.orion.ops.entity.domain.ApplicationInfoDO;
import com.orion.ops.entity.domain.ApplicationProfileDO;
import com.orion.ops.entity.request.ApplicationConfigEnvRequest;
import com.orion.ops.entity.request.ApplicationConfigRequest;
import com.orion.ops.entity.request.ApplicationEnvRequest;
import com.orion.ops.entity.vo.ApplicationEnvVO;
import com.orion.ops.service.api.ApplicationEnvService;
import com.orion.ops.service.api.HistoryValueService;
import com.orion.ops.utils.DataQuery;
import com.orion.ops.utils.Valid;
import com.orion.spring.SpringHolder;
import com.orion.utils.Strings;
import com.orion.utils.collect.Lists;
import com.orion.utils.collect.Maps;
import com.orion.utils.convert.Converts;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import javax.annotation.Resource;
import java.util.Date;
import java.util.List;
import java.util.Map;
import java.util.Optional;

/**
 * app环境 实现
 *
 * @author Jiahang Li
 * @version 1.0.0
 * @since 2021/7/4 11:23
 */
@Service("applicationEnvService")
public class ApplicationEnvServiceImpl implements ApplicationEnvService {

    @Resource
    private ApplicationInfoDAO applicationInfoDAO;

    @Resource
    private ApplicationProfileDAO applicationProfileDAO;

    @Resource
    private ApplicationEnvDAO applicationEnvDAO;

    @Resource
    private HistoryValueService historyValueService;

    @Override
    @Transactional(rollbackFor = Exception.class)
    public Long addAppEnv(ApplicationEnvRequest request) {
        // 数据检查
        Long appId = request.getAppId();
        Long profileId = request.getProfileId();
        String key = request.getKey();
        Valid.notNull(applicationInfoDAO.selectById(appId), MessageConst.APP_ABSENT);
        Valid.notNull(applicationProfileDAO.selectById(profileId), MessageConst.PROFILE_ABSENT);
        // 重复检查
        ApplicationEnvDO env = applicationEnvDAO.selectOneRel(appId, profileId, key);
        // 修改
        if (env != null) {
            SpringHolder.getBean(ApplicationEnvService.class).updateAppEnv(env, request);
            return env.getId();
        }
        // 新增
        ApplicationEnvDO insert = new ApplicationEnvDO();
        insert.setAppId(appId);
        insert.setProfileId(profileId);
        insert.setAttrKey(key);
        insert.setAttrValue(request.getValue());
        insert.setDescription(request.getDescription());
        applicationEnvDAO.insert(insert);
        // 插入历史值
        Long id = insert.getId();
        historyValueService.addHistory(id, HistoryValueType.APP_ENV, HistoryOperator.ADD, null, request.getValue());
        return id;
    }

    @Override
    @Transactional(rollbackFor = Exception.class)
    public Integer updateAppEnv(ApplicationEnvRequest request) {
        // 查询
        ApplicationEnvDO before = applicationEnvDAO.selectById(request.getId());
        Valid.notNull(before, MessageConst.ENV_ABSENT);
        return SpringHolder.getBean(ApplicationEnvService.class).updateAppEnv(before, request);
    }

    @Override
    @Transactional(rollbackFor = Exception.class)
    public Integer updateAppEnv(ApplicationEnvDO before, ApplicationEnvRequest request) {
        // 查询
        Long id = before.getId();
        String beforeValue = before.getAttrValue();
        String afterValue = request.getValue();
        if (Const.IS_DELETED.equals(before.getDeleted())) {
            // 设置新增历史值
            historyValueService.addHistory(id, HistoryValueType.APP_ENV, HistoryOperator.ADD, null, afterValue);
            // 恢复
            applicationEnvDAO.setDeleted(id, Const.NOT_DELETED);
        } else if (afterValue != null && !afterValue.equals(beforeValue)) {
            // 检查是否修改了值 增加历史值
            historyValueService.addHistory(id, HistoryValueType.APP_ENV, HistoryOperator.UPDATE, beforeValue, afterValue);
        }
        // 更新
        ApplicationEnvDO update = new ApplicationEnvDO();
        update.setId(id);
        update.setAttrValue(afterValue);
        update.setDescription(request.getDescription());
        update.setUpdateTime(new Date());
        return applicationEnvDAO.updateById(update);
    }

    @Override
    @Transactional(rollbackFor = Exception.class)
    public Integer deleteAppEnv(List<Long> idList) {
        int effect = 0;
        for (Long id : idList) {
            // 获取元数据
            ApplicationEnvDO env = applicationEnvDAO.selectById(id);
            Valid.notNull(env, MessageConst.ENV_ABSENT);
            String key = env.getAttrKey();
            Valid.isTrue(ApplicationEnvAttr.of(key) == null, "{} " + MessageConst.FORBID_DELETE, key);
            // 删除
            effect += applicationEnvDAO.deleteById(id);
            // 插入历史值
            historyValueService.addHistory(id, HistoryValueType.APP_ENV, HistoryOperator.DELETE, env.getAttrValue(), null);
        }
        // 设置日志参数
        EventParamsHolder.addParam(EventKeys.ID_LIST, idList);
        EventParamsHolder.addParam(EventKeys.COUNT, effect);
        return effect;
    }

    @Override
    @Transactional(rollbackFor = Exception.class)
    public void saveEnv(Long appId, Long profileId, Map<String, String> env) {
        ApplicationEnvService self = SpringHolder.getBean(ApplicationEnvService.class);
        // 倒排
        List<Map.Entry<String, String>> entries = Lists.newList(env.entrySet());
        for (int i = entries.size() - 1; i >= 0; i--) {
            // 更新
            Map.Entry<String, String> entry = entries.get(i);
            ApplicationEnvRequest request = new ApplicationEnvRequest();
            request.setAppId(appId);
            request.setProfileId(profileId);
            request.setKey(entry.getKey());
            request.setValue(entry.getValue());
            self.addAppEnv(request);
        }
    }

    @Override
    public DataGrid<ApplicationEnvVO> listAppEnv(ApplicationEnvRequest request) {
        LambdaQueryWrapper<ApplicationEnvDO> wrapper = new LambdaQueryWrapper<ApplicationEnvDO>()
                .like(Strings.isNotBlank(request.getKey()), ApplicationEnvDO::getAttrKey, request.getKey())
                .like(Strings.isNotBlank(request.getValue()), ApplicationEnvDO::getAttrValue, request.getValue())
                .like(Strings.isNotBlank(request.getDescription()), ApplicationEnvDO::getDescription, request.getDescription())
                .eq(ApplicationEnvDO::getAppId, request.getAppId())
                .eq(ApplicationEnvDO::getProfileId, request.getProfileId())
                .eq(ApplicationEnvDO::getSystemEnv, Const.NOT_SYSTEM)
                .orderByDesc(ApplicationEnvDO::getUpdateTime);
        return DataQuery.of(applicationEnvDAO)
                .page(request)
                .wrapper(wrapper)
                .dataGrid(ApplicationEnvVO.class);
    }

    @Override
    public ApplicationEnvVO getAppEnvDetail(Long id) {
        ApplicationEnvDO env = applicationEnvDAO.selectById(id);
        Valid.notNull(env, MessageConst.UNKNOWN_DATA);
        return Converts.to(env, ApplicationEnvVO.class);
    }

    @Override
    public String getAppEnvValue(Long appId, Long profileId, String key) {
        LambdaQueryWrapper<ApplicationEnvDO> wrapper = new LambdaQueryWrapper<ApplicationEnvDO>()
                .eq(ApplicationEnvDO::getAppId, appId)
                .eq(ApplicationEnvDO::getProfileId, profileId)
                .eq(ApplicationEnvDO::getAttrKey, key)
                .last(Const.LIMIT_1);
        return Optional.ofNullable(applicationEnvDAO.selectOne(wrapper))
                .map(ApplicationEnvDO::getAttrValue)
                .orElse(null);
    }

    @Override
    public MutableLinkedHashMap<String, String> getAppProfileEnv(Long appId, Long profileId) {
        MutableLinkedHashMap<String, String> env = Maps.newMutableLinkedMap();
        LambdaQueryWrapper<ApplicationEnvDO> wrapper = new LambdaQueryWrapper<ApplicationEnvDO>()
                .eq(ApplicationEnvDO::getAppId, appId)
                .eq(ApplicationEnvDO::getProfileId, profileId)
                .orderByAsc(ApplicationEnvDO::getId);
        applicationEnvDAO.selectList(wrapper).forEach(e -> env.put(e.getAttrKey(), e.getAttrValue()));
        return env;
    }

    @Override
    public MutableLinkedHashMap<String, String> getAppProfileFullEnv(Long appId, Long profileId) {
        // 查询应用环境
        ApplicationInfoDO app = Valid.notNull(applicationInfoDAO.selectById(appId), MessageConst.APP_ABSENT);
        ApplicationProfileDO profile = Valid.notNull(applicationProfileDAO.selectById(profileId), MessageConst.PROFILE_ABSENT);
        MutableLinkedHashMap<String, String> env = Maps.newMutableLinkedMap();
        env.put(EnvConst.APP_PREFIX + EnvConst.APP_ID, app.getId() + Const.EMPTY);
        env.put(EnvConst.APP_PREFIX + EnvConst.APP_NAME, app.getAppName());
        env.put(EnvConst.APP_PREFIX + EnvConst.APP_TAG, app.getAppTag());
        env.put(EnvConst.APP_PREFIX + EnvConst.PROFILE_ID, profile.getId() + Const.EMPTY);
        env.put(EnvConst.APP_PREFIX + EnvConst.PROFILE_NAME, profile.getProfileName());
        env.put(EnvConst.APP_PREFIX + EnvConst.PROFILE_TAG, profile.getProfileTag());
        // 插入应用环境变量
        Map<String, String> appProfileEnv = this.getAppProfileEnv(app.getId(), profile.getId());
        appProfileEnv.forEach((k, v) -> {
            env.put(EnvConst.APP_PREFIX + k, v);
        });
        return env;
    }

    @Override
    public Integer deleteAppProfileEnvByAppProfileId(Long appId, Long profileId) {
        LambdaQueryWrapper<ApplicationEnvDO> wrapper = new LambdaQueryWrapper<ApplicationEnvDO>()
                .eq(appId != null, ApplicationEnvDO::getAppId, appId)
                .eq(profileId != null, ApplicationEnvDO::getProfileId, profileId);
        return applicationEnvDAO.delete(wrapper);
    }

    @Override
    @Transactional(rollbackFor = Exception.class)
    public void configAppEnv(ApplicationConfigRequest request) {
        ApplicationEnvService self = SpringHolder.getBean(ApplicationEnvService.class);
        StageType stageType = StageType.of(request.getStageType());
        List<ApplicationEnvRequest> list = Lists.newList();
        Long appId = request.getAppId();
        Long profileId = request.getProfileId();
        ApplicationConfigEnvRequest requestEnv = request.getEnv();
        // 构建产物目录
        String bundlePath = requestEnv.getBundlePath();
        if (!Strings.isBlank(bundlePath)) {
            ApplicationEnvRequest bundlePathEnv = new ApplicationEnvRequest();
            bundlePathEnv.setKey(ApplicationEnvAttr.BUNDLE_PATH.getKey());
            bundlePathEnv.setValue(bundlePath);
            bundlePathEnv.setDescription(ApplicationEnvAttr.BUNDLE_PATH.getDescription());
            list.add(bundlePathEnv);
        }
        // 产物传输目录
        String transferPath = requestEnv.getTransferPath();
        if (!Strings.isBlank(transferPath)) {
            ApplicationEnvRequest transferPathEnv = new ApplicationEnvRequest();
            transferPathEnv.setKey(ApplicationEnvAttr.TRANSFER_PATH.getKey());
            transferPathEnv.setValue(transferPath);
            transferPathEnv.setDescription(ApplicationEnvAttr.TRANSFER_PATH.getDescription());
            list.add(transferPathEnv);
        }
        // 产物传输类型
        String transferDirType = requestEnv.getTransferDirType();
        if (!Strings.isBlank(transferDirType)) {
            ApplicationEnvRequest transferDirTypeEnv = new ApplicationEnvRequest();
            transferDirTypeEnv.setKey(ApplicationEnvAttr.TRANSFER_DIR_TYPE.getKey());
            transferDirTypeEnv.setValue(TransferDirType.of(transferDirType).getValue());
            transferDirTypeEnv.setDescription(ApplicationEnvAttr.TRANSFER_DIR_TYPE.getDescription());
            list.add(transferDirTypeEnv);
        }
        // 发布序列
        Integer releaseSerial = requestEnv.getReleaseSerial();
        if (releaseSerial != null) {
            ApplicationEnvRequest releaseSerialEnv = new ApplicationEnvRequest();
            releaseSerialEnv.setKey(ApplicationEnvAttr.RELEASE_SERIAL.getKey());
            releaseSerialEnv.setValue(SerialType.of(releaseSerial).getValue());
            releaseSerialEnv.setDescription(ApplicationEnvAttr.RELEASE_SERIAL.getDescription());
            list.add(releaseSerialEnv);
        }
        // 异常处理
        Integer exceptionHandler = requestEnv.getExceptionHandler();
        if (exceptionHandler != null) {
            ApplicationEnvRequest exceptionHandlerEnv = new ApplicationEnvRequest();
            exceptionHandlerEnv.setKey(ApplicationEnvAttr.EXCEPTION_HANDLER.getKey());
            exceptionHandlerEnv.setValue(ExceptionHandlerType.of(exceptionHandler).getValue());
            exceptionHandlerEnv.setDescription(ApplicationEnvAttr.EXCEPTION_HANDLER.getDescription());
            list.add(exceptionHandlerEnv);
        }
        // 构建检查是否有构建序列
        if (StageType.BUILD.equals(stageType)) {
            self.checkInitSystemEnv(appId, profileId);
        }
        // 添加环境变量
        for (ApplicationEnvRequest env : list) {
            env.setAppId(appId);
            env.setProfileId(profileId);
            self.addAppEnv(env);
        }
    }

    @Override
    @Transactional(rollbackFor = Exception.class)
    public void syncAppEnv(Long id, Long appId, Long profileId, List<Long> targetProfileIdList) {
        ApplicationEnvService self = SpringHolder.getBean(ApplicationEnvService.class);
        List<ApplicationEnvDO> envList;
        // 查询数据
        if (Const.N_N_L_1.equals(id)) {
            // 全量同步
            LambdaQueryWrapper<ApplicationEnvDO> wrapper = new LambdaQueryWrapper<ApplicationEnvDO>()
                    .eq(ApplicationEnvDO::getAppId, appId)
                    .eq(ApplicationEnvDO::getProfileId, profileId)
                    .eq(ApplicationEnvDO::getSystemEnv, Const.NOT_SYSTEM)
                    .orderByAsc(ApplicationEnvDO::getUpdateTime);
            envList = applicationEnvDAO.selectList(wrapper);
        } else {
            // 查询数据
            ApplicationEnvDO env = applicationEnvDAO.selectById(id);
            Valid.notNull(env, MessageConst.UNKNOWN_DATA);
            envList = Lists.singleton(env);
        }
        // 同步数据
        for (Long targetProfileId : targetProfileIdList) {
            // 同步环境变量
            for (ApplicationEnvDO syncEnv : envList) {
                ApplicationEnvRequest request = new ApplicationEnvRequest();
                request.setAppId(syncEnv.getAppId());
                request.setProfileId(targetProfileId);
                request.setKey(syncEnv.getAttrKey());
                request.setValue(syncEnv.getAttrValue());
                request.setDescription(syncEnv.getDescription());
                self.addAppEnv(request);
            }
            // 初始化系统变量
            self.checkInitSystemEnv(appId, targetProfileId);
        }
        // 设置日志参数
        EventParamsHolder.addParam(EventKeys.ID, id);
        EventParamsHolder.addParam(EventKeys.APP_ID, appId);
        EventParamsHolder.addParam(EventKeys.PROFILE_ID, profileId);
        EventParamsHolder.addParam(EventKeys.ID_LIST, targetProfileIdList);
        EventParamsHolder.addParam(EventKeys.ENV_COUNT, envList.size());
        EventParamsHolder.addParam(EventKeys.PROFILE_COUNT, targetProfileIdList.size());
    }

    @Override
    @Transactional(rollbackFor = Exception.class)
    public void copyAppEnv(Long appId, Long targetAppId) {
        ApplicationEnvService self = SpringHolder.getBean(ApplicationEnvService.class);
        // 查询
        LambdaQueryWrapper<ApplicationEnvDO> wrapper = new LambdaQueryWrapper<ApplicationEnvDO>()
                .eq(ApplicationEnvDO::getAppId, appId)
                .eq(ApplicationEnvDO::getSystemEnv, Const.NOT_SYSTEM)
                .orderByAsc(ApplicationEnvDO::getUpdateTime);
        List<ApplicationEnvDO> envList = applicationEnvDAO.selectList(wrapper);
        // 插入
        for (ApplicationEnvDO env : envList) {
            env.setId(null);
            env.setAppId(targetAppId);
            env.setCreateTime(null);
            env.setUpdateTime(null);
            applicationEnvDAO.insert(env);
            // 插入历史值
            historyValueService.addHistory(env.getId(), HistoryValueType.APP_ENV, HistoryOperator.ADD, null, env.getAttrValue());
        }
        // 初始化系统变量
        envList.stream()
                .map(ApplicationEnvDO::getProfileId)
                .distinct()
                .forEach(profileId -> self.checkInitSystemEnv(targetAppId, profileId));
    }

    @Override
    public Integer getBuildSeqAndIncrement(Long appId, Long profileId) {
        // 构建序号
        int seq;
        String buildSeqValue = this.getAppEnvValue(appId, profileId, ApplicationEnvAttr.BUILD_SEQ.getKey());
        if (!Strings.isBlank(buildSeqValue)) {
            if (Strings.isInteger(buildSeqValue)) {
                seq = Integer.parseInt(buildSeqValue);
            } else {
                seq = Const.N_0;
            }
        } else {
            seq = Const.N_0;
        }
        seq++;
        // 修改构建序列
        ApplicationEnvRequest update = new ApplicationEnvRequest();
        update.setAppId(appId);
        update.setProfileId(profileId);
        update.setKey(ApplicationEnvAttr.BUILD_SEQ.getKey());
        update.setValue(seq + Const.EMPTY);
        update.setDescription(ApplicationEnvAttr.BUILD_SEQ.getDescription());
        this.addAppEnv(update);
        return seq;
    }

    @Override
    @Transactional(rollbackFor = Exception.class)
    public void checkInitSystemEnv(Long appId, Long profileId) {
        List<ApplicationEnvDO> list = Lists.newList();
        String buildSeqValue = this.getAppEnvValue(appId, profileId, ApplicationEnvAttr.BUILD_SEQ.getKey());
        if (buildSeqValue == null) {
            // 构建序列
            ApplicationEnvDO buildSeq = ApplicationEnvAttr.BUILD_SEQ.getInitEnv();
            buildSeq.setAppId(appId);
            buildSeq.setProfileId(profileId);
            list.add(buildSeq);
        }
        // 插入
        list.forEach(applicationEnvDAO::insert);
    }

}
