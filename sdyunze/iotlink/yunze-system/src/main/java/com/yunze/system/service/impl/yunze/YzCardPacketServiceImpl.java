package com.yunze.system.service.impl.yunze;


import com.yunze.common.mapper.yunze.YzAgentPacketMapper;
import com.yunze.common.mapper.yunze.YzCardPacketMapper;
import com.yunze.common.utils.yunze.VeDate;
import com.yunze.system.service.yunze.IYzCardPacketService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * 资费计划 业务实现类
 * 
 * @author root
 */
@Service
public class YzCardPacketServiceImpl implements IYzCardPacketService
{

    @Autowired
    private YzCardPacketMapper cardPacketMapper;
    @Autowired
    private YzAgentPacketMapper agentPacketMapper;


    @Override
    public boolean add(Map<String, Object> map) {
        String package_id = map.get("package_id").toString();
        package_id = package_id.substring(package_id.length()-4,package_id.length());//后四位 作为 资费计划 的 编码头
        map.put("packet_id","YZ"+package_id+ VeDate.getNo(4));
        return cardPacketMapper.add(map)>0;
    }

    @Override
    public Map<String, Object> find(Map<String, Object> map) {
        List<Map<String, Object>> Rarr = cardPacketMapper.find(map);
        if(Rarr!=null && Rarr.size()>0){
            return Rarr.get(0);
        }else{
            return null ;
        }
    }

    @Override
    public boolean update(Map<String, Object> map) {
        return cardPacketMapper.update(map)>0;
    }

    @Override
    public Map<String, Object> findPacket(Map<String, Object> map) {
        String agent_idStr = map.get("agent_idStr").toString();
        List<String> agent_id =new ArrayList<>();
        agent_id.add(agent_idStr);
        map.put("agent_id",agent_id);
        Map<String, Object> Rmap = new HashMap<String, Object>();
        if(agent_idStr.equals("100")){
            Rmap.put("Packet",cardPacketMapper.findPacket(map));
            Rmap.put("agentPacket",agentPacketMapper.findExistence(map));
        }else{
            Rmap.put("Packet",agentPacketMapper.findPacket(map));
            Rmap.put("agentPacket",agentPacketMapper.findExistence(map));
        }
        return Rmap;
    }
}
