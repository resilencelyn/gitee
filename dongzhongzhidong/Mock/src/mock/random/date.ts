import { integer } from './basic/number.js';
import { faker } from '@faker-js/faker';
// 日期占位符集合。
import dayjs from 'dayjs';
import customParseFormat from 'dayjs/plugin/customParseFormat.js';
dayjs.extend(customParseFormat);
function date(format = 'YYYY-MM-DD') {
    return dayjs(faker.date.past()).format(format);
}
// 返回一个随机的日期字符串。
function datetime(format = 'YYYY-MM-DD HH:mm:ss') {
    return date(format);
}
function time(format = 'HH:mm:ss') {
    return date(format);
}

function now(unit: string, format: string) {
    // now(unit) now(format)
    if (arguments.length === 1) {
        // now(format)
        if (!/year|month|day|hour|minute|second|week/.test(unit)) {
            format = unit;
            unit = '';
        }
    }
    unit = (unit || '').toLowerCase();
    format = format || 'yyyy-MM-dd HH:mm:ss';

    const date = new Date();

    /* jshint -W086 */
    // 参考自 http://momentjs.cn/docs/#/manipulating/start-of/
    switch (unit) {
        case 'year':
            date.setMonth(0);
        case 'month':
            date.setDate(1);
        case 'week':
        case 'day':
            date.setHours(0);
        case 'hour':
            date.setMinutes(0);
        case 'minute':
            date.setSeconds(0);
        case 'second':
            date.setMilliseconds(0);
    }
    switch (unit) {
        case 'week':
            date.setDate(date.getDate() - date.getDay());
    }

    return dayjs(date).format(format);
}

function _randomDate(min = new Date(0), max = new Date()) {
    return new Date(integer(min.getTime(), max.getTime()));
}

function timestamp(min: Date, max: Date) {
    if (min instanceof Date && max instanceof Date) return _randomDate(min, max).getTime();
    return _randomDate().getTime();
}

export {
    // 返回一个随机的时间字符串。
    time,
    // 返回一个随机的日期和时间字符串。
    datetime,
    date,
    // 返回当前的日期和时间字符串。
    now,
    // KonghaYao 新增函数：生成固定时间段的时间戳
    timestamp,
};
