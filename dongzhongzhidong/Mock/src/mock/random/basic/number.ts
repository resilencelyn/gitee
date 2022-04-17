import { random } from 'lodash-es';
import { isNumber } from 'lodash-es';
import { decode } from '../paramsParser';

// 返回一个随机的整数。
const integer = decode(
    function (min = -2 ^ 10, max = 2 ^ 10) {
        return random(min, max, false);
    },
    ['number', 'number'],
);
const natural = decode(
    function (min = 0, max = 2 ^ 10) {
        return random(min, max, false);
    },
    ['number', 'number'],
);

const float = decode(
    function (min = -2 ^ 10, max = 2 ^ 10, dmin = 1, dmax = 17) {
        if (dmin < 1) dmin = 1;
        if (dmax > 17) dmax = 17;
        // 获取小数位数
        const dcount = random(dmin, dmax, false);
        const floatPoint = min === max ? random(0, 1, true) + min : random(min, max, true);

        return parseFloat(floatPoint.toFixed(dcount));
    },
    ['number', 'number', 'number', 'number'],
);

export { integer, isNumber, random, natural, integer as int, float };
