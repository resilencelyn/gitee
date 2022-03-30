package com.yunze.system.service.impl.yunze;


import com.alibaba.fastjson.JSON;
import com.rabbitmq.client.BuiltinExchangeType;
import com.yunze.common.config.RabbitMQConfig;
import com.yunze.common.core.domain.entity.SysDictData;
import com.yunze.common.core.domain.entity.SysUser;
import com.yunze.common.mapper.yunze.*;
import com.yunze.common.utils.yunze.Different;
import com.yunze.common.utils.yunze.PageUtil;
import com.yunze.common.utils.yunze.Upload;
import com.yunze.system.mapper.SysDictDataMapper;
import com.yunze.system.mapper.SysUserMapper;
import com.yunze.system.service.yunze.IYzOrderService;
import com.yunze.system.service.yunze.IYzUserService;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.amqp.rabbit.core.RabbitTemplate;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.web.multipart.MultipartFile;

import javax.annotation.Resource;
import java.io.File;
import java.io.IOException;
import java.util.*;

/**
 * 卡信息 业务实现类
 *
 * @author root
 */
@Service
public class YzOrderServiceImpl implements IYzOrderService
{
    private static final Logger log = LoggerFactory.getLogger(YzOrderServiceImpl.class);

    @Resource
    private IYzUserService iYzUserService;
    @Resource
    private YzOrderMapper yzOrderMapper;
    @Autowired
    private RabbitTemplate rabbitTemplate;
    @Autowired
    private RabbitMQConfig mQConfig;
    @Resource
    private YzCardFlowMapper cardFlowMapper;
    @Resource
    private SysDictDataMapper dictDataMapper;
    @Autowired
    private SysUserMapper userMapper;
    @Resource
    private RabbitMQConfig rabbitMQConfig;

    @Override
    public Map<String, Object> selMap(Map<String, Object> map) {
        Map<String, Object> omp=new HashMap<String, Object>();
        Integer currenPage=map.get("pageNum")!=null?Integer.parseInt(map.get("pageNum").toString()):0;
        Integer pageSize=map.get("pageSize")!=null?Integer.parseInt(map.get("pageSize").toString()):10;
        //权限过滤
        if(map.get("agent_id")!=null){
            List<Integer> agent_id = (List<Integer>) map.get("agent_id");
            if(!Different.Is_existence(agent_id,100)){
                List<String>  user_id =  iYzUserService.getUserID(map);
                map.put("user_id",user_id);
            }
        }
        PageUtil pu = null;
        List<Map<String,Object>> Rlist = null;
        Integer  rowCount = yzOrderMapper.selMapCount(map);
        rowCount=rowCount!=null?rowCount:0;
        pu = new PageUtil(rowCount,currenPage , pageSize);//初始化分页工具类
        map.put("StarRow", pu.getStarRow());
        map.put("PageSize", pu.getPageSize());
        Rlist = yzOrderMapper.selMap(map);
        omp.put("Pu", pu);
        omp.put("Data",Rlist );
        return omp;
    }

    @Override
    public Map<String, Object> find(Map<String, Object> map) {

        return null;
    }

    @Override
    public List<String> outOrder(Map<String, Object> map) {
        return null;
    }

    @Override
    public Map<String, Object> findOrder(Map<String, Object> map) {
        return yzOrderMapper.findOrder(map);
    }



