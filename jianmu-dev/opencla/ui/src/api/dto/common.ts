/**
 * 错误信息
 */
export interface IErrorMessageVo
  extends Readonly<{
    errorCode: string;
    message: string;
  }> {
}

/**
 * sso错误信息
 */
export interface ISsoErrorMessageVo
  extends Readonly<{
    success: boolean;
    timestamp: number;
    errorCode: string;
    errorMsg: string;
  }> {
}