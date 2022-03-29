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

import cn.chongho.inf.flink.model.DbTableColumn;
import cn.chongho.inf.flink.model.WebResult;
import cn.chongho.inf.flink.service.DbTableColumnService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.*;

import javax.annotation.Resource;
import java.util.List;

/**
 * @author ming
 */
@Controller
@RequestMapping("/dbtablecolumn")
@Slf4j
public class DbTableColumnController {

    @Resource
    private DbTableColumnService dbTableColumnService;


    @GetMapping("/list")
    public String list(@RequestParam(defaultValue = "1") int page, @RequestParam(defaultValue = "10")int pageSize,
                       @RequestParam(defaultValue = "")String query, ModelMap map){
        DbTableColumn dbTableColumn = new DbTableColumn();
        List<DbTableColumn> dbTableColumnList = dbTableColumnService.getDbTableColumnByPage(page,  pageSize , dbTableColumn);
        int count = dbTableColumnService.selectCount(dbTableColumn);
        map.put("dbTableColumnList",dbTableColumnList);
        map.put("count",count);
        map.put("maxPage",count/10);
        map.put("page",page);
        map.put("pageSize",pageSize);
        map.put("query",query);
        map.put("nowBegin",pageSize * (page - 1 )+1);
        map.put("nowEnd",pageSize * (page - 1 ) + dbTableColumnList.size());
        return "dbsource/index";
    }


    @GetMapping("/listInfo")
    @ResponseBody
    public WebResult listInfo(@RequestParam("tableId") Integer tableId){
        DbTableColumn dbTableColumn = new DbTableColumn();
        dbTableColumn.setTableId(tableId);
        List<DbTableColumn> dbTableColumnList = dbTableColumnService.getAllDbTableColumn(dbTableColumn);
        return WebResult.success(dbTableColumnList);
    }

    @GetMapping(value = "edit")
    public String edit(@RequestParam(defaultValue = "0") int id, ModelMap map){
        if(id != 0){
            map.put("dbTableColumn", dbTableColumnService.getAllDbTableColumn(new DbTableColumn(id)).get(0));
        }
        return "dbsource/edit";
    }


    @RequestMapping("delete")
    @ResponseBody
    public WebResult delete(int id ){
        DbTableColumn dbTableColumn = new DbTableColumn(id);
        dbTableColumnService.deleteDbTableColumn(dbTableColumn);
        return WebResult.success();
    }

    @RequestMapping("update")
    @ResponseBody
    public WebResult update(DbTableColumn dbTableColumn ){
        if (dbTableColumnService.updateDbTableColumn(dbTableColumn)) {
            return WebResult.success();
        }
        return WebResult.unKnown();
    }

    @RequestMapping("add")
    @ResponseBody
    public WebResult add(DbTableColumn dbTableColumn ){
        if (dbTableColumnService.addDbTableColumn(dbTableColumn)) {
            return WebResult.success();
        }
        return WebResult.unKnown();
    }
}
