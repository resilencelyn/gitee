package com.yunze.common.mapper.yunze;


import java.util.List;
import java.util.Map;

/**
 * 系统用户 企业用户用
 */
public interface YzUserMapper {

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
     * 获取用户 权限过滤
     * @param map
     * @return
     */
    public List<String> getUserID(Map<String, Object> map);


    /**
     * 获取部门id
     * @param map 权限过滤
     * @return
     */
    public List<String> getDeptID(Map<String, Object> map);


    /**
     * 查询部门管理数据
     * @param map
     * @return
     */
    public Integer selectDeptListCount(Map map);


    /**
     * 获取当月系统登录IP总数
     * @param map
     * @return
     */
    public List<Map<String, String>> selectLoginCount(Map map);


    /**
     * 授信修改
     * @param map
     * @return
     */
    public int updCredit(Map map);

    /**
     *预存修改
     * @param map
     * @return
     */
    public int updDepositAmount(Map map);

    /**
     * 利润修改
     * @param map
     * @return
     */
    public int updProfitAmount(Map map);


    /**
     * 查询企业下 金额数据
     * @param map
     * @return
     */
    public Map<String, Object> findDeptAmount(Map map);




    public List<Map<String, String>> findPreDepositDeduction(Map map);


}
