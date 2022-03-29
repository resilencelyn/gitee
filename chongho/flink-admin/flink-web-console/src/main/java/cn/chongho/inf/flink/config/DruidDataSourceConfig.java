package cn.chongho.inf.flink.config;

import com.alibaba.druid.pool.DruidDataSource;
import lombok.Data;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import tk.mybatis.mapper.common.Mapper;
import tk.mybatis.spring.mapper.MapperScannerConfigurer;

import javax.sql.DataSource;

/**
 * Druid的DataResource配置类
 * @author feihu.wang
 * @since 2022-02-14
 */
//@Configuration
@Data
public class DruidDataSourceConfig  {

    private Logger logger = LoggerFactory.getLogger(DruidDataSourceConfig.class);
    @Value("${spring.datasource.url}")
    private String        url;

    @Value("${spring.datasource.username}")
    private String        username;

    @Value("${spring.datasource.password}")
    private String        password;

    @Value("${spring.datasource.driver-class-name}")
    private String        driverClass;

    @Value("${spring.datasource.initialSize}")
    private int           initialSize;

    @Value("${spring.datasource.min-idle}")
    private int           minIdle;

    @Value("${spring.datasource.max-active}")
    private int           maxActive;

    @Value("${spring.datasource.max-wait}")
    private int           maxWait;

    @Value("${spring.datasource.min-evictable-idle-rime-millis}")
    private int           minEvictableIdleTimeMillis;


    @Value("${spring.datasource.test-while-idle}")
    private boolean       testWhileIdle;

    @Bean
    public DataSource dataSource() {
        logger.info("注入druid！！！");
        DruidDataSource datasource = new DruidDataSource();
        datasource.setUrl(url);
        datasource.setDriverClassName(driverClass);
        datasource.setUsername(username);
        datasource.setPassword(password);
        datasource.setInitialSize(initialSize);
        datasource.setMinIdle(minIdle);
        datasource.setMaxWait(maxWait);
        datasource.setMaxActive(maxActive);
        datasource.setMinEvictableIdleTimeMillis(minEvictableIdleTimeMillis);
        try {
			datasource.setFilters("stat,wall,stat,log4j,default");
		} catch (Exception e) {
			e.printStackTrace();
            logger.error("初始化DataSource失败",e);
		}
        return datasource;
    }

    @Bean
    public MapperScannerConfigurer scannerConfigurer(){
        MapperScannerConfigurer configurer = new MapperScannerConfigurer();
        configurer.setBasePackage("cn.chongho.inf.flink.mapper");
        configurer.setMarkerInterface(Mapper.class);
        return configurer;
    }

}
