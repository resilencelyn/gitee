package com.jd.platform.jlog.dashboard.utils;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;

/**
 * 时间工具类
 * @author wuweifeng
 * @version 1.0
 * @date 2021-09-16
 */
public class DateUtils {
    /**
     * 加减时间方法
     */
    public static String addAndSubtractTime(String createTime, long milliSeconds) {
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        try {
            //数据时间
            Date current = sdf.parse(createTime);
            //转化后时间
            Date after = new Date(current.getTime() + milliSeconds);
            //返回
            return sdf.format(after);
        } catch (ParseException e) {
            return null;
        }

    }
}
