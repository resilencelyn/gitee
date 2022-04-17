import Constant from '../constant.js';
import { getType } from '../util.js';
import { parser as Parser } from '../parser.js';
import * as typeset from './index';
import { HandlerFunction } from './index';
import { VirtualValue } from '../HandleDelay/virtualValue';
import { functionHandler, pathHandler } from '../HandleDelay/index.js';
export interface Context {
    /* 当前访问路径，只有属性名，不包括生成规则 */
    path: (string | number)[];
    /** 原来模板中的位置 */
    templatePath: (string | number)[];
    /* 最终属性值的上下文 */
    currentContext: Object | any[];
    /*  属性值模板的上下文 */
    templateCurrentContext: Object | any[];
    /** 推迟执行的函数模板 */
    delay: {
        /** 值引用依赖 */
        path: VirtualValue<string>[];
        /**函数引用依赖 */
        function: VirtualValue<Function>[];
    };
    /* 整个原始模板 */
    templateRoot: any;
}

const createContext = (obj: Partial<Context>): Context => {
    return {
        path: [],
        templatePath: [],
        currentContext: {},
        delay: {
            function: [],
            path: [],
        },
        templateCurrentContext: {},
        templateRoot: '',
        ...obj,
    };
};
/** 模板对象解析器，所有的函数的解析入口 */
export function gen(template: unknown, key: string = '', context?: Context) {
    const type = getType(template);
    const func: HandlerFunction<any> | undefined = (typeset as any)[type];

    if (func) {
        if (context === undefined) {
            context = createContext({
                templateRoot: template,
            });
        }
        const data = func({
            type,
            template: template as any,
            name: key,
            parsedName: key ? ('' + key).replace(Constant.RE_KEY, '$1') : key,
            rule: Parser(key),
            context,
        });
        return data;
    } else {
        return template;
    }
}

export function genInput(template: string) {
    const context = createContext({
        templateRoot: template,
    });
    const result = gen(template, '', context);
    pathHandler(result, context.delay.path);
    functionHandler(result, context.delay.function);
    return result;
}
