import { float } from '../random/basic/number';
import { HandlerFunction, HandlerOptions } from './index';
const store = new Map<string, number>();
export const number = ({
    template,
    rule: {
        decimal,
        dmin,
        dmax,
        range: [min, max],
        count,
        step,
    },
    context,
}: HandlerOptions<number>) => {
    const [, dec] = template.toString().split('.');
    if (decimal) {
        // 'float1|.1-10': 10,
        // 'float2|1-100.1-10': 1,
        // 'float3|999.1-10': 1,
        // 'float4|.3-10': 123.123,

        const result = float(min, max, dmin, dmax); // 内置字符串转数字
        if (dec) {
            // 这里是原来有小数部分时，进行小数部分覆盖
            const floatString = result
                .toString()
                .replace(new RegExp(`\\.\\d{0,${dec.length}}|(?<=[^\\.]+)$`, 'm'), '.' + dec);

            return parseFloat(floatString);
        } else {
            return result;
        }
    } else {
        // integer
        // 'grade1|1-100': 1,
        if (step) {
            const key = context.templatePath.join();
            const val = store.get(key) || template;
            store.set(key, val + 1);
            return val + 1;
        } else {
            return typeof count === 'number' ? count : 1;
        }
    }
};
