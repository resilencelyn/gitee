package com.dinglz.mariamanager;

import com.moandjiezana.toml.Toml;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

import java.io.File;

@SpringBootApplication
public class MariaManagerApplication {


    private final static Logger logger = LoggerFactory.getLogger(MariaManagerApplication.class);

    public static void main(String[] args) {
        SpringApplication.run(MariaManagerApplication.class, args);
        Toml toml = new Toml().read(new File("setting.toml"));
        logger.info("反向http地址:"+ toml.getString("http.url","http://localhost:999/"));
    }

}
