// 使用 lodash 提供的方式
import { isObject, isArray } from 'lodash-es';
/** 获取类型的字符串描述，为小写 */
export function getType(obj: any): string {
    return obj === null || obj === undefined
        ? String(obj)
        : Object.prototype.toString
              .call(obj)
              .match(/\[object (\w+)\]/)![1]
              .toLowerCase();
}

export function isObjectOrArray(value: any) {
    return isObject(value) || isArray(value);
}

export function isNumeric(value: any) {
    return !isNaN(parseFloat(value)) && isFinite(value);
}
export { isString, isObject, isArray, isRegExp, isFunction } from 'lodash-es';
