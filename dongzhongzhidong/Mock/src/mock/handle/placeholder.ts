import Constant from '../constant';
import * as Random from '../random/index';
import { VirtualValue } from '../HandleDelay/virtualValue';
import { Context, gen } from './gen';

// 占位符即是字符串函数的意思
function placeholder(resultString: string, context: Context) {
    let delayTrigger = false;
    const result = resultString.replace(Constant.RE_PLACEHOLDER, function (all, _, key, params) {
        // 路径触发函数
        if (key.includes('/')) {
            if (delayTrigger === false) {
                // 延迟到最后加载
                delayTrigger = true;
            }
            return all;
        }

        // 解析占位符的参数
        if (key in Random) {
            const args = params
                ? params.split(/\s*,\s*/).map((param: string) => {
                      // 递归解析参数中的占位符
                      if (Constant.RE_PLACEHOLDER.test(param)) {
                          return placeholder(param, context);
                      }
                      return param;
                  })
                : [];
            //! 这里传入字符串给所有的 Random 函数
            return (Random as any)[key].apply(context.currentContext, args);
        }
        return all;
    });
    if (delayTrigger) {
        console.log('使用虚拟');
        const vir = new VirtualValue(result, context);
        context.delay.path.push(vir);
        return vir;
    } else {
        return result;
    }
}
export { placeholder };
