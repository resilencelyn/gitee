import { random, times } from 'lodash-es';
import { character } from '../basic.js';
import { capitalize } from '../helper.js';
// 随机生成一个句子，第一个单词的首字母大写。
function sentence(min = 12, max = 18) {
    const len = random(min, max);
    const string = times(len, () => word()).join(' ');
    return capitalize(string) + '.';
}

// 随机生成一个单词。
function word(min = 3, max = 10) {
    const len = random(min, max);
    return times(len, () => character('lower')).join('');
}

// 随机生成一段文本。
function paragraph(min = 3, max = 7) {
    const len = random(min, max);
    return times(len, () => sentence()).join('\n');
}

// 随机生成一句标题，其中每个单词的首字母大写。
function title(min = 3, max = 7) {
    const len = random(min, max);
    return times(len, () => capitalize(word())).join('');
}
export { word, paragraph, sentence, title };
