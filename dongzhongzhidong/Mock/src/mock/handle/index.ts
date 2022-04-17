import { ParserResult } from '../parser';
import { Context } from './gen';

export { array } from './array';
export { boolean } from './boolean';
export { number } from './number';
export { object } from './object';
export { regexp } from './regexp';
export { string } from './string';

export type HandlerOptions<T = string> = {
    // 属性值类型
    type: string;
    // 属性值模板
    template: T;
    // 属性名 + 生成规则
    name: string;
    // 属性名
    parsedName: string;
    // 解析后的生成规则
    rule: ParserResult;
    // 相关上下文
    context: Context;
};
export type HandlerFunction<T> = (Input: HandlerOptions) => T;
