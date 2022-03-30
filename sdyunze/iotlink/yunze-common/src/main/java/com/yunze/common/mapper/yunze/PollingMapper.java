package com.yunze.common.mapper.yunze;

import java.util.List;
import java.util.Map;

public interface PollingMapper {

    /**
     * 查询总数
     * @param map
     * @return
     */
    public int MapCount(Map map);

    /**
     *查询
     * @param map
     * @return
     */
    public List<Map <String,Object>> getList(Map map);
}
