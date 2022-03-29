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
import cn.chongho.inf.flink.model.Cluster;
import cn.chongho.inf.flink.model.WebResult;
import cn.chongho.inf.flink.request.ClusterListRequest;
import cn.chongho.inf.flink.service.ClusterService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.*;

import javax.annotation.Resource;
import javax.servlet.http.HttpSession;
import java.text.ParseException;
import java.util.List;

/**
 * @author ming
 */
@Controller
@RequestMapping("/cluster")
@Slf4j
public class ClusterController {

    @Resource
    private ClusterService clusterService;

    @GetMapping("/list")
    public String list(ClusterListRequest clusterListRequest, ModelMap map) throws ParseException {
        int page = clusterListRequest.getPage();
        int pageSize = clusterListRequest.getPageSize();

        Cluster cluster = new Cluster();
        cluster.setEnableFlag(Constant.EnableFlag.ENABLE.ordinal());

        List<Cluster> clusters = clusterService.getClusterByPage(page,  pageSize, cluster);
        int count = clusterService.selectCount(cluster);
        map.put("clusters", clusters);
        map.put("count",count);
        map.put("maxPage",count/10);
        map.put("page",page);
        map.put("pageSize",pageSize);
        map.put("nowBegin",pageSize * (page - 1 ) + 1);
        map.put("nowEnd",pageSize * (page - 1 ) + clusters.size());

        map.put("clusterListRequest", clusterListRequest);

        return "cluster/index";
    }

    @GetMapping(value = "edit")
    public String edit(@RequestParam(defaultValue = "0") int id, ModelMap map){
        if(id != 0){
            map.put("cluster", clusterService.getClusterById(id));
        }
        return "cluster/edit";
    }

    @RequestMapping("delete")
    @ResponseBody
    public WebResult delete(int id , HttpSession httpSession){
        clusterService.delete(id);
        return WebResult.success();
    }

    @RequestMapping("update")
    @ResponseBody
    public WebResult update(Cluster cluster, HttpSession httpSession){
        if (clusterService.update(cluster)) {
            return WebResult.success();
        }
        return WebResult.unKnown();
    }
}
