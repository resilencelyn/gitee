package com.orion.ops.controller;

import com.orion.lang.collect.MutableLinkedHashMap;
import com.orion.lang.wrapper.DataGrid;
import com.orion.lang.wrapper.HttpWrapper;
import com.orion.ops.annotation.EventLog;
import com.orion.ops.annotation.RestWrapper;
import com.orion.ops.consts.Const;
import com.orion.ops.consts.MessageConst;
import com.orion.ops.consts.env.EnvViewType;
import com.orion.ops.consts.event.EventType;
import com.orion.ops.entity.request.MachineEnvRequest;
import com.orion.ops.entity.vo.MachineEnvVO;
import com.orion.ops.service.api.MachineEnvService;
import com.orion.ops.utils.Valid;
import com.orion.utils.Exceptions;
import com.orion.utils.collect.Maps;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import javax.annotation.Resource;
import java.util.List;
import java.util.Map;

/**
 * 机器环境变量
 *
 * @author Jiahang Li
 * @version 1.0.0
 * @since 2021/4/15 10:06
 */
@RestController
@RestWrapper
@RequestMapping("/orion/api/machine-env")
public class MachineEnvController {

    @Resource
    private MachineEnvService machineEnvService;

    /**
     * 添加
     */
    @RequestMapping("/add")
    public Long add(@RequestBody MachineEnvRequest request) {
        Valid.notBlank(request.getKey());
        Valid.notNull(request.getValue());
        Valid.notNull(request.getMachineId());
        return machineEnvService.addEnv(request);
    }

    /**
     * 更新
     */
    @RequestMapping("/update")
    public Integer update(@RequestBody MachineEnvRequest request) {
        Valid.notNull(request.getId());
        Valid.notNull(request.getValue());
        return machineEnvService.updateEnv(request);
    }

    /**
     * 删除
     */
    @RequestMapping("/delete")
    @EventLog(EventType.DELETE_MACHINE_ENV)
    public Integer delete(@RequestBody MachineEnvRequest request) {
        List<Long> idList = Valid.notEmpty(request.getIdList());
        return machineEnvService.deleteEnv(idList);
    }

    /**
     * 列表
     */
    @RequestMapping("/list")
    public DataGrid<MachineEnvVO> list(@RequestBody MachineEnvRequest request) {
        Valid.notNull(request.getMachineId());
        return machineEnvService.listEnv(request);
    }

    /**
     * 详情
     */
    @RequestMapping("/detail")
    public MachineEnvVO detail(@RequestBody MachineEnvRequest request) {
        Long id = Valid.notNull(request.getId());
        return machineEnvService.getEnvDetail(id);
    }

    /**
     * 同步
     */
    @RequestMapping("/sync")
    @EventLog(EventType.SYNC_MACHINE_ENV)
    public HttpWrapper<?> sync(@RequestBody MachineEnvRequest request) {
        Valid.notNull(request.getId());
        Valid.notNull(request.getMachineId());
        Valid.notEmpty(request.getTargetMachineIdList());
        machineEnvService.syncMachineEnv(request);
        return HttpWrapper.ok();
    }

    /**
     * 视图
     */
    @RequestMapping("/view")
    public String view(@RequestBody MachineEnvRequest request) {
        Valid.notNull(request.getMachineId());
        EnvViewType viewType = Valid.notNull(EnvViewType.of(request.getViewType()));
        request.setLimit(Const.N_100000);
        // 查询列表
        Map<String, String> env = Maps.newLinkedMap();
        machineEnvService.listEnv(request).forEach(e -> env.put(e.getKey(), e.getValue()));
        return viewType.toValue(env);
    }

    /**
     * 视图保存
     */
    @RequestMapping("/view-save")
    public Integer viewSave(@RequestBody MachineEnvRequest request) {
        Long machineId = Valid.notNull(request.getMachineId());
        String value = Valid.notBlank(request.getValue());
        EnvViewType viewType = Valid.notNull(EnvViewType.of(request.getViewType()));
        try {
            MutableLinkedHashMap<String, String> result = viewType.toMap(value);
            machineEnvService.saveEnv(machineId, result);
            return result.size();
        } catch (Exception e) {
            throw Exceptions.argument(MessageConst.PARSE_ERROR, e);
        }
    }

}
