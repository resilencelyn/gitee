package poetry.jianjia;

import java.io.IOException;
import java.lang.annotation.Annotation;
import java.lang.reflect.Type;

import okhttp3.RequestBody;
import okhttp3.ResponseBody;
import poetry.jianjia.http.Streaming;

/**
 * 默认的数据转换器，如果调用者没有添加{@link Converter}对象，就会使用这个默认的数据转换器
 *
 * @author 裴云飞
 * @date 2021/1/17
 */
class BuiltInConverters extends Converter.Factory {

    @Override
    public Converter<?, RequestBody> requestBodyConverter(Type type,
                                                                              Annotation[] parameterAnnotations, Annotation[] methodAnnotations, JianJia jianJia) {
        if (RequestBody.class.isAssignableFrom(Utils.getRawType(type))) {
            return RequestBodyConverter.INSTANCE;
        }
        return null;
    }

    @Override
    public Converter<ResponseBody, ?> responseBodyConverter(Type type, Annotation[] annotations, JianJia jianJia) {
        if (type == ResponseBody.class) {
            return Utils.isAnnotationPresent(annotations, Streaming.class) ?
                    StreamingResponseBodyConverter.INSTANCE :
                    BufferingResponseBodyConverter.INSTANCE;
        }
        if (type == Void.class) {
            return VoidResponseBodyConverter.INSTANCE;
        }
        return null;
    }

    static final class RequestBodyConverter implements Converter<RequestBody, RequestBody> {
        static final RequestBodyConverter INSTANCE = new RequestBodyConverter();

        @Override public RequestBody convert(RequestBody value) {
            return value;
        }
    }

    static final class StreamingResponseBodyConverter implements Converter<ResponseBody, ResponseBody> {

        static final StreamingResponseBodyConverter INSTANCE = new StreamingResponseBodyConverter();

        @Override
        public ResponseBody convert(ResponseBody value) throws IOException {
            return value;
        }
    }

    static final class BufferingResponseBodyConverter implements Converter<ResponseBody, ResponseBody> {

        static final BufferingResponseBodyConverter INSTANCE = new BufferingResponseBodyConverter();

        @Override
        public ResponseBody convert(ResponseBody value) throws IOException {
            try {
                return Utils.buffer(value);
            } finally {
                value.close();
            }
        }
    }

    static final class VoidResponseBodyConverter implements Converter<ResponseBody, Void> {

        static final VoidResponseBodyConverter INSTANCE = new VoidResponseBodyConverter();

        @Override
        public Void convert(ResponseBody value) throws IOException {
            value.close();
            return null;
        }
    }

    static final class ToStringConverter implements Converter<Object, String> {

        static final ToStringConverter INSTANCE = new ToStringConverter();

        /**
         * 直接调用toString方法
         *
         * @param value
         * @return
         * @throws IOException
         */
        @Override
        public String convert(Object value) throws IOException {
            return value.toString();
        }
    }
}
