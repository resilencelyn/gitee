package com.jd.platform.jlog.dashboard.controller;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;
import com.jd.platform.jlog.dashboard.entity.TracerListVO;
import com.jd.platform.jlog.dashboard.entity.TracerVO;
import com.jd.platform.jlog.dashboard.model.QueryListModel;
import com.jd.platform.jlog.dashboard.service.TracerService;
import com.jd.platform.jlog.dashboard.utils.DateUtils;
import com.jd.platform.jlog.dashboard.utils.ResultHelper;
import com.jd.platform.jlog.dashboard.utils.ZstdUtils;
import org.apache.commons.beanutils.BeanUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;

import javax.annotation.Resource;
import java.nio.charset.StandardCharsets;
import java.util.List;
import java.util.Map;

/**
 * @author shenkaiwen5
 * @version 1.0
 * @date 2021-09-01
 */
@Controller
@RequestMapping("tracer/cart")
public class CartTracerController {
    /**
     * 访问前缀
     */
    private static final String prefix = "tracer/cart";
    /**
     * 日志
     */
    private static Logger logger = LoggerFactory.getLogger(CartTracerController.class);
    /**
     * 查询服务
     */
    @Resource
    private TracerService tracerService;

    /**
     * 主页
     */
    @RequestMapping()
    public String cart() {
        return prefix;
    }

    /**
     * 详情页
     */
    @GetMapping("/detail")
    public String detail(@RequestParam("id") String id,
                         @RequestParam("pin") String pin,
                         @RequestParam("uuid") String uuid,
                         @RequestParam("createTime") String createTime,
                         ModelMap mmap) throws Exception {

        //加synchronized，保证Calendar不出错
        String beginTime = DateUtils.addAndSubtractTime(createTime, -5000L);
        String endTime = DateUtils.addAndSubtractTime(createTime, 5000L);
        //查询数据
        Map<String, Object> map = tracerService.findOne(id, pin, uuid, beginTime, endTime);

        //转化其中被压缩的response
        String response = map.get("responseContent").toString();
        byte[] base64 = java.util.Base64.getDecoder().decode(response.getBytes(StandardCharsets.UTF_8));
        byte[] zstd = ZstdUtils.decompressBytes(base64);
        map.put("responseContent", new String(zstd));

        //转化其中被压缩的body
        try {
            JSONObject request = JSON.parseObject(map.get("requestContent").toString());
            if (request.containsKey("body")) {
                request.put("body", JSON.parseObject(request.getString("body")));
            }

            if (request.containsKey("wholeRequest")) {
                request.put("wholeRequest", JSON.parseObject(request.getString("wholeRequest")));
            }
            map.put("requestContent", request.toJSONString());
        } catch (Exception e) {
            logger.info("CartTracerController.detail", e);
        }

        //转为结果类
        TracerVO tracerVO = new TracerVO();
        BeanUtils.populate(tracerVO, map);
        //存入返回模板值
        mmap.put("tracerVO", tracerVO);
        //logger.info(new String(zstd));

        return "tracer/detail";
    }

    /**
     * 列表
     */
    @RequestMapping("/list")
    @ResponseBody
    public TracerListVO list(QueryListModel queryListModel) {
        //这里下发的不一定是完整数据，可能缺几个属性，完整的是查询单个返回TracerVO
        TracerListVO listVO = tracerService.list(queryListModel);
        if (listVO == null) {
            return ResultHelper.fail();
        }
        //处理时间格式，这存的时间拿map接收无法@JsonFormat
        List<Map<String, Object>> rows = listVO.getRows();
        for (Map<String, Object> map : rows) {
            //处理createTime
            String oldTime = map.get("createTime").toString();
            map.put("createTime", oldTime
                    .replace("T", " ")
                    .substring(0, oldTime.length() - 2)
            );
            //处理tracerId
            String tracerId = map.get("tracerId").toString();
            map.put("tracerId", tracerId);
        }

        return ResultHelper.success(listVO);
    }

    public static void main(String[] args) {
        JSONObject request = JSON.parseObject(k);
        JSONObject body = JSON.parseObject(request.getString("body"));
        request.put("body", body);
        System.out.println(request.toJSONString());
    }

    static String k = "{\n" +
            "    \"uemps\": \"0-2\",\n" +
            "    \"eid\": \"eidA3c568123des8qlKRNmrTQ26 FzACzVpLW6pSkqSwsJSDlqszGSPYsLdFRVCNQijc2VVokz2sqnqwF5bys2sqzp6viknQcdWzM4Wv54PNXDi YUND\",\n" +
            "    \"agent\": \"okhttp/3.12.1;jdmall;android;version/10.2.4;build/91254;screen/720x1436;os/8.1.0;\",\n" +
            "    \"screen\": \"1436*720\",\n" +
            "    \"d_brand\": \"vivo\",\n" +
            "    \"gfid\": \"subCartCount\",\n" +
            "    \"clientVersion\": \"10.2.4\",\n" +
            "    \"body\": \"{\\\"cartuuid\\\":\\\"89fb296e-7aff-45d2-84b3-a46e2700bf8a\\\",\\\"coord_type\\\":\\\"\\\",\\\"latitude\\\":\\\"30.076485\\\",\\\"longitude\\\":\\\"103.126714\\\",\\\"refer\\\":\\\"1\\\",\\\"type\\\":\\\"prescription\\\",\\\"userType\\\":\\\"0\\\"}\",\n" +
            "    \"uuid\": \"863486043131991-f8e7a0ebb9d1\",\n" +
            "    \"wsm\": \"0\",\n" +
            "    \"osVersion\": \"8.1.0\",\n" +
            "    \"client\": \"android\",\n" +
            "    \"lang\": \"zh_CN\",\n" +
            "    \"networkType\": \"wifi\",\n" +
            "    \"soaKey\": \"2e2i4fRvyfAek5MfHNCe;fMeBpvzbQimFgonJp9YpBqyMcz1VvO4sWcyIBRb8VdOTNQDVo7\",\n" +
            "    \"area\": \"22_2047_2053_41320\",\n" +
            "    \"ext\": \"{\\\"prstate\\\":\\\"0\\\"}\",\n" +
            "    \"wifiBssid\": \"unknown\",\n" +
            "    \"appName\": \"cartsoa\",\n" +
            "    \"ip\": \"2409:8a62:711d:40a0:c46f:7f2f:c3d9:acb7\",\n" +
            "    \"uri\": \"subCartCount\",\n" +
            "    \"d_model\": \"V1818CA\",\n" +
            "    \"partner\": \"vivo\",\n" +
            "    \"port\": \"39858\",\n" +
            "    \"build\": \"91254\",\n" +
            "    \"serverIp\": \"11.114.56.246\",\n" +
            "    \"location\": \"中国_四川省_雅安市_移动\",\n" +
            "    \"sdkVersion\": \"27\",\n" +
            "    \"aid\": \"7541719c4ab6e487\",\n" +
            "    \"tracerId\": 9647708048786560\n" +
            "}";

}
