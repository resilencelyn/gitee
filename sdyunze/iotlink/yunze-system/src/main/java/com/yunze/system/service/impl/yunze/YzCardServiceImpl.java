package com.yunze.system.service.impl.yunze;


import com.alibaba.fastjson.JSON;
import com.rabbitmq.client.BuiltinExchangeType;
import com.yunze.common.config.RabbitMQConfig;
import com.yunze.common.core.domain.entity.SysDictData;
import com.yunze.common.core.domain.entity.SysUser;
import com.yunze.common.core.redis.RedisCache;
import com.yunze.common.mapper.yunze.YzCardFlowMapper;
import com.yunze.common.mapper.yunze.YzCardMapper;
import com.yunze.apiCommon.mapper.YzCardRouteMapper;
import com.yunze.common.mapper.yunze.YzExecutionTaskMapper;
import com.yunze.common.utils.yunze.*;
import com.yunze.system.mapper.SysDeptMapper;
import com.yunze.system.mapper.SysDictDataMapper;
import com.yunze.system.mapper.SysUserMapper;
import com.yunze.system.service.yunze.IYzCardService;
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
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.*;
import java.util.concurrent.TimeUnit;

/**
 * 卡信息 业务实现类
 *
 * @author root
 */
@Service
public class YzCardServiceImpl implements IYzCardService {
    private static final Logger log = LoggerFactory.getLogger(YzCardServiceImpl.class);

    @Resource
    private YzCardMapper yzCardMapper;
    @Autowired
    private SysDeptMapper deptMapper;
    @Autowired
    private YzExecutionTaskMapper executionTaskMapper;
    @Autowired
    private YzCardFlowMapper cardFlowMapper;
    @Autowired
    private SysDictDataMapper dictDataMapper;
    @Autowired
    private SysUserMapper userMapper;
    @Resource
    private YzCardRouteMapper yzCardRouteMapper;
    @Resource
    private IYzUserService iYzUserService;
    @Resource
    private RabbitTemplate rabbitTemplate;
    @Resource
    private RabbitMQConfig rabbitMQConfig;
    @Resource
    private RedisCache redisCache;


    @Override
    public Map<String, Object> selMap(Map<String, Object> map) {
        Map<String, Object> omp = new HashMap<String, Object>();
        Integer currenPage = map.get("pageNum") != null ? Integer.parseInt(map.get("pageNum").toString()) : 0;
        Integer pageSize = map.get("pageSize") != null ? Integer.parseInt(map.get("pageSize").toString()) : 10;

        Map<String, Object> CountMap = new HashMap<>();
        CountMap.putAll(map);
        CountMap.remove("queryParams");
        boolean is_Internal = false;
        //System.out.println(CountMap.remove("queryParams"));
        //权限过滤
        if (map.get("agent_id") != null) {
            List<Integer> agent_id = (List<Integer>) map.get("agent_id");
            if (!Different.Is_existence(agent_id, 100)) {
                List<String> user_id = iYzUserService.getUserID(CountMap);
                map.put("user_id", user_id);
                CountMap.put("user_id", user_id);
            } else {
                is_Internal = true;//内部人员 部门是 100 的 可看字段增加
            }
        } else {
            is_Internal = true;//内部人员 部门是 100 的 可看字段增加
        }
        map.put("is_Internal", is_Internal);
        map = getChannelIdArr(map);


        CountMap.put("channel_id", map.get("channel_id"));
        PageUtil pu = null;
        List<Map<String, Object>> Rlist = null;
        boolean selLianTong = (boolean) map.get("selLianTong");

        Integer rowCount = null;
        if (selLianTong) {
            rowCount = yzCardMapper.selMapLianTongCount(CountMap);
        } else {
            rowCount = yzCardMapper.selMapCount(CountMap);
        }
      /*
        //同查询条件 缓存 查询总数 120 S
        Object isExecute = redisCache.getCacheObject(JSON.toJSONString(CountMap));
        if (isExecute == null) {
            if (selLianTong) {
                rowCount = yzCardMapper.selMapLianTongCount(CountMap);
            } else {
                rowCount = yzCardMapper.selMapCount(CountMap);
            }
            //redis 存储
            redisCache.setCacheObject(JSON.toJSONString(CountMap), rowCount, 120, TimeUnit.SECONDS);//120 秒缓存
        } else {
            rowCount = Integer.parseInt(isExecute.toString());
        }*/

        rowCount = rowCount != null ? rowCount : 0;
        pu = new PageUtil(rowCount, currenPage, pageSize);//初始化分页工具类
        map.put("StarRow", pu.getStarRow());
        map.put("PageSize", pu.getPageSize());
        if (selLianTong) {
            Rlist = yzCardMapper.selMapLianTong(map);
        } else {
            Rlist = yzCardMapper.selMap(map);
        }
        //数据打包'
        //System.out.println(map);
        //System.out.println(yzCardMapper.selMap(map));
        omp.put("Pu", pu);
        omp.put("Data", Rlist);
        omp.put("Pmap", map);
        omp.put("is_Internal", is_Internal);
        return omp;

    }

    @Override
    public Map<String, Object> find(Map<String, Object> map) {

        Map<String, Object> findMap = yzCardMapper.find(map);
        if (findMap == null) {
            findMap = yzCardMapper.findNotRoute(map);
        }
        return findMap;
    }


