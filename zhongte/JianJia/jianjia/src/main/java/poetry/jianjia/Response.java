package poetry.jianjia;

import okhttp3.Headers;
import okhttp3.Protocol;
import okhttp3.Request;
import okhttp3.ResponseBody;

/**
 * @author 裴云飞
 * @date 2021/1/23
 */
public class Response<T> {

    public static <T> Response<T> success(T body) {
        return success(body, new okhttp3.Response.Builder() //
                .code(200)
                .message("OK")
                .protocol(Protocol.HTTP_1_1)
                .request(new Request.Builder().url("http://localhost/").build())
                .build());
    }

    public static <T> Response<T> success(T body, okhttp3.Response rawResponse) {
        Utils.checkNotNull(rawResponse, "rawResponse == null");
        if (!rawResponse.isSuccessful()) {
            throw new IllegalArgumentException("rawResponse must be successful response");
        }
        return new Response<>(rawResponse, body, null);
    }

    public static <T> Response<T> success(T body, Headers headers) {
        Utils.checkNotNull(headers, "headers == null");
        return success(body, new okhttp3.Response.Builder() //
                .code(200)
                .message("OK")
                .protocol(Protocol.HTTP_1_1)
                .headers(headers)
                .request(new Request.Builder().url("http://localhost/").build())
                .build());
    }

    public static <T> Response<T> error(int code, ResponseBody body) {
        if (code < 400) throw new IllegalArgumentException("code < 400: " + code);
        return error(body, new okhttp3.Response.Builder() //
                .code(code)
                .message("Response.error()")
                .protocol(Protocol.HTTP_1_1)
                .request(new Request.Builder().url("http://localhost/").build())
                .build());
    }

    public static <T> Response<T> error(ResponseBody body, okhttp3.Response rawResponse) {
        Utils.checkNotNull(body, "body == null");
        Utils.checkNotNull(rawResponse, "rawResponse == null");
        if (rawResponse.isSuccessful()) {
            throw new IllegalArgumentException("rawResponse should not be successful response");
        }
        return new Response<>(rawResponse, null, body);
    }

    private final okhttp3.Response rawResponse;
    private final T body;
    private final ResponseBody errorBody;

    private Response(okhttp3.Response rawResponse, T body, ResponseBody errorBody) {
        this.rawResponse = rawResponse;
        this.body = body;
        this.errorBody = errorBody;
    }

    public okhttp3.Response raw() {
        return rawResponse;
    }

    public int code() {
        return rawResponse.code();
    }

    public boolean isSuccessful() {
        return rawResponse.isSuccessful();
    }

    public String message() {
        return rawResponse.message();
    }

    public String header(String name) {
        return rawResponse.header(name);
    }

    public String header(String name, String defaultValue) {
        return rawResponse.header(name, defaultValue);
    }

    public T body() {
        return body;
    }

    public ResponseBody errorBody() {
        return errorBody;
    }

    public String toString() {
        return rawResponse.toString();
    }
}
