package com.yunze.system.service.impl.yunze;


import com.yunze.common.annotation.DataScope;
import com.yunze.common.mapper.yunze.YzAgentPackageMapper;
import com.yunze.common.mapper.yunze.YzAgentPacketMapper;
import com.yunze.common.utils.yunze.Different;
import com.yunze.common.utils.yunze.PageUtil;
import com.yunze.common.utils.yunze.VeDate;
import com.yunze.system.service.yunze.IYzAgentPackageService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import javax.annotation.Resource;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * 资费组 业务实现类
 * 
 * @author root
 */
@Service
public class YzAgentPackageServiceImpl implements IYzAgentPackageService
{

   
    @Resource
    private YzAgentPackageMapper yzAgentPackageMapper;
    @Resource
    private YzAgentPacketMapper yzAgentPacketMapper;

    @Override
    @DataScope(deptAlias = "d" , userAlias = "a" , isMap=true)
    public Map<String, Object> selMap(Map<String, Object> map) {
        Map<String, Object> omp=new HashMap<String, Object>();

        //查询数据前过滤 资费计划 条件数据 判断 是否选择资费计划 查询条件未选择时 不做条件限制
        Object packetType = map.get("packetType");
        Object packetValue = map.get("packetValue");
        boolean Sel = false;
        if(packetValue!=null  && packetType !=null && packetValue.toString().trim().length()>0 && packetType.toString().trim().length()>0){
            Sel = true;
        }
        if(!Sel){
            List<String> packet_valid_type = (List<String>) map.get("packet_valid_type");
            List<String> in_stock = (List<String>) map.get("in_stock");
            List<String> is_month = (List<String>) map.get("is_month");
            List<String> balance_pay = (List<String>) map.get("balance_pay");
            List<String> wechat_pay = (List<String>) map.get("wechat_pay");
            Sel = packet_valid_type!=null && packet_valid_type.size()>0?true:Sel;
            if(!Sel){ Sel = in_stock!=null && in_stock.size()>0?true:Sel;}
            if(!Sel){ Sel = is_month!=null && is_month.size()>0?true:Sel;}
            if(!Sel){ Sel = balance_pay!=null && balance_pay.size()>0?true:Sel;}
            if(!Sel){ Sel = wechat_pay!=null && wechat_pay.size()>0?true:Sel;}
        }

        List<String> package_idArr = null;
        if(Sel){
            Map<String, Object> getPacketMap = map ;
            getPacketMap.remove("pageNum");
            getPacketMap.remove("pageSize");
            package_idArr = yzAgentPacketMapper.getPackage_id(getPacketMap);
            map.put("package_idArr",package_idArr);
            if(package_idArr!=null && package_idArr.size()>0){
            }else{
                omp.put("Pu", new PageUtil(1,1 , 10));
                omp.put("Data", new ArrayList<Map<String,Object>>());
                return  omp;//选择了 资费计划 查询条件 5r未查询到 资费计划时 直接跳出查询返回 null
            }
        }

        Integer currenPage=map.get("pageNum")!=null?Integer.parseInt(map.get("pageNum").toString()):0;
        Integer pageSize=map.get("pageSize")!=null?Integer.parseInt(map.get("pageSize").toString()):10;

        Map<String, Object> CountMap = new HashMap<>();
        CountMap.putAll(map);
        CountMap.remove("pageNum");
        CountMap.remove("pageSize");
        List<String> CountArr = yzAgentPackageMapper.selMapCount(CountMap);
        Integer rowCount = CountArr!=null && CountArr.size()>0?CountArr.size():0;

        PageUtil pu=new PageUtil(rowCount,currenPage , pageSize);//初始化分页工具类
        map.put("StarRow", pu.getStarRow());
        map.put("PageSize", pu.getPageSize());
        omp.put("Pu", pu);
        List<Map<String,Object>> cardPackageArr = yzAgentPackageMapper.selMap(map);
        omp.put("Data", cardPackageArr);
        //如果查询到了资费组信息 获取 资费计划
        if(cardPackageArr!=null && cardPackageArr.size()>0){
            List<String> package_id = new ArrayList<String>();
            //获取查询的 资费组 ID
            for (int i = 0; i <cardPackageArr.size() ; i++) {
                package_id.add(cardPackageArr.get(i).get("package_id").toString());
            }
            map.put("package_id",package_id);
            //添加  资费组 ID 查询条件
            map.put("package_id",package_id);
            map.remove("pageNum");
            map.remove("pageSize");
            omp.put("PackeData", yzAgentPacketMapper.FindList(map));
        }

        return omp;
    }

    @Override
    public boolean add(Map<String, Object> map) {
        map.put("package_id","YZ"+ VeDate.getNo(4));
        return yzAgentPackageMapper.add(map)>0;
    }

    @Override
    public Map<String, Object> find(Map<String, Object> map) {
        List<Map<String, Object>> Rarr = yzAgentPackageMapper.find(map);
        if(Rarr!=null && Rarr.size()>0){
            return yzAgentPackageMapper.find(map).get(0);
        }else{
            return null ;
        }
    }

    @Override
    public List<Map<String, Object>> findPackage(Map<String, Object> map) {
        List<String> agent_id = (List<String>) map.get("agent_id");
        if(Different.Is_existence(agent_id,"100")){
            return yzAgentPackageMapper.find(map);
        }else{
            return  yzAgentPackageMapper.find(map);
        }
    }

    @Override
    public boolean update(Map<String, Object> map) {
        return yzAgentPackageMapper.update(map)>0;
    }
}