    @Override
    public String uploadCard(MultipartFile file, boolean updateSupport, SysUser User) throws IOException {
        String filename = file.getOriginalFilename();
        String ReadName = UUID.randomUUID().toString().replace("-", "") + filename;
        String newName = UUID.randomUUID().toString().replace("-", "") + "_CardImport";

        try {
            /**
             * 获取当前项目的工作路径
             */
            File file2 = new File("");
            String filePath = file2.getCanonicalPath();
            File newFile = new File(filePath + "/upload/uploadCard/" + ReadName);
            File Url = new File(filePath + "/upload/uploadCard/1.txt");//tomcat 生成路径
            Upload.mkdirsmy(Url);
            file.transferTo(newFile);

            //1.创建路由 绑定 生产队列 发送消息
            //导卡 路由队列
            String polling_queueName = "admin_saveCard_queue";
            String polling_routingKey = "admin.saveCard.queue";
            String polling_exchangeName = "admin_exchange";//路由
            try {
                // rabbitMQConfig.creatExchangeQueue(polling_exchangeName, polling_queueName, polling_routingKey, null, null, null, BuiltinExchangeType.DIRECT);
                Map<String, Object> start_type = new HashMap<>();
                start_type.put("type", "importCardData");//启动类型
                start_type.put("filePath", filePath);//项目根目录
                start_type.put("ReadName", ReadName);//上传新文件名
                start_type.put("newName", newName);//输出文件名
                start_type.put("User", User);//登录用户信息
                rabbitTemplate.convertAndSend(polling_exchangeName, polling_routingKey, JSON.toJSONString(start_type), message -> {
                    // 设置消息过期时间 30 分钟 过期
                    message.getMessageProperties().setExpiration("" + (30 * 1000 * 60));
                    return message;
                });
            } catch (Exception e) {
                System.out.println("导入 卡号 失败 " + e.getMessage().toString());
                return ("连接管理 导入 操作失败！");
            }


            /*String path = filePath + "/upload/uploadCard/" + ReadName;
            ExcelConfig excelConfig = new ExcelConfig();
            String columns[] = {"msisdn","iccid","imsi","open_date","activate_date","agent_id","channel_id","is_pool","batch_date","remarks","status_id","package_id","imei","type","network_type","is_sms","sms_number","gprs","user_id"};
            String maxVid  = yzCardMapper.findMaxVid();
            maxVid = maxVid!=null?maxVid:"16800000000";
            Long maxVidInt = Long.parseLong(maxVid);
            List<Map<String, String>> list = excelConfig.getExcelListMap(path,columns,maxVidInt);
            String  create_by = " [ "+User.getDept().getDeptName()+" ] - "+" [ "+User.getUserName()+" ] ";



            if(list!=null && list.size()>0){
                //筛选出  vid msisdn iccid imsi 的 卡号 重复项
                HashMap<String, Object> map = new HashMap<>();
                map.put("card_arrs",list);
                map.put("type","3");
                List<String>  iccidarr = yzCardMapper.isExistence(map);

                String SaveUrl = "/getcsv/"+newName+".csv";
                String task_name ="连接管理上传 [导入] ";
                Map<String, Object> task_map = new HashMap<String, Object>();
                task_map.put("auth",create_by);
                task_map.put("task_name",task_name);
                task_map.put("url",SaveUrl);
                task_map.put("agent_id", User.getDept().getDeptId());

                executionTaskMapper.add(task_map);//添加执行 任务表

                //1.判断上传数据与数据库现有数据做对比 大于 0 证明有 以存在数据
                if(iccidarr.size()>0){
                    //上传数据>数据库查询 赛选出
                    List<String> list1 = new ArrayList<>();
                    for (int i = 0; i <list.size() ; i++) {
                        list1.add(list.get(i).get("iccid"));
                    }
                    //找出与数据库已存在 相同 ICCID 去除 重复 iccid
                    List<Map<String, String>> Out_list_Different = Different.getIdentical(list1,iccidarr,"iccid");
                    if(Out_list_Different.size()>0){
                        OutCSV(Out_list_Different,newName,"ICCID重复导入失败！",create_by,"导入失败");
                    }

                    list = Different.delIdentical(list,iccidarr,"iccid");//删除相同的数据 进行批量上传
                }
                map.put("card_arrs",list);//更新 list
                map.put("create_by",create_by);
                try {
                    if(list.size()>0){
                        int  sInt = yzCardMapper.importCard(map);
                        if(sInt>0){
                            OutCSV(list,newName,"成功导入卡列表数据 ["+sInt+"] 条",create_by,"导入成功");
                            executionTaskMapper.set_end_time(task_map);//任务结束
                        }
                    }else{
                        executionTaskMapper.set_end_time(task_map);//任务结束
                        return " 上传数据已全部在数据库，无需上传卡信息！ ";
                    }
                }catch (DuplicateKeyException e){
                    System.out.println("===="+e.getCause().toString());
                    String[] solit=e.getCause().toString().split("'");
                    OutCSV(list,newName,e.getCause().toString(),create_by,"导入失败");
                    executionTaskMapper.set_end_time(task_map);//任务结束
                    return "上传excel异常 [插入数据 DuplicateKeyException ] "+e.getCause().toString() ;
                }catch (Exception e){
                    System.out.println("===="+e.getCause().toString());
                    OutCSV(list,newName,e.getCause().toString(),create_by,"导入失败");
                    executionTaskMapper.set_end_time(task_map);//任务结束
                    return "上传excel异常 [插入数据 Exception] "+e.getCause().toString() ;
                }

            }else{
                return "上传表格数据不能为空！";
            }*/
        } catch (Exception e) {
            System.out.println(e);
            return "上传excel异常";
        }

        return "连接管理 导入指令 已发送，详细信息请在 【执行日志管理】查询！";
    }

    @Override
    public Map<String, Object> findRoute(Map<String, Object> map) {
        return yzCardMapper.findRoute(map);
    }

    @Override
    public List<Map<String, Object>> getDeptName() {
        return deptMapper.getDeptName();
    }

