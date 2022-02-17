package poetry.jianjia.http;

import java.lang.annotation.Documented;
import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * 以map的形式添加请求头，map中每一项的键和值都不能为空，否则会抛异常
 *
 * @author 裴云飞
 * @date 2021/1/25
 */
@Documented
@Target(ElementType.PARAMETER)
@Retention(RetentionPolicy.RUNTIME)
public @interface HeaderMap {
}
