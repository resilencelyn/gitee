package com.jd.platform.jlog.dashboard.controller;

import com.jd.platform.jlog.dashboard.db.ConnectionPool;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.*;

import javax.annotation.Resource;
import java.sql.*;

/**
 * QueryController
 *
 * @author wuweifeng
 * @version 1.0
 * @date 2021-08-31
 */
@Controller
@RequestMapping("/tracer")
public class QueryController {

    /**
     * 连接池
     */
    @Resource
    private ConnectionPool connectionPool;

    private static final Logger logger = LoggerFactory.getLogger(QueryController.class);

    @PostMapping("/query")
    @ResponseBody
    public String test1(@RequestParam("sql")String sql) {
        StringBuffer buffer = new StringBuffer();
        try {
            Connection connection = connectionPool.getConnection();
            Statement statement = connection.createStatement();

            ResultSet resultSet = statement.executeQuery(sql);
            ResultSetMetaData rsmd = resultSet.getMetaData();
            int columnsNumber = rsmd.getColumnCount();

            while (resultSet.next()) {
                for (int i = 1; i <= columnsNumber; i++) {
                    if (i > 1) {
                        buffer.append(",  ");
                    }
                    String columnValue = resultSet.getString(i);
                    buffer.append(columnValue + " " + rsmd.getColumnName(i) + "\n");
                }
            }
        } catch (SQLException e) {
            e.printStackTrace();
            logger.error("query error", e);
        }


        return buffer.toString();
    }

    @GetMapping("/query")
    public String test2() {
        return "tracer/query";
    }
}
