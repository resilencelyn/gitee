package com.yunze.system.service.yunze.sysgl;

import java.util.Map;

/**
 * 系统首页数据
 */
public interface IYzIndexpageService {

    /**
     * 查询首页数据
     * @param map
     * @return
     */
    public Map <String,Object> findToDay(Map<String, Object> map);


}
