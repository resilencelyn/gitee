import { get } from 'lodash-es';
import constant from '../constant';
import { Context } from '../handle/gen';

export const pathHandler = (result: any, delay: Context['delay']['path']) => {
    delay.forEach((vir) => {
        vir.value = vir.value.replace(constant.RE_PLACEHOLDER, (all, _, key: string) => {
            if (key.includes('/')) {
                const path = key.split('/');
                if (path[0] === '') {
                    // 绝对定位
                    return get(result, path.slice(1));
                } else {
                    // 相对定位
                    let cur = [...vir.context.path];
                    // ! 矫正定位问题
                    cur.pop();
                    let findParent = true;
                    path.forEach((i) => {
                        if (findParent && i === '..') {
                            cur.pop();
                        } else {
                            findParent = false;
                            cur.push(i);
                        }
                    });
                    return get(result, cur);
                }
            }
            return all;
        });
        vir.replaceToReal();
    });
};

export const functionHandler = (result: any, func: Context['delay']['function']) => {
    func.forEach((vir) => {
        const path = vir.context.path;

        const thisArg = path.length ? get(result, path) : result;

        vir.value = vir.value.call(thisArg, result, vir.context);
        vir.replaceToReal();
    });
};
