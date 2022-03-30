package com.yunze.common.mapper.yunze;

import java.util.List;
import java.util.Map;

public interface YrootlowHisMapper {

    /**
     * 查询所有
     * */
    public List<Map<String,Object>> ListHis(Map map);

    /**
     * 查询总数
     * @param map
     * @return
     */
    public int selMapHis(Map map);

    public Map<String,Object> getById(Map<String, Object> map);

    /**
     * 用量详情导出查询
     * */
    public List<Map<String,Object>> exportFlowHis (Map map);


    /**
     * 用量详情导出 （同步导出 分组 、备注、发货时间）iccid
     * */
    public List<String> exportFlowHisIccid(Map<String, Object> map);


    /**
     * 日、月 用量折线图
     * @param map
     * @return
     */
    public List<Map<String,Object>> getDayMonthSum(Map<String, Object> map);




    /**
     * 获取前 5 用量排名
     * @param map
     * @return
     */
    public List<Map<String,Object>> getTop5(Map<String, Object> map);

    /**
     * 查询 求和显示总量
     * @param map
     * @return
     */
    public String getTotal_show_flow(Map<String, Object> map);


    /**
     * 获取前 5 用量排名 月用量
     * @param map
     * @return
     */
    public List<Map<String,Object>> monthGetTop5(Map<String, Object> map);

}
