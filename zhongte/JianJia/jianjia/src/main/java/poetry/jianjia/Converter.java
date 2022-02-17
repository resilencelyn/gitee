package poetry.jianjia;

import java.io.IOException;
import java.lang.annotation.Annotation;
import java.lang.reflect.Type;

import okhttp3.RequestBody;
import okhttp3.ResponseBody;
import poetry.jianjia.http.Body;
import poetry.jianjia.http.Field;
import poetry.jianjia.http.FieldMap;
import poetry.jianjia.http.Header;
import poetry.jianjia.http.HeaderMap;
import poetry.jianjia.http.Part;
import poetry.jianjia.http.Path;
import poetry.jianjia.http.Query;
import poetry.jianjia.http.QueryMap;

/**
 * 数据转换器，Converter这个类的实例由Factory创建，由{@linkplain JianJia.Builder#addConverterFactory(Factory)}方法来进行初始化的
 *
 * @author 裴云飞
 * @date 2021/1/17
 */
public interface Converter<F, T> {

    /**
     * 把F转化为T
     *
     * @param value
     * @return
     * @throws IOException
     */
    T convert(F value) throws IOException;

    abstract class Factory {

        /**
         * 返回一个处理请求体的转换器，这个转换器主要是为了处理{@link Body}注解，{@link Part}注解。
         *
         * @param type
         * @param parameterAnnotations
         * @param methodAnnotations
         * @param jianJia
         * @return
         */
        public Converter<?, RequestBody> requestBodyConverter(Type type,
                                                              Annotation[] parameterAnnotations, Annotation[] methodAnnotations, JianJia jianJia) {
            return null;
        }

        /**
         * 返回一个处理响应体的转换器，例如：Call<SimpleResponse>，则响应体的类型应该是SimpleResponse。
         *
         * @param type
         * @param annotations
         * @param jianJia
         * @return
         */
        public Converter<ResponseBody, ?> responseBodyConverter(Type type,
                                                                          Annotation[] annotations, JianJia jianJia) {
            return null;
        }

        /**
         * 返回一个处理字符串的转换器，这个转换器主要是为了处理{@link Field}注解, {@link FieldMap}注解，
       * {@link Header}注解，{@link HeaderMap}注解，{@link Path}注解，{@link Query}注解，
         * {@link QueryMap}注解
         *
         * @param type
         * @param annotations
         * @param jianJia
         * @return
         */
        public Converter<?, String> stringConverter(Type type, Annotation[] annotations, JianJia jianJia) {
            return null;
        }
    }
}
