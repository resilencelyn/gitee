package com.yunze.common.mapper.yunze;


import java.util.List;
import java.util.Map;


/**
 * 卡板信息 数据层
 *
 * @author root
 */
public interface YzCardMapper
{
    /**
     *查询
     * @param map
     * @return
     */
    public List<Map<String,Object>> selMap(Map<String, Object> map);

    /**
     * 查询总数
     * @param map
     * @return
     */
    public Integer selMapCount(Map<String, Object> map);


    /**
     *查询 [切割iccid 19 位]
     * @param map
     * @return
     */
    public List<Map<String,Object>> selMapLianTong(Map<String, Object> map);

    /**
     * 查询总数 [切割iccid 19 位]
     * @param map
     * @return
     */
    public Integer selMapLianTongCount(Map<String, Object> map);


    /**
     * 查询单卡信息
     * @param map
     * @return
     */
    public Map<String,Object> find(Map<String, Object> map);

    /**
     * 查询单条 卡板数据 数据 不关联 Route
     * @param map
     * @return
     */
    public Map<String,Object> findNotRoute(Map<String, Object> map);


    /**
     * 导入卡板数据
     */
    public int importCard(Map<String, Object> map);


    /**
     * 查询单卡信息
     * @param map
     * @return
     */
    public List<String> isExistence(Map<String, Object> map);


    /**
     * 查询目前最大自定义号码
     * @return
     */
    public String findMaxVid();


    /**
     * 查询 卡号 分配 通道 状态
     * @param map
     * @return
     */
    public Map<String, Object> findRoute(Map<String, Object> map);



    /**
     *按当前赛选条件获取导出数据
     * @param map
     * @return
     */
    public List<Map<String,Object>> outCard(Map<String, Object> map);


    /**
     *按当前赛选条件获取导出数据 [切割iccid 19 位]
     * @param map
     * @return
     */
    public List<Map<String,Object>> outCardLianTong(Map<String, Object> map);


    /**
     *按当前赛选条件获取导出数据 iccid
     * @param map
     * @return
     */
    public List<String> outCardIccid(Map<String, Object> map);



    /**
     *按当前赛选条件获取导出数据 iccid  [切割iccid 19 位]
     * @param map
     * @return
     */
    public List<String> outCardIccidLianTong(Map<String, Object> map);


    /**
     * 获取匹配筛选条件下的卡板id
     * @param map
     * @return
     */
    public List<String> selId(Map<String, Object> map);



    /**
     * 获取匹配筛选条件下的卡板id iccid  [切割iccid 19 位]
     * @param map
     * @return
     */
    public List<String> selIdLianTong(Map<String, Object> map);

    /**
     * 卡所属划分
     * @param map
     * @return
     */
    public int dividCard(Map<String, Object> map);


    /**
     * 通道下 已有使用量的卡 且 剩余 用量>0
     * @param map
     * @return
     */
    public List<Map<String,Object>> channelCardCommonly(Map<String, Object> map);

    /**
     * 通道下 超出用量 和 暂未使用用量查询 Serious  超出用量的 ， Slight 没用量的
     * @param map
     * @return
     */
    public List<Map<String,Object>> channelCardSeriousAndSlight(Map<String, Object> map);


    /**
     * 获取某通道下大于百分之多少的 卡信息  percentage 0.8 = 80%
     * @param map
     * @return
     */
    public List<Map<String,Object>> findPercentage(Map<String, Object> map);


    /**
     * 修改 激活时间
     * @param map
     * @return
     */
    public int updActivate(Map<String, Object> map);


    /**
     * 激活时间为空得的 iccid
     * @param map
     * @return
     */
    public List<Map<String,Object>> activate_dateNull(Map<String, Object> map);


    /**
     * 修改卡状态
     * @param map
     * @return
     */
    public int updStatusId(Map<String, Object> map);

    /**
     * 修改 批量 卡状态
     * @param map
     * @return
     */
    public int updStatusIdArr(Map<String, Object> map);


    public int updction(Map<String, Object> map);

    /**
     * 查询通道下 所属卡
     * @param map
     * @return
     */
    public List<Map<String,Object>> findChannelIdCar(Map<String, Object> map);


    /**
     * 修改 卡用量
     * @param map
     * @return
     */
    public int updUsed(Map<String, Object> map);


