/**
 * 获取授权url
 */
export interface IAuthorizationUrlGettingDto extends Readonly<{
  thirdPartyType: string;
  redirectUri: string;
}> {
}

/**
 * 授权url返回值
 */
export interface IAuthorizationUrlVo extends Readonly<{
  authorizationUrl: string;
}> {
}

/**
 * 返回令牌
 */
export interface ILoggingDto extends Readonly<{
  code: string;
  thirdPartyType: string;
  redirectUri: string;
}> {
}

/**
 * token
 */
export interface ITokenVo extends Readonly<{
  token: string;
}> {
}

/**
 * 获取用户信息
 */
export interface IUserinfoGettingDto extends Readonly<{
  thirdPartyType: string;
}> {
}

/**
 * 用户信息
 */
export interface IUserinfoVo extends Readonly<{
  email: string;
  username: string;
  headUrl: string;
  signFlag: boolean;
}> {
}