    @Override
    public String exportData(Map<String, Object> map, SysUser User) {
        Object MapAgent_id = map.get("agent_id");
        //导出时 未选中 当前 企业编号时 且登录 部门不是 总平台 赋值部门
        if (MapAgent_id == null && User.getDeptId() != 100) {
            List<String> agent_idArr = new ArrayList<String>();
            agent_idArr.add("" + User.getDeptId());
            map.put("agent_id", agent_idArr);
        }
        map.remove("pageNum");
        map.remove("pageSize");
        map = getChannelIdArr(map);
        List<String> outCardIccidArr = null;
        //权限过滤
        if (map.get("agent_id") != null) {
            List<Integer> agent_id = (List<Integer>) map.get("agent_id");
            if (!Different.Is_existence(agent_id, 100)) {
                List<String> user_id = iYzUserService.getUserID(map);
                map.put("user_id", user_id);
            }
        }
        map = getChannelIdArr(map);
        boolean selLianTong = (boolean) map.get("selLianTong");
        if (selLianTong) {
            outCardIccidArr = yzCardMapper.outCardIccidLianTong(map);
        } else {
            outCardIccidArr = yzCardMapper.outCardIccid(map);
        }
        if (outCardIccidArr != null && outCardIccidArr.size() > 0) {
            String create_by = " [ " + User.getDept().getDeptName() + " ] - " + " [ " + User.getUserName() + " ] ";
            String newName = UUID.randomUUID().toString().replace("-", "") + "_CardOut";

            String agent_id = User.getDept().getDeptId().toString();
            String task_name = "-连接管理 [导出] ";
            String SaveUrl = "/getcsv/" + newName + ".csv";

            Map<String, Object> task_map = new HashMap<String, Object>();
            task_map.put("auth", create_by);
            task_map.put("task_name", task_name);
            task_map.put("url", SaveUrl);
            task_map.put("agent_id", agent_id);
            task_map.put("type", "1");

            //获取字典信息
            List<SysDictData> stateOptions = dictDataMapper.selectDictDataByType("yunze_card_status_ShowId");//卡状态
            List<SysDictData> card_types = dictDataMapper.selectDictDataByType("yunze_card_card_type");//卡类型
            List<SysDictData> customize_whether = dictDataMapper.selectDictDataByType("yunze_customize_whether");//系统是否
            List<SysDictData> cardConnection_type = dictDataMapper.selectDictDataByType("yz_cardConnection_type");//断开网状态
            //获取 用户信息
            List<Map<String, Object>> userArr = userMapper.find_simple();


            //1.创建路由 绑定 生产队列 发送消息
            //导卡 路由队列
            String polling_queueName = "admin_OutCard_queue";
            String polling_routingKey = "admin_OutCard_queue";
            String polling_exchangeName = "admin_exchange";//路由
            try {
                rabbitMQConfig.creatExchangeQueue(polling_exchangeName, polling_queueName, polling_routingKey, null, null, null, BuiltinExchangeType.DIRECT);
                Map<String, Object> start_type = new HashMap<>();
                start_type.put("type", "importCardData");//启动类型
                start_type.put("newName", newName);//输出文件名
                start_type.put("task_map", task_map);//
                start_type.put("create_by", create_by);//
                start_type.put("User", User);
                start_type.put("outCardIccidArr", outCardIccidArr);
                start_type.put("userArr", userArr);
                start_type.put("stateOptions", stateOptions);
                start_type.put("card_types", card_types);
                start_type.put("customize_whether", customize_whether);
                start_type.put("cardConnection_type", cardConnection_type);
                start_type.put("map", map);

                rabbitTemplate.convertAndSend(polling_exchangeName, polling_routingKey, JSON.toJSONString(start_type), message -> {
                    // 设置消息过期时间 30 分钟 过期
                    message.getMessageProperties().setExpiration("" + (30 * 1000 * 60));
                    return message;
                });
            } catch (Exception e) {
                System.out.println("导入 卡号 失败 " + e.getMessage().toString());
                return "连接管理 导入 操作失败！";
            }
        } else {
            return "您当前的筛选的查询条件 未找到数据！导出任务取消！";
        }
        return "已下发执行日志可在【系统管理】》【日志管理】》【执行日志】查看";
    }

    @Override
    public List<String> outCardIccid(Map<String, Object> map) {
        Map<String, Object> CountMap = new HashMap<>();
        CountMap.putAll(map);
        CountMap.remove("pageNum");
        CountMap.remove("pageSize");
        //权限过滤
        List<String> user_id = iYzUserService.getUserID(CountMap);
        map.put("user_id", user_id);
        return yzCardMapper.outCardIccid(map);
    }


    @Override
    public List<String> selId(Map<String, Object> map, boolean selLianTong) {
        Map<String, Object> CountMap = new HashMap<>();
        CountMap.putAll(map);
        CountMap.remove("pageNum");
        CountMap.remove("pageSize");
        //权限过滤
        List<String> user_id = iYzUserService.getUserID(CountMap);
        map.put("user_id", user_id);

        if (selLianTong) {
            return yzCardMapper.selIdLianTong(map);
        } else {
            return yzCardMapper.selId(map);
        }
    }

    @Override
    public boolean updStatusId(Map<String, Object> map) {
        return yzCardMapper.updStatusId(map) > 0;
    }


