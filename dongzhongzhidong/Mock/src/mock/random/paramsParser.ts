/**
 * @description  文本类型不需要进行更改
 */
export const decodes = {
    boolean(input: string): boolean {
        return JSON.parse(input);
    },
    number(input: string): number | string {
        const result = parseInt(input);
        return isNaN(result) ? input : result;
    },
    // json 可以同时转化多类型的数据，适合于重载的场景
    json(input: string): unknown {
        try {
            return JSON.parse(input);
        } catch (e) {
            return input;
        }
    },
};
/** 包裹函数即可实现类型转化 */
export const decode = <T extends Function>(func: T, decodeKey: (keyof typeof decodes)[]) => {
    return function (...args: any[]) {
        return func.apply(
            null,
            args.map((item, index) => {
                if (item === 'undefined') return;
                const key = decodeKey[index];
                if (key && key in decodes) {
                    return decodes[key].call(null, item);
                }
                return item;
            }),
        );
    } as unknown as T;
};
