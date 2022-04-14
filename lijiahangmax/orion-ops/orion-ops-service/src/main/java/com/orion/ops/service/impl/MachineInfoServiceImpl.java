package com.orion.ops.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.LambdaQueryWrapper;
import com.orion.exception.AuthenticationException;
import com.orion.exception.ConnectionRuntimeException;
import com.orion.lang.wrapper.DataGrid;
import com.orion.lang.wrapper.HttpWrapper;
import com.orion.ops.consts.Const;
import com.orion.ops.consts.MessageConst;
import com.orion.ops.consts.event.EventKeys;
import com.orion.ops.consts.event.EventParamsHolder;
import com.orion.ops.consts.history.HistoryOperator;
import com.orion.ops.consts.history.HistoryValueType;
import com.orion.ops.consts.machine.MachineConst;
import com.orion.ops.consts.machine.MachineProperties;
import com.orion.ops.consts.machine.ProxyType;
import com.orion.ops.dao.MachineEnvDAO;
import com.orion.ops.dao.MachineInfoDAO;
import com.orion.ops.dao.MachineProxyDAO;
import com.orion.ops.entity.domain.MachineEnvDO;
import com.orion.ops.entity.domain.MachineInfoDO;
import com.orion.ops.entity.domain.MachineProxyDO;
import com.orion.ops.entity.request.MachineInfoRequest;
import com.orion.ops.entity.vo.MachineInfoVO;
import com.orion.ops.service.api.*;
import com.orion.ops.utils.DataQuery;
import com.orion.ops.utils.Utils;
import com.orion.ops.utils.ValueMix;
import com.orion.process.Processes;
import com.orion.remote.channel.SessionHolder;
import com.orion.remote.channel.SessionStore;
import com.orion.remote.channel.ssh.CommandExecutor;
import com.orion.utils.Exceptions;
import com.orion.utils.Strings;
import com.orion.utils.Valid;
import com.orion.utils.collect.Lists;
import com.orion.utils.convert.Converts;
import com.orion.utils.io.Streams;
import com.orion.utils.net.IPs;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import javax.annotation.Resource;
import java.io.IOException;
import java.util.Date;
import java.util.List;
import java.util.Objects;
import java.util.Optional;

/**
 * 机器信息服务
 *
 * @author Jiahang Li
 * @since 2021-03-29
 */
@Slf4j
@Service("machineInfoService")
public class MachineInfoServiceImpl implements MachineInfoService {

    @Resource
    private MachineInfoDAO machineInfoDAO;

    @Resource
    private MachineProxyDAO machineProxyDAO;

    @Resource
    private MachineEnvDAO machineEnvDAO;

    @Resource
    private MachineEnvService machineEnvService;

    @Resource
    private MachineTerminalService machineTerminalService;

    @Resource
    private ApplicationMachineService applicationMachineService;

    @Resource
    private FileTailService fileTailService;

    @Resource
    private SchedulerTaskMachineService schedulerTaskMachineService;

    @Resource
    private HistoryValueService historyValueService;

    @Override
    @Transactional(rollbackFor = Exception.class)
    public Long addMachine(MachineInfoRequest request) {
        // 检查proxyId
        this.checkProxy(request.getProxyId());
        MachineInfoDO entity = new MachineInfoDO();
        String password = request.getPassword();
        this.copyProperties(request, entity);
        // 添加机器
        entity.setMachineStatus(Const.ENABLE);
        machineInfoDAO.insert(entity);
        if (Strings.isNotBlank(password)) {
            entity.setPassword(ValueMix.encrypt(password));
            machineInfoDAO.updateById(entity);
        }
        Long id = entity.getId();
        // 初始化环境变量
        machineEnvService.initEnv(id);
        // 设置日志参数
        EventParamsHolder.addParams(entity);
        return id;
    }

    @Override
    @Transactional(rollbackFor = Exception.class)
    public Integer updateMachine(MachineInfoRequest request) {
        // 检查proxyId
        this.checkProxy(request.getProxyId());
        MachineInfoDO entity = new MachineInfoDO();
        String password = request.getPassword();
        this.copyProperties(request, entity);
        if (Strings.isNotBlank(password)) {
            entity.setPassword(ValueMix.encrypt(password));
        }
        // 修改
        int effect = machineInfoDAO.updateById(entity);
        // 设置日志参数
        EventParamsHolder.addParams(entity);
        return effect;
    }

