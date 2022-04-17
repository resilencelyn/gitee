import { times } from 'lodash-es';
import RendExp from 'randexp';
import { HandlerFunction, HandlerOptions } from '.';
export const regexp = ({ rule: { count = 1 }, template }: HandlerOptions<RegExp>): string | string[] => {
    // 'name': /regexp/,
    // 'name|1-5': /regexp/
    const gen = new RendExp(template);
    return count === 1 ? gen.gen() : times(count, () => gen.gen());
};
