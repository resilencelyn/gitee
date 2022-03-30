package com.yunze.common.mapper.yunze;


import java.util.List;
import java.util.Map;

/**
 * 卡状态变更记录表
 */
public interface YzCardInfoChangeMapper {

    /**
     * 查询
     * @param map
     * @return
     * @throws Exception
     */
    public List<Map<String, String>> selMap(Map map);


    /**
     * 查询总数
     * @param map
     * @return
     * @throws Exception
     */
    public int selMapCount(Map map);



    /**
     * 新增
     * @param map
     * @return
     * @throws Exception
     */
    public int save(Map map);


    /**
     * 新增
     * @param map
     * @return
     * @throws Exception
     */
    public int addinfo(Map map);

}
