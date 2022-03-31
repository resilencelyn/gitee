import {
  IAgreementCheckingDto,
  IAgreementCheckVo,
  IAgreementVo,
  IAuthorizationEmailSigningDto,
  IAuthorizationEmailViewingDto,
  IAuthorizationEmailVo,
  IEnterpriseMailVo,
  IOtherEmailSigningDto,
} from './dto/agreements';
import { restProxy } from '@/api/index';

// 获取协议内容
const agreeUrl = '/agreements';
// 检查协议
const check = '/agreements/check';
// 查询授权邮箱列表
const emails = '/agreements/emails';
// 授权邮箱签署
const authEmailUrl = '/agreements/authorizations';
// 其他邮箱签署
const otherEmailUrl = '/agreements/others';
// 查看企业邮件内容
const enterpriseMailsUrl = '/agreements/enterprise_mails';

/**
 * 签署协议
 */
export function agreeSign(): Promise<void> {
  return restProxy({
    url: `${agreeUrl}`,
    method: 'post',
    auth: true,
  });
}

/**
 * 查看协议
 */
export function seeCla(): Promise<IAgreementVo> {
  return restProxy({
    url: `${agreeUrl}`,
    method: 'get',
  });
}

/**
 * 检查
 * @param dto
 */
export function checkSign(dto: IAgreementCheckingDto): Promise<IAgreementCheckVo> {
  return restProxy({
    url: `${check}`,
    method: 'get',
    payload: dto,
  });
}

/**
 * 查询授权邮箱列表
 * @param dto
 */
export function getEmailList(dto: IAuthorizationEmailViewingDto): Promise<IAuthorizationEmailVo> {
  return restProxy({
    url: `${emails}`,
    method: 'get',
    payload: dto,
    auth: true,
  });
}

/**
 * 签署协议-授权邮箱
 * @param dto
 */
export function authEmailSign(dto: IAuthorizationEmailSigningDto): Promise<void> {
  return restProxy({
    url: `${authEmailUrl}`,
    method: 'post',
    payload: dto,
    auth: true,
  });
}

/**
 * 签署协议-其他邮箱
 * @param dto
 */
export function otherEmailSign(dto: IOtherEmailSigningDto): Promise<void> {
  return restProxy({
    url: `${otherEmailUrl}`,
    method: 'post',
    payload: dto,
    auth: true,
  });
}

/**
 * 查看企业邮件内容
 */
export function getEnterpriseEmails(): Promise<IEnterpriseMailVo> {
  return restProxy({
    url: `${enterpriseMailsUrl}`,
    method: 'get',
  });
}