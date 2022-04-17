import { sample } from 'game-random';
import { decode } from '../paramsParser';
import { characterPools } from './CharacterList';

// !需要 decode 来做兼容处理
const character = decode(
    (pool: string = 'default') => {
        pool = pool.toLowerCase();
        if (pool in characterPools) {
            return sample(characterPools[pool as keyof typeof characterPools]);
        } else {
            return sample(pool.split(''));
        }
    },
    ['json'],
);
export { character, character as char };
