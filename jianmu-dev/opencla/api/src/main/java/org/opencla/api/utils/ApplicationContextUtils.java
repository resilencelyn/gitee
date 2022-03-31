package org.opencla.api.utils;

import org.springframework.beans.BeansException;
import org.springframework.context.ApplicationContext;
import org.springframework.context.ApplicationContextAware;
import org.springframework.stereotype.Component;

/**
 * spring的回调
 */
@Component
public class ApplicationContextUtils implements ApplicationContextAware {

    /**
     * 保留下来工厂
     */
    private static ApplicationContext applicationContext;

    /**
     * 提供根据bean type在工厂中获取对象
     * @param beanType
     * @param <T>
     * @return
     */
    public static <T> T getBean(Class<T> beanType) {
        return applicationContext.getBean(beanType);
    }

    /**
     * 在非spring管理的类中也能获取Spring中的bean
     *
     * @param applicationContext
     * @throws BeansException
     */
    @Override
    public void setApplicationContext(ApplicationContext applicationContext) throws BeansException {
        ApplicationContextUtils.applicationContext = applicationContext;
    }
}