import { sample, sampleSize } from 'game-random';
import { cloneDeep, isNumber, times } from 'lodash-es';
import { gen } from './gen.js';
import { HandlerFunction, HandlerOptions } from './index.js';
const store = new Map<string, number>();
/** 数组类型的构建器 */

export const array = ({ rule, template, context }: HandlerOptions<any[]>) => {
    const { min, max, parameters, count, step } = rule;
    let result: any[] = [];

    // 'name|1': []
    // 'name|count': []
    // 'name|min-max': []
    if (template.length === 0) return [];

    // 'arr': [{ 'email': '@EMAIL' }, { 'email': '@EMAIL' }]
    if (parameters.length === 0) {
        return template.map((item, index) => {
            return gen(item, index.toString(), {
                ...context,
                path: context.path,
                templatePath: context.templatePath,
                currentContext: result,
                templateCurrentContext: template,
            });
        });
    } else {
        // 'data|+1': [{}, {}]
        if (isNumber(step)) {
            //! array 依照上级的路径进行判断
            const path = context.path;

            // 从存储中获取一个元素
            const storeKey = path.join();
            const totalIndex = (store.get(storeKey) || 1) + step;
            store.set(storeKey, totalIndex);

            const index = totalIndex % template.length;
            return gen(template[index], index.toString(), {
                ...context,
                path: context.path,
                templatePath: context.templatePath,
                currentContext: [],
                templateCurrentContext: template,
            });
        } else if (min === max && max === count) {
            // ! array|1 是抽取 ，array|2 是重复
            // 'method|1': ['GET', 'POST', 'HEAD', 'DELETE']
            if (count === 1) {
                const value = sample(template);
                return gen(value, template.indexOf(value).toString(), {
                    ...context,
                    path: context.path,
                    templatePath: context.templatePath,
                    currentContext: [],
                    templateCurrentContext: template,
                });
            } else {
                const data = times(count, () =>
                    template.map((i) => {
                        const key = template.indexOf(i).toString();
                        return gen(i, key, {
                            ...context,
                            path: context.path,
                            templatePath: context.templatePath,
                            currentContext: [],
                            templateCurrentContext: template,
                        });
                    }),
                );
                return data;
            }
        } else if (count) {
            // 'data|1-10': [{}]

            return times(count!, () => {
                return template.map((item, index) => {
                    const result = gen(item, index.toString(), {
                        ...context,
                        path: context.path,
                        templatePath: context.templatePath,
                        currentContext: [],
                        templateCurrentContext: template,
                    });
                    return result;
                });
            });
        }
    }
};