    /**
     * 通道下 所属卡 设置达量停机的卡
     * @param map
     * @return
     */
    public  List<Map<String,Object>>  findChannelIdCarStop(Map<String, Object> map);


    /**
     * 备份 卡所属关联
     * @return
     */
    public List<Map<String,Object>> BackupAssociate();


    /**
     * 查询单卡激活时间
     * @param map
     * @return
     */
    public String findActivateDate(Map<String, Object> map);


    /**
     * yz_card_info 新增 剩余
     * @param map
     * @return
     */
    public int addInfoRemaining(Map<String, Object> map);

    /**
     * 连接管理 设置时 备份 卡所属关联
     * @param map
     * @return
     */
    public List<Map<String,Object>> SetBackupAssociate(Map<String, Object> map);


    /**
     * 连接管理设置 修改
     * @param map
     * @return
     */
    public int SetCard(Map<String, Object> map);


    /**
     * 修改 imei
     * @param map
     * @return
     */
    public int updimei(Map<String, Object> map);




    /**
     * 查询 卡号 分配 通道
     * @param map
     * @return
     */
    public List<Map<String,Object>> findRouteArr(Map<String, Object> map);

    /**
     *月初有订购期内流量卡 进行 复机
     * */
    public List<Map<String,Object>> StatusChange(Map map);



    /**
     * 通道下 所属卡 设置 未订购资费断网 的卡
     * @param map
     * @return
     */
    public  List<Map<String,Object>>  findChannelIdDisconnected(Map<String, Object> map);


    /**
     * 查询字典信息
     * @param map
     * @return
     */
    public List<Map<String,Object>> findDict(Map<String, Object> map);

    /**
     * 连接管理设置 修改
     * @param map
     * @return
     */
    public int updConnectionStatus(Map<String, Object> map);

    /**
     * 查询使用降序查询时间
     * */
    public Map<String,Object> selTime(Map<String, Object> map);

    /**
     * 新增
     * @param map
     * @return
     * @throws Exception
     */
    public List<Map<String,Object>> addsel(Map map);



    /**
     * 查询 代理 所属卡
     * @param map
     * @return
     */
    public List<String> findBelongingCard(Map<String, Object> map);


    /**
     * 修改 分组 备注
     * @param map
     * @return
     */
    public List<Map<String,Object>> findGroupingAndRemarks(Map<String, Object> map);


    /**
     * 修改分组 备注
     * @param map
     * @return
     */
    public int updGroupingAndRemarks(Map<String, Object> map);


    /**
     * 查询 代理下 卡号
     * @param map
     * @return
     */
    public List<String> findAgentIccid(Map<String, Object> map);




    /**
     * 查询下级代理商 返回 格式 1,2
     * @param map
     * @return
     */
    public String queryChildrenAreaInfo(Map<String, Object> map);


    /**
     * 获取 所属代理下 卡分组
     * @param map
     * @return
     */
    public List<String> getCardGrouping(Map<String, Object> map);


    /**
     * 查询 销售 合伙人
     * @param map
     * @return
     */
    public List<Map<String,Object>> findSalesPartner(Map<String, Object> map);

    /**
     * C端充值 轮询 卡状态变更失败的 进行再次 复机；（前提是没用完用量）
     * */
    public List<Map<String,Object>> SleMulti (Map map);

    /**
     * 划卡回滚
     * @param map
     * @return
     */
    public int dividCardBk(Map<String, Object> map);



    /**
     * 修改 备注 分组
     */
    public int UpdateFill(Map<String, Object> map);

    /**
     * 同步时间
     */
    public int updDueExpireTime(Map<String, Object> map);


    /**
     * 用量详情导出 （同步导出 分组 、备注、发货时间）
     * */
    public List<Map<String,Object>> hisList (Map map);


    /**
     * 查询已订购用量状态异常卡号
     * @param map
     * @return
     */
    public List<Map<String,Object>> findStatusAbnormal (Map map);


    /**
     * 卡状态 分组 统计
     * @param map
     * @return
     */
    public List<Map<String,Object>> status_ShowIdGroup (Map map);


    /**
     * 获取 vid
     * @param map
     * @return
     */
    public List<Map<String,Object>> selVid (Map map);


}













