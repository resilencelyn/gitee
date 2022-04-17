import { isArrayLikeObject, isNumber } from 'lodash-es';

/** 执行多次该函数 */
function each<T>(
    number: number,
    iterator: (this: T | undefined, index: number, value: number) => boolean | void,
    thisArg?: T,
): void;
/** 遍历数组 */
function each<E, T>(
    arrLike: E[],
    iterator: (this: T | undefined, value: E, index: number) => boolean | void,
    thisArg?: T,
): void;
function each<O extends Object, T>(
    obj: O,
    iterator: (this: T | undefined, value: O, key: keyof O) => boolean | void,
    thisArg?: T,
): void;
function each(obj: any, iterator: any, thisArg?: any): void {
    let i;
    let key;
    if (isNumber(obj)) {
        /** 重复使用函数 */
        [...Array(obj).keys()].forEach((index) => iterator.call(thisArg, index, index));
    } else if (isArrayLikeObject(obj)) {
        /** 遍历类数组对象 */
        for (i = 0; i < obj.length; i++) {
            const result = iterator.call(thisArg, obj[i], i, obj);
            if (result === false) break;
        }
    } else {
        /** 遍历对象 */
        // 注意这里是被打断，而非跳过
        for (key in obj) {
            const result = iterator.call(thisArg, obj[key], key, obj);
            if (result === false) break;
        }
    }
}
export { each };
