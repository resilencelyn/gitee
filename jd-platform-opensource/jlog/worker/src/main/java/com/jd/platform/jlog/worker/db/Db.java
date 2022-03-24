package com.jd.platform.jlog.worker.db;

import com.alibaba.fastjson.JSON;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Component;

import javax.annotation.Resource;
import java.nio.charset.StandardCharsets;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.SQLException;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;

import static java.nio.charset.StandardCharsets.*;

/**
 * https://blog.csdn.net/linglongxin24/article/details/53769957
 *
 * @author wuweifeng
 * @version 1.0
 * @date 2021-08-23
 */
@Component
public class Db {
    /**
     * 连接池
     */
    @Resource
    private DbConfig dbConfig;

    private Logger logger = LoggerFactory.getLogger(getClass());

    /**
     * 执行数据库插入操作
     *
     * @param datas     插入数据表中key为列名和value为列对应的值的Map对象的List集合
     * @param tableName 要插入的数据库的表名
     * @return 影响的行数
     * @throws SQLException SQL异常
     */
    public int insertAll(String tableName, List<Map<String, Object>> datas) throws SQLException {
        //影响的行数
        int affectRowCount;
        PreparedStatement preparedStatement = null;
        Connection connection = dbConfig.getConnection();
        try {
            Map<String, Object> valueMap = datas.get(0);
            //获取数据库插入的Map的键值对的值
            Set<String> keySet = valueMap.keySet();
            Iterator<String> iterator = keySet.iterator();
            //要插入的字段sql，其实就是用key拼起来的
            StringBuilder columnSql = new StringBuilder();
            //要插入的字段值，其实就是？
            StringBuilder unknownMarkSql = new StringBuilder();
            Object[] keys = new Object[valueMap.size()];
            int i = 0;
            while (iterator.hasNext()) {
                String key = iterator.next();
                keys[i] = key;
                columnSql.append(i == 0 ? "" : ",");
                columnSql.append(key);

                unknownMarkSql.append(i == 0 ? "" : ",");
                unknownMarkSql.append("?");
                i++;
            }
            //开始拼插入的sql语句
            StringBuilder sql = new StringBuilder();
            sql.append("INSERT INTO ");
            sql.append(tableName);
            sql.append(" (");
            sql.append(columnSql);
            sql.append(" )  VALUES (");
            sql.append(unknownMarkSql);
            sql.append(" )");

            //执行SQL预编译
            preparedStatement = connection.prepareStatement(sql.toString());
            //设置不自动提交，以便于在出现异常的时候数据库回滚**/
            connection.setAutoCommit(false);
//            logger.info(sql.toString());
            for (Map<String, Object> data : datas) {
                for (int k = 0; k < keys.length; k++) {
                    Object val = data.get(keys[k]);
                    if(val instanceof byte[]){
                        val = new String((byte[]) val, ISO_8859_1);
                    }
                    preparedStatement.setObject(k + 1, val);
                }
                preparedStatement.addBatch();
            }
            int[] arr = preparedStatement.executeBatch();
            connection.commit();
            affectRowCount = arr.length;
//            logger.info("成功了插入了" + affectRowCount + "行");
        } catch (Exception e) {
//            if (connection != null) {
//                connection.rollback();
//            }
            e.printStackTrace();
            throw e;
        } finally {
            if (preparedStatement != null) {
                preparedStatement.close();
            }
            if (connection != null) {
                connection.close();
            }
        }
        return affectRowCount;
    }

}
