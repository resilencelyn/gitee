import { IAuthorizationEmailVo } from '@/api/dto/agreements';
import { Mutable } from '@/utils/lib';

// 邮箱列表返回值
export interface IAuthorizationEmailList extends Mutable<IAuthorizationEmailVo> {
}