    @Override
    public String importRecharge(MultipartFile file,Map<String, Object> map) throws IOException {
        String filename=file.getOriginalFilename();
        String ReadName = UUID.randomUUID().toString().replace("-","")+filename;
        String newName = UUID.randomUUID().toString().replace("-","")+"_importRecharge";
        try {
            // 获取当前项目的工作路径
            File file2 = new File("");
            String filePath = file2.getCanonicalPath();
            File newFile = new File(filePath+"/upload/importRecharge/"+ReadName);
            File Url=new File(filePath+"/upload/importRecharge/1.txt");//tomcat 生成路径
            Upload.mkdirsmy(Url);
            file.transferTo(newFile);
            //或充值 资费计划信息
            String agent_id = map.get("agent_id").toString();
            Map<String,Object> PacketMap = null;
            if(agent_id.equals("100")){
                PacketMap = cardFlowMapper.FindPacket(map);
            }else{
                PacketMap = cardFlowMapper.FindPacketAgent(map);
            }
            if(PacketMap!=null){
                String ord_type = "2";
                //添加订单生成参数
                Map<String, Object> OrderMap = new HashMap<>();
                PacketMap.put("validate_type",map.get("validate_type"));//生效类型
                PacketMap.put("ord_type",ord_type);//订单类型

                OrderMap.put("add_parameter",JSON.toJSONString(PacketMap));//加包参数
                OrderMap.put("ord_type",ord_type);//订单类型 用量充值
                OrderMap.put("ord_name","平台批量充值");//交易名称 用量充值
                OrderMap.put("wx_ord_no",null);//微信订单号
                OrderMap.put("status","1");//支付状态 支付成功
                OrderMap.put("price",PacketMap.get("packet_price"));//交易金额
                OrderMap.put("account","0");//交易后金额
                OrderMap.put("pay_type","s");//支付方式 平台记录
                OrderMap.put("is_profit","0");//是否需要分润 0否-1是
                OrderMap.put("add_package","0");//是否已经执行过加包 0否 1 是
                OrderMap.put("show_status","0");//是否显示交易记录:0否-1显示
                OrderMap.put("packet_id",map.get("packet_id"));//资费计划 ID
                OrderMap.put("validate_type",map.get("validate_type"));//生效类型
                OrderMap.put("open_id",null);//openid 微信支付记录操作人
                OrderMap.put("agent_id",agent_id);// 所属记录企业id
                OrderMap.put("profit_type","0");//分润类型
                OrderMap.put("info",null);//备注
                OrderMap.put("cre_type","sys");//生成方式  系统 sys 人工 ai

                map.put("OrderMap",OrderMap);

                //1.创建路由 绑定 生产队列 发送消息
                //路由队列
                String addOrder_exchangeName = "admin_exchange", addOrder_queueName = "admin_OrderImportRecharge_queue", addOrder_routingKey = "admin.OrderImportRecharge.queue",
                        addOrder_del_exchangeName = "dlx_"+addOrder_exchangeName,addOrder_del_queueName = "dlx_"+addOrder_queueName, addOrder_del_routingKey = "dlx_"+addOrder_routingKey;
                try {
                    mQConfig.creatExchangeQueue(addOrder_exchangeName, addOrder_queueName, addOrder_routingKey, addOrder_del_exchangeName, addOrder_del_queueName, addOrder_del_routingKey,null);
                    Map<String, Object> start_type = new HashMap<>();
                    start_type.put("filePath", filePath);//项目根目录
                    start_type.put("ReadName", ReadName);//上传新文件名
                    start_type.put("newName", newName);//输出文件名
                    start_type.put("map", map);//参数
                    rabbitTemplate.convertAndSend(addOrder_exchangeName, addOrder_routingKey, JSON.toJSONString(start_type), message -> {
                        // 设置消息过期时间 60 分钟 过期
                        message.getMessageProperties().setExpiration("" + (60 * 1000 * 60));
                        return message;
                    });
                } catch (Exception e) {
                    System.out.println("平台导入充值生产指令  失败 " + e.getMessage().toString());
                    return ("平台导入充值生产指令 操作失败！");
                }
            }else{
                return ("未找到 您的名下 资费计划ID 【"+map.get("packet_id")+"】 充值取消! ");
            }
        }catch (Exception e){
            System.out.println(e);
            return "上传excel异常";
        }
        return "资费订购 平台导入充值指令 已发送，充值详细信息请在 【执行任务管理】查询！";
    }

