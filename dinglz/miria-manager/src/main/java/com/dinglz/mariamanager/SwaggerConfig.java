package com.dinglz.mariamanager;

import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import springfox.documentation.builders.ApiInfoBuilder;
import springfox.documentation.builders.PathSelectors;
import springfox.documentation.builders.RequestHandlerSelectors;
import springfox.documentation.service.ApiInfo;
import springfox.documentation.service.Contact;
import springfox.documentation.spi.DocumentationType;
import springfox.documentation.spring.web.plugins.Docket;
import springfox.documentation.swagger2.annotations.EnableSwagger2;

@Configuration
@EnableSwagger2
public class SwaggerConfig {

    @Bean
    public Docket StartRestApi()
    {
        return new Docket(DocumentationType.SWAGGER_2)
                .apiInfo(apiInfo())
                .enable(true)
                .select()
                .apis(RequestHandlerSelectors.basePackage("com.dinglz.mariamanager"))
                .paths(PathSelectors.any())
                .build();
    }

    private ApiInfo apiInfo()
    {
        return new ApiInfoBuilder().title("Maria 中控管理器！")
                .description("用于统一登录QQ机器人，并实现消息分发，统一操作QQ机器人等功能")
                .contact(new Contact("丁丁(dinglz)","https://gitee.com/dinglz","dlz1149978856@163.com"))
                .build();
    }
}
