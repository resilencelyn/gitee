import { ComponentPublicInstance } from 'vue';
import { Router } from 'vue-router';
import { Store } from 'vuex';
import { IRootState } from '@/model';
import { HttpError, TimeoutError } from '@/utils/rest/error';
import { IErrorMessageVo, ISsoErrorMessageVo } from '@/api/dto/common';
import { namespace as sessionNs } from '@/store/modules/session';
import { isWap } from '@/utils/ua';

/**
 * 全局错误处理
 * @param error
 * @param instance
 * @param info
 * @param router
 * @param store
 */
export async function globalErrorHandler(
  error: Error,
  instance: ComponentPublicInstance | null,
  info: string | null,
  router: Router,
  store: Store<IRootState>,
) {
  const proxy = instance as any;

  if (error instanceof TimeoutError) {
    await router.push({ name: 'network-error' });
    return;
  }

  if (error instanceof HttpError) {
    const {
      response: { status, data },
    } = error as HttpError;

    switch (status) {
      case 400:
      case 403:
        // 常规处理
        const msg = (data as IErrorMessageVo).message ||
          // 兼容sso错误处理
          (data as ISsoErrorMessageVo).errorMsg;

        if (isWap()) {
          proxy.$toast({ message: msg, className: 'toast-index' });
        } else {
          proxy.$error(msg);
        }
        break;
      case 401:
        // 清理token
        store.commit(`${sessionNs}/mutateDeletion`);

        break;
      case 404:
      case 500:
        await router.push({
          name: 'http-status-error',
          params: { value: status },
        });
        break;
      // TODO 待扩展，处理其他错误码
    }
    return;
  }

  proxy.$error(`未知错误：${error.message}`);
  console.error(error);
  if (info) {
    console.error(info);
  }
}
