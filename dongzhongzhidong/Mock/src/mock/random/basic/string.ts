import { times } from 'lodash-es';
import { character } from './character.js';
import { random } from 'game-random';
import { decode } from '../paramsParser.js';
function _string(): string;
function _string(length: number): string;
function _string(poolTag: string, length: number): string;
function _string(min: number, max: number): string;
function _string(poolTag: string, min: number, max: number): string;
function _string(...args: any): string {
    let len: number = 0;
    let pool: string;
    let min: number;
    let max: number;
    switch (args.length) {
        case 0: // ()
            len = random(3, 7, false);
            break;
        case 1: // ( length )
            [len] = args;
            break;
        case 2:
            // ( pool, length )
            if (typeof args[0] === 'string') {
                [pool, len] = args;
            } else {
                // ( min, max )
                [min, max] = args;
                len = random(min, max, false);
            }
            break;
        case 3:
            // ( pool, min, max )
            [pool, min, max] = args;
            len = random(min, max, false);
            break;
    }
    return times(len, () => character(pool)).join('');
}
export { capitalize } from 'lodash-es';
export const upper = (str: string) => str.toUpperCase();
export const lower = (str: string) => str.toLowerCase();
const string = decode(_string, ['number', 'number', 'number']);
export { string, string as str };
