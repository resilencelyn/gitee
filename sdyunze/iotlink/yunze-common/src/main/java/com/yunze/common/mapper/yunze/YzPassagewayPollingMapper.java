package com.yunze.common.mapper.yunze;


import java.util.List;
import java.util.Map;

/**
 * 通道轮序详情表
 */
public interface YzPassagewayPollingMapper {

    /**
     * 查询
     * @param map
     * @return
     * @throws Exception
     */
    List<Map<String, Object>> selMap(Map<String, Object> map);


    /**
     * 查询总数
     * @param map
     * @return
     * @throws Exception
     */
    public int selMapCount(Map<String, Object> map);


    /**
     * 新增 轮序详情
     * @param map
     * @return
     */
    public int add(Map<String, Object> map);

    /**
     * 修改当前轮序 数量
     * @param map
     * @return
     */
    public int update_cd_current(Map<String, Object> map);


    /**
     *查询  近三天 正在执行的轮序任务
     * @return
     */
    public List<Map<String, Object>> find_execute();



}