    @Override
    public String dividCard(Map<String, Object> map) {
        String Message = "";
        map.remove("pageNum");
        map.remove("pageSize");
        map = getChannelIdArr(map);
        //权限过滤
        if (map.get("agent_id") != null) {
            List<Integer> agent_id = (List<Integer>) map.get("agent_id");
            if (!Different.Is_existence(agent_id, 100)) {
                List<String> user_id = iYzUserService.getUserID(map);
                map.put("user_id", user_id);
            }
        }
        boolean selLianTong = (boolean) map.get("selLianTong");
        List<String> dividIdArr = selId(map, selLianTong);

        if (dividIdArr != null && dividIdArr.size() > 0) {
            //1.创建路由 绑定 生产队列 发送消息
            //导卡 路由队列
            String polling_queueName = "admin_DistributeCard_queue";
            String polling_routingKey = "admin.DistributeCard.queue";
            String polling_exchangeName = "admin_exchange";//路由
            try {
                rabbitMQConfig.creatExchangeQueue(polling_exchangeName, polling_queueName, polling_routingKey, null, null, null, BuiltinExchangeType.DIRECT);
                Map<String, Object> start_type = new HashMap<>();
                start_type.putAll(map);
                start_type.put("type", "DistributeCard");//启动类型
                start_type.put("dividIdArr", dividIdArr);//需要划分的数据
                rabbitTemplate.convertAndSend(polling_exchangeName, polling_routingKey, JSON.toJSONString(start_type), message -> {
                    // 设置消息过期时间 30 分钟 过期
                    message.getMessageProperties().setExpiration("" + (30 * 1000 * 60));
                    return message;
                });
            } catch (Exception e) {
                System.out.println("划卡 失败 " + e.getMessage().toString());
                return ("连接管理 划卡 操作失败！");
            }
            Message = "当前筛选条件下需要划分的数据 [ " + dividIdArr.size() + " ] 条 至 [ " + map.get("set_dept_name") + " ] [ " + map.get("set_user_name") + " ] 指令已下发详情查看 【执行日志管理】 ！";
        } else {
            Message = "当前筛选条件下未找到需要划分的数据！请核对后重试！";
        }
        return Message;
    }


    /**
     * 获取 运营商类型 的通道id
     *
     * @param map
     * @return
     */
    private Map<String, Object> getChannelIdArr(Map<String, Object> map) {
        //判断是否选择 运营商类型
         map.put("selLianTong", false);

        if (map.get("cd_operator_type") != null) {
            List<String> cd_operator_type = (List<String>) map.get("cd_operator_type");
            if ( cd_operator_type.size() > 0) {
                List<Map<String, Object>> smap = yzCardRouteMapper.find_simpleOperatorArr(map);
                //添加 【符合 运营类型】 的 通道 查询条件
                List<String> channel_id = new ArrayList<String>();

                if(map.get("channel_id") != null){
                    List<String> Channel = (List<String>) map.get("channel_id");
                    channel_id.addAll(Channel);
                }

                if (smap != null && smap.size() > 0) {
                    for (int i = 0; i < smap.size(); i++) {
                        channel_id.add(smap.get(i).get("cd_id").toString());
                    }
                }
                //未找到相匹配的通道时 将 通道id传参 -1 使查询不到相对应数据
                if (channel_id.size() == 0) {
                    channel_id.add("-1");
                }else {
                    map.put("channel_id", channel_id);
                }
                //如果是联通的查询 条件 且 条件是 卡号 查询长度大于19 或 起止 条件 是 iccid
                boolean LianTong = false;
                for (int i = 0; i < cd_operator_type.size(); i++) {
                    if(cd_operator_type.get(i).equals("2")){
                        LianTong = true;
                    }
                }
                if (LianTong) {
                    Object type = map.get("type");
                    Object value = map.get("value");
                    if (value != null && value.toString().length() > 0 && type != null) {
                        map.put("selLianTong", true);
                    }
                    Object StartAndEndtype = map.get("StartAndEndtype");
                    Object StartValue = map.get("StartValue");
                    Object EndValue = map.get("EndValue");
                    if (StartAndEndtype != null && StartAndEndtype.equals("3") && StartValue != null && EndValue != null) {
                        map.put("selLianTong", true);
                    }
                }
            }
        }
        return map;
    }


    @Override
    public String importSet(MultipartFile file, Map<String, Object> map) throws IOException {
        String filename = file.getOriginalFilename();
        String ReadName = UUID.randomUUID().toString().replace("-", "") + filename;
        try {
            // 获取当前项目的工作路径
            File file2 = new File("");
            String filePath = file2.getCanonicalPath();
            File newFile = new File(filePath + "/upload/importSet/" + ReadName);
            File Url = new File(filePath + "/upload/importSet/1.txt");//tomcat 生成路径
            Upload.mkdirsmy(Url);
            file.transferTo(newFile);
            //1.创建路由 绑定 生产队列 发送消息
            String addOrder_exchangeName = "admin_exchange", addOrder_queueName = "admin_CardImportSet_queue", addOrder_routingKey = "admin.CardImportSet.queue",
                    addOrder_del_exchangeName = "dlx_" + addOrder_exchangeName, addOrder_del_queueName = "dlx_" + addOrder_queueName, addOrder_del_routingKey = "dlx_" + addOrder_routingKey;
            try {
                //rabbitMQConfig.creatExchangeQueue(addOrder_exchangeName, addOrder_queueName, addOrder_routingKey, addOrder_del_exchangeName, addOrder_del_queueName, addOrder_del_routingKey, null);
                Map<String, Object> start_type = new HashMap<>();
                start_type.put("filePath", filePath);//项目根目录
                start_type.put("ReadName", ReadName);//上传新文件名
                start_type.put("map", map);//参数
                rabbitTemplate.convertAndSend(addOrder_exchangeName, addOrder_routingKey, JSON.toJSONString(start_type), message -> {
                    // 设置消息过期时间 60 分钟 过期
                    message.getMessageProperties().setExpiration("" + (60 * 1000 * 60));
                    return message;
                });
            } catch (Exception e) {
                System.out.println("连接设置 生产指令  失败 " + e.getMessage().toString());
                return ("连接设置 生产指令 操作失败！");
            }
        } catch (Exception e) {
            System.out.println(e);
            return "上传excel异常";
        }
        return "连接设置 指令 已发送，连接设置详细信息请在 【执行日志管理】查询！";
    }


