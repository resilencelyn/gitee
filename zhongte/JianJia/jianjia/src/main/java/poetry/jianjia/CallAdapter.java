package poetry.jianjia;

import java.lang.annotation.Annotation;
import java.lang.reflect.Type;


/**
 * 将响应类型为{@code R}的{@link Call}适配为{@code T}类型。实例由对应的Factory来创建，
 * 这个对应的Factory是通过{@linkplain JianJia.Builder#addCallAdapterFactory(Factory)}方法添加到JianJia对象中的
 *
 * @author 裴云飞
 * @date 2021/1/17
 */
public interface CallAdapter<R, T> {

    /**
     * 返回响应类型，例如，"Call <Repo>"的响应类型是"Repo"。
     *
     * @return
     */
    Type responseType();

    /**
     * Call<T> 是适配成另外一个另外一个对象
     *
     * @param call
     * @return
     */
    T adapt(Call<R> call);

    abstract class Factory {

        /**
         * 创建CallAdapter对象
         *
         * @param returnType 带有泛型的返回值
         * @param annotations
         * @param jianJia
         * @return
         */
        public abstract CallAdapter<?, ?> get(Type returnType, Annotation[] annotations,
                                                        JianJia jianJia);
    }
}
