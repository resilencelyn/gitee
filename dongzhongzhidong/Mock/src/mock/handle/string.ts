import { times } from 'lodash-es';
import { string as _string } from '../random/basic/string';
import { VirtualValue } from '../HandleDelay/virtualValue';
import { HandlerOptions } from './index.js';
import { placeholder as Placeholder } from './placeholder';
export const string = (options: HandlerOptions<string>): string | VirtualValue<string> => {
    // options 需要传递，所以不能直接解构
    const { rule, context, template } = options;
    const { range, count } = rule;
    let result: string | VirtualValue<string> = template;

    if (template.length) {
        //  'foo': '★', count 为 undefined 时
        // 'star|1-5': '★',
        result = count === undefined ? template : times(count, () => template).join('');

        // PlaceHolder 指的是在值的位置上又出现了 @语法兼容的方法
        // 'email|1-10': '@EMAIL, ',
        result = Placeholder(result, context);
    } else {
        // 'ASCII|1-10': '',
        // 'ASCII': '',
        result = range.length ? _string(count!) : template;
    }
    return result;
};