    @Override
    public String importSelImei(MultipartFile file, Map<String, Object> map) throws IOException {
        String filename = file.getOriginalFilename();
        String ReadName = UUID.randomUUID().toString().replace("-", "") + filename;
        try {
            // 获取当前项目的工作路径
            File file2 = new File("");
            String filePath = file2.getCanonicalPath();
            File newFile = new File(filePath + "/upload/importSelImei/" + ReadName);
            File Url = new File(filePath + "/upload/importSelImei/1.txt");//tomcat 生成路径
            Upload.mkdirsmy(Url);
            file.transferTo(newFile);
            //1.创建路由 绑定 生产队列 发送消息
            String addOrder_exchangeName = "admin_exchange", addOrder_queueName = "admin_CardImportSelImei_queue", addOrder_routingKey = "admin.CardImportSelImei.queue",
                    addOrder_del_exchangeName = "dlx_" + addOrder_exchangeName, addOrder_del_queueName = "dlx_" + addOrder_queueName, addOrder_del_routingKey = "dlx_" + addOrder_routingKey;
            try {
                //rabbitMQConfig.creatExchangeQueue(addOrder_exchangeName, addOrder_queueName, addOrder_routingKey, addOrder_del_exchangeName, addOrder_del_queueName, addOrder_del_routingKey, null);
                Map<String, Object> start_type = new HashMap<>();
                start_type.put("filePath", filePath);//项目根目录
                start_type.put("ReadName", ReadName);//上传新文件名
                start_type.put("map", map);//参数
                rabbitTemplate.convertAndSend(addOrder_exchangeName, addOrder_routingKey, JSON.toJSONString(start_type), message -> {
                    // 设置消息过期时间 60 分钟 过期
                    message.getMessageProperties().setExpiration("" + (60 * 1000 * 60));
                    return message;
                });
            } catch (Exception e) {
                System.out.println("特殊操作查询IMEI 生产指令  失败 " + e.getMessage().toString());
                return ("特殊操作查询IMEI 生产指令 操作失败！");
            }
        } catch (Exception e) {
            System.out.println(e);
            return "上传excel异常";
        }
        return "特殊操作查询IMEI 指令 已发送，连接设置详细信息请在 【执行日志管理】查询！";
    }

    @Override
    public String status(MultipartFile file, Map<String, Object> map) throws IOException {
        {
            String filename = file.getOriginalFilename();
            String ReadName = UUID.randomUUID().toString().replace("-", "") + filename;
            try {
                // 获取当前项目的工作路径
                File file2 = new File("");
                String filePath = file2.getCanonicalPath();
                File newFile = new File(filePath + "/upload/importSelImei/" + ReadName);
                File Url = new File(filePath + "/upload/importSelImei/1.txt");//tomcat 生成路径
                Upload.mkdirsmy(Url);
                file.transferTo(newFile);
                //1.创建路由 绑定 生产队列 发送消息
                String addOrder_exchangeName = "admin_exchange", addOrder_queueName = "admin_CardImportBatch_queue", addOrder_routingKey = "admin.CardImportBatch.queue",
                        addOrder_del_exchangeName = "dlx_" + addOrder_exchangeName, addOrder_del_queueName = "dlx_" + addOrder_queueName, addOrder_del_routingKey = "dlx_" + addOrder_routingKey;
                try {
                   // rabbitMQConfig.creatExchangeQueue(addOrder_exchangeName, addOrder_queueName, addOrder_routingKey, addOrder_del_exchangeName, addOrder_del_queueName, addOrder_del_routingKey, null);
                    Map<String, Object> start_type = new HashMap<>();
                    start_type.put("filePath", filePath);//项目根目录
                    start_type.put("ReadName", ReadName);//上传新文件名
                    start_type.put("map", map);//参数
                    rabbitTemplate.convertAndSend(addOrder_exchangeName, addOrder_routingKey, JSON.toJSONString(start_type), message -> {
                        // 设置消息过期时间 60 分钟 过期
                        message.getMessageProperties().setExpiration("" + (60 * 1000 * 60));
                        return message;
                    });
                } catch (Exception e) {
                    System.out.println("批量停复机、断开网 生产指令  失败 " + e.getMessage().toString());
                    return ("批量停复机、断开网 生产指令 操作失败！");
                }
            } catch (Exception e) {
                System.out.println(e);
                return "上传excel异常";
            }
            return "批量停复机、断开网 指令 已发送，连接设置详细信息请在 【执行日志管理】查询！";
        }
    }


    @Override
    public Map<String, Object> CardNumberImport(MultipartFile file, Map<String, Object> map) throws IOException {
        String filename = file.getOriginalFilename();
        String ReadName = UUID.randomUUID().toString().replace("-", "") + filename;
        try {
            // 获取当前项目的工作路径
            File file2 = new File("");
            String filePath = file2.getCanonicalPath();
            File newFile = new File(filePath + "/upload/cardNumber/" + ReadName);
            File Url = new File(filePath + "/upload/cardNumber/1.txt");//tomcat 生成路径
            Upload.mkdirsmy(Url);
            file.transferTo(newFile);
            ExcelConfig excelConfig = new ExcelConfig();
            String columns[] = {"cardNumber"};
            List<Map<String, Object>> list = excelConfig.getExcelListMap(filePath + "/upload/cardNumber/" + ReadName, columns);
            //System.out.println(list.toString());
            //System.out.println(list);
            map.put("UpArr", list);
        } catch (Exception e) {
            System.out.println(e);
        }
        return selMap(map);
    }


