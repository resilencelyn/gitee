import Mock from '../dist/mock.browser.esm.js';
const Random = Mock.Random;
const total = {
    string: [
        () =>
            Mock.mock({
                'string|1-10': '★',
            }),

        () =>
            Mock.mock({
                'string|3': '★★★',
            }),
    ],
    number: [
        () => {
            Mock.mock({
                'number|+1': 202,
            });
            Mock.mock({
                'number|+1': 202,
            });
        },

        () =>
            Mock.mock({
                'number|1-100': 100,
            }),

        () =>
            Mock.mock({
                'number|1-100.1-10': 1,
            }),

        () =>
            Mock.mock({
                'number|123.1-10': 1,
            }),

        () =>
            Mock.mock({
                'number|123.3': 1,
            }),

        () =>
            Mock.mock({
                'number|123.10': 1.123,
            }),
    ],
    boolean: [
        () =>
            Mock.mock({
                'boolean|1': true,
            }),
        () =>
            Mock.mock({
                'boolean|1-20': true,
            }),
    ],
    object: [
        () =>
            Mock.mock({
                'object|2': {
                    310000: '上海市',
                    320000: '江苏省',
                    330000: '浙江省',
                    340000: '安徽省',
                },
            }),
        () =>
            Mock.mock({
                'object|2-4': {
                    110000: '北京市',
                    120000: '天津市',
                    130000: '河北省',
                    140000: '山西省',
                },
            }),
    ],
    array: [
        () =>
            Mock.mock({
                'array|1': ['AMD', 'CMD', 'UMD'],
            }),
        () => {
            Mock.mock({
                'array|+1': ['AMD', 'CMD', 'UMD'],
            });
            Mock.mock({
                'array|+1': ['AMD', 'CMD', 'UMD'],
            });
        },

        () =>
            Mock.mock({
                'array|1-10': ['Mock.js'],
            }),
        () =>
            Mock.mock({
                'array|1-10': ['Hello', 'Mock.js', '!'],
            }),
        () =>
            Mock.mock({
                'array|3': ['Mock.js'],
            }),
        () =>
            Mock.mock({
                'array|3': ['Hello', 'Mock.js', '!'],
            }),
        () =>
            Mock.mock({
                'array|1-10': [
                    {
                        'name|+1': ['Hello', 'Mock.js', '!'],
                    },
                ],
            }),
    ],
    regexp: [
        () =>
            Mock.mock({
                regexp: /[a-z][A-Z][0-9]/,
            }),
        () =>
            Mock.mock({
                regexp: /\w\W\s\S\d\D/,
            }),
        () =>
            Mock.mock({
                regexp: /\d{5,10}/,
            }),
        () =>
            Mock.mock({
                'regexp|3': /\d{5,10}\-/,
            }),
        () =>
            Mock.mock({
                'regexp|1-5': /\d{5,10}\-/,
            }),
    ],
};
// Object.entries(total).forEach(([key, value]) => {
//     value.forEach((i) => i());
// });

Mock.mock({
    foo: 'Syntax Demo',
    some: {
        lang: 'typescript',
        name(result, context) {
            return this.lang;
        },
    },
});
// Mock.mock({
//     'foo|2': 'Hello',
//     js: null,
//     nested: {
//         a: {
//             b: {
//                 c: 'Mock.js',
//             },
//         },
//     },
//     absolutePath: '@/foo @/nested/a/b/c',
// });
// Mock.mock({
//     foo: 'Hello',
//     nested: {
//         a: {
//             b: {
//                 c: 'Mock.js',
//             },
//         },
//     },
//     relativePath: {
//         a: {
//             b: {
//                 c: '@../../../foo @../../../nested/a/b/c',
//             },
//         },
//     },
// });

debugger;

// Random.boolean()
Random.boolean();
Mock.mock('@boolean');
Mock.mock('@boolean()');

// Random.boolean( min, max, current )
Random.boolean(1, 9, true);
Mock.mock('@boolean(1, 9, true)');
// Random.natural()
Random.natural();
Mock.mock('@natural');
Mock.mock('@natural()');

