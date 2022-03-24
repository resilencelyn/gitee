package com.jd.platform.jlog.dashboard.controller;

import com.jd.platform.jlog.dashboard.entity.TracerListVO;
import com.jd.platform.jlog.dashboard.service.TracerService;
import com.jd.platform.jlog.dashboard.utils.ResultHelper;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;

import javax.annotation.Resource;
import java.util.List;
import java.util.Map;

/**
 * @author shenkaiwen5
 * @version 1.0
 * @date 2021-09-16
 */
@Controller
@RequestMapping("tracer/cart")
public class CartLinkController {
    /**
     * 查询服务
     */
    @Resource
    private TracerService tracerService;


    @RequestMapping("/link")
    public String queryLink(@RequestParam("id") String tracerId,
                            @RequestParam("createTime") String createTime,
                            ModelMap mmap) {
        mmap.put("id", tracerId);
        mmap.put("createTime", createTime);
        return "tracer/link";
    }

    /**
     * 链路日志
     */
    @RequestMapping("/logList")
    @ResponseBody
    public TracerListVO list(@RequestParam("id") long tracerId,
                             @RequestParam("createTime") String createTime,
                             @RequestParam("pageNum")long pageNum) {

        TracerListVO listVO = tracerService.findLogByTracerId(tracerId, createTime, pageNum);
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
        }

        return ResultHelper.success(listVO);
    }
}