    @Override
    public String importSetCardInfo(MultipartFile file, Map<String, Object> map) throws IOException {
        String filename = file.getOriginalFilename();
        String ReadName = UUID.randomUUID().toString().replace("-", "") + filename;
        try {
            // 获取当前项目的工作路径
            File file2 = new File("");
            String filePath = file2.getCanonicalPath();
            File newFile = new File(filePath + "/upload/importSetCardInfo/" + ReadName);
            File Url = new File(filePath + "/upload/importSetCardInfo/1.txt");//tomcat 生成路径
            Upload.mkdirsmy(Url);
            file.transferTo(newFile);
            //1.创建路由 绑定 生产队列 发送消息
            String addOrder_exchangeName = "admin_exchange", addOrder_queueName = "admin_importSetCardInfo_queue", addOrder_routingKey = "admin.importSetCardInfo.queue",
                    addOrder_del_exchangeName = "dlx_" + addOrder_exchangeName, addOrder_del_queueName = "dlx_" + addOrder_queueName, addOrder_del_routingKey = "dlx_" + addOrder_routingKey;
            try {
               // rabbitMQConfig.creatExchangeQueue(addOrder_exchangeName, addOrder_queueName, addOrder_routingKey, addOrder_del_exchangeName, addOrder_del_queueName, addOrder_del_routingKey, null);
                Map<String, Object> start_type = new HashMap<>();
                start_type.put("filePath", filePath);//项目根目录
                start_type.put("ReadName", ReadName);//上传新文件名
                start_type.put("map", map);//参数
                rabbitTemplate.convertAndSend(addOrder_exchangeName, addOrder_routingKey, JSON.toJSONString(start_type), message -> {
                    // 设置消息过期时间 60 分钟 过期
                    message.getMessageProperties().setExpiration("" + (60 * 1000 * 60));
                    return message;
                });
            } catch (Exception e) {
                System.out.println("特殊操作变更卡分组、备注 生产指令  失败 " + e.getMessage().toString());
                return ("特殊操作变更卡分组、备注 生产指令 操作失败！");
            }
        } catch (Exception e) {
            System.out.println(e);
            return "上传excel异常";
        }
        return "特殊操作变更卡分组、备注 指令 已发送，连接设置详细信息请在 【执行日志管理】查询！";
    }

    @Override
    public List<String> getCardGrouping(Map<String, Object> map) {
        //查询所属下 分组
        if (map.get("agent_id") != null) {
            map.put("agent_id", yzCardMapper.queryChildrenAreaInfo(map));
        }

        return yzCardMapper.getCardGrouping(map);
    }

    @Override
    public boolean updActivate(Map<String, Object> map) {
        return yzCardMapper.updActivate(map) > 0;
    }


    @Override
    public boolean UpdateFill(Map<String, Object> map) {
        return yzCardMapper.UpdateFill(map) > 0;
    }

    /***停机*/
    @Override
    public String stoppedarr(Map<String, Object> map) {
        {
            //1.创建路由 绑定 生产队列 发送消息
            String addOrder_exchangeName = "admin_exchange", addOrder_queueName = "admin_Stopped_queue", addOrder_routingKey = "admin.Stopped.queue",
                    addOrder_del_exchangeName = "dlx_" + addOrder_exchangeName, addOrder_del_queueName = "dlx_" + addOrder_queueName, addOrder_del_routingKey = "dlx_" + addOrder_routingKey;
            try {
                Map<String, Object> start_type = new HashMap<>();
                start_type.put("map", map);//参数
                rabbitTemplate.convertAndSend(addOrder_exchangeName, addOrder_routingKey, JSON.toJSONString(start_type), message -> {
                    // 设置消息过期时间 60 分钟 过期
                    message.getMessageProperties().setExpiration("" + (60 * 1000 * 60));
                    return message;
                });
            } catch (Exception e) {
                System.out.println("批量 【停机】 生产指令  失败 " + e.getMessage().toString());
                return ("批量 【停机】 生产指令 操作失败！");
            }

            return "批量 【停机】 指令 已发送，连接设置详细信息请在 【执行日志管理】查询！";
        }
    }

    /***复机*/
    @Override
    public String machinearr(Map<String, Object> map) {
        {
            //1.创建路由 绑定 生产队列 发送消息
            String addOrder_exchangeName = "admin_exchange", addOrder_queueName = "admin_Machine_queue", addOrder_routingKey = "admin.Machine.queue",
                    addOrder_del_exchangeName = "dlx_" + addOrder_exchangeName, addOrder_del_queueName = "dlx_" + addOrder_queueName, addOrder_del_routingKey = "dlx_" + addOrder_routingKey;
            try {
                Map<String, Object> start_type = new HashMap<>();
                start_type.put("map", map);//参数
                rabbitTemplate.convertAndSend(addOrder_exchangeName, addOrder_routingKey, JSON.toJSONString(start_type), message -> {
                    // 设置消息过期时间 60 分钟 过期
                    message.getMessageProperties().setExpiration("" + (60 * 1000 * 60));
                    return message;
                });
            } catch (Exception e) {
                System.out.println("批量 【复机】 生产指令  失败 " + e.getMessage().toString());
                return ("批量 【复机】 生产指令 操作失败！");
            }

            return "批量 【复机】 指令 已发送，连接设置详细信息请在 【执行日志管理】查询！";
        }
    }

    /***断网*/
    @Override
    public String disconnectNetworkarr(Map<String, Object> map) {
        {
            //1.创建路由 绑定 生产队列 发送消息
            String addOrder_exchangeName = "admin_exchange", addOrder_queueName = "admin_DisconnectNetwork_queue", addOrder_routingKey = "admin.DisconnectNetwork.queue",
                    addOrder_del_exchangeName = "dlx_" + addOrder_exchangeName, addOrder_del_queueName = "dlx_" + addOrder_queueName, addOrder_del_routingKey = "dlx_" + addOrder_routingKey;
            try {
                Map<String, Object> start_type = new HashMap<>();
                start_type.put("map", map);//参数
                rabbitTemplate.convertAndSend(addOrder_exchangeName, addOrder_routingKey, JSON.toJSONString(start_type), message -> {
                    // 设置消息过期时间 60 分钟 过期
                    message.getMessageProperties().setExpiration("" + (60 * 1000 * 60));
                    return message;
                });
            } catch (Exception e) {
                System.out.println("批量 【断网】 生产指令  失败 " + e.getMessage().toString());
                return ("批量 【断网】 生产指令 操作失败！");
            }

            return "批量 【断网】 指令 已发送，连接设置详细信息请在 【执行日志管理】查询！";
        }
    }

