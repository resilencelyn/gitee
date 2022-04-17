import { range as _range } from 'lodash-es';
import { decode } from '../paramsParser';
export const range = decode(_range, ['number', 'number', 'number']);