// Random.natural( min )
Random.natural(10000);
Mock.mock('@natural(10000)');

// Random.natural( min, max )
Random.natural(60, 100);
Mock.mock('@natural(60, 100)');
// Random.integer()
Random.integer();
Mock.mock('@integer');
Mock.mock('@integer()');

// Random.integer( min )
Random.integer(10000);
Mock.mock('@integer(10000)');

// Random.integer( min, max )
Random.integer(60, 100);
Mock.mock('@integer(60, 100)');

// Random.float()
Random.float();
Mock.mock('@float');
Mock.mock('@float()');

// Random.float( min )
Random.float(0);
Mock.mock('@float(0)');

// Random.float( min, max )
Random.float(60, 100);
Mock.mock('@float(60, 100)');

// Random.float( min, max, dmin )
Random.float(60, 100, 3);
Mock.mock('@float(60, 100, 3)');

// Random.float( min, max, dmin, dmax )
Random.float(60, 100, 3, 5);
Mock.mock('@float(60, 100, 3, 5)');

// Random.character()
Random.character();
Mock.mock('@character');
Mock.mock('@character()');

// Random.character( 'lower/upper/number/symbol' )
Random.character('lower');
Random.character('upper');
Random.character('number');
Random.character('symbol');

Mock.mock('@character("lower")');
Mock.mock('@character("upper")');
Mock.mock('@character("number")');
Mock.mock('@character("symbol")');

// Random.character( pool )
Random.character('aeiou');
Mock.mock('@character("aeiou")');
// Random.string()
Random.string();
Mock.mock('@string');
Mock.mock('@string()');

// Random.string( length )
Random.string(5);
Mock.mock('@string(5)');

// Random.string( pool, length )
Random.string('lower', 5);
Random.string('upper', 5);
Random.string('number', 5);
Random.string('symbol', 5);
Random.string('aeiou', 5);

Mock.mock('@string("lower", 5)');
Mock.mock('@string("upper", 5)');
Mock.mock('@string("number", 5)');
Mock.mock('@string("symbol", 5)');
Mock.mock('@string("aeiou", 5)');

// Random.string( min, max )
Random.string(7, 10);
Mock.mock('@string(7, 10)');

// Random.string( pool, min, max )
Random.string('lower', 1, 3);
Random.string('upper', 1, 3);
Random.string('number', 1, 3);
Random.string('symbol', 1, 3);
Random.string('aeiou', 1, 3);

Mock.mock('@string("lower", 1, 3)');
Mock.mock('@string("upper", 1, 3)');
Mock.mock('@string("number", 1, 3)');
Mock.mock('@string("symbol", 1, 3)');
Mock.mock('@string("aeiou", 1, 3)');

// Random.range( stop )
Random.range(10);
Mock.mock('@range(10)');

// Random.range( start, stop )
Random.range(3, 7);
Mock.mock('@range(3, 7)');

// Random.range( start, stop, step )
Random.range(1, 10, 2);
Random.range(1, 10, 3);

Mock.mock('@range(1, 10, 2)');
Mock.mock('@range(1, 10, 3)');
// Random.date()
Random.date();
Mock.mock('@date');
Mock.mock('@date()');

// Random.date( format )
Random.date('YYYY-MM-DD');

// Random.time()
Random.time();
Mock.mock('@time');
Mock.mock('@time()');

// Random.time( format )
Random.time('A HH:mm:ss');
Random.time('a HH:mm:ss');
Random.time('HH:mm:ss');
Random.time('H:m:s');

Mock.mock('@time("A HH:mm:ss")');
Mock.mock('@time("a HH:mm:ss")');
Mock.mock('@time("HH:mm:ss")');
Mock.mock('@time("H:m:s")');

Mock.mock('@datetime("HH H hh h mm m ss s SS S A a T")');
// Random.datetime()
Random.datetime();
Mock.mock('@datetime');
Mock.mock('@datetime()');

// Random.datetime( format )
Random.datetime('YYYY-MM-DD A HH:mm:ss');
Random.datetime('YY-MM-DD a HH:mm:ss');
Random.datetime('Y-MM-DD HH:mm:ss');
Random.datetime('Y-M-d H:m:s');

