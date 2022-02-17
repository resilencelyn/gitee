package poetry.jianjia;

import java.io.IOException;
import java.lang.annotation.Annotation;
import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Type;
import java.util.concurrent.Executor;

import okhttp3.Request;
import poetry.jianjia.http.SkipCallbackExecutor;

/**
 * 默认的类型转换器，如果调用者没有添加{@link CallAdapter}对象，就会使用这个默认的类型转换器
 *
 * @author 裴云飞
 * @date 2021/1/17
 */

class DefaultCallAdapterFactory extends CallAdapter.Factory {

    private final Executor callbackExecutor;

    public DefaultCallAdapterFactory(Executor callbackExecutor) {
        this.callbackExecutor = callbackExecutor;
    }

    @Override
    public CallAdapter<?, ?> get(Type returnType, Annotation[] annotations, JianJia jianJia) {
        if (Utils.getRawType(returnType) != Call.class) {
            return null;
        }
        // 返回值没有泛型
        if (!(returnType instanceof ParameterizedType)) {
            throw new IllegalArgumentException(
                    "Call return type must be parameterized as Call<Foo> or Call<? extends Foo>");
        }
        Type responseType = Utils.getParameterUpperBound(0, (ParameterizedType) returnType);
        // executor为空说明不需要将结果回调到主线程
        Executor executor = Utils.isAnnotationPresent(annotations, SkipCallbackExecutor.class) ?
                null : callbackExecutor;
        return new CallAdapter<Object, Call<?>>() {

            @Override
            public Type responseType() {
                return responseType;
            }

            @Override
            public Call<?> adapt(Call<Object> call) {
                return executor == null ? call : new ExecutorCallbackCall<>(executor, call);
            }
        };
    }

    static final class ExecutorCallbackCall<T> implements Call<T> {

        final Executor callbackExecutor;
        final Call<T> delegate;

        ExecutorCallbackCall(Executor callbackExecutor, Call<T> delegate) {
            this.callbackExecutor = callbackExecutor;
            this.delegate = delegate;
        }

        @Override
        public Response<T> execute() throws IOException {
            return delegate.execute();
        }

        @Override
        public void enqueue(Callback<T> callback) {
            Utils.checkNotNull(callback, "callback == null");
            delegate.enqueue(new Callback<T>() {
                @Override
                public void onResponse(Call<T> call, Response<T> response) {
                    callbackExecutor.execute(new Runnable() {
                        @Override
                        public void run() {
                            if (delegate.isCanceled()) {
                                callback.onFailure(ExecutorCallbackCall.this, new IOException("Canceled"));
                            } else {
                                callback.onResponse(ExecutorCallbackCall.this, response);
                            }
                        }
                    });
                }

                @Override
                public void onFailure(Call<T> call, Throwable t) {
                    callbackExecutor.execute(new Runnable() {
                        @Override public void run() {
                            callback.onFailure(ExecutorCallbackCall.this, t);
                        }
                    });
                }
            });
        }

        @Override
        public boolean isExecuted() {
            return delegate.isExecuted();
        }

        @Override
        public void cancel() {
            delegate.cancel();
        }

        @Override
        public boolean isCanceled() {
            return delegate.isCanceled();
        }

        @Override
        public Call<T> clone() {
            return new ExecutorCallbackCall<>(callbackExecutor, delegate.clone());
        }

        @Override
        public Request request() {
            return delegate.request();
        }
    }
}
