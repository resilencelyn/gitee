/**
 *    Copyright (c) [2022] [中和农信]
 *    [Software Name] is licensed under Mulan PSL v2.
 *    You can use this software according to the terms and conditions of the Mulan PSL v2.
 *    You may obtain a copy of Mulan PSL v2 at:
 *             http://license.coscl.org.cn/MulanPSL2
 *    THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *    See the Mulan PSL v2 for more details.
 *
 */
package cn.chongho.inf.flink.aop;

import cn.chongho.inf.flink.model.WebResult;
import cn.chongho.inf.flink.model.AdminUser;
import lombok.extern.slf4j.Slf4j;
import org.aspectj.lang.JoinPoint;
import org.aspectj.lang.ProceedingJoinPoint;
import org.aspectj.lang.annotation.*;
import org.aspectj.lang.reflect.MethodSignature;
import org.springframework.stereotype.Component;
import org.springframework.ui.ModelMap;
import org.springframework.validation.support.BindingAwareModelMap;
import org.springframework.web.context.request.RequestContextHolder;
import org.springframework.web.context.request.ServletRequestAttributes;
import org.springframework.web.servlet.ModelAndView;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpSession;
import java.util.List;


/**
 * 安全认证的aop拦截器
 * @author feihu.wang
 * @since 2022-02-14
 */
@Aspect
@Component
@Slf4j
public class SecurityAspect {

    private static final String LOGIN_URL = "/admin/login";

    private static final String LOGINOUT_URL = "/admin/loginout";

    @Pointcut("execution(public * cn.chongho.inf.flink.controller..*.*(..))")
    public void verification() {
    }

    @Around("verification()")
    public Object around(ProceedingJoinPoint pjp) throws Throwable {
        ServletRequestAttributes attributes = (ServletRequestAttributes) RequestContextHolder.getRequestAttributes();

        HttpServletRequest request = attributes.getRequest();
        HttpSession session = request.getSession();


        String uri = request.getRequestURI();
        if(uri.lastIndexOf("/") == uri.length()-1){
            uri = uri.substring(0,uri.length()-1);
        }
        log.info("request url:{}", uri);
        if(LOGIN_URL.equals(uri) || LOGINOUT_URL.equals(uri)){
            return pjp.proceed();
        }
        if((session.getAttribute("loginUser") == null || session.getAttribute("authorities") == null) ){
            Class returnType = ((MethodSignature) pjp.getSignature()).getReturnType();
            if(returnType == String.class || returnType == ModelAndView.class){
                //返回到登录页面
                return "/nologin";
            }else{
                //返回没有权限的json
                return WebResult.noLogin();
            }
        }
        AdminUser user = (AdminUser) session.getAttribute("loginUser");
        //判断是否有权限
        List<String> authorities = (List<String>) session.getAttribute("authorities");
        if(authorities.contains(uri) || -1 == user.getId() || LOGINOUT_URL.equals(uri)) {
            return pjp.proceed();
        }else{
            Class returnType = ((MethodSignature) pjp.getSignature()).getReturnType();
            if(returnType == String.class || returnType == ModelAndView.class){
                //返回到登录页面
                return "/303";
            }else{
                //返回没有权限的json
                return WebResult.noAuthority();
            }
        }
    }


    @Pointcut("execution(public * cn.chongho.inf.flink.controller.*.list(..))")
    public void setLoginUser() {
    }

    @AfterReturning("setLoginUser()")
    public void arounda(JoinPoint joinPoint) {
        Object[] args = joinPoint.getArgs();
        for(Object object : args){
            if(object instanceof BindingAwareModelMap){
                ServletRequestAttributes attributes = (ServletRequestAttributes) RequestContextHolder.getRequestAttributes();

                HttpServletRequest request = attributes.getRequest();
                HttpSession session = request.getSession();
                AdminUser user = (AdminUser) session.getAttribute("loginUser");
                ModelMap modelMap = (ModelMap) object;
                modelMap.put("loginUserId", user.getId());
            }
        }
    }
}
