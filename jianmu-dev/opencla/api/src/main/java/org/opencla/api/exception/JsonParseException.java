package org.opencla.api.exception;

import org.opencla.api.enumeration.ErrorCodeEnum;

/**
 * json解析异常
 */
public class JsonParseException extends BaseBusinessException {
    public JsonParseException() {
        super(ErrorCodeEnum.JSON_PARSE_EXCEPTION);
    }
}
