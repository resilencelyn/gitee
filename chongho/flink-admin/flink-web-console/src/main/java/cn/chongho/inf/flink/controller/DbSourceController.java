/**
 *    Copyright (c) [2022] [中和农信]
 *    [Software Name] is licensed under Mulan PSL v2.
 *    You can use this software according to the terms and conditions of the Mulan PSL v2.
 *    You may obtain a copy of Mulan PSL v2 at:
 *             http://license.coscl.org.cn/MulanPSL2
 *    THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *    See the Mulan PSL v2 for more details.
 *
 */
package cn.chongho.inf.flink.controller;

import cn.chongho.inf.flink.constants.Constant;
import cn.chongho.inf.flink.constants.DbColunmDataType;
import cn.chongho.inf.flink.model.DbSource;
import cn.chongho.inf.flink.model.DbTable;
import cn.chongho.inf.flink.model.WebResult;
import cn.chongho.inf.flink.request.DbListRequest;
import cn.chongho.inf.flink.service.DbSourceService;
import cn.chongho.inf.flink.service.DbTableService;
import cn.chongho.inf.flink.utils.HttpSessionUtils;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.BeanUtils;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.*;

import javax.annotation.Resource;
import javax.servlet.http.HttpSession;
import java.util.Date;
import java.util.List;

/**
 * @author ming
 */
@Controller
@RequestMapping("/dbSource")
@Slf4j
public class DbSourceController {

    @Resource
    private DbSourceService dbSourceService;

    @Resource
    private DbTableService dbTableService;


    @GetMapping("/list")
    public String list(DbListRequest dbListRequest, ModelMap map){
        DbSource dbSource = new DbSource();
        BeanUtils.copyProperties(dbListRequest ,dbSource);
        int page = dbListRequest.getPage();
        int pageSize = dbListRequest.getPageSize();
        List<DbSource> dbSourceList = dbSourceService.getDbSourceByPage(page,  pageSize , dbSource);
        int count = dbSourceService.selectCount(dbSource);
        map.put("dbSourceList",dbSourceList);
        map.put("count",count);
        map.put("maxPage",count/10);
        map.put("page",page);
        map.put("pageSize",pageSize);
        map.put("nowBegin",pageSize * (page - 1 )+1);
        map.put("nowEnd",pageSize * (page - 1 ) + dbSourceList.size());

        map.put("dbListRequest",dbListRequest);
        return "dbsource/index";
    }

    @GetMapping(value = "edit")
    public String edit(@RequestParam(defaultValue = "0") int id, ModelMap map){
        if(id != 0){
            map.put("dbSource", dbSourceService.getDbSourceById(id));
        }
        return "dbsource/edit";
    }

    @GetMapping(value = "addTable")
    public String addTable(@RequestParam(defaultValue = "0") int id, ModelMap map){
        map.put("dbSource", dbSourceService.getDbSourceById(id));
        DbTable dbTable = new DbTable();
        dbTable.setDbId(id);
        map.put("dbTableList", dbTableService.getAllDbTable(dbTable));
        map.put("typeList", DbColunmDataType.ElasticsearchDataType.getTypes());
        map.put("typeJson", String.join(",", DbColunmDataType.ElasticsearchDataType.getTypes()));
        return "dbsource/addTable";
    }


    @RequestMapping("delete")
    @ResponseBody
    public WebResult delete(int id , HttpSession httpSession){
        DbSource dbSource = new DbSource(id);
        dbSource.setEnableFlag(Constant.EnableFlag.NOT.ordinal());
        dbSource.setUpdateTime(new Date());
        dbSource.setUpdateUserId(HttpSessionUtils.getLoginUser(httpSession).getId());
        dbSourceService.updateDbSource(dbSource);
        return WebResult.success();
    }

    @RequestMapping("update")
    @ResponseBody
    public WebResult update(DbSource dbSource , HttpSession httpSession){
        dbSource.setUpdateTime(new Date());
        dbSource.setUpdateUserId(HttpSessionUtils.getLoginUser(httpSession).getId());
        if (dbSourceService.updateDbSource(dbSource)) {
            return WebResult.success();
        }
        return WebResult.unKnown();
    }

    @RequestMapping("add")
    @ResponseBody
    public WebResult add(DbSource dbSource , HttpSession httpSession){
        dbSource.setCreateTime(new Date());
        dbSource.setUpdateUserId(HttpSessionUtils.getLoginUser(httpSession).getId());
        if (dbSourceService.addDbSource(dbSource)) {
            return WebResult.success();
        }
        return WebResult.unKnown();
    }

    @RequestMapping("syncDbInfo")
    @ResponseBody
    public WebResult syncDbInfo(int id){
        if (dbSourceService.syncDbInfo(id)) {
            return WebResult.success();
        }
        return WebResult.unKnown();
    }
}