    @Override
    public String exportallorders(Map<String, Object> map, SysUser User) {
        Object MapAgent_id = map.get("agent_id");
        //导出时 未选中 当前 企业编号时 且登录 部门不是 总平台 赋值部门
        if(MapAgent_id==null && User.getDeptId()!=100){
            List<String> agent_idArr = new ArrayList<String>();
            agent_idArr.add(""+User.getDeptId());
            map.put("agent_id",agent_idArr);
        }
        map.remove("pageNum");
        map.remove("pageSize");
        List<Map<String, Object>> outCardIccidArr = null;
        //权限过滤
        if(map.get("agent_id")!=null){
            List<Integer> agent_id = (List<Integer>) map.get("agent_id");
            if(!Different.Is_existence(agent_id,100)){
                List<String>  user_id =  iYzUserService.getUserID(map);
                map.put("user_id",user_id);
            }
        }
        outCardIccidArr = yzOrderMapper.exportallorders(map);

        if(outCardIccidArr!=null && outCardIccidArr.size()>0){
            String  create_by = " [ "+User.getDept().getDeptName()+" ] - "+" [ "+User.getUserName()+" ] ";
            String newName = UUID.randomUUID().toString().replace("-","")+"_Exportallorders";

            String  agent_id = User.getDept().getDeptId().toString();
            String task_name = create_by+"-订单管理 [导出] ";
            String SaveUrl = "/getcsv/"+newName+".csv";

            Map<String, Object> task_map = new HashMap<String, Object>();
            task_map.put("auth",create_by);
            task_map.put("task_name",task_name);
            task_map.put("url",SaveUrl);
            task_map.put("agent_id", agent_id);
            task_map.put("type", "11");

            //获取字典信息
            List<SysDictData> orderArr = dictDataMapper.selectDictDataByType("yunze_order_status");//订单状态
            List<SysDictData> pay_type = dictDataMapper.selectDictDataByType("yunze_card_pay_type");//支付方式
            List<SysDictData> customize_whether = dictDataMapper.selectDictDataByType("yunze_customize_whether");//自定义是否
            List<SysDictData> card_takeEffect = dictDataMapper.selectDictDataByType("yunze_card_takeEffect_type");//资费计划生效类别

            //获取 企业信息
            List<Map<String,Object>> arrorders = yzOrderMapper.arrorders(map);
            List<Map<String,Object>> deptArr = userMapper.find_flow();

            //1.创建路由 绑定 生产队列 发送消息
            //导卡 路由队列
            String polling_queueName = "admin_Exportallorders_queue";
            String polling_routingKey = "admin.Exportallorders.queue";
            String polling_exchangeName = "admin_exchange";//路由
            try {
                rabbitMQConfig.creatExchangeQueue(polling_exchangeName, polling_queueName, polling_routingKey, null, null, null, BuiltinExchangeType.DIRECT);
                Map<String, Object> start_type = new HashMap<>();
                start_type.put("type", "importCardData");//启动类型
                start_type.put("newName", newName);//输出文件名
                start_type.put("task_map", task_map);//
                start_type.put("create_by", create_by);//
                start_type.put("User", User);
                start_type.put("outCardIccidArr",outCardIccidArr);
                start_type.put("deptArr",deptArr);
                start_type.put("arrorders",arrorders);
                start_type.put("stateOptions", orderArr);
                start_type.put("pay_type",pay_type);
                start_type.put("customize_whether",customize_whether);
                start_type.put("cardConnection_type",card_takeEffect);
                start_type.put("map",map);

                rabbitTemplate.convertAndSend(polling_exchangeName, polling_routingKey, JSON.toJSONString(start_type), message -> {
                    // 设置消息过期时间 30 分钟 过期
                    message.getMessageProperties().setExpiration("" + (30 * 1000 * 60));
                    return message;
                });
            } catch (Exception e){
                System.out.println("导入 卡号 失败 " + e.getMessage().toString());
                return "全部订单 导入 操作失败！";
            }
        }else {
            return "您当前的筛选的查询条件 未找到数据！导出任务取消！";
        }
        return "已下发执行任务可在【设备管理】》【执行任务管理】查看";
    }


}






















