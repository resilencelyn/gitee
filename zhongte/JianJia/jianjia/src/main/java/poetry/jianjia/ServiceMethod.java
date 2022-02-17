package poetry.jianjia;

import java.lang.reflect.Method;
import java.lang.reflect.Type;

/**
 * @author 裴云飞
 * @date 2021/1/17
 */

abstract class ServiceMethod<T> {

    /**
     * 解析注解
     *
     * @param jianJia
     * @param method
     * @param <T>
     * @return
     */
    static <T> ServiceMethod<T> parseAnnotations(JianJia jianJia, Method method) {
        RequestFactory requestFactory = RequestFactory.parseAnnotations(jianJia, method);
        Type returnType = method.getGenericReturnType();
        if (Utils.hasUnresolvableType(returnType)) {
            throw Utils.methodError(method,
                    "Method return type must not include a type variable or wildcard: %s", returnType);
        }
        if (returnType == void.class) {
            throw Utils.methodError(method, "Service methods cannot return void.");
        }
        return HttpServiceMethod.parseAnnotations(jianJia, method, requestFactory);
    }

    public abstract T invoke(Object[] args);
}
