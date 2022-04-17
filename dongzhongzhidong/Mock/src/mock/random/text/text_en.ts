import { random, times, capitalize } from 'lodash-es';
import { faker } from '@faker-js/faker';
import { decode } from '../paramsParser';

// 随机生成一个句子，第一个单词的首字母大写。
const sentence = decode(
    (min = 12, max = 18) => {
        const len = random(min, max);
        return faker.lorem.sentence(len);
    },
    ['number', 'number'],
);

// 随机生成一个单词。
const word = decode(
    (min = 3, max = 10) => {
        const len = random(min, max);
        return faker.lorem.word(len);
    },
    ['number', 'number'],
);

// 随机生成一段文本。
const paragraph = decode(
    (min = 3, max = 7) => {
        const len = random(min, max);
        return faker.lorem.paragraph(len);
    },
    ['number', 'number'],
);

// 随机生成一句标题，其中每个单词的首字母大写。
const title = decode(
    (min = 3, max = 7) => {
        const len = random(min, max);
        return times(len, () => capitalize(word())).join('');
    },
    ['number', 'number'],
);

export { word, paragraph, sentence, title };