    @Override
    @Transactional(rollbackFor = Exception.class)
    public Integer deleteMachine(List<Long> idList) {
        int effect = 0;
        for (Long id : idList) {
            // 删除机器
            effect += machineInfoDAO.deleteById(id);
            // 删除环境变量
            effect += machineEnvService.deleteEnvByMachineId(id);
            // 删除终端配置
            effect += machineTerminalService.deleteTerminalByMachineId(id);
            // 删除应用机器
            effect += applicationMachineService.deleteAppMachineByMachineId(id);
            // 删除日志文件
            effect += fileTailService.deleteByMachineId(id);
            // 删除调度任务
            effect += schedulerTaskMachineService.deleteByMachineId(id);
        }
        // 设置日志参数
        EventParamsHolder.addParam(EventKeys.ID_LIST, idList);
        EventParamsHolder.addParam(EventKeys.COUNT, idList.size());
        return effect;
    }

    @Override
    @Transactional(rollbackFor = Exception.class)
    public Integer updateStatus(List<Long> idList, Integer status) {
        int effect = 0;
        for (Long id : idList) {
            MachineInfoDO entity = new MachineInfoDO();
            entity.setId(id);
            entity.setMachineStatus(status);
            entity.setUpdateTime(new Date());
            effect += machineInfoDAO.updateById(entity);
        }
        // 设置日志参数
        EventParamsHolder.addParam(EventKeys.ID_LIST, idList);
        EventParamsHolder.addParam(EventKeys.COUNT, effect);
        EventParamsHolder.addParam(EventKeys.OPERATOR, Const.ENABLE.equals(status) ? Const.ENABLE_LABEL : Const.DISABLE_LABEL);
        return effect;
    }

    @Override
    public DataGrid<MachineInfoVO> listMachine(MachineInfoRequest request) {
        LambdaQueryWrapper<MachineInfoDO> wrapper = new LambdaQueryWrapper<MachineInfoDO>()
                .in(Lists.isNotEmpty(request.getIdList()), MachineInfoDO::getId, request.getIdList())
                .like(Strings.isNotBlank(request.getHost()), MachineInfoDO::getMachineHost, request.getHost())
                .like(Strings.isNotBlank(request.getName()), MachineInfoDO::getMachineName, request.getName())
                .like(Strings.isNotBlank(request.getTag()), MachineInfoDO::getMachineTag, request.getTag())
                .like(Strings.isNotBlank(request.getDescription()), MachineInfoDO::getDescription, request.getDescription())
                .like(Strings.isNotBlank(request.getUsername()), MachineInfoDO::getUsername, request.getUsername())
                .eq(Objects.nonNull(request.getProxyId()), MachineInfoDO::getProxyId, request.getProxyId())
                .eq(Objects.nonNull(request.getStatus()), MachineInfoDO::getMachineStatus, request.getStatus())
                .eq(Objects.nonNull(request.getId()), MachineInfoDO::getId, request.getId())
                .ne(Objects.nonNull(request.getExcludeId()), MachineInfoDO::getId, request.getExcludeId())
                .ne(Const.ENABLE.equals(request.getSkipHost()), MachineInfoDO::getId, Const.ENABLE)
                .orderByAsc(MachineInfoDO::getId);
        return DataQuery.of(machineInfoDAO)
                .page(request)
                .wrapper(wrapper)
                .dataGrid(MachineInfoVO.class);
    }

    @Override
    public MachineInfoVO machineDetail(Long id) {
        MachineInfoDO machine = machineInfoDAO.selectById(id);
        Valid.notNull(machine, MessageConst.INVALID_MACHINE);
        MachineInfoVO vo = Converts.to(machine, MachineInfoVO.class);
        Optional.ofNullable(machine.getProxyId())
                .map(machineProxyDAO::selectById)
                .ifPresent(p -> {
                    vo.setProxyHost(p.getProxyHost());
                    vo.setProxyPort(p.getProxyPort());
                    vo.setProxyType(p.getProxyType());
                });
        return vo;
    }

