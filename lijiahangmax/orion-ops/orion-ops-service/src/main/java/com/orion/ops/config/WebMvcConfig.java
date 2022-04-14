package com.orion.ops.config;

import com.orion.exception.*;
import com.orion.exception.argument.CodeArgumentException;
import com.orion.exception.argument.HttpWrapperException;
import com.orion.exception.argument.InvalidArgumentException;
import com.orion.exception.argument.RpcWrapperException;
import com.orion.lang.wrapper.HttpWrapper;
import com.orion.ops.consts.MessageConst;
import com.orion.ops.interceptor.AuthenticateInterceptor;
import com.orion.ops.interceptor.IpFilterInterceptor;
import com.orion.ops.interceptor.RoleInterceptor;
import com.orion.ops.interceptor.UserActiveInterceptor;
import lombok.extern.slf4j.Slf4j;
import org.springframework.context.annotation.Configuration;
import org.springframework.http.converter.HttpMessageNotReadableException;
import org.springframework.validation.BindException;
import org.springframework.web.bind.MethodArgumentNotValidException;
import org.springframework.web.bind.annotation.ExceptionHandler;
import org.springframework.web.bind.annotation.RestControllerAdvice;
import org.springframework.web.method.annotation.MethodArgumentTypeMismatchException;
import org.springframework.web.multipart.MaxUploadSizeExceededException;
import org.springframework.web.servlet.config.annotation.CorsRegistry;
import org.springframework.web.servlet.config.annotation.InterceptorRegistry;
import org.springframework.web.servlet.config.annotation.WebMvcConfigurer;

import javax.annotation.Resource;
import javax.servlet.http.HttpServletRequest;
import java.io.IOException;
import java.io.InterruptedIOException;

/**
 * spring mvc 配置
 *
 * @author Jiahang Li
 * @version 1.0.0
 * @since 2021/4/2 10:24
 */
@Slf4j
@Configuration
@RestControllerAdvice
public class WebMvcConfig implements WebMvcConfigurer {

    @Resource
    private IpFilterInterceptor ipFilterInterceptor;

    @Resource
    private AuthenticateInterceptor authenticateInterceptor;

    @Resource
    private RoleInterceptor roleInterceptor;

    @Resource
    private UserActiveInterceptor userActiveInterceptor;

    @Override
    public void addInterceptors(InterceptorRegistry registry) {
        // IP拦截器
        registry.addInterceptor(ipFilterInterceptor)
                .addPathPatterns("/**")
                .order(5);
        // 认证拦截器
        registry.addInterceptor(authenticateInterceptor)
                .addPathPatterns("/orion/api/**")
                .order(10);
        // 权限拦截器
        registry.addInterceptor(roleInterceptor)
                .addPathPatterns("/orion/api/**")
                .order(20);
        // 活跃拦截器
        registry.addInterceptor(userActiveInterceptor)
                .addPathPatterns("/orion/api/**")
                .excludePathPatterns("/orion/api/auth/**")
                .order(30);
    }

    @Override
    public void addCorsMappings(CorsRegistry registry) {
        registry.addMapping("/**")
                .allowCredentials(true)
                .allowedOriginPatterns("*")
                .allowedMethods("*")
                .allowedHeaders("*")
                .maxAge(3600);
    }

    @ExceptionHandler(value = Exception.class)
    public HttpWrapper<?> normalExceptionHandler(HttpServletRequest request, Exception ex) {
        log.error("normalExceptionHandler url: {}, 抛出异常: {}, message: {}", request.getRequestURI(), ex.getClass(), ex.getMessage());
        ex.printStackTrace();
        return HttpWrapper.error().msg(MessageConst.EXCEPTION_MESSAGE).data(ex.getMessage());
    }

    @ExceptionHandler(value = ApplicationException.class)
    public HttpWrapper<?> applicationExceptionHandler(HttpServletRequest request, Exception ex) {
        log.error("applicationExceptionHandler url: {}, 抛出异常: {}, message: {}", request.getRequestURI(), ex.getClass(), ex.getMessage());
        ex.printStackTrace();
        return HttpWrapper.error().msg(ex.getMessage());
    }

