package com.orion.ops.service.impl;

import com.alibaba.fastjson.JSON;
import com.orion.location.region.LocationRegions;
import com.orion.location.region.core.Region;
import com.orion.ops.consts.Const;
import com.orion.ops.consts.EnableType;
import com.orion.ops.consts.MessageConst;
import com.orion.ops.consts.event.EventKeys;
import com.orion.ops.consts.event.EventParamsHolder;
import com.orion.ops.consts.system.SystemCleanType;
import com.orion.ops.consts.system.SystemEnvAttr;
import com.orion.ops.consts.system.ThreadPoolMetricsType;
import com.orion.ops.entity.domain.SystemEnvDO;
import com.orion.ops.entity.dto.SystemSpaceAnalysisDTO;
import com.orion.ops.entity.request.ConfigIpListRequest;
import com.orion.ops.entity.request.SystemEnvRequest;
import com.orion.ops.entity.vo.IpListConfigVO;
import com.orion.ops.entity.vo.SystemAnalysisVO;
import com.orion.ops.entity.vo.SystemOptionVO;
import com.orion.ops.entity.vo.ThreadPoolMetricsVO;
import com.orion.ops.interceptor.IpFilterInterceptor;
import com.orion.ops.service.api.SystemEnvService;
import com.orion.ops.service.api.SystemService;
import com.orion.ops.utils.FileCleaner;
import com.orion.ops.utils.Utils;
import com.orion.remote.channel.SessionHolder;
import com.orion.utils.Strings;
import com.orion.utils.Threads;
import com.orion.utils.Valid;
import com.orion.utils.convert.Converts;
import com.orion.utils.io.Files1;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import javax.annotation.Resource;
import java.io.File;
import java.util.Arrays;
import java.util.List;
import java.util.stream.Collectors;

/**
 * 系统服务实现
 *
 * @author Jiahang Li
 * @version 1.0.0
 * @since 2022/2/15 22:15
 */
@Slf4j
@Service("systemService")
public class SystemServiceImpl implements SystemService {

    private SystemSpaceAnalysisDTO systemSpace;

    @Resource
    private SystemEnvService systemEnvService;

    @Resource
    private IpFilterInterceptor ipFilterInterceptor;

    public SystemServiceImpl() {
        this.systemSpace = new SystemSpaceAnalysisDTO();
    }

    @Override
    public IpListConfigVO getIpInfo(String ip) {
        IpListConfigVO ipConfig = new IpListConfigVO();
        // 查询黑名单
        ipConfig.setBlackIpList(SystemEnvAttr.BLACK_IP_LIST.getValue());
        // 查询白名单
        ipConfig.setWhiteIpList(SystemEnvAttr.WHITE_IP_LIST.getValue());
        // 查询是否启用过滤
        ipConfig.setEnableIpFilter(EnableType.of(SystemEnvAttr.ENABLE_IP_FILTER.getValue()).getValue());
        // 查询是否启用IP白名单
        ipConfig.setEnableWhiteIpList(EnableType.of(SystemEnvAttr.ENABLE_WHITE_IP_LIST.getValue()).getValue());
        // ip
        ipConfig.setCurrentIp(ip);
        Region region = LocationRegions.getRegion(ip);
        if (region != null) {
            StringBuilder location = new StringBuilder()
                    .append(region.getProvince())
                    .append(Const.DASHED)
                    .append(region.getCity())
                    .append(Const.DASHED)
                    .append(region.getCountry());
            location.append(" (").append(region.getNet()).append(')');
            ipConfig.setIpLocation(location.toString());
        }
        return ipConfig;
    }

