package com.yunze.quartz.task.yunze.backups;

import com.alibaba.fastjson.JSON;
import com.yunze.common.mapper.yunze.YzCardMapper;
import com.yunze.common.utils.yunze.VeDate;
import com.yunze.common.utils.yunze.WriteCSV;
import org.springframework.stereotype.Component;

import javax.annotation.Resource;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

/**
 * 回滚 划卡备份
 */
@Component("rollbackCardagent")
public class RollbackCardagent {
    @Resource
    private YzCardMapper yzCardMapper;
    @Resource
    private WriteCSV writeCSV;


    /**
     * 读取  划卡备份 回滚 属性
     * @param Url 文件地址
     * @param error_agent_id 错误的 agent_id
     * @param error_user_id 错误的 user_id
     */
    public void rollbackError(String Url,Integer error_agent_id,Integer error_user_id){

        List<Map<String,Object>>   dataList = WriteCSV.readCSV(Url,null);

        //分组 更具 dept_id-user_id
        Map<String, List<Map<String,Object>>> collect = dataList.stream().collect(Collectors.groupingBy(scope->scope.get("dept_id").toString()+'-'+scope.get("user_id").toString()));
        System.out.println(JSON.toJSONString(collect));
        System.out.println("==========================");
        int UpdCpunt = 0 ;
        for(Map.Entry<String, List<Map<String, Object>>> entry:collect.entrySet()){
            String Keys[] = entry.getKey().split("-");
            List<Map<String, Object>>  idArr = entry.getValue();
            String dept_id = Keys[0];
            String user_id = Keys[1];
            Map<String,Object> setMap = new HashMap<>();
            setMap.put("dept_id",dept_id);
            setMap.put("user_id",user_id);
            setMap.put("error_agent_id",error_agent_id);
            setMap.put("error_user_id",error_user_id);
            setMap.put("idArr",idArr);
            int CUpdCpunt = yzCardMapper.dividCardBk(setMap);
            UpdCpunt += CUpdCpunt;
            System.out.println(" 总修改 ："+UpdCpunt+" ，本次 【"+ VeDate.getStringDateSSS()+"】 ["+entry.getKey()+"] 修改 "+CUpdCpunt+" idArr.size() "+idArr.size());
        }

        System.out.println(" 修改完成 "+VeDate.getStringDateSSS()+"  UpdCpunt "+UpdCpunt);
    }

    public static void main(String[] args) {
        RollbackCardagent init = new RollbackCardagent();
        //init.initMQConfig();
    }


}
