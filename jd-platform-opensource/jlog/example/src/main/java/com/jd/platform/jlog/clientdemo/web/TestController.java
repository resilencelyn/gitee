package com.jd.platform.jlog.clientdemo.web;


import com.jd.platform.jlog.common.model.TracerBean;
import com.jd.platform.jlog.common.handler.TagConfig;
import com.jd.platform.jlog.core.Configurator;
import com.jd.platform.jlog.core.ConfiguratorFactory;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.http.MediaType;
import org.springframework.stereotype.Component;
import org.springframework.web.bind.annotation.*;

/**
 * @author shenkaiwen5
 * @version 1.0
 * @date 2021-12-27
 */
@Component
//@ConfigurationProperties()
@RestController
public class TestController {

    private TagConfig tagConfig ;

    public TagConfig getTagConfig() {
        return tagConfig;
    }

    public void setTagConfig(TagConfig tagConfig) {
        this.tagConfig = tagConfig;
    }

    /**
     * do nothing
     * just as an adapter for this project common log helper
     *
     */
    private static Logger RequestLog = LoggerFactory.getLogger("RequestLog");

    @RequestMapping("/index")
    public Object index()  {
        TracerBean tracerBean = new TracerBean();
        tracerBean.setTracerId("11111");

        Configurator configurator = ConfiguratorFactory.getInstance();
        try{
            configurator.putConfig("/test","val1");
        }catch (Exception e){
            e.printStackTrace();
        }

        String val = configurator.getString("/test");
        System.out.println("val ===>   "+val);
        RequestLog.info("哈哈哈哈哈哈");

        return tracerBean;
    }

    @RequestMapping("/log")
    public Object log() {
        RequestLog.info("|errno=val3||node=val4||这是随便的log|");
        return 1;
    }


    @PostMapping(value = "/test", consumes = MediaType.APPLICATION_JSON_VALUE)
    public Object test(@RequestParam Integer uid, @RequestParam Integer newKey,@RequestBody TestReq req) {
        RequestLog.info("|errno=val3||node=val4||这是随便的log|");
        if(newKey == 1){
            return 1;
        }
        return new Response("滴滴员工tangbohu的终身代号是什么？？？是9527");
    }



}
