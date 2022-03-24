package com.jd.platform.jlog.clientdemo;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

/**
 * demo启动类
 *
 * @author shenkaiwen5
 * @version 1.0
 * @date 2021-12-27
 */
@SpringBootApplication
public class ExampleApplication {

    public static void main(String[] args) {
        try {
            SpringApplication.run(ExampleApplication.class, args);
        }catch (Exception e){
            e.printStackTrace();
        }
    }
}
