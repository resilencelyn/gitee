package poetry.jianjia;

import java.lang.invoke.MethodHandles;
import java.lang.reflect.Constructor;
import java.lang.reflect.Method;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.Executor;

import ohos.eventhandler.EventHandler;
import ohos.eventhandler.EventRunner;
import ohos.system.version.SystemVersion;

import static java.util.Collections.singletonList;

/**
 * @author 裴云飞
 * @date 2021/1/17
 */

class Platform {

    private static final Platform PLATFORM = findPlatform();

    static Platform get() {
        return PLATFORM;
    }

    private static Platform findPlatform() {
        try {
            Class.forName("ohos.system.version.SystemVersion");
            if (SystemVersion.getApiVersion() != 0) {
                return new Harmony();
            }
        } catch (ClassNotFoundException ignored) {}
        try {
            Class.forName("java.util.Optional");
            return new Java8();
        } catch (ClassNotFoundException ignored) {
        }
        return new Platform();
    }

    public Executor defaultCallbackExecutor() {
        return null;
    }

    public List<? extends CallAdapter.Factory> defaultCallAdapterFactories(Executor callbackExecutor) {
        return singletonList(new DefaultCallAdapterFactory(callbackExecutor));
    }

    public int defaultConverterFactoriesSize() {
        return 1;
    }

    public List<? extends Converter.Factory> defaultConverterFactories() {
        return Collections.emptyList();
    }

    public boolean isDefaultMethod(Method method) {
        return method.isDefault();
    }

    Object invokeDefaultMethod(Method method, Class<?> declaringClass, Object object, Object... args)
            throws Throwable {
        Constructor<MethodHandles.Lookup> constructor = MethodHandles.Lookup.class.getDeclaredConstructor(Class.class, int.class);
        constructor.setAccessible(true);
        return constructor.newInstance(declaringClass, -1 /* trusted */)
                .unreflectSpecial(method, declaringClass)
                .bindTo(object)
                .invokeWithArguments(args);
    }

    static class Harmony extends Platform {

        @Override
        public Executor defaultCallbackExecutor() {
            return new MainThreadExecutorOnHarmony();
        }

        static class MainThreadExecutorOnHarmony implements Executor {

            private EventHandler eventHandler = new EventHandler(EventRunner.getMainEventRunner());

            @Override
            public void execute(Runnable runnable) {
                eventHandler.postTask(runnable);
            }
        }
    }

    static class Java8 extends Platform {

    }

}
