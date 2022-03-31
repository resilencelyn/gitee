package com.yunze.business.annotation;

import java.lang.annotation.*;

/**
 * 描述： 日志注解自定义
 */
@Target(ElementType.METHOD)
@Retention(RetentionPolicy.RUNTIME)
@Documented
public @interface LogAnnotation {
    String action() default "";
}
