/**
 * 查看协议
 */
export interface IAgreementVo extends Readonly<{
  agreementContent: string;
  projectName: string;
}> {
}

/**
 * 检查
 */
export interface IAgreementCheckingDto extends Readonly<{
  email: string;
}> {
}

/**
 * 检查返回值
 */
export interface IAgreementCheckVo extends Readonly<{
  isSign: boolean;
}> {
}

/**
 * 查询授权邮箱列表-返回值
 */
export interface IAuthorizationEmailVo extends Readonly<[{
  email: string;
  labels: string[];
  isSigned: boolean;
}]> {
}

/**
 * 查询授权邮箱列表-参数
 */
export interface IAuthorizationEmailViewingDto extends Readonly<{
  thirdPartyType: string;
}> {
}

/**
 * 签署协议-授权邮箱
 */
export interface IAuthorizationEmailSigningDto extends Readonly<{
  thirdPartyType: string;
  email: string;
}> {
}

/**
 * 签署协议-其他邮箱
 */
export interface IOtherEmailSigningDto extends Readonly<{
  email: string;
  checkCode: string;
  emailFlag: string;
}> {
}

/**
 * 查看企业邮件内容
 */
export interface IEnterpriseMailVo extends Readonly<{
  enterpriseMail: string;
}> {
}