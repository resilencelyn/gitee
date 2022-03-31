package org.opencla.api.exception;

import org.opencla.api.enumeration.ErrorCodeEnum;
import org.opencla.api.dto.vo.ErrorMessageVo;

/**
 * 基础异常类
 */
public abstract class BaseBusinessException extends RuntimeException {
    private final String message;
    private final ErrorCodeEnum errorCode;

    protected BaseBusinessException(ErrorCodeEnum errorCode, String message) {
        this.errorCode = errorCode;
        this.message = message;
    }

    protected BaseBusinessException(ErrorCodeEnum errorCode) {
        this(errorCode, null);
    }

    /**
     * 构建错误信息
     *
     * @return
     */
    public ErrorMessageVo buildErrorMessage() {
        return ErrorMessageVo.builder()
                .errorCode(this.errorCode.name())
                .message(this.message == null ? this.errorCode.getMessage() : this.message)
                .build();
    }
}
