/*
    ## Mock.Random

    工具类，用于生成各种随机数据。
*/

import { sample } from 'game-random';
import { shuffle as _shuffle } from 'lodash-es';
// import { shuffle as _shuffle } from 'game-random/src/shuffle';
export * from './basic/basic';
export * from './color/color';
export * from './name/name';
export * from './misc/misc';
export * from './text/text';
export * from './web/web';
export * from './date';
export * from './image/image';
export * from './address/address';
export * from './file';

export const pick = sample;
export const shuffle = _shuffle;