    /***开网*/
    @Override
    public String openNetworkarr(Map<String, Object> map) {
        {
            //1.创建路由 绑定 生产队列 发送消息
            String addOrder_exchangeName = "admin_exchange", addOrder_queueName = "admin_OpenNetwork_queue", addOrder_routingKey = "admin.OpenNetwork.queue",
                    addOrder_del_exchangeName = "dlx_" + addOrder_exchangeName, addOrder_del_queueName = "dlx_" + addOrder_queueName, addOrder_del_routingKey = "dlx_" + addOrder_routingKey;
            try {
                Map<String, Object> start_type = new HashMap<>();
                start_type.put("map", map);//参数
                rabbitTemplate.convertAndSend(addOrder_exchangeName, addOrder_routingKey, JSON.toJSONString(start_type), message -> {
                    // 设置消息过期时间 60 分钟 过期
                    message.getMessageProperties().setExpiration("" + (60 * 1000 * 60));
                    return message;
                });
            } catch (Exception e) {
                System.out.println("批量 【开网】 生产指令  失败 " + e.getMessage().toString());
                return ("批量 【开网】 生产指令 操作失败！");
            }

            return "批量 【开网】 指令 已发送，连接设置详细信息请在 【执行日志管理】查询！";
        }
    }

    /**
     * 批量同步用量
     */
    @Override
    public String consumptionarr(Map<String, Object> map) {
        {
            //1.创建路由 绑定 生产队列 发送消息
            String addOrder_exchangeName = "admin_exchange", addOrder_queueName = "admin_Consumption_queue", addOrder_routingKey = "admin.Consumption.queue",
                    addOrder_del_exchangeName = "dlx_" + addOrder_exchangeName, addOrder_del_queueName = "dlx_" + addOrder_queueName, addOrder_del_routingKey = "dlx_" + addOrder_routingKey;
            try {
                Map<String, Object> start_type = new HashMap<>();
                start_type.put("map", map);//参数
                rabbitTemplate.convertAndSend(addOrder_exchangeName, addOrder_routingKey, JSON.toJSONString(start_type), message -> {
                    // 设置消息过期时间 60 分钟 过期
                    message.getMessageProperties().setExpiration("" + (60 * 1000 * 60));
                    return message;
                });
            } catch (Exception e) {
                System.out.println("批量 【同步用量】 生产指令  失败 " + e.getMessage().toString());
                return ("批量 【同步用量】 生产指令 操作失败！");
            }

            return "批量 【同步用量】 指令 已发送，连接设置详细信息请在 【执行日志管理】查询！";
        }
    }

    /**
     * 批量同步状态
     */
    @Override
    public String publicmethodarr(Map<String, Object> map) {
        {
            //1.创建路由 绑定 生产队列 发送消息
            String addOrder_exchangeName = "admin_exchange", addOrder_queueName = "admin_PublicMethod_queue", addOrder_routingKey = "admin.PublicMethod.queue",
                    addOrder_del_exchangeName = "dlx_" + addOrder_exchangeName, addOrder_del_queueName = "dlx_" + addOrder_queueName, addOrder_del_routingKey = "dlx_" + addOrder_routingKey;
            try {
                Map<String, Object> start_type = new HashMap<>();
                start_type.put("map", map);//参数
                rabbitTemplate.convertAndSend(addOrder_exchangeName, addOrder_routingKey, JSON.toJSONString(start_type), message -> {
                    // 设置消息过期时间 60 分钟 过期
                    message.getMessageProperties().setExpiration("" + (60 * 1000 * 60));
                    return message;
                });
            } catch (Exception e) {
                System.out.println("批量 【同步状态】 生产指令  失败 " + e.getMessage().toString());
                return ("批量 【同步状态】 生产指令 操作失败！");
            }

            return "批量 【同步状态】 指令 已发送，连接设置详细信息请在 【执行日志管理】查询！";
        }


    }

    /**
     * 批量 【同步状态和用量】
     */
    @Override
    public String consumptionandstatearr(Map<String, Object> map) {
        {
            //1.创建路由 绑定 生产队列 发送消息
            String addOrder_exchangeName = "admin_exchange", addOrder_queueName = "admin_ConsumptionAndState_queue", addOrder_routingKey = "admin.ConsumptionAndState.queue",
                    addOrder_del_exchangeName = "dlx_" + addOrder_exchangeName, addOrder_del_queueName = "dlx_" + addOrder_queueName, addOrder_del_routingKey = "dlx_" + addOrder_routingKey;
            try {
                Map<String, Object> start_type = new HashMap<>();
                start_type.put("map", map);//参数
                rabbitTemplate.convertAndSend(addOrder_exchangeName, addOrder_routingKey, JSON.toJSONString(start_type), message -> {
                    // 设置消息过期时间 60 分钟 过期
                    message.getMessageProperties().setExpiration("" + (60 * 1000 * 60));
                    return message;
                });
            } catch (Exception e) {
                System.out.println("批量 【同步状态和用量】 生产指令  失败 " + e.getMessage().toString());
                return ("批量 【同步状态和用量】 生产指令 操作失败！");
            }

            return "批量 【同步状态和用量】 指令 已发送，连接设置详细信息请在 【执行日志管理】查询！";
        }
    }

    @Override
    public Map<String, Object> getIccid(Map<String, Object> map) {
        Map<String, Object> Rmap = null;
        List<Map<String, Object>> selVidArr = yzCardMapper.selVid(map);
        if (selVidArr != null && selVidArr.size() > 0) {
            Rmap = selVidArr.get(0);
            boolean is_Internal = false;
            //权限过滤
            if (map.get("agent_id") != null) {
                List<Integer> agent_id = (List<Integer>) map.get("agent_id");
                if (!Different.Is_existence(agent_id, 100)) {
                } else {
                    is_Internal = true;//内部人员 部门是 100 的 可看字段增加
                }
            } else {
                is_Internal = true;//内部人员 部门是 100 的 可看字段增加
            }
            Rmap.put("is_Internal", is_Internal);
        }
        return Rmap;
    }

