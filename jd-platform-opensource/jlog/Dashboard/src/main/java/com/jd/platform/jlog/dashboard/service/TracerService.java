package com.jd.platform.jlog.dashboard.service;

import com.jd.platform.jlog.dashboard.db.Db;
import com.jd.platform.jlog.dashboard.db.WhereCause;
import com.jd.platform.jlog.dashboard.entity.TracerListVO;
import com.jd.platform.jlog.dashboard.model.QueryListModel;
import com.jd.platform.jlog.dashboard.utils.DateUtils;
import com.jd.platform.jlog.dashboard.utils.DbUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Service;
import org.springframework.util.StringUtils;

import javax.annotation.Resource;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

/**
 * @author wuweifeng
 * @version 1.0
 * @date 2021-09-01
 */
@Service
public class TracerService {
    /**
     * 数据库
     */
    @Resource
    private Db db;
    /**
     * 表名
     */
    private static final String MODEL_TABLE_NAME = "tracer_model";
    /**
     * 日志表
     */
    private static final String LOG_TABLE_NAME = "tracer_log";
    /**
     * 日志
     */
    private static final Logger logger = LoggerFactory.getLogger(TracerService.class);

    /**
     * 查询列表
     */
    public TracerListVO list(QueryListModel queryListModel) {
        TracerListVO tracerListVO = new TracerListVO();

        List<WhereCause> causes = DbUtils.convertTracerToCause(queryListModel);

        String[] columns = {"uri", "tracerId", "pin", "uuid", "createTime",
                "costTime", "clientType", "clientVersion", "userIp", "serverIp"};

        try {
            List<Map<String, Object>> list = db.count(MODEL_TABLE_NAME, causes);
            Map<String, Object> map = list.get(0);
            //总条数
            long count = 0;
            for (String key : map.keySet()) {
                count = Long.valueOf(map.get(key).toString());
                break;
            }
            tracerListVO.setTotal(count);

            //默认按时间倒序排序
            boolean needOrderBy = true;
            //如果数量特别多，就不用排序了，不排序性能高的多
            if (count >= 500) {
                needOrderBy = false;
            }
            tracerListVO.setRows(db.query(MODEL_TABLE_NAME, columns, causes, queryListModel.getPageNum(), needOrderBy));

            return tracerListVO;
        } catch (Exception e) {
            logger.error("TracerService.list [error]", e);
            e.printStackTrace();
            return null;
        }

    }

    /**
     * QuerySingleModel
     * <p>
     * 理论只有一条
     */
    public Map<String, Object> findOne(String tracerId, String pin, String uuid, String beginTime, String endTime) {
        List<WhereCause> causes = new ArrayList<>(1);
        if (!StringUtils.isEmpty(pin)) {
            DbUtils.addEqualWhereCause(causes, "pin", pin);
        } else if (!StringUtils.isEmpty(uuid)) {
            DbUtils.addEqualWhereCause(causes, "uuid", uuid);
        }
        //查单条也要带着时间
        if (!StringUtils.isEmpty(beginTime)) {
            DbUtils.addGeWhereCause(causes, "createTime", beginTime);
        }
        if (!StringUtils.isEmpty(endTime)) {
            DbUtils.addLeWhereCause(causes, "createTime", endTime);
        }

        DbUtils.addEqualWhereCause(causes, "tracerId", tracerId);

        String[] columns = {"pin", "requestContent", "responseContent"};

        try {
            List<Map<String, Object>> list = db.queryOne(MODEL_TABLE_NAME, columns, causes, 1);
            return list.get(0);
        } catch (Exception e) {
            logger.error("TracerService.findOne [error]", e);
            e.printStackTrace();
            return null;
        }

    }

    /**
     * 查询某个tracerId的链路日志
     */
    public TracerListVO findLogByTracerId(long tracerId, String createTime, long pageNum) {
        TracerListVO tracerListVO = new TracerListVO();

        List<WhereCause> causes = new ArrayList<>(1);
        if (!StringUtils.isEmpty(createTime)) {
            String beginTime = DateUtils.addAndSubtractTime(createTime, -5000L);
            String endTime = DateUtils.addAndSubtractTime(createTime, 5000L);
            //查单条也要带着时间
            DbUtils.addGeWhereCause(causes, "createTime", beginTime);
            DbUtils.addLeWhereCause(causes, "createTime", endTime);
        }
        DbUtils.addEqualWhereCause(causes, "tracerId", tracerId);
        try {
            List<Map<String, Object>> list = db.queryOne(LOG_TABLE_NAME, null, causes, pageNum);
            tracerListVO.setRows(list);

            List<Map<String, Object>> counts = db.count(LOG_TABLE_NAME, causes);
            Map<String, Object> map = counts.get(0);
            //总条数
            long count = 0;
            for (String key : map.keySet()) {
                count = Long.valueOf(map.get(key).toString());
                break;
            }
            tracerListVO.setTotal(count);
            return tracerListVO;
        } catch (Exception e) {
            logger.error("TracerService.findOne [error]", e);
            e.printStackTrace();
            return null;
        }
    }
}