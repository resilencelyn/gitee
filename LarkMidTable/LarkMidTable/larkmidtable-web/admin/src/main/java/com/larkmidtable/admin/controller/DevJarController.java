package com.larkmidtable.admin.controller;

import com.larkmidtable.admin.entity.DevTask;
import com.larkmidtable.admin.mapper.DevJarMapper;
import com.larkmidtable.core.biz.model.ReturnT;
import io.swagger.annotations.Api;
import io.swagger.annotations.ApiOperation;
import org.springframework.web.bind.annotation.*;

import javax.annotation.Resource;
import javax.servlet.http.HttpServletRequest;
import java.text.SimpleDateFormat;
import java.util.Date;


@RestController
@RequestMapping("/api/devJar")
@Api(tags = "数据开发-JAR作业开发")
public class DevJarController  extends BaseController {
	private SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");

	@Resource
	private DevJarMapper devJarMapper;

	@ApiOperation("新增数据")
	@PostMapping("/add")
	public ReturnT<String> insert(HttpServletRequest request, @RequestBody DevTask entity) {
		entity.setCreate_time(sdf.format(new Date()));
		this.devJarMapper.save(entity);
		return ReturnT.SUCCESS;
	}


	@ApiOperation("修改数据")
	@PostMapping(value = "/update")
	public ReturnT<String> update(@RequestBody DevTask entity) {
		entity.setUpdate_time(sdf.format(new Date()));
		devJarMapper.update(entity);
		return ReturnT.SUCCESS;
	}

}
