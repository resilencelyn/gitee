package com.yunze;

import org.mybatis.spring.annotation.MapperScan;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;


@SpringBootApplication
@MapperScan("com.yunze.common.mapper")
@MapperScan("com.yunze.apiCommon.mapper")
class CWebShoppingApplication {
    public static void main(String[] args) {
        SpringApplication.run(CWebShoppingApplication.class);
    }
}