    @Override
    @Transactional(rollbackFor = Exception.class)
    public void configIpList(ConfigIpListRequest request) {
        // 检查名单
        String blackIpList = request.getBlackIpList();
        String whiteIpList = request.getWhiteIpList();
        this.validIpConfig(blackIpList);
        this.validIpConfig(whiteIpList);
        // 设置黑名单
        SystemEnvDO blackEnv = systemEnvService.selectByName(SystemEnvAttr.BLACK_IP_LIST.getKey());
        SystemEnvRequest updateBlack = new SystemEnvRequest();
        updateBlack.setValue(blackIpList);
        systemEnvService.updateEnv(blackEnv, updateBlack);
        SystemEnvAttr.BLACK_IP_LIST.setValue(blackIpList);
        // 设置白名单
        SystemEnvDO whiteEnv = systemEnvService.selectByName(SystemEnvAttr.WHITE_IP_LIST.getKey());
        SystemEnvRequest updateWhite = new SystemEnvRequest();
        updateWhite.setValue(whiteIpList);
        systemEnvService.updateEnv(whiteEnv, updateWhite);
        SystemEnvAttr.WHITE_IP_LIST.setValue(whiteIpList);
        // 更改启用状态
        EnableType enableIpFilter = EnableType.of(request.getEnableIpFilter());
        SystemEnvDO filterEnv = systemEnvService.selectByName(SystemEnvAttr.ENABLE_IP_FILTER.getKey());
        SystemEnvRequest updateFilter = new SystemEnvRequest();
        updateFilter.setValue(enableIpFilter.getLabel());
        systemEnvService.updateEnv(filterEnv, updateFilter);
        SystemEnvAttr.ENABLE_IP_FILTER.setValue(enableIpFilter.getLabel());
        // 更改启用列表
        EnableType enableWhiteIp = EnableType.of(request.getEnableWhiteIpList());
        SystemEnvDO enableWhiteIpEnv = systemEnvService.selectByName(SystemEnvAttr.ENABLE_WHITE_IP_LIST.getKey());
        SystemEnvRequest updateEnableWhiteIp = new SystemEnvRequest();
        updateEnableWhiteIp.setValue(enableWhiteIp.getLabel());
        systemEnvService.updateEnv(enableWhiteIpEnv, updateEnableWhiteIp);
        SystemEnvAttr.ENABLE_WHITE_IP_LIST.setValue(enableWhiteIp.getLabel());
        // 设置日志参数
        EventParamsHolder.addParams(request);
        // 设置 ip 过滤器
        Boolean enableIpFilterValue = enableIpFilter.getValue();
        Boolean enableWhiteIpValue = enableWhiteIp.getValue();
        ipFilterInterceptor.set(enableIpFilterValue, enableWhiteIpValue, enableWhiteIpValue ? whiteIpList : blackIpList);
    }

    @Override
    public void cleanSystemFile(SystemCleanType cleanType) {
        // 清理
        Threads.start(() -> FileCleaner.cleanDir(cleanType));
        // 设置日志参数
        EventParamsHolder.addParam(EventKeys.LABEL, cleanType.getLabel());
    }

    @Override
    public void analysisSystemSpace() {
        // 临时文件
        File tempPath = new File(SystemEnvAttr.TEMP_PATH.getValue());
        List<File> tempFiles = Files1.listFiles(tempPath, true);
        long tempFilesBytes = tempFiles.stream().mapToLong(File::length).sum();
        systemSpace.setTempFileCount(tempFiles.size());
        systemSpace.setTempFileSize(Files1.getSize(tempFilesBytes));
        tempFiles.clear();

        // 日志文件
        File logPath = new File(SystemEnvAttr.LOG_PATH.getValue());
        List<File> logFiles = Files1.listFiles(logPath, true);
        long logFilesBytes = logFiles.stream().mapToLong(File::length).sum();
        systemSpace.setLogFileCount(logFiles.size());
        systemSpace.setLogFileSize(Files1.getSize(logFilesBytes));
        logFiles.clear();

        // 交换文件
        File swapPath = new File(SystemEnvAttr.SWAP_PATH.getValue());
        List<File> swapFiles = Files1.listFiles(swapPath, true);
        long swapFilesBytes = swapFiles.stream().mapToLong(File::length).sum();
        systemSpace.setSwapFileCount(swapFiles.size());
        systemSpace.setSwapFileSize(Files1.getSize(swapFilesBytes));
        swapFiles.clear();

        // 构建产物
        File buildPath = new File(SystemEnvAttr.DIST_PATH.getValue() + Const.BUILD_DIR);
        List<File> buildFiles = Files1.listFiles(buildPath, true);
        long buildFilesBytes = buildFiles.stream().filter(File::isFile).mapToLong(File::length).sum();
        int distVersions = Files1.listDirs(buildPath).size();
        systemSpace.setDistVersionCount(distVersions);
        systemSpace.setDistFileSize(Files1.getSize(buildFilesBytes));
        buildFiles.clear();

        // vcs产物
        File vcsPath = new File(SystemEnvAttr.VCS_PATH.getValue());
        List<File> vcsPaths = Files1.listFilesFilter(vcsPath, (f, n) -> f.isDirectory() && !Const.EVENT.equals(n), false, true);
        int vcsVersionCount = 0;
        long vcsDirFilesBytes = 0L;
        for (File vcsDir : vcsPaths) {
            vcsVersionCount += Files1.listDirs(vcsDir).size();
            List<File> vcsDirFiles = Files1.listFiles(vcsDir, true);
            vcsDirFilesBytes += vcsDirFiles.stream().mapToLong(File::length).sum();
        }
        systemSpace.setVcsVersionCount(vcsVersionCount);
        systemSpace.setVcsVersionFileSize(Files1.getSize(vcsDirFilesBytes));
        log.info("分析占用磁盘空间完成 {}", JSON.toJSONString(systemSpace));
    }

