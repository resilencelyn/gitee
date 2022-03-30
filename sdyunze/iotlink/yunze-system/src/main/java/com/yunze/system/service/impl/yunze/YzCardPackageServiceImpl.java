package com.yunze.system.service.impl.yunze;


import com.yunze.common.annotation.DataScope;
import com.yunze.common.mapper.yunze.YzAgentPackageMapper;
import com.yunze.common.mapper.yunze.YzAgentPacketMapper;
import com.yunze.common.mapper.yunze.YzCardPackageMapper;
import com.yunze.common.mapper.yunze.YzCardPacketMapper;
import com.yunze.common.utils.yunze.Different;
import com.yunze.common.utils.yunze.PageUtil;
import com.yunze.common.utils.yunze.VeDate;
import com.yunze.system.service.yunze.IYzCardPackageService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;
import org.springframework.transaction.interceptor.TransactionAspectSupport;

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
public class YzCardPackageServiceImpl implements IYzCardPackageService
{

    @Autowired
    private YzCardPackageMapper cardPackageMapper;
    @Autowired
    private YzAgentPackageMapper agentPackageMapper;
    @Autowired
    private YzCardPacketMapper cardPacketMapper;
    @Autowired
    private YzAgentPacketMapper agentPacketMapper;


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
            package_idArr = cardPacketMapper.getPackage_id(getPacketMap);
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
        List<String> CountArr = agentPackageMapper.selMapCount(CountMap);
        Integer rowCount = CountArr!=null && CountArr.size()>0?CountArr.size():0;

        PageUtil pu=new PageUtil(rowCount,currenPage , pageSize);//初始化分页工具类
        map.put("StarRow", pu.getStarRow());
        map.put("PageSize", pu.getPageSize());
        omp.put("Pu", pu);
        List<Map<String,Object>> cardPackageArr = cardPackageMapper.selMap(map);
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
            omp.put("PackeData", cardPacketMapper.FindList(map));
        }

        return omp;
    }

    @Override
    public boolean add(Map<String, Object> map) {
        map.put("package_id","YZ"+ VeDate.getNo(4));
        return cardPackageMapper.add(map)>0;
    }

    @Override
    public Map<String, Object> find(Map<String, Object> map) {
        List<Map<String, Object>> Rarr = cardPackageMapper.find(map);
        if(Rarr!=null && Rarr.size()>0){
            return Rarr.get(0);
        }else{
            return null ;
        }
    }

    @Override
    public List<Map<String, Object>> findPackage(Map<String, Object> map) {
        List<String> agent_id = (List<String>) map.get("agent_id");
        if(Different.Is_existence(agent_id,"100")){
            return cardPackageMapper.find(map);
        }else{
            return  agentPackageMapper.find(map);
        }
    }

    @Override
    public boolean update(Map<String, Object> map) {
        return cardPackageMapper.update(map)>0;
    }

    @Override
    @Transactional
    public String tariffDivision(Map<String, Object> map) {

        boolean bool = false,bool_Package = false,bool_Packet = false;

        String ParentDept_id = map.get("ParentDept_id").toString();//操作人所属企业 id
        try {
            int isPackageCount = agentPackageMapper.isExist(map);
            int isPacketCount = agentPacketMapper.isExist(map);

            if(isPackageCount==0 && isPacketCount==0){//新增

                if(ParentDept_id.equals("100")){//平台划分到企业
                    bool_Package =  agentPackageMapper.add(map)>0;
                    bool_Packet =  agentPacketMapper.add(map)>0;
                }else{
                    //企业 划分到 用户
                    bool_Package =  agentPackageMapper.addAgent(map)>0;
                    bool_Packet =  agentPacketMapper.addAgent(map)>0;
                }
            }else if(isPackageCount>0 && isPacketCount==0){
                if(ParentDept_id.equals("100")) {//平台划分到企业
                    bool = agentPacketMapper.add(map)>0;
                }else{
                    bool = agentPacketMapper.addAgent(map)>0;
                }
            }else if(isPackageCount>0 && isPacketCount>0){//修改 查询系统 现有最新数据 删除 本次更新 必填字段
                String dept_id = map.get("dept_id").toString();
                String user_id = map.get("user_id").toString();
                String agent_id = map.get("agent_id").toString();
                String package_id = map.get("package_id").toString();
                String id = map.get("id").toString();


                //修改时 dept_id  user_id  agent_id 做成 list 数据 作为查询条件
                map.remove("user_id");
                map.remove("dept_id");
                map.remove("agent_id");
                map.remove("id");//查询 资费组 时 把id去掉 是用来 查询 资费计划的

              /*  List<String>  dept_idArr = new ArrayList<String>();
                dept_idArr.add(dept_id.toString());
                List<String>  agent_idArr = new ArrayList<String>();
                agent_idArr.add(agent_id.toString());
                map.put("dept_id",dept_idArr);
                map.put("agent_id", agent_idArr);*/
                List<String>  package_idArr = new ArrayList<String>();
                package_idArr.add(package_id);
                map.put("package_id",package_idArr);//资费计划 用
                map.put("package_idArr",package_idArr);//资费 组用

                List<Map<String, Object>> PackagArr = null;
                //判断是否为 平台划分到企业
                PackagArr = ParentDept_id.equals("100")? cardPackageMapper.find(map):agentPackageMapper.find(map);

                System.out.println(PackagArr);
                Map<String, Object> Package =  PackagArr.get(0);
                Package.put("dept_id",dept_id);
                map.put("id",id);
                //删除 获取数据 更新 必要字段

                //判断是否为 平台划分到企业
                List<Map<String, Object>> PacketArr = null;
                PacketArr = ParentDept_id.equals("100")? cardPacketMapper.find(map):agentPacketMapper.find(map);

                System.out.println(PacketArr);
                Map<String, Object> Packet =  PacketArr.get(0);

                Packet.remove("error_flow");
                Packet.remove("error_so");
                Packet.remove("packet_cost");
                Packet.remove("dept_id");
                Packet.remove("user_id");

                Packet.put("set_error_so",map.get("set_error_so"));
                Packet.put("set_packet_cost",map.get("set_packet_cost"));
                Packet.put("dept_id",dept_id);
                Packet.put("user_id",user_id);
                Packet.put("set_packet_price",map.get("set_packet_price"));


                bool_Package =  agentPackageMapper.updatePackage(Package)>0;
                bool_Packet =  agentPacketMapper.updatePacket(Packet)>0;
            }
            if(bool_Package==true && bool_Packet==true){
                bool = true;
            }
            if(!bool){
                TransactionAspectSupport.currentTransactionStatus().setRollbackOnly();
                System.out.println("平台划分资费 操作回滚 》 tariffDivision bool_Package : "+bool_Package+" bool_Packet ： "+bool_Packet);
            }else{
                return  ParentDept_id.equals("100")?"平台划分资费 操作成功！":"企业划分资费 操作成功！";
            }

        }catch (Exception e){
            TransactionAspectSupport.currentTransactionStatus().setRollbackOnly();
            System.out.println("平台划分资费 异常 操作回滚 》 tariffDivision "+e.toString());
        }
        return "平台划分资费 异常 ! ";
    }
}
