package poetry.jianjia.http;

import java.lang.annotation.Documented;
import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * 当你发送一个post或put请求，但是又不想作为请求参数或表单的方式发送请求时，
 * 使用该注解定义的参数可以直接传入一个实体类，内部会把该实体序列化并将序列化后的结果直接作为请求体发送出去
 *
 * @author 裴云飞
 * @date 2021/1/26
 */
@Documented
@Target(ElementType.PARAMETER)
@Retention(RetentionPolicy.RUNTIME)
public @interface Body {

}
