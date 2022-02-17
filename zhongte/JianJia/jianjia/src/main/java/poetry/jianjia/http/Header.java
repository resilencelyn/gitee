package poetry.jianjia.http;

import java.lang.annotation.Documented;
import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * 作用于参数上的注解，用于添加请求头
 *
 * @author 裴云飞
 * @date 2021/1/25
 */
@Documented
@Target(ElementType.PARAMETER)
@Retention(RetentionPolicy.RUNTIME)
public @interface Header {

    String value();
}
