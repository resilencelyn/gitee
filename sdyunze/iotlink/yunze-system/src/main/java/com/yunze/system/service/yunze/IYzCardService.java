package com.yunze.system.service.yunze;


import com.yunze.common.core.domain.entity.SysUser;
import org.springframework.web.multipart.MultipartFile;

import java.io.IOException;
import java.util.List;
import java.util.Map;

/**
 * 卡板信息 业务层
 * @author root
 */
public interface IYzCardService
{
    /**
     *查询
     * @param map
     * @return
     */
    public Map<String,Object> selMap(Map<String, Object> map);


    /**
     * 获取单卡详情
     * @param map
     * @return
     */
    public Map<String,Object> find(Map<String, Object> map);

    /**
     * 导入卡信息
     * @param file
     * @param updateSupport
     * @return
     * @throws IOException
     */
    public String uploadCard(MultipartFile file, boolean updateSupport, SysUser User) throws IOException ;




    /**
     * 查询 卡号 分配 通道 状态
     * @param map
     * @return
     */
    public Map<String,Object> findRoute(Map<String, Object> map);

    /**
     * 获取部门名称
     * @return
     */
    public List<Map<String,Object>> getDeptName();



    /**
     * 查询 卡号 分配 通道 状态
     * @param map
     * @return
     */
    public String exportData(Map<String, Object> map,SysUser User);


    /**
     *按当前赛选条件获取导出数据 iccid
     * @param map
     * @return
     */
    public List<String> outCardIccid(Map<String, Object> map);


    /**
     * 卡所属划分
     * @param map
     * @return
     */
    public String dividCard(Map<String, Object> map);


    /**
     * 获取匹配筛选条件下的卡板id
     * @param map
     * @param selLianTong
     * @return
     */
    public List<String> selId(Map<String, Object> map,boolean selLianTong);





    /**
     * 修改卡状态
     * @param map
     * @return
     */
    public  boolean updStatusId(Map<String, Object> map);



    /**
     * 连接管理设置
     * @param file
     * @return
     * @throws IOException
     */
    public String importSet(MultipartFile file, Map<String, Object> map) throws IOException ;

    /**
     * 卡号导入查询
     * @param file
     * @return
     * @throws IOException
     */
    public Map<String, Object> CardNumberImport(MultipartFile file, Map<String, Object> map) throws IOException ;

    /**
     * 特殊操作查询IMEI
     * @param file
     * @return
     * @throws IOException
     */
    public String   importSelImei(MultipartFile file, Map<String, Object> map) throws IOException ;

    /**
     * 批量停复机、断开网
     * @param file
     * @return
     * @throws IOException
     */
    public String  status (MultipartFile file, Map<String, Object> map) throws IOException ;


    /**
     * 特殊操作变更卡分组、备注
     * @param file
     * @return
     * @throws IOException
     */
    public String   importSetCardInfo(MultipartFile file, Map<String, Object> map) throws IOException ;




    /**
     * 获取 所属代理下 卡分组
     * @param map
     * @return
     */
    public List<String> getCardGrouping(Map<String, Object> map);

    /** 修改 备注 分组*/
    public boolean UpdateFill(Map<String, Object> map);


    /**
     * 修改 激活时间
     * @param map
     * @return
     */
    public boolean updActivate(Map<String, Object> map);


    /**
     * 批量 停机
     * @return
     */
    public String stoppedarr(Map<String, Object> map);

    /**
     * 批量 复机
     * @return
     */
    public String machinearr(Map<String, Object> map);

    /**
     * 批量 断网
     * @return
     */
    public String disconnectNetworkarr(Map<String, Object> map);

    /**
     * 批量 开网
     * @return
     */
    public String openNetworkarr(Map<String, Object> map);

    /**
     * 批量 同步用量
     * @return
     */
    public  String consumptionarr(Map<String, Object> map);

    /**
     * 批量 同步用量
     * @return
     */
    public  String publicmethodarr(Map<String, Object> map);

    /**
     * 批量 同步用量
     * @return
     */
    public  String consumptionandstatearr(Map<String, Object> map);











}
