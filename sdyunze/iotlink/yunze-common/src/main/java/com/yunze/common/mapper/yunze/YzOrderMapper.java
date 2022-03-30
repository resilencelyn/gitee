package com.yunze.common.mapper.yunze;

import java.util.List;
import java.util.Map;

/**
 * 订单 数据层
 *
 * @author yunze
 */
public interface YzOrderMapper {

    /**
     * 获取执行的加包订单 去生产任务
     *
     * @return
     */
    public List<Map<String, Object>> findAddPackage(Map<String, Object> map);


    /**
     * 查询
     *
     * @param map
     * @return
     */
    public List<Map<String, Object>> selMap(Map<String, Object> map);

    /**
     * 查询总数
     *
     * @param map
     * @return
     */
    public Integer selMapCount(Map<String, Object> map);


    /**
     * 修改已经加包的订单 加包状态
     *
     * @param map
     * @return
     */
    public int updAddPackage(Map<String, Object> map);


    /**
     * 自动 修改已经加包的订单加包状态
     *
     * @return
     */
    public int updAutomatic();

    public int updActivate(Map<String, Object> map);


    /**
     * 修改 加包执行状态
     *
     * @param map
     * @return
     */
    public int updAddPackageInfo(Map<String, Object> map);


    /**
     * 查询订单加包参数
     *
     * @param map
     * @return
     */
    public Map<String, Object> findOrder(Map<String, Object> map);


    /**
     * 批量插入订单
     *
     * @param map
     * @return
     */
    public int importOrder(Map<String, Object> map);


    /**
     * 新增单条订单
     *
     * @param map
     * @return
     */
    public int save(Map<String, Object> map);


    /**
     * 购物插入订单
     *
     * @param map
     * @return
     */
    public int ShoppingSave(Map<String, Object> map);


    /**
     * 导出
     */
    public List<Map<String, Object>> exportallorders(Map map);

    public List<Map<String, Object>> arrorders(Map map);


    /**
     * getOrderSum
     * @param map
     * @return
     */
    public String getOrderSum(Map<String, Object> map);



}

