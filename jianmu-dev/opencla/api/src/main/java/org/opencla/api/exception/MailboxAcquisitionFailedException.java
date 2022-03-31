package org.opencla.api.exception;

import org.opencla.api.enumeration.ErrorCodeEnum;

/**
 * 获取邮箱失败异常
 */
public class MailboxAcquisitionFailedException extends BaseBusinessException {
    public MailboxAcquisitionFailedException() {
        super(ErrorCodeEnum.MAILBOX_ACQUISITION_FAILED);
    }

    public MailboxAcquisitionFailedException(String message) {
        super(ErrorCodeEnum.MAILBOX_ACQUISITION_FAILED, message);
    }
}