    @Override
    public Long copyMachine(Long id) {
        MachineInfoDO machine = machineInfoDAO.selectById(id);
        Valid.notNull(machine, MessageConst.INVALID_MACHINE);
        String sourceMachineName = machine.getMachineName();
        String targetMachineName = sourceMachineName + Utils.getCopySuffix();
        machine.setId(null);
        machine.setCreateTime(null);
        machine.setUpdateTime(null);
        machine.setMachineName(targetMachineName);
        machineInfoDAO.insert(machine);
        Long insertId = machine.getId();
        // 复制环境变量
        LambdaQueryWrapper<MachineEnvDO> wrapper = new LambdaQueryWrapper<MachineEnvDO>()
                .eq(MachineEnvDO::getMachineId, id)
                .orderByAsc(MachineEnvDO::getUpdateTime);
        machineEnvDAO.selectList(wrapper).forEach(e -> {
            // 插入环境变量
            e.setMachineId(insertId);
            e.setCreateTime(null);
            e.setUpdateTime(null);
            machineEnvDAO.insert(e);
            // 插入历史值
            historyValueService.addHistory(e.getId(), HistoryValueType.MACHINE_ENV, HistoryOperator.ADD, null, e.getAttrValue());
        });
        // 设置日志参数
        EventParamsHolder.addParam(EventKeys.ID, id);
        EventParamsHolder.addParam(EventKeys.SOURCE, sourceMachineName);
        EventParamsHolder.addParam(EventKeys.TARGET, targetMachineName);
        return insertId;
    }

    @Override
    public MachineInfoDO selectById(Long id) {
        return machineInfoDAO.selectById(id);
    }

    @Override
    public Integer testPing(Long id) {
        MachineInfoDO machine = machineInfoDAO.selectById(id);
        Valid.notNull(machine, MessageConst.INVALID_MACHINE);
        boolean ping = IPs.ping(machine.getMachineHost(), Const.MS_S_3);
        return ping ? Const.ENABLE : Const.DISABLE;
    }

    @Override
    public Integer testConnect(Long id) {
        SessionStore s = null;
        try {
            s = this.connectSessionStore(Valid.notNull(machineInfoDAO.selectById(id), MessageConst.INVALID_MACHINE));
            return Const.ENABLE;
        } catch (Exception e) {
            return Const.DISABLE;
        } finally {
            Streams.close(s);
        }
    }

    @Override
    public SessionStore openSessionStore(Long id) {
        return this.openSessionStore(Valid.notNull(machineInfoDAO.selectById(id), MessageConst.INVALID_MACHINE));
    }

    @Override
    public SessionStore openSessionStore(MachineInfoDO machine) {
        Valid.notNull(machine, MessageConst.INVALID_MACHINE);
        // 检查状态
        if (!Const.ENABLE.equals(machine.getMachineStatus())) {
            throw Exceptions.codeArgument(HttpWrapper.HTTP_ERROR_CODE, MessageConst.MACHINE_NOT_ENABLE);
        }
        Exception ex = null;
        String msg = MessageConst.CONNECT_ERROR;
        for (int i = 0, t = MachineConst.CONNECT_RETRY_TIMES; i < t; i++) {
            log.info("远程机器建立连接-尝试连接远程服务器 第{}次尝试 machineId: {}, host: {}", (i + 1), machine.getId(), machine.getMachineHost());
            try {
                return this.connectSessionStore(machine);
            } catch (Exception e) {
                ex = e;
                if (e instanceof ConnectionRuntimeException) {
                    // retry
                } else if (e instanceof AuthenticationException) {
                    msg = MessageConst.AUTH_EXCEPTION_MESSAGE;
                    break;
                } else {
                    break;
                }
            }
        }
        ex.printStackTrace();
        throw Exceptions.codeArgument(HttpWrapper.HTTP_ERROR_CODE, "机器 " + machine.getMachineHost() + " " + msg, ex);
    }

