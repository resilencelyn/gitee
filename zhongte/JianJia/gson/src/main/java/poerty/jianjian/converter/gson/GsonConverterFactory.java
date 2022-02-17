package poerty.jianjian.converter.gson;

import com.google.gson.Gson;
import com.google.gson.TypeAdapter;
import com.google.gson.reflect.TypeToken;

import java.lang.annotation.Annotation;
import java.lang.reflect.Type;

import okhttp3.RequestBody;
import okhttp3.ResponseBody;
import poetry.jianjia.Converter;
import poetry.jianjia.JianJia;

/**
 * @author 裴云飞
 * @date 2021/1/26
 */
public final class GsonConverterFactory extends Converter.Factory {

    private final Gson gson;

    public static GsonConverterFactory create() {
        return create(new Gson());
    }

    public static GsonConverterFactory create(Gson gson) {
        if (gson == null) {
            throw new NullPointerException("gson == null");
        }
        return new GsonConverterFactory(gson);
    }

    private GsonConverterFactory(Gson gson) {
        this.gson = gson;
    }

    @Override
    public Converter<?, RequestBody> requestBodyConverter(Type type, Annotation[] parameterAnnotations, Annotation[] methodAnnotations, JianJia jianJia) {
        TypeAdapter<?> adapter = gson.getAdapter(TypeToken.get(type));
        return new GsonRequestBodyConverter<>(gson, adapter);
    }

    @Override
    public Converter<ResponseBody, ?> responseBodyConverter(Type type, Annotation[] annotations, JianJia jianJia) {
        TypeAdapter<?> adapter = gson.getAdapter(TypeToken.get(type));
        return new GsonResponseBodyConverter<>(gson, adapter);
    }
}