    @Override
    public String cancelrealname(MultipartFile file, Map<String, Object> map) throws IOException {
        String filename = file.getOriginalFilename();
        String ReadName = UUID.randomUUID().toString().replace("-", "") + filename;
        try {
            // 获取当前项目的工作路径
            File file2 = new File("");
            String filePath = file2.getCanonicalPath();
            File newFile = new File(filePath + "/upload/cancelrealname/" + ReadName);
            File Url = new File(filePath + "/upload/cancelrealname/1.txt");//tomcat 生成路径
            Upload.mkdirsmy(Url);
            file.transferTo(newFile);
            //1.创建路由 绑定 生产队列 发送消息
            String addOrder_exchangeName = "admin_exchange", addOrder_queueName = "admin_CardCancelrealname_queue", addOrder_routingKey = "admin.CardCancelrealname.queue",
                    addOrder_del_exchangeName = "dlx_" + addOrder_exchangeName, addOrder_del_queueName = "dlx_" + addOrder_queueName, addOrder_del_routingKey = "dlx_" + addOrder_routingKey;
            try {
               // rabbitMQConfig.creatExchangeQueue(addOrder_exchangeName, addOrder_queueName, addOrder_routingKey, addOrder_del_exchangeName, addOrder_del_queueName, addOrder_del_routingKey, null);
                Map<String, Object> start_type = new HashMap<>();
                start_type.put("filePath", filePath);//项目根目录
                start_type.put("ReadName", ReadName);//上传新文件名
                start_type.put("map", map);//参数
                rabbitTemplate.convertAndSend(addOrder_exchangeName, addOrder_routingKey, JSON.toJSONString(start_type), message -> {
                    // 设置消息过期时间 60 分钟 过期
                    message.getMessageProperties().setExpiration("" + (60 * 1000 * 60));
                    return message;
                });
            } catch (Exception e) {
                System.out.println("批量取消实名 生产指令  失败 " + e.getMessage().toString());
                return ("批量取消实名 生产指令 操作失败！");
            }
        } catch (Exception e) {
            System.out.println(e);
            return "上传excel异常";
        }
        return "批量取消实名 指令 已发送，连接设置详细信息请在 【执行任务管理】查询！";

    }

    @Override
    public Map<String, Object> cardMatch(Map<String, Object> map) {
        Map<String, Object> Rmap = new HashMap<>();
        Map<String, Object> Pmap = new HashMap<>();
        Map<String, Object> cardMatchMap = null;
        String cardNo = map.get("cardNo").toString();
        String PcardNo = map.get("cardNo").toString();

        Integer cardMatchCount = 0;
        Pmap.put("selType", map.get("selType"));

        int frequency = 0;
        while (true) {

            Pmap.put("cardNo", PcardNo);
            cardMatchCount = yzCardMapper.cardMatchCount(Pmap);
            cardMatchCount = cardMatchCount != null ? cardMatchCount : 0;
            if (cardMatchCount > 0) {
                cardMatchMap = yzCardMapper.cardMatchOne(Pmap);
                break;
            } else {
                PcardNo = PcardNo.substring(0, PcardNo.length() - 1);
            }
            frequency += 1;
            if (frequency == 5) {
                break;
            }
        }
        boolean is_Internal = false;
        //权限过滤
        if (map.get("agent_id") != null) {
            List<Integer> agent_id = (List<Integer>) map.get("agent_id");
            if (!Different.Is_existence(agent_id, 100)) {
            } else {
                is_Internal = true;//内部人员 部门是 100 的 可看字段增加
            }
        } else {
            is_Internal = true;//内部人员 部门是 100 的 可看字段增加
        }
        Rmap.put("is_Internal", is_Internal);


        Rmap.put("cardCount", cardMatchCount);
        Rmap.put("cardMatchMap", cardMatchMap);
        Rmap.put("cardPrefix", PcardNo);
        Rmap.put("cardSuffix", cardNo.substring(cardNo.length() - frequency));
        return Rmap;

    }


    @Override
    public String importCardReplace(MultipartFile file, Map<String, Object> map) {
        String filename = file.getOriginalFilename();
        String ReadName = UUID.randomUUID().toString().replace("-", "") + filename;
        try {
            // 获取当前项目的工作路径
            File file2 = new File("");
            String filePath = file2.getCanonicalPath();
            File newFile = new File(filePath + "/upload/importCardReplace/" + ReadName);
            File Url = new File(filePath + "/upload/importCardReplace/1.txt");//tomcat 生成路径
            Upload.mkdirsmy(Url);
            file.transferTo(newFile);
            //1.创建路由 绑定 生产队列 发送消息
            String addOrder_exchangeName = "admin_exchange", addOrder_queueName = "admin_CardImportReplace_queue", addOrder_routingKey = "admin.CardImportReplace.queue";
            try {
                Map<String, Object> start_type = new HashMap<>();
                start_type.put("filePath", filePath);//项目根目录
                start_type.put("ReadName", ReadName);//上传新文件名
                start_type.put("map", map);//参数
                rabbitTemplate.convertAndSend(addOrder_exchangeName, addOrder_routingKey, JSON.toJSONString(start_type), message -> {
                    // 设置消息过期时间 60 分钟 过期
                    message.getMessageProperties().setExpiration("" + (60 * 1000 * 60));
                    return message;
                });
            } catch (Exception e) {
                System.out.println("批量更新卡信息 生产指令  失败 " + e.getMessage().toString());
                return ("批量更新卡信息 生产指令 操作失败！");
            }
        } catch (Exception e) {
            System.out.println(e);
            return "上传excel异常";
        }
        return "批量更新卡信息 指令 已发送，更新卡信息 详细信息请在 【执行日志管理】查询！";
    }



}




























