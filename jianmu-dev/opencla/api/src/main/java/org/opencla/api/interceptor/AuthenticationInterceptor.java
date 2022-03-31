package org.opencla.api.interceptor;

import org.opencla.api.api.impl.GiteeApi;
import org.opencla.api.exception.AccessTokenDoesNotExistException;
import org.opencla.api.utils.ApplicationContextUtils;
import org.opencla.api.utils.UserContextHolder;
import org.springframework.lang.Nullable;
import org.springframework.web.servlet.HandlerInterceptor;
import org.springframework.web.servlet.ModelAndView;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

public class AuthenticationInterceptor implements HandlerInterceptor {
    @Override
    public boolean preHandle(HttpServletRequest request, HttpServletResponse response, Object handler) throws Exception {
        String accessToken = UserContextHolder.getAccessToken();

        // TODO: 写死gitee, 以后根据依赖倒置原则修改此处
        GiteeApi giteeApi = ApplicationContextUtils.getBean(GiteeApi.class);
        try {
            giteeApi.getUserInfo(accessToken);
        } catch (AccessTokenDoesNotExistException e) {
            response.sendError(HttpServletResponse.SC_UNAUTHORIZED);
            return false;
        }
        return true;
    }

    @Override
    public void postHandle(HttpServletRequest request, HttpServletResponse response, Object handler, @Nullable ModelAndView modelAndView) throws Exception {
        HandlerInterceptor.super.postHandle(request, response, handler, modelAndView);
    }

    @Override
    public void afterCompletion(HttpServletRequest request, HttpServletResponse response, Object handler, @Nullable Exception ex) throws Exception {
        HandlerInterceptor.super.afterCompletion(request, response, handler, ex);
    }
}
