/*
    ## Address
*/
import DICT from './address_dict.json';
const REGION = ['东北', '华北', '华东', '华中', '华南', '西南', '西北'];

import { natural } from '../basic/number.js';
import { sample } from 'game-random';
import { decode } from '../paramsParser';

// 随机生成一个大区。
function region() {
    return sample(REGION);
}
// 随机生成一个（中国）省（或直辖市、自治区、特别行政区）。
function province() {
    return sample(DICT).name;
}

// 随机生成一个（中国）市。
const city = decode(
    (prefix: boolean) => {
        const province = sample(DICT);
        const city = sample(province.children);
        return prefix ? [province.name, city.name].join(' ') : city.name;
    },
    ['boolean'],
);

// 随机生成一个（中国）县。
const county = decode(
    (prefix: boolean) => {
        const province = sample(DICT);
        const city = sample(province.children);
        const county =
            city.children && city.children.length
                ? sample(city.children)
                : {
                      name: '-',
                  };
        return prefix ? [province.name, city.name, county.name].join(' ') : county.name;
    },
    ['boolean'],
);

// 随机生成一个邮政编码（六位数字）。
const zip = decode(
    (len: number) => {
        let zip = '';
        for (let i = 0; i < (len || 6); i++) zip += natural(0, 9);
        return zip;
    },
    ['number'],
);

//  function address() {}
//  function phone() {}
//  function areacode() {}
//  function street() {}
//  function street_suffixes() {}
//  function street_suffix() {}
//  function states() {}
//  function state() {}

export { region, province, city, county, zip };
