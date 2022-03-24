package com.jd.platform.jlog.clientdemo.custom;

import com.jd.platform.jlog.client.cache.RespMap;
import com.jd.platform.jlog.common.utils.FastJsonUtils;

import java.util.Map;

/**
 * @author tangbohu
 * @version 1.0.0
 * @ClassName CustomRespMap.java
 * @Description TODO
 * @createTime 2022年03月21日 20:39:00
 */
public class CustomRespMap implements RespMap {

    @Override
    public Map<String, Object> appendRespMap(String cnt) {

        return FastJsonUtils.toMap(cnt);
    }
}
