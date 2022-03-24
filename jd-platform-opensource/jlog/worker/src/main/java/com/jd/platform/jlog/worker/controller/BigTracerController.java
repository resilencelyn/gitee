package com.jd.platform.jlog.worker.controller;

import com.jd.platform.jlog.worker.disruptor.Producer;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.multipart.MultipartFile;

import javax.annotation.Resource;
import java.io.IOException;

/**
 * 大日志对象
 * @author shenkaiwen5
 * @version 1.0
 * @date 2021-10-14
 */
@RestController
@RequestMapping("/big")
public class BigTracerController {

    @Resource
    private Producer producer;

    private Logger logger = LoggerFactory.getLogger(BigTracerController.class);


    @RequestMapping("/receive")
    public String receiveBigTrace(@RequestParam("data") MultipartFile file) {
        try {
            byte[] bytes = file.getBytes();
            producer.sendData(bytes);

            return "success";
        } catch (IOException e) {
            logger.error("BigTracerController.receiveBigTrace [error]", e);
        }
        return "false";
    }
}
