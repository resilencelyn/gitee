package com.jd.platform.jlog.worker;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

/**
 * 启动器
 */
@SpringBootApplication
public class WorkerApplication {

    public static void main(String[] args) {
        try {
            SpringApplication.run(WorkerApplication.class, args);
        }catch (Exception e){
            e.printStackTrace();
        }
    }

}
