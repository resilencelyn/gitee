export default {
    yyyy: 'getFullYear',
    yy: function (date: Date) {
        return ('' + date.getFullYear()).slice(2);
    },
    y: 'yy',

    MM: function (date: Date) {
        const m = date.getMonth() + 1;
        return m < 10 ? '0' + m : m;
    },
    M: function (date: Date) {
        return date.getMonth() + 1;
    },

    dd: function (date: Date) {
        const d = date.getDate();
        return d < 10 ? '0' + d : d;
    },
    d: 'getDate',

    HH: function (date: Date) {
        const h = date.getHours();
        return h < 10 ? '0' + h : h;
    },
    H: 'getHours',
    hh: function (date: Date) {
        const h = date.getHours() % 12;
        return h < 10 ? '0' + h : h;
    },
    h: function (date: Date) {
        return date.getHours() % 12;
    },

    mm: function (date: Date) {
        const m = date.getMinutes();
        return m < 10 ? '0' + m : m;
    },
    m: 'getMinutes',

    ss: function (date: Date) {
        const s = date.getSeconds();
        return s < 10 ? '0' + s : s;
    },
    s: 'getSeconds',

    SS: function (date: Date) {
        const ms = date.getMilliseconds();
        return (ms < 10 && '00' + ms) || (ms < 100 && '0' + ms) || ms;
    },
    S: 'getMilliseconds',

    A: function (date: Date) {
        return date.getHours() < 12 ? 'AM' : 'PM';
    },
    a: function (date: Date) {
        return date.getHours() < 12 ? 'am' : 'pm';
    },
    T: 'getTime',
};
