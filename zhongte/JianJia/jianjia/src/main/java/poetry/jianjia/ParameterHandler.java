package poetry.jianjia;

import java.io.IOException;
import java.lang.reflect.Array;
import java.lang.reflect.Method;
import java.util.Map;

import okhttp3.Headers;
import okhttp3.MultipartBody;
import okhttp3.RequestBody;

/**
 * @author 裴云飞
 * @date 2021/1/23
 */

abstract class ParameterHandler<T> {

    abstract void apply(RequestBuilder builder, T value) throws IOException;

    final ParameterHandler<Iterable<T>> iterable() {
        return new ParameterHandler<Iterable<T>>() {
            @Override
            void apply(RequestBuilder builder, Iterable<T> value) throws IOException {
                if (value == null) {
                    return;
                }
                for (T t : value) {
                    ParameterHandler.this.apply(builder, t);
                }
            }
        };
    }

    final ParameterHandler<Object> array() {
        return new ParameterHandler<Object>() {
            @Override
            void apply(RequestBuilder builder, Object value) throws IOException {
                if (value == null) {
                    return;
                }
                int size = Array.getLength(value);
                for (int i = 0; i < size; i++) {
                    //noinspection unchecked
                    ParameterHandler.this.apply(builder, (T) Array.get(value, i));
                }
            }
        };
    }

    static final class RelativeUrl extends ParameterHandler<Object> {

        Method method;
        int p;

        RelativeUrl(Method method, int p) {
            this.method = method;
            this.p = p;
        }

        @Override
        void apply(RequestBuilder builder, Object value) throws IOException {
            if (value == null) {
                throw Utils.parameterError(method, p, "@Url parameter is null.");
            }
            builder.setRelativeUrl(value);
        }
    }

    static final class Path<T> extends ParameterHandler<T> {

        Method method;
        int p;
        String name;
        Converter<T, String> valueConverter;
        boolean encoded;

        Path(Method method, int p, String name, Converter<T, String> valueConverter, boolean encoded) {
            this.method = method;
            this.p = p;
            this.name = name;
            this.valueConverter = valueConverter;
            this.encoded = encoded;
        }

        @Override
        void apply(RequestBuilder builder, T value) throws IOException {
            if (value == null) {
                throw Utils.parameterError(method, p,
                        "Path parameter \"" + name + "\" value must not be null.");
            }
            builder.addPathParam(name, valueConverter.convert(value), encoded);
        }
    }

    static final class Query<T> extends ParameterHandler<T> {

        private final String name;
        private final Converter<T, String> valueConverter;
        private final boolean encoded;

        Query(String name, Converter<T, String> valueConverter, boolean encoded) {
            this.name = Utils.checkNotNull(name, "name == null");
            this.valueConverter = valueConverter;
            this.encoded = encoded;
        }

        @Override
        void apply(RequestBuilder builder, T value) throws IOException {
            if (value == null) {
                return;
            }
            String queryValue = valueConverter.convert(value);
            if (queryValue == null) {
                return;
            }
            builder.addQueryParam(name, queryValue, encoded);
        }
    }

    static final class QueryMap<T> extends ParameterHandler<Map<String, T>> {

        private final Method method;
        private final int p;
        private final Converter<T, String> valueConverter;
        private final boolean encoded;

        QueryMap(Method method, int p, Converter<T, String> valueConverter, boolean encoded) {
            this.method = method;
            this.p = p;
            this.valueConverter = valueConverter;
            this.encoded = encoded;
        }

        @Override
        void apply(RequestBuilder builder, Map<String, T> value) throws IOException {
            if (value == null) {
                throw Utils.parameterError(method, p, "Query map was null");
            }
            for (Map.Entry<String, T> entry : value.entrySet()) {
                String entryKey = entry.getKey();
                if (entryKey == null) {
                    throw Utils.parameterError(method, p, "Query map contained null key.");
                }
                T entryValue = entry.getValue();
                if (entryValue == null) {
                    throw Utils.parameterError(method, p,
                            "Query map contained null value for key '" + entryKey + "'.");
                }
                String convertedEntryValue = valueConverter.convert(entryValue);
                if (convertedEntryValue == null) {
                    throw Utils.parameterError(method, p, "Query map value '"
                            + entryValue
                            + "' converted to null by "
                            + valueConverter.getClass().getName()
                            + " for key '"
                            + entryKey
                            + "'.");
                }
                builder.addQueryParam(entryKey, convertedEntryValue, encoded);
            }
        }
    }

    static final class Header<T> extends ParameterHandler<T> {

        private final String name;
        private final Converter<T, String> valueConverter;

        Header(String name, Converter<T, String> valueConverter) {
            this.name = Utils.checkNotNull(name, "name == null");
            this.valueConverter = valueConverter;
        }

        @Override
        void apply(RequestBuilder builder, T value) throws IOException {
            if (value == null) {
                return;
            }
            String queryValue = valueConverter.convert(value);
            if (queryValue == null) {
                return;
            }
            builder.addHeader(name, queryValue);
        }
    }

    static final class HeaderMap<T> extends ParameterHandler<Map<String, T>> {

        private final Method method;
        private final int p;
        private final Converter<T, String> valueConverter;

        HeaderMap(Method method, int p, Converter<T, String> valueConverter) {
            this.method = method;
            this.p = p;
            this.valueConverter = valueConverter;
        }

