/*
    ## Miscellaneous
*/
import { v4 as uuid } from 'uuid';
import id from './id';
import guid from './guid';
import { natural } from '../basic/number';
import { decode } from '../paramsParser';
/*
    生成一个全局的自增整数。
    类似自增主键（auto increment primary key）。
*/
let key = 0;
const increment = decode(
    (step: number) => {
        return (key += +step || 1);
    },
    ['number'],
);
const inc = increment;

// Dice
function d4() {
    return natural(1, 4);
}
function d6() {
    return natural(1, 6);
}
function d8() {
    return natural(1, 8);
}
function d12() {
    return natural(1, 12);
}
function d20() {
    return natural(1, 20);
}
function d100() {
    return natural(1, 100);
}
export { d4, d6, d8, d12, d20, d100, guid, uuid, id, increment, inc };