    @ExceptionHandler(value = {HttpMessageNotReadableException.class, MethodArgumentTypeMismatchException.class,
            HttpMessageNotReadableException.class, MethodArgumentNotValidException.class, BindException.class})
    public HttpWrapper<?> httpRequestExceptionHandler(HttpServletRequest request, Exception ex) {
        log.error("httpRequestExceptionHandler url: {}, http请求异常: {}, message: {}", request.getRequestURI(), ex.getClass(), ex.getMessage());
        ex.printStackTrace();
        return HttpWrapper.error().msg(MessageConst.INVALID_PARAM);
    }

    @ExceptionHandler(value = {InvalidArgumentException.class, IllegalArgumentException.class})
    public HttpWrapper<?> invalidArgumentExceptionHandler(HttpServletRequest request, Exception ex) {
        log.error("invalidArgumentExceptionHandler url: {}, 参数异常: {}, message: {}", request.getRequestURI(), ex.getClass(), ex.getMessage());
        ex.printStackTrace();
        return HttpWrapper.error().msg(ex.getMessage());
    }

    @ExceptionHandler(value = {IOException.class, IORuntimeException.class})
    public HttpWrapper<?> ioExceptionHandler(HttpServletRequest request, Exception ex) {
        log.error("ioExceptionHandler url: {}, io异常: {}, message: {}", request.getRequestURI(), ex.getClass(), ex.getMessage());
        ex.printStackTrace();
        return HttpWrapper.error().msg(MessageConst.IO_EXCEPTION_MESSAGE).data(ex.getMessage());
    }

    @ExceptionHandler(value = {SftpException.class, com.jcraft.jsch.SftpException.class})
    public HttpWrapper<?> sftpExceptionHandler(HttpServletRequest request, Exception ex) {
        log.error("sftpExceptionHandler url: {}, sftp处理异常: {}, message: {}", request.getRequestURI(), ex.getClass(), ex.getMessage());
        ex.printStackTrace();
        return HttpWrapper.error().msg(MessageConst.SFTP_OPERATOR_ERROR).data(ex.getMessage());
    }

    @ExceptionHandler(value = EncryptException.class)
    public HttpWrapper<?> encryptExceptionHandler(HttpServletRequest request, Exception ex) {
        log.error("encryptExceptionHandler url: {}, 数据加密异常: {}, message: {}", request.getRequestURI(), ex.getClass(), ex.getMessage());
        ex.printStackTrace();
        return HttpWrapper.error().msg(MessageConst.ENCRYPT_ERROR).data(ex.getMessage());
    }

    @ExceptionHandler(value = DecryptException.class)
    public HttpWrapper<?> decryptExceptionHandler(HttpServletRequest request, Exception ex) {
        log.error("decryptExceptionHandler url: {}, 数据解密异常: {}, message: {}", request.getRequestURI(), ex.getClass(), ex.getMessage());
        ex.printStackTrace();
        return HttpWrapper.error().msg(MessageConst.DECRYPT_ERROR).data(ex.getMessage());
    }

    @ExceptionHandler(value = VcsException.class)
    public HttpWrapper<?> vcsExceptionHandler(HttpServletRequest request, Exception ex) {
        log.error("vcsExceptionHandler url: {}, vcs处理异常: {}, message: {}", request.getRequestURI(), ex.getClass(), ex.getMessage());
        ex.printStackTrace();
        return HttpWrapper.error().msg(MessageConst.VCS_OPERATOR_ERROR).data(ex.getMessage());
    }

    @ExceptionHandler(value = {TaskExecuteException.class, ExecuteException.class})
    public HttpWrapper<?> taskExceptionHandler(HttpServletRequest request, Exception ex) {
        log.error("taskExceptionHandler url: {}, task处理异常: {}, message: {}", request.getRequestURI(), ex.getClass(), ex.getMessage());
        ex.printStackTrace();
        return HttpWrapper.error().msg(MessageConst.TASK_ERROR).data(ex.getMessage());
    }