    @Override
    public SystemAnalysisVO getSystemAnalysis() {
        SystemAnalysisVO vo = Converts.to(systemSpace, SystemAnalysisVO.class);
        // 挂载秘钥数
        vo.setMountKeyCount(SessionHolder.getLoadKeys().size());
        // IP黑名单
        String blackIpList = systemEnvService.getEnvValue(SystemEnvAttr.BLACK_IP_LIST.getKey());
        if (Strings.isBlank(blackIpList)) {
            vo.setBlackIpListCount(0L);
        } else {
            long validBlackIpCount = Arrays.stream(blackIpList.split(Const.LF))
                    .filter(Strings::isNotBlank)
                    .count();
            vo.setBlackIpListCount(validBlackIpCount);
        }
        // IP白名单
        String whiteIpList = systemEnvService.getEnvValue(SystemEnvAttr.WHITE_IP_LIST.getKey());
        if (Strings.isBlank(whiteIpList)) {
            vo.setWhiteIpListCount(0L);
        } else {
            long validWhiteIpCount = Arrays.stream(whiteIpList.split(Const.LF))
                    .filter(Strings::isNotBlank)
                    .count();
            vo.setWhiteIpListCount(validWhiteIpCount);
        }
        // 文件清理
        vo.setFileCleanThreshold(Integer.valueOf(SystemEnvAttr.FILE_CLEAN_THRESHOLD.getValue()));
        vo.setAutoCleanFile(EnableType.of(SystemEnvAttr.ENABLE_AUTO_CLEAN_FILE.getValue()).getValue());
        return vo;
    }

    @Override
    public void updateSystemOption(SystemEnvAttr env, String value) {
        String key = env.getKey();
        String beforeValue = env.getValue();
        // 更新系统配置
        SystemEnvDO systemEnv = systemEnvService.selectByName(key);
        SystemEnvRequest updateEnv = new SystemEnvRequest();
        updateEnv.setValue(value);
        systemEnvService.updateEnv(systemEnv, updateEnv);
        env.setValue(value);
        // 设置日志参数
        EventParamsHolder.addParam(EventKeys.KEY, env.getDescription());
        EventParamsHolder.addParam(EventKeys.LABEL, env.getDescription());
        EventParamsHolder.addParam(EventKeys.BEFORE, beforeValue);
        EventParamsHolder.addParam(EventKeys.AFTER, value);
    }

    @Override
    public SystemOptionVO getSystemOptions() {
        SystemOptionVO options = new SystemOptionVO();
        options.setAutoCleanFile(EnableType.of(SystemEnvAttr.ENABLE_AUTO_CLEAN_FILE.getValue()).getValue());
        options.setFileCleanThreshold(Integer.valueOf(SystemEnvAttr.FILE_CLEAN_THRESHOLD.getValue()));
        options.setAllowMultipleLogin(EnableType.of(SystemEnvAttr.ALLOW_MULTIPLE_LOGIN.getValue()).getValue());
        options.setLoginFailureLock(EnableType.of(SystemEnvAttr.LOGIN_FAILURE_LOCK.getValue()).getValue());
        options.setLoginIpBind(EnableType.of(SystemEnvAttr.LOGIN_IP_BIND.getValue()).getValue());
        options.setLoginTokenAutoRenew(EnableType.of(SystemEnvAttr.LOGIN_TOKEN_AUTO_RENEW.getValue()).getValue());
        options.setLoginTokenExpire(Integer.valueOf(SystemEnvAttr.LOGIN_TOKEN_EXPIRE.getValue()));
        options.setLoginFailureLockThreshold(Integer.valueOf(SystemEnvAttr.LOGIN_FAILURE_LOCK_THRESHOLD.getValue()));
        options.setLoginTokenAutoRenewThreshold(Integer.valueOf(SystemEnvAttr.LOGIN_TOKEN_AUTO_RENEW_THRESHOLD.getValue()));
        options.setResumeEnableSchedulerTask(EnableType.of(SystemEnvAttr.RESUME_ENABLE_SCHEDULER_TASK.getValue()).getValue());
        options.setSftpUploadThreshold(Integer.valueOf(SystemEnvAttr.SFTP_UPLOAD_THRESHOLD.getValue()));
        options.setStatisticsCacheExpire(Integer.valueOf(SystemEnvAttr.STATISTICS_CACHE_EXPIRE.getValue()));
        return options;
    }

    @Override
    public List<ThreadPoolMetricsVO> getThreadPoolMetrics() {
        return Arrays.stream(ThreadPoolMetricsType.values())
                .map(Utils::getThreadPoolMetrics)
                .collect(Collectors.toList());
    }

    /**
     * 校验 ip 过滤列表
     *
     * @param ipList ipList
     */
    private void validIpConfig(String ipList) {
        if (Strings.isBlank(ipList)) {
            return;
        }
        String[] lines = ipList.split(Const.LF);
        for (String ip : lines) {
            if (Strings.isBlank(ip)) {
                continue;
            }
            Valid.isTrue(Utils.validIpLine(ip), Strings.format("{} " + MessageConst.INVALID_CONFIG, ip));
        }
    }

}
