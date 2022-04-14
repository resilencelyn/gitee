package com.orion.ops.controller;

import com.orion.lang.wrapper.DataGrid;
import com.orion.ops.annotation.EventLog;
import com.orion.ops.annotation.RestWrapper;
import com.orion.ops.consts.event.EventType;
import com.orion.ops.consts.machine.ProxyType;
import com.orion.ops.entity.request.MachineProxyRequest;
import com.orion.ops.entity.vo.MachineProxyVO;
import com.orion.ops.service.api.MachineProxyService;
import com.orion.ops.utils.Valid;
import com.orion.utils.Strings;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import javax.annotation.Resource;
import java.util.List;

/**
 * 机器代理
 *
 * @author Jiahang Li
 * @version 1.0.0
 * @since 2021/4/3 21:11
 */
@RestController
@RestWrapper
@RequestMapping("/orion/api/proxy")
public class MachineProxyController {

    @Resource
    private MachineProxyService machineProxyService;

    /**
     * 添加
     */
    @RequestMapping("/add")
    @EventLog(EventType.ADD_MACHINE_PROXY)
    public Long addProxy(@RequestBody MachineProxyRequest request) {
        request.setId(null);
        this.check(request);
        if (!Strings.isBlank(request.getUsername())) {
            Valid.notNull(request.getPassword());
        }
        return machineProxyService.addProxy(request);
    }

    /**
     * 修改
     */
    @RequestMapping("/update")
    @EventLog(EventType.UPDATE_MACHINE_PROXY)
    public Integer update(@RequestBody MachineProxyRequest request) {
        Valid.notNull(request.getId());
        this.check(request);
        return machineProxyService.updateProxy(request);
    }

    /**
     * 列表
     */
    @RequestMapping("/list")
    public DataGrid<MachineProxyVO> list(@RequestBody MachineProxyRequest request) {
        return machineProxyService.listProxy(request);
    }

    /**
     * 详情
     */
    @RequestMapping("/detail")
    public MachineProxyVO detail(@RequestBody MachineProxyRequest request) {
        Long id = Valid.notNull(request.getId());
        return machineProxyService.getProxyDetail(id);
    }

    /**
     * 删除
     */
    @RequestMapping("/delete")
    @EventLog(EventType.DELETE_MACHINE_PROXY)
    public Integer delete(@RequestBody MachineProxyRequest request) {
        List<Long> idList = Valid.notEmpty(request.getIdList());
        return machineProxyService.deleteProxy(idList);
    }

    /**
     * 合法校验
     */
    private void check(MachineProxyRequest request) {
        Valid.notBlank(request.getHost());
        Valid.notNull(request.getPort());
        Valid.notNull(ProxyType.of(request.getType()));
    }

}