        @Override
        void apply(RequestBuilder builder, Map<String, T> value) throws IOException {
            if (value == null) {
                throw Utils.parameterError(method, p, "Header map was null");
            }
            for (Map.Entry<String, T> entry : value.entrySet()) {
                String headerName = entry.getKey();
                if (headerName == null) {
                    throw Utils.parameterError(method, p, "Header map contained null key.");
                }
                T headerValue = entry.getValue();
                if (headerValue == null) {
                    throw Utils.parameterError(method, p,
                            "Header map contained null value for key '" + headerName + "'.");
                }
                builder.addHeader(headerName, valueConverter.convert(headerValue));
            }
        }
    }

    static final class Field<T> extends ParameterHandler<T> {

        private final String name;
        private final Converter<T, String> valueConverter;
        private final boolean encoded;

        Field(String name, Converter<T, String> valueConverter, boolean encoded) {
            this.name = Utils.checkNotNull(name, "name == null");
            this.valueConverter = valueConverter;
            this.encoded = encoded;
        }

        @Override
        void apply(RequestBuilder builder, T value) throws IOException {
            if (value == null) {
                return;
            }
            String fieldValue = valueConverter.convert(value);
            if (fieldValue == null) {
                return;
            }
            builder.addFormField(name, fieldValue, encoded);
        }
    }

    static final class FieldMap<T> extends ParameterHandler<Map<String, T>> {

        private final Method method;
        private final int p;
        private final Converter<T, String> valueConverter;
        private final boolean encoded;

        FieldMap(Method method, int p, Converter<T, String> valueConverter, boolean encoded) {
            this.method = method;
            this.p = p;
            this.valueConverter = valueConverter;
            this.encoded = encoded;
        }

        @Override
        void apply(RequestBuilder builder, Map<String, T> value) throws IOException {
            if (value == null) {
                throw Utils.parameterError(method, p, "Query map was null");
            }

            for (Map.Entry<String, T> entry : value.entrySet()) {
                String entryKey = entry.getKey();
                if (entryKey == null) {
                    throw Utils.parameterError(method, p, "Query map contained null key.");
                }
                T entryValue = entry.getValue();
                if (entryValue == null) {
                    throw Utils.parameterError(method, p,
                            "Query map contained null value for key '" + entryKey + "'.");
                }
                String fieldEntry = valueConverter.convert(entryValue);
                if (fieldEntry == null) {
                    throw Utils.parameterError(method, p, "Query map value '"
                            + entryValue
                            + "' converted to null by "
                            + valueConverter.getClass().getName()
                            + " for key '"
                            + entryKey
                            + "'.");
                }
                builder.addFormField(entryKey, fieldEntry, encoded);
            }
        }
    }

    static final class Body<T> extends ParameterHandler<T> {

        final Method method;
        final int p;
        final Converter<T, RequestBody> converter;

        Body(Method method, int p, Converter<T, RequestBody> converter) {
            this.method = method;
            this.p = p;
            this.converter = converter;
        }

        @Override
        void apply(RequestBuilder builder, T value) throws IOException {
            if (value == null) {
                throw Utils.parameterError(method, p, "Body parameter value must not be null.");
            }
            RequestBody body;
            try {
                body = converter.convert(value);
            } catch (IOException e) {
                throw Utils.parameterError(method, e, p, "Unable to convert " + value + " to RequestBody");
            }
            builder.setBody(body);
        }
    }

    static final class Part<T> extends ParameterHandler<T> {
        private final Method method;
        private final int p;
        private final Headers headers;
        private final Converter<T, RequestBody> converter;

        Part(Method method, int p, Headers headers, Converter<T, RequestBody> converter) {
            this.method = method;
            this.p = p;
            this.headers = headers;
            this.converter = converter;
        }

        @Override void apply(RequestBuilder builder, T value) {
            if (value == null) return; // Skip null values.

            RequestBody body;
            try {
                body = converter.convert(value);
            } catch (IOException e) {
                throw Utils.parameterError(method, p, "Unable to convert " + value + " to RequestBody", e);
            }
            builder.addPart(headers, body);
        }
    }

    static final class RawPart extends ParameterHandler<MultipartBody.Part> {
        static final RawPart INSTANCE = new RawPart();

        private RawPart() {
        }

        @Override void apply(RequestBuilder builder, MultipartBody.Part value) {
            if (value != null) { // Skip null values.
                builder.addPart(value);
            }
        }
    }

    static final class PartMap<T> extends ParameterHandler<Map<String, T>> {
        private final Method method;
        private final int p;
        private final Converter<T, RequestBody> valueConverter;
        private final String transferEncoding;

        PartMap(Method method, int p,
                Converter<T, RequestBody> valueConverter, String transferEncoding) {
            this.method = method;
            this.p = p;
            this.valueConverter = valueConverter;
            this.transferEncoding = transferEncoding;
        }

        @Override void apply(RequestBuilder builder, Map<String, T> value)
                throws IOException {
            if (value == null) {
                throw Utils.parameterError(method, p, "Part map was null.");
            }

            for (Map.Entry<String, T> entry : value.entrySet()) {
                String entryKey = entry.getKey();
                if (entryKey == null) {
                    throw Utils.parameterError(method, p, "Part map contained null key.");
                }
                T entryValue = entry.getValue();
                if (entryValue == null) {
                    throw Utils.parameterError(method, p,
                            "Part map contained null value for key '" + entryKey + "'.");
                }

                Headers headers = Headers.of(
                        "Content-Disposition", "form-data; name=\"" + entryKey + "\"",
                        "Content-Transfer-Encoding", transferEncoding);

                builder.addPart(headers, valueConverter.convert(entryValue));
            }
        }
    }
}
