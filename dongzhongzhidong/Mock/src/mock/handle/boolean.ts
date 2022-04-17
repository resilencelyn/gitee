import { HandlerOptions } from '.';
import { bool } from '../random/basic/boolean';
export function boolean({ rule, template, context }: HandlerOptions<boolean>): boolean | boolean[] {
    const { max, min, count } = rule;
    if (count === max && count === min) {
        // 'prop|trueFalsePoint-maxEdge': false, 选定正反分界线和最大值
        return bool(min!, max!, template);
    } else {
        return bool(1, count!, template);
        // 'prop|multiple': false, 概率为 1/multiple
    }
}
