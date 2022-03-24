package com.jd.platform.jlog.clientdemo.custom;

import com.jd.platform.jlog.core.FileConfigurator;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.io.IOException;
import java.util.List;
import java.util.Set;
import java.util.stream.Collectors;

import static com.jd.platform.jlog.core.ConfiguratorFactory.*;

/**
 * @author tangbohu
 * @version 1.0.0
 * @desc 基于redis实现自定义配置器示例，主要用于重写获取workers实现（毕竟是动态的，文件配置器不好处理）
 * @ClassName CustomConfigurator.java
 * @createTime 2022年03月21日 11:37:00
 */
@Component
public class CustomConfigurator extends FileConfigurator {
    private CustomConfigurator() throws IOException {
        super();
        /**
         * 由于配置器构建是static方法，在spring注入之前已经完成了 这里自定义配置器需要手动覆盖
         */
        cover(this);
    }


    @Autowired
    private RedisUtil redisUtil;


    /**
     * 获取list
     */
    @Override
    public List<String> getList(String key) {
        Set<Object> set = redisUtil.getMembers(key);
        return set.stream().map(v->(String)v).collect(Collectors.toList());
    }


    @Override
    public String getType() {
        return "custom-redis";
    }

}
