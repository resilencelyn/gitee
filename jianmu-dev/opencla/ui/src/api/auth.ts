import {
  IAuthorizationUrlGettingDto,
  IAuthorizationUrlVo,
  ILoggingDto,
  ITokenVo,
  IUserinfoGettingDto,
  IUserinfoVo,
} from './dto/auth';
import { restProxy } from '@/api/index';

const authUrl = '/auth/url';
const tokenUrl = '/auth/login';
const userUrl = '/auth/users';

/**
 * 获取授权url
 * @param dto
 */
export function getAuthUrl(dto: IAuthorizationUrlGettingDto): Promise<IAuthorizationUrlVo> {
  return restProxy({
    url: `${authUrl}`,
    method: 'get',
    payload: dto,
    auth: true,
  });
}

/**
 * 返回令牌
 * @param dto
 */
export function getToken(dto: ILoggingDto): Promise<ITokenVo> {
  return restProxy({
    url: `${tokenUrl}`,
    method: 'get',
    payload: dto,
    auth: true,
  });
}

/**
 * 获取用户信息
 * @param dto
 */
export function getUserInfo(dto: IUserinfoGettingDto): Promise<IUserinfoVo> {
  return restProxy({
    url: `${userUrl}`,
    method: 'get',
    payload: dto,
    auth: true,
  });
}