    @ExceptionHandler(value = ConnectionRuntimeException.class)
    public HttpWrapper<?> connectionExceptionHandler(HttpServletRequest request, Exception ex) {
        log.error("connectionExceptionHandler url: {}, connect异常: {}, message: {}", request.getRequestURI(), ex.getClass(), ex.getMessage());
        ex.printStackTrace();
        return HttpWrapper.error().msg(MessageConst.CONNECT_ERROR).data(ex.getMessage());
    }

    @ExceptionHandler(value = {TimeoutException.class, java.util.concurrent.TimeoutException.class})
    public HttpWrapper<?> timeoutExceptionHandler(HttpServletRequest request, Exception ex) {
        log.error("timeoutExceptionHandler url: {}, timeout异常: {}, message: {}", request.getRequestURI(), ex.getClass(), ex.getMessage());
        ex.printStackTrace();
        return HttpWrapper.error().msg(MessageConst.TIMEOUT_ERROR).data(ex.getMessage());
    }

    @ExceptionHandler(value = {InterruptedException.class, InterruptedRuntimeException.class, InterruptedIOException.class})
    public HttpWrapper<?> interruptExceptionHandler(HttpServletRequest request, Exception ex) {
        log.error("interruptExceptionHandler url: {}, interrupt异常: {}, message: {}", request.getRequestURI(), ex.getClass(), ex.getMessage());
        ex.printStackTrace();
        return HttpWrapper.error().msg(MessageConst.INTERRUPT_ERROR).data(ex.getMessage());
    }

    @ExceptionHandler(value = UnsafeException.class)
    public HttpWrapper<?> unsafeExceptionHandler(HttpServletRequest request, Exception ex) {
        log.error("unsafeExceptionHandler url: {}, unsafe异常: {}, message: {}", request.getRequestURI(), ex.getClass(), ex.getMessage());
        ex.printStackTrace();
        return HttpWrapper.error().msg(MessageConst.UNSAFE_OPERATOR).data(ex.getMessage());
    }

    @ExceptionHandler(value = LogException.class)
    public HttpWrapper<?> logExceptionHandler(HttpServletRequest request, LogException ex) {
        log.error("logExceptionHandler url: {}, 处理异常打印日志: {}, message: {}", request.getRequestURI(), ex.getClass(), ex.getMessage());
        if (ex.hasCause()) {
            ex.printStackTrace();
        }
        return HttpWrapper.error().msg(MessageConst.VCS_OPERATOR_ERROR).data(ex.getMessage());
    }

    @ExceptionHandler(value = ParseCronException.class)
    public HttpWrapper<?> parseCronExceptionHandler(ParseCronException ex) {
        return HttpWrapper.error().msg(MessageConst.ERROR_EXPRESSION).data(ex.getMessage());
    }

    @ExceptionHandler(value = MaxUploadSizeExceededException.class)
    public HttpWrapper<?> maxUploadSizeExceededExceptionHandler(HttpServletRequest request, MaxUploadSizeExceededException ex) {
        log.error("maxUploadSizeExceededExceptionHandler url: {}, 上传异常: {}, message: {}", request.getRequestURI(), ex.getClass(), ex.getMessage(), ex);
        return HttpWrapper.error().msg(MessageConst.FILE_TOO_LARGE).data(ex.getMessage());
    }

    @ExceptionHandler(value = CodeArgumentException.class)
    public HttpWrapper<?> codeArgumentExceptionHandler(CodeArgumentException ex) {
        return HttpWrapper.error().code(ex.getCode()).msg(ex.getMessage());
    }

    @ExceptionHandler(value = HttpWrapperException.class)
    public HttpWrapper<?> httpWrapperExceptionHandler(HttpWrapperException ex) {
        return ex.getWrapper();
    }

    @ExceptionHandler(value = RpcWrapperException.class)
    public HttpWrapper<?> rpcWrapperExceptionHandler(RpcWrapperException ex) {
        return ex.getWrapper().toHttpWrapper();
    }

}
