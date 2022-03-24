package com.jd.platform.jlog.worker.controller;

import com.jd.platform.jlog.worker.db.Db;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import javax.annotation.Resource;
import java.sql.SQLException;
import java.util.*;

/**
 * @author wuweifeng
 * @version 1.0
 * @date 2021-08-23
 */
@RestController
@RequestMapping("/user")
public class UserController {

    @Resource
    private Db db;

    @GetMapping("/save")
    public Object save() throws SQLException {

        String sql = "select * from abc;";

        List<Map<String, Object>> datas = new ArrayList<>();

        Map<String, Object> map0 = new HashMap<>();
        map0.put("clientType", 1);
        map0.put("pin", "abcd");
       // map0.put("createTime", DateUtil.formatDateTime(new Date()));

        Map<String, Object> map1 = new HashMap<>();
        map1.put("clientType", 1);
        map1.put("pin", "abcd");
      //  map1.put("createTime", DateUtil.formatDateTime(new Date()));

        datas.add(map0);
        datas.add(map1);


//        List<JSONObject> jsonObjects = db.exeSql(sql);

        return   db.insertAll("tracer_model", datas);
    }
}