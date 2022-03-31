package org.opencla.api.handler;

import org.opencla.api.enumeration.ErrorCodeEnum;
import org.opencla.api.exception.BaseBusinessException;
import org.opencla.api.dto.vo.ErrorMessageVo;
import lombok.extern.slf4j.Slf4j;
import org.springframework.http.HttpStatus;
import org.springframework.validation.BindException;
import org.springframework.validation.BindingResult;
import org.springframework.web.bind.MethodArgumentNotValidException;
import org.springframework.web.bind.annotation.ExceptionHandler;
import org.springframework.web.bind.annotation.ResponseStatus;
import org.springframework.web.bind.annotation.RestControllerAdvice;

/**
 * @author Daihw
 * @class ExceptionHandler
 * @description 全局异常处理
 * @create 2022/1/11 10:08 上午
 */
@Slf4j
@RestControllerAdvice
public class ApiExceptionHandler {
    /**
     * 业务处理异常
     *
     * @param e
     * @return
     */
    @ExceptionHandler(BaseBusinessException.class)
    @ResponseStatus(HttpStatus.BAD_REQUEST)
    public ErrorMessageVo baseBusinessException(BaseBusinessException e) {
        ErrorMessageVo errorMessage = e.buildErrorMessage();
        log.warn("异常码：{}，message：{}", errorMessage.getErrorCode(), errorMessage.getMessage());
        return errorMessage;
    }

    /**
     * 参数校验异常
     *
     * @param e
     * @return
     */
    @ExceptionHandler(BindException.class)
    @ResponseStatus(HttpStatus.BAD_REQUEST)
    public ErrorMessageVo bindException(BindException e) {
        BindingResult bindingResult = e.getBindingResult();
        String msg = bindingResult.getFieldErrors().get(0).getDefaultMessage();
        log.warn("参数校验失败，失败信息：{}", msg);
        return ErrorMessageVo.builder()
                .errorCode(ErrorCodeEnum.PARAMETER_INVALID.name())
                .message(msg)
                .build();
    }

    /**
     * 参数校验异常
     *
     * @param e
     * @return
     */
    @ExceptionHandler(MethodArgumentNotValidException.class)
    @ResponseStatus(HttpStatus.BAD_REQUEST)
    public ErrorMessageVo handleMethodArgumentNotValidException(MethodArgumentNotValidException e) {
        BindingResult bindingResult = e.getBindingResult();
        String msg = bindingResult.getFieldErrors().get(0).getDefaultMessage();
        log.warn("参数校验失败，失败信息：{}", msg);
        return ErrorMessageVo.builder()
                .errorCode(ErrorCodeEnum.PARAMETER_INVALID.name())
                .message(msg)
                .build();
    }
}
