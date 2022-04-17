const lower = 'abcdefghijklmnopqrstuvwxyz'.split('');
const upper = lower.map((i) => i.toUpperCase());
const number = '0123456789'.split('');
const symbol = '!@#$%^&*()[]'.split('');
export const characterPools = {
    lower,
    upper,
    number,
    symbol,
    alpha: lower.concat(upper),
    default: [...lower, ...upper, ...number, ...symbol],
};
