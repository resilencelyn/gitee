import { random } from 'game-random';
import { decode } from '../paramsParser';
/** 布尔随机，
 * @param cur 反转结果
 */

const boolean = decode(
    (midPoint: number, max: number, cur = true) => {
        // ! cur:boolean 是划分概率的一个启动符号
        const result = random(0, max, true) < midPoint;
        return cur ? result : !result;
    },
    ['number', 'number', 'boolean'],
);
export { boolean, boolean as bool };
