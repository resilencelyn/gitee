import DICT from '../address/address_dict.json';
import { date } from '../date';
import { string } from '../basic/string';
import { sample } from 'game-random';
/*
    随机生成一个 18 位身份证。

    [身份证](http://baike.baidu.com/view/1697.htm#4)
        地址码 6 + 出生日期码 8 + 顺序码 3 + 校验码 1
    [《中华人民共和国行政区划代码》国家标准(GB/T2260)](http://zhidao.baidu.com/question/1954561.html)
*/
export default function id() {
    const rank = ['7', '9', '10', '5', '8', '4', '2', '1', '6', '3', '7', '9', '10', '5', '8', '4', '2'];
    const last = ['1', '0', 'X', '9', '8', '7', '6', '5', '4', '3', '2'];

    let id = sample(DICT).id + date('YYYYMMDD') + string('number', 3);

    let sum = id.split('').reduce((a, b, i) => {
        return a + parseInt(b, 10) * parseInt(rank[i], 10);
    }, 0);
    id += last[sum % 11];
    return id;
}
