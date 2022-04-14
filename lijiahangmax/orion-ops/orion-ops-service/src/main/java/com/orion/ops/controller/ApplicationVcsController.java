package com.orion.ops.controller;

import com.orion.lang.wrapper.DataGrid;
import com.orion.lang.wrapper.HttpWrapper;
import com.orion.ops.annotation.EventLog;
import com.orion.ops.annotation.RestWrapper;
import com.orion.ops.consts.app.VcsAuthType;
import com.orion.ops.consts.app.VcsTokenType;
import com.orion.ops.consts.event.EventType;
import com.orion.ops.entity.request.ApplicationVcsRequest;
import com.orion.ops.entity.vo.ApplicationVcsBranchVO;
import com.orion.ops.entity.vo.ApplicationVcsCommitVO;
import com.orion.ops.entity.vo.ApplicationVcsInfoVO;
import com.orion.ops.entity.vo.ApplicationVcsVO;
import com.orion.ops.service.api.ApplicationVcsService;
import com.orion.ops.utils.Valid;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import javax.annotation.Resource;
import java.util.List;

/**
 * 应用仓库api
 *
 * @author Jiahang Li
 * @version 1.0.0
 * @since 2021/11/29 8:55
 */
@RestController
@RestWrapper
@RequestMapping("/orion/api/app-vcs")
public class ApplicationVcsController {

    @Resource
    private ApplicationVcsService applicationVcsService;

    /**
     * 添加版本仓库
     */
    @RequestMapping("/add")
    @EventLog(EventType.ADD_VCS)
    public Long addAppVcs(@RequestBody ApplicationVcsRequest request) {
        Valid.allNotBlank(request.getName(), request.getUrl());
        VcsAuthType authType = Valid.notNull(VcsAuthType.of(request.getAuthType()));
        if (VcsAuthType.TOKEN.equals(authType)) {
            Valid.notNull(VcsTokenType.of(request.getTokenType()));
            Valid.notBlank(request.getPrivateToken());
        }
        return applicationVcsService.addAppVcs(request);
    }

    /**
     * 更新版本仓库
     */
    @RequestMapping("/update")
    @EventLog(EventType.UPDATE_VCS)
    public Integer updateAppVcs(@RequestBody ApplicationVcsRequest request) {
        Valid.notNull(request.getId());
        Valid.allNotBlank(request.getName(), request.getUrl());
        return applicationVcsService.updateAppVcs(request);
    }

    /**
     * 通过id删除
     */
    @RequestMapping("/delete")
    @EventLog(EventType.DELETE_VCS)
    public Integer deleteAppVcs(@RequestBody ApplicationVcsRequest request) {
        Long id = Valid.notNull(request.getId());
        return applicationVcsService.deleteAppVcs(id);
    }

    /**
     * 获取列表
     */
    @RequestMapping("/list")
    public DataGrid<ApplicationVcsVO> listAppVcs(@RequestBody ApplicationVcsRequest request) {
        return applicationVcsService.listAppVcs(request);
    }

    /**
     * 获取详情
     */
    @RequestMapping("/detail")
    public ApplicationVcsVO getAppVcsDetail(@RequestBody ApplicationVcsRequest request) {
        Long id = Valid.notNull(request.getId());
        return applicationVcsService.getAppVcsDetail(id);
    }

    /**
     * 仓库初始化
     */
    @RequestMapping("/init")
    @EventLog(EventType.INIT_VCS)
    public HttpWrapper<?> initVcs(@RequestBody ApplicationVcsRequest request) {
        Long id = Valid.notNull(request.getId());
        applicationVcsService.initEventVcs(id, false);
        return HttpWrapper.ok();
    }

    /**
     * 仓库重新初始化
     */
    @RequestMapping("/re-init")
    @EventLog(EventType.RE_INIT_VCS)
    public HttpWrapper<?> reInitVcs(@RequestBody ApplicationVcsRequest request) {
        Long id = Valid.notNull(request.getId());
        applicationVcsService.initEventVcs(id, true);
        return HttpWrapper.ok();
    }

    /**
     * 获取版本信息列表
     */
    @RequestMapping("/info")
    public ApplicationVcsInfoVO getVcsInfo(@RequestBody ApplicationVcsRequest request) {
        Valid.notNull(request.getId());
        return applicationVcsService.getVcsInfo(request);
    }

    /**
     * 获取分支列表
     */
    @RequestMapping("/branch")
    public List<ApplicationVcsBranchVO> getVcsBranchList(@RequestBody ApplicationVcsRequest request) {
        Long id = Valid.notNull(request.getId());
        return applicationVcsService.getVcsBranchList(id);
    }

    /**
     * 获取提交列表
     */
    @RequestMapping("/commit")
    public List<ApplicationVcsCommitVO> getVcsCommitList(@RequestBody ApplicationVcsRequest request) {
        Long id = Valid.notNull(request.getId());
        String branchName = Valid.notBlank(request.getBranchName());
        return applicationVcsService.getVcsCommitList(id, branchName);
    }

    /**
     * 清空应用构建历史版本
     */
    @RequestMapping("/clean")
    @EventLog(EventType.CLEAN_VCS)
    public HttpWrapper<?> cleanBuildVcs(@RequestBody ApplicationVcsRequest request) {
        Long id = Valid.notNull(request.getId());
        applicationVcsService.cleanBuildVcs(id);
        return HttpWrapper.ok();
    }

}
