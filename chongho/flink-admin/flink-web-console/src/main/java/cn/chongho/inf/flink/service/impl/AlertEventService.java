package cn.chongho.inf.flink.service.impl;

import cn.chongho.inf.flink.model.DingTalkMsg;
import com.alibaba.fastjson.JSONObject;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;
import org.springframework.web.client.RestTemplate;


/**
 * @author ming
 */
@Service
@Slf4j
public class AlertEventService {

    @Autowired
    private RestTemplate restTemplate;

    @Value("${alert-event.url}")
    private String alertEventUrl;

    /**
     * 告警上传
     * @return
     */
    public boolean eventUpload(DingTalkMsg dingTalkMsg){
        String alert = alertEventUrl + "/api/alert/flink/event";
        try {
            JSONObject jsonObject = restTemplate.postForObject(alert, dingTalkMsg, JSONObject.class);
            log.info("alert result: {}", jsonObject.toJSONString());
            return jsonObject.getBoolean("data");
        }catch (Exception e){
            log.error("alert error:",e);
        }
        return false;
    }
}
