package poetry.jianjia.http;

import java.lang.annotation.Documented;
import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * head请求，声明在方法上的注解，在运行时有效
 *
 * @author 裴云飞
 * @date 2021/1/18
 */
@Documented
@Target(ElementType.METHOD)
@Retention(RetentionPolicy.RUNTIME)
public @interface HEAD {

    /**
     * HEAD注解一般必须添加相对路径或绝对路径或者全路径，如果不想在HEAD注解后添加请求路径，
     * 则可以在方法的第一个参数中用{@link Url @Url}注解添加请求路径
     *
     * @return
     */
    String value() default "";
}
