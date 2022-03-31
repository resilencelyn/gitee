import { ILoggingDto, ITokenVo } from '@/api/dto/auth';
import { Mutable } from '@/utils/lib';

/**
 * vuex状态
 */
export interface IState {
  session?: ITokenVo;
}

/**
 * 存储
 */
export interface IStorage {
  [key: string]: IState;
}

export interface ILoggingDtoParams extends Mutable<ILoggingDto> {
}