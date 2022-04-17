/*
    ## Name

    [Beyond the Top 1000 Names](http://www.ssa.gov/oact/babynames/limits.html)
*/
import { sample } from 'game-random';
import Name from './name.json';
const { cfirstName, clastName } = Name;
import { faker } from '@faker-js/faker';
import { decode } from '../paramsParser';
// 随机生成一个常见的英文名。
function first() {
    return faker.name.firstName();
}

// 随机生成一个常见的英文姓。
function last() {
    return faker.name.lastName();
}

// 随机生成一个常见的英文姓名。
const name = decode(
    (middle: boolean) => {
        const mid = middle ? ` ${first()} ` : ' ';
        return `${first()} ${mid} ${last()}`;
    },
    ['boolean'],
);

/*
    随机生成一个常见的中文姓。
    [世界常用姓氏排行](http://baike.baidu.com/view/1719115.htm)
    [玄派网 - 网络小说创作辅助平台](http://xuanpai.sinaapp.com/)
*/
function cfirst() {
    return sample(cfirstName);
}
/*
    随机生成一个常见的中文名。
    [中国最常见名字前50名_三九算命网](http://www.name999.net/xingming/xingshi/20131004/48.html)
*/
function clast() {
    return sample(clastName);
}
// 随机生成一个常见的中文姓名。
function cname() {
    return cfirst() + clast();
}
export { first, last, name, cfirst, clast, cname };
