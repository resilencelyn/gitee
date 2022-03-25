package com.larkmidtable.admin.controller;

import com.larkmidtable.admin.entity.APIAuth;
import com.larkmidtable.admin.mapper.APIAuthMapper;
import com.larkmidtable.core.biz.model.ReturnT;
import io.swagger.annotations.Api;
import io.swagger.annotations.ApiOperation;
import org.springframework.web.bind.annotation.*;

import javax.annotation.Resource;
import javax.servlet.http.HttpServletRequest;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

@RestController
@RequestMapping("/api/apiAuth")
@Api(tags = "数据服务- 权限控制")
public class APIAuthController extends BaseController {
	private SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
	@Resource
	private APIAuthMapper apiAuthMapper;

    @ApiOperation("获取所有数据")
    @GetMapping("/list")
    public ReturnT<Map<String, Object>> selectList(@RequestParam(value = "current", required = false, defaultValue = "1") int current,
											 @RequestParam(value = "size", required = false, defaultValue = "10") int size,
											 @RequestParam(value = "name", required = false) String name) {
		// page list
		List<APIAuth> list = apiAuthMapper.findAll();
		Map<String, Object> maps = new HashMap<>();
		maps.put("recordsTotal", list.size());    // 过滤后的总记录数
		maps.put("data", list);                    // 分页列表
		return new ReturnT<>(maps);
    }

    @ApiOperation("新增数据")
	@PostMapping("/add")
    public ReturnT<String> insert(HttpServletRequest request, @RequestBody APIAuth entity) {
		entity.setUpdate_time(sdf.format(new Date()));
        this.apiAuthMapper.save(entity);
		return ReturnT.SUCCESS;
    }

    @ApiOperation("修改数据")
	@PostMapping(value = "/update")
    public ReturnT<String> update(@RequestBody APIAuth entity) {
		APIAuth project = apiAuthMapper.getById(entity.getId());
		project.setGroup_id(entity.getGroup_id());
		project.setToken_id(entity.getToken_id());
        project.setUpdate_time(sdf.format(new Date()));
		apiAuthMapper.update(project);
		return ReturnT.SUCCESS;
    }

	@RequestMapping(value = "/remove", method = RequestMethod.POST)
    @ApiOperation("删除数据")
    public ReturnT<String> delete(int id) {
		int result = apiAuthMapper.delete(id);
		return result != 1 ? ReturnT.FAIL : ReturnT.SUCCESS;
    }
}
