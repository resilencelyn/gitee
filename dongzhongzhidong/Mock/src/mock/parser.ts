/*
     parser ,将 ( 字符串模板中的函数参数 ) 解析为对象
 */

import { random } from 'game-random';
import { isNumber } from 'lodash-es';
import Constant from './constant.js';
function parseRange(_range?: string) {
    // 解析 min，max 值
    const [, _min, _max] = (_range && _range.match(Constant.RE_RANGE)) || [];
    const min = (_min && parseInt(_min, 10)) || 1;
    let max = (_max && parseInt(_max, 10)) || undefined;
    let count: number | undefined;
    // 求得两者之间的一个整数
    if (isNumber(min)) {
        count = isNumber(max) ? random(min, max, false) : min;
        if (max === undefined) max = count;
    }
    return { min, max, count };
}
/** 解析数据模板 */
export function parser(paramString = '') {
    // 解析整个指令字符串
    const parameters: [string, string, string, string, string] | [] = (paramString.match(Constant.RE_KEY) as any) || [];
    const [, _name, _step, _range, _drange] = parameters;
    const { min, max, count } = parseRange(_range);
    const { min: dmin, max: dmax, count: dcount } = parseRange(_drange);

    const result = {
        /** 没有匹配到则为空数组 */
        parameters,
        range: [min, max],
        min,
        step: _step === undefined ? undefined : parseInt(_step, 10),
        max,
        count,
        decimal: typeof dcount === 'number',
        dmin,
        dmax,
        dcount,
    };
    return result;
}
export type ParserResult = ReturnType<typeof parser>;
