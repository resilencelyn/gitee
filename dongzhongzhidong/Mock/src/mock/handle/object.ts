import { sampleSize } from 'game-random';
import Constant from '../constant.js';
import { VirtualValue } from '../HandleDelay/virtualValue';
import { gen } from './gen.js';
import { HandlerOptions } from './index.js';

const setValueToObject = (obj: any, key: string, value: VirtualValue<any> | any) => {
    if (value instanceof VirtualValue) {
        value.parent = obj;
        value.key = key;
    }
    obj[key] = value;
};

export const object = ({
    template,
    rule: { min, max, count, parameters },
    context,
}: HandlerOptions<{ [key: string]: any }>) => {
    const { path, templatePath } = context!;
    const result = {} as any;
    // 'obj|min-max': {}
    if (parameters.length) {
        console.log(count, min, max);
        sampleSize(Object.keys(template), count).forEach((key) => {
            const name = key.replace(Constant.RE_KEY, '$1');

            setValueToObject(
                result,
                name,
                gen(template[key], key, {
                    ...context,
                    path: [...path, name],
                    templatePath: [...templatePath, key],
                    currentContext: result,
                    templateCurrentContext: template,
                }),
            );
        });
    } else {
        // 'obj': {}
        Object.entries(template).forEach(([key, value]) => {
            const name = key.replace(Constant.RE_KEY, '$1');
            if (typeof value === 'function') {
                //! 非函数类型的键值对优先于函数类型，保证依赖加载
                // 函数会进行延迟执行
                const vir = new VirtualValue(value, context);
                context.delay.function.push(vir);
                setValueToObject(result, name, vir);
            } else {
                setValueToObject(
                    result,
                    name,
                    gen(value, key, {
                        ...context,
                        path: path.concat(key),
                        templatePath: templatePath.concat(key),
                        currentContext: result,
                        templateCurrentContext: template,
                    }),
                );
            }
        });
    }
    return result;
};