Mock.mock('@datetime("YYYY-MM-DD A HH:mm:ss")');
Mock.mock('@datetime("YY-MM-DD a HH:mm:ss")');
Mock.mock('@datetime("Y-MM-DD HH:mm:ss")');
Mock.mock('@datetime("Y-M-d H:m:s")');

Mock.mock('@datetime("YYYY YY Y MM M DD d HH H hh h mm m ss s SS S A a T")');
// Ranndom.now()
Random.now();
Mock.mock('@now');
Mock.mock('@now()');

// Ranndom.now( unit )
Random.now('year');
Random.now('month');
Random.now('week');
Random.now('day');
Random.now('hour');
Random.now('minute');
Random.now('second');

// Ranndom.now( format )
Random.now('yyyy-MM-dd HH:mm:ss SS');

// Ranndom.now( unit, format )
Random.now('day', 'yyyy-MM-dd HH:mm:ss SS');
// Random.image()
Random.image();
// Random.image( size )
Random.image('200x100');
// Random.image( size, background )
Random.image('200x100', '#FF6600');
// Random.image( size, background, text )
Random.image('200x100', '#4A7BF7', 'Hello');
// Random.image( size, background, foreground, text )
Random.image('200x100', '#50B347', '#FFF', 'Mock.js');
// Random.image( size, background, foreground, format, text )
Random.image('200x100', '#894FC4', '#FFF', 'png', '!');
// Random.dataImage()
Random.dataImage();

// Random.dataImage( size )
Random.dataImage('200x100');

// Random.dataImage( size, text )
Random.dataImage('200x100', 'Hello Mock.js!');
// Random.color()
Random.color();
Mock.mock('@color');
Mock.mock('@color()');
// Random.hex()
Random.hex();
Mock.mock('@hex');
Mock.mock('@hex()');
// Random.rgb()
Random.rgb();
Mock.mock('@rgb');
Mock.mock('@rgb()');
// Random.rgba()
Random.rgba();
Mock.mock('@rgba');
Mock.mock('@rgba()');
// Random.hsl()
Random.hsl();
Mock.mock('@hsl');
Mock.mock('@hsl()');
// Random.paragraph()
Random.paragraph();

Mock.mock('@paragraph');

Mock.mock('@paragraph()');

// Random.paragraph( len )
Random.paragraph(2);

Mock.mock('@paragraph(2)');

// Random.paragraph( min, max )
Random.paragraph(1, 3);

Mock.mock('@paragraph(1, 3)');

// Random.sentence()
Random.sentence();
Mock.mock('@sentence');
Mock.mock('@sentence()');

// Random.sentence( len )
Random.sentence(5);
Mock.mock('@sentence(5)');

// Random.sentence( min, max )
Random.sentence(3, 5);
Mock.mock('@sentence(3, 5)');

// Random.word()
Random.word();
Mock.mock('@word');
Mock.mock('@word()');

// Random.word( len )
Random.word(5);
Mock.mock('@word(5)');

// Random.word( min, max )
Random.word(3, 5);
Mock.mock('@word(3, 5)');

// Random.title()
Random.title();
Mock.mock('@title');
Mock.mock('@title()');

// Random.title( len )
Random.title(5);
Mock.mock('@title(5)');

// Random.title( min, max )
Random.title(3, 5);
Mock.mock('@title(3, 5)');

// Random.cparagraph()
Random.cparagraph();

Mock.mock('@cparagraph');

Mock.mock('@cparagraph()');

// Random.cparagraph( len )
Random.cparagraph(2);

Mock.mock('@cparagraph(2)');

// Random.cparagraph( min, max )
Random.cparagraph(1, 3);

Mock.mock('@cparagraph(1, 3)');

// Random.csentence()
Random.csentence();
Mock.mock('@csentence');
Mock.mock('@csentence()');

// Random.csentence( len )
Random.csentence(5);
Mock.mock('@csentence(5)');

// Random.csentence( min, max )
Random.csentence(3, 5);
Mock.mock('@csentence(3, 5)');

