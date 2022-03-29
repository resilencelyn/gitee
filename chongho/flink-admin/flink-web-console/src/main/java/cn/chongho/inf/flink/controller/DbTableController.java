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

import cn.chongho.inf.flink.model.AddDbTable;
import cn.chongho.inf.flink.model.DbTable;
import cn.chongho.inf.flink.model.WebResult;
import cn.chongho.inf.flink.service.DbTableService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;

import javax.annotation.Resource;
import java.util.List;

/**
 * @author ming
 */
@Controller
@RequestMapping("/dbtable")
@Slf4j
public class DbTableController {

    @Resource
    private DbTableService dbTableService;


    @GetMapping("/list")
    public String list(@RequestParam(defaultValue = "1") int page, @RequestParam(defaultValue = "10")int pageSize,
                       @RequestParam(defaultValue = "")String query, ModelMap map){
        DbTable dbTable = new DbTable();
        List<DbTable> dbTableList = dbTableService.getDbTableByPage(page,  pageSize , dbTable);
        int count = dbTableService.selectCount(dbTable);
        map.put("dbTableList",dbTableList);
        map.put("count",count);
        map.put("maxPage",count/10);
        map.put("page",page);
        map.put("pageSize",pageSize);
        map.put("query",query);
        map.put("nowBegin",pageSize * (page - 1 )+1);
        map.put("nowEnd",pageSize * (page - 1 ) + dbTableList.size());
        return "dbsource/index";
    }

    @GetMapping("/listInfo")
    @ResponseBody
    public WebResult listInfo(@RequestParam("dbId") Integer dbId){
        DbTable dbTable = new DbTable();
        dbTable.setDbId(dbId);
        List<DbTable> dbTableList = dbTableService.getAllDbTable(dbTable);
        return WebResult.success(dbTableList);
    }

    @GetMapping(value = "edit")
    public String edit(@RequestParam(defaultValue = "0") int id, ModelMap map){
        if(id != 0){
            map.put("dbTable", dbTableService.getDbTableById(id));
        }
        return "dbsource/edit";
    }


    @RequestMapping("delete")
    @ResponseBody
    public WebResult delete(int id ){
        DbTable dbTable = new DbTable(id);
        dbTableService.deleteDbTable(dbTable);
        return WebResult.success();
    }

    @RequestMapping("update")
    @ResponseBody
    public WebResult update(DbTable dbTable ){
        if (dbTableService.updateDbTable(dbTable)) {
            return WebResult.success();
        }
        return WebResult.unKnown();
    }

    @RequestMapping("addTableAndColumn")
    @ResponseBody
    public WebResult add(AddDbTable addDbTable ){
        if (dbTableService.addTableAndColumn(addDbTable)) {
            return WebResult.success();
        }
        return WebResult.unKnown();
    }
}
