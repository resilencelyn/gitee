package poetry.jianjia;

import java.io.IOException;

import okhttp3.Request;

/**
 * 通过调用相应的方法向服务器发送请求并返回响应。每一次调用都产生请求和响应。
 * 如果出现了失败重试的情况，可以调用{@link #clone}方法复制。
 * 同步调用 采用{@link #execute}方法。
 * 异步采用{@link #enqueue}方法，在任何情况下，一个请求都有可以通过{@link #cancel}取消，
 * 一个Call在写入请求或读取响应的时候是可能产生IO异常的。
 *
 * @param <T> 响应体类型
 *
 * @author 裴云飞
 * @date 2021/1/23
 */
public interface Call<T> extends Cloneable {

    /**
     * 同步请求，直接返回响应对象
     *
     * @return
     * @throws IOException
     */
    Response<T> execute() throws IOException;

    /**
     * 异步请求，通过回调将结果告知调用者
     *
     * @param callback
     */
    void enqueue(Callback<T> callback);

    /**
     * 如果请求正在执行，返回true
     *
     * @return
     */
    boolean isExecuted();

    /**
     * 取消请求
     */
    void cancel();

    /**
     * 如果请求取消了，返回true
     *
     * @return
     */
    boolean isCanceled();

    /**
     * 复制一个新的call对象
     *
     * @return
     */
    Call<T> clone();

    /**
     * 返回请求对象
     *
     * @return 请求对象
     */
    Request request();
}