// Random.cword()
Random.cword();
Mock.mock('@cword');
Mock.mock('@cword()');

// Random.cword( pool )
Random.cword('零一二三四五六七八九十');
Mock.mock('@cword("零一二三四五六七八九十")');

// Random.cword( length )
Random.cword(3);
Mock.mock('@cword(3)');

// Random.cword( pool, length )
Random.cword('零一二三四五六七八九十', 3);
Mock.mock('@cword("零一二三四五六七八九十", 3)');

// Random.cword( min, max )
Random.cword(3, 5);
Mock.mock('@cword(3, 5)');

// Random.cword( pool, min, max )
Random.cword('零一二三四五六七八九十', 5, 7);
Mock.mock('@cword("零一二三四五六七八九十", 5, 7)');
// Random.ctitle()
Random.ctitle();
Mock.mock('@ctitle');
Mock.mock('@ctitle()');

// Random.ctitle( len )
Random.ctitle(5);
Mock.mock('@ctitle(5)');

// Random.ctitle( min, max )
Random.ctitle(3, 5);
Mock.mock('@ctitle(3, 5)');

// Random.first()
Random.first();
Mock.mock('@first');
Mock.mock('@first()');
// Random.last()
Random.last();
Mock.mock('@last');
Mock.mock('@last()');
// Random.name()
Random.name();
Mock.mock('@name');
Mock.mock('@name()');

// Random.name( middle )
Random.name(true);
Mock.mock('@name(true)');
// Random.cfirst()
Random.cfirst();
Mock.mock('@cfirst');
Mock.mock('@cfirst()');
// Random.clast()
Random.clast();
Mock.mock('@clast');
Mock.mock('@clast()');
// Random.cname()
Random.cname();
Mock.mock('@cname');
Mock.mock('@cname()');
// Random.url()
Random.url();
Mock.mock('@url');
Mock.mock('@url()');
// Random.domain()
Random.domain();
Mock.mock('@domain');
Mock.mock('@domain()');
// Random.protocol()
Random.protocol();
Mock.mock('@protocol');
Mock.mock('@protocol()');
// Random.tld()
Random.tld();
Mock.mock('@tld');
Mock.mock('@tld()');
// Random.email()
Random.email();
Mock.mock('@email');
Mock.mock('@email()');
// Random.ip()
Random.ip();
Mock.mock('@ip');
Mock.mock('@ip()');
// Random.region()
Random.region();
Mock.mock('@region');
Mock.mock('@region()');
// Random.province()
Random.province();
Mock.mock('@province');
Mock.mock('@province()');
// Random.city()
Random.city();
Mock.mock('@city');
Mock.mock('@city()');
// Random.city( prefix )
Random.city(true);
Mock.mock('@city(true)');
// Random.county()
Random.county();
Mock.mock('@county');
Mock.mock('@county()');
// Random.county( prefix )
Random.county(true);
Mock.mock('@county(true)');
// Random.zip()
Random.zip();
Mock.mock('@zip');
Mock.mock('@zip()');
// Random.capitalize( word )
Random.capitalize('hello');
Mock.mock('@capitalize("hello")');
// Random.upper( str )
Random.upper('hello');
Mock.mock('@upper("hello")');
// Random.lower( str )
Random.lower('HELLO');
Mock.mock('@lower("HELLO")');
// Random.pick( arr )
Random.pick(['a', 'e', 'i', 'o', 'u']);
Mock.mock('@pick(["a", "e", "i", "o", "u"])');
// Random.shuffle( arr )
Random.shuffle(['a', 'e', 'i', 'o', 'u']);
Mock.mock('@shuffle(["a", "e", "i", "o", "u"])');
// Random.guid()
Random.guid();
Mock.mock('@guid');
Mock.mock('@guid()');
// Random.id()
Random.id();
Mock.mock('@id');
Mock.mock('@id()');
// Random.increment()
Random.increment();
Mock.mock('@increment');
Mock.mock('@increment()');

// Random.increment( step )
Random.increment(100);
Mock.mock('@increment(100)');
Random.increment(1000);
Mock.mock('@increment(1000)');