    /**
     * 打开sessionStore
     *
     * @param machine machine
     * @return SessionStore
     */
    private SessionStore connectSessionStore(MachineInfoDO machine) {
        Valid.notNull(machine, MessageConst.INVALID_MACHINE);
        Long proxyId = machine.getProxyId();
        SessionStore session;
        try {
            session = SessionHolder.getSession(machine.getMachineHost(), machine.getSshPort(), machine.getUsername());
            String password = machine.getPassword();
            if (Strings.isNotBlank(password)) {
                session.setPassword(ValueMix.decrypt(password));
            }
            MachineProxyDO proxy = null;
            if (proxyId != null) {
                proxy = machineProxyDAO.selectById(proxyId);
            }
            if (proxy != null) {
                ProxyType proxyType = ProxyType.of(proxy.getProxyType());
                String proxyPassword = proxy.getProxyPassword();
                if (!Strings.isBlank(proxyPassword)) {
                    proxyPassword = ValueMix.decrypt(proxyPassword);
                }
                if (ProxyType.HTTP.equals(proxyType)) {
                    session.setHttpProxy(proxy.getProxyHost(), proxy.getProxyPort(), proxy.getProxyUsername(), proxyPassword);
                } else if (ProxyType.SOCKET4.equals(proxyType)) {
                    session.setSocket4Proxy(proxy.getProxyHost(), proxy.getProxyPort(), proxy.getProxyUsername(), proxyPassword);
                } else if (ProxyType.SOCKET5.equals(proxyType)) {
                    session.setSocket5Proxy(proxy.getProxyHost(), proxy.getProxyPort(), proxy.getProxyUsername(), proxyPassword);
                }
                session.setHttpProxy(proxy.getProxyHost(), proxy.getProxyPort(), proxy.getProxyUsername(), proxyPassword);
            }
            session.connect(MachineConst.CONNECT_TIMEOUT);
            log.info("远程机器建立连接-成功 {}@{}:{}", machine.getUsername(), machine.getMachineHost(), machine.getSshPort());
            return session;
        } catch (Exception e) {
            log.error("远程机器建立连接-失败 {}@{}:{} {}", machine.getUsername(), machine.getMachineHost(), machine.getSshPort(), e);
            throw e;
        }
    }

    @Override
    public String getPropertiesResultSync(Long id, MachineProperties property) {
        return getCommandResultSync(id, property.getCommand());
    }

    @Override
    public String getCommandResultSync(Long id, String command) {
        String res;
        if (id.equals(Const.HOST_MACHINE_ID)) {
            // 本机
            res = this.runHostCommand(command);
        } else {
            // 远程
            res = this.runRemoteCommand(id, command);
        }
        if (res == null) {
            return null;
        }
        if (res.endsWith(Const.LF)) {
            return res.substring(0, res.length() - 1);
        }
        return res;
    }

    @Override
    public String getMachineName(Long id) {
        return machineInfoDAO.selectMachineName(id);
    }

    /**
     * 执行本地命令
     */
    private String runHostCommand(String command) {
        try {
            return Processes.getOutputResultString(command);
        } catch (Exception e) {
            log.error("执行本机命令-失败 {} {}", command, e);
            return null;
        }
    }

    /**
     * 执行远程机器命令
     */
    private String runRemoteCommand(Long id, String command) {
        SessionStore session = null;
        CommandExecutor executor = null;
        try {
            session = this.openSessionStore(id);
            executor = session.getCommandExecutor(Strings.replaceCRLF(command));
            executor.connect();
            String res = SessionStore.getCommandOutputResultString(executor);
            log.info("执行机器命令-成功 {} {} {}", id, command, res);
            return res;
        } catch (Exception e) {
            log.error("执行机器命令-失败 {} {} {}", id, command, e);
            if (e instanceof IOException) {
                throw Exceptions.ioRuntime(e);
            } else if (e instanceof ConnectionRuntimeException) {
                throw (ConnectionRuntimeException) e;
            } else if (e instanceof AuthenticationException) {
                throw (AuthenticationException) e;
            }
            return null;
        } finally {
            Streams.close(executor);
            Streams.close(session);
        }
    }

    /**
     * 检查代理
     */
    private void checkProxy(Long proxyId) {
        if (proxyId == null) {
            return;
        }
        MachineProxyDO proxy = machineProxyDAO.selectById(proxyId);
        Valid.notNull(proxy, MessageConst.INVALID_PROXY);
    }

    /**
     * 复制属性
     */
    private void copyProperties(MachineInfoRequest request, MachineInfoDO entity) {
        entity.setId(request.getId());
        entity.setProxyId(request.getProxyId());
        entity.setMachineHost(request.getHost());
        entity.setSshPort(request.getSshPort());
        entity.setMachineName(request.getName());
        entity.setMachineTag(request.getTag());
        entity.setDescription(request.getDescription());
        entity.setUsername(request.getUsername());
        entity.setPassword(null);
        entity.setAuthType(request.getAuthType());
        entity.setMachineStatus(request.getStatus());
    }

}
