package org.opencla.api.utils;

import org.springframework.web.context.request.RequestContextHolder;
import org.springframework.web.context.request.ServletRequestAttributes;

import java.util.Objects;

/**
 * @author Daihw
 * @class UserContextHolder
 * @description UserContextHolder
 * @create 2022/1/10 5:49 下午
 */
public class UserContextHolder {
    /**
     * 获取签名
     *
     * @return
     */
    public static String getAccessToken() {
        return (((ServletRequestAttributes) Objects.requireNonNull(RequestContextHolder.getRequestAttributes()))
                .getRequest())
                .getHeader("Authorization");
    }
}
