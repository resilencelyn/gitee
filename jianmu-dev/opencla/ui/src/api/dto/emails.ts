/**
 * 发送邮箱验证码
 */
export interface IEmailVerificationCodeSendingDto extends Readonly<{
  emailFlag: string;
  emailAddress: string;
  imageCode?: string;
}> {
}

/**
 * 刷新图片验证码
 */
export interface IEmailImageCodeRefreshingDto extends Readonly<{
  emailFlag: string;
  emailAddress: string;
}> {
}

/**
 * 图片验证码返回值
 */
export interface IImageCodeVo extends Readonly<{
  base64: string;
}> {
}