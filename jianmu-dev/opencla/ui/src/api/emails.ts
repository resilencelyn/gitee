import { IEmailImageCodeRefreshingDto, IEmailVerificationCodeSendingDto, IImageCodeVo } from '@/api/dto/emails';
import { restProxy } from '@/api/index';


const send = '/emails/send';
const imageCode = '/emails/image_code';

/**
 * 发送邮箱验证码
 * @param dto
 */
export function fetchEmailCode(dto: IEmailVerificationCodeSendingDto): Promise<void> {
  return restProxy({
    url: `${send}`,
    method: 'post',
    payload: dto,
    auth: true,
  });
}

/**
 * 刷新图片验证码
 * @param dto
 */
export function fetchImageCode(dto: IEmailImageCodeRefreshingDto): Promise<IImageCodeVo> {
  return restProxy({
    url: `${imageCode}`,
    method: 'patch',
    payload: dto,
    auth: true,
  });
}
