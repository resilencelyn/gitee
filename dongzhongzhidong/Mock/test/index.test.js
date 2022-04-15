import testTree from 'jest-test-tree/dist/jest-test-tree.esm';
import Mock from '../dist/mock.esm.js';

const loop = (func, num) => [...Array(num).keys()].map((i) => func(i));
testTree({
    String: [
        [
            '字符串随机次数叠加',
            () =>
                Mock.mock({
                    'string|1-10': '★',
                }),
            (res) => {
                res.string.split('').forEach((i) => expect(i).toBe('★'));
                expect(res.string.length).toBeLessThanOrEqual(10);
                expect(res.string.length).toBeGreaterThanOrEqual(1);
                return;
            },
        ],
        [
            '字符串固定次数叠加',
            () =>
                Mock.mock({
                    'string|3': '★★★',
                }),
            (res) => {
                res.string.split('').forEach((i) => expect(i).toBe('★'));
                expect(res.string.length).toBe(9);
                return;
            },
        ],
    ],
    Number: [
        [
            '数字自增',
            () =>
                Mock.mock({
                    'number|+1': 202,
                }),
            (res) => {
                console.log('TODO 1 Number', res.number);
                return;
            },
        ],
        [
            '随机范围',
            () =>
                loop(
                    () =>
                        Mock.mock({
                            'number|1-20': 20,
                        }),
                    20,
                ),
            (res) =>
                res.forEach(({ number: i }) => {
                    expect(i).toBeGreaterThanOrEqual(1);
                    expect(i).toBeLessThanOrEqual(20);
                }),
        ],
        [
            '随机范围（小数位）1',
            () =>
                loop(
                    () =>
                        Mock.mock({
                            'number|1-20.1-10': 1,
                        }),
                    20,
                ),
            (res) =>
                res.forEach(({ number: i }) => {
                    let [int, d] = (i + '').split('.');
                    int = int - 0;
                    d = d - 0;
                    expect(int).toBeGreaterThanOrEqual(1);
                    expect(int).toBeLessThanOrEqual(20);
                    expect(d).toBeGreaterThanOrEqual(0);
                    expect(d).toBeLessThanOrEqual(10 ** 10);
                }),
        ],
        [
            '随机范围（小数位）2',
            () =>
                loop(
                    () =>
                        Mock.mock({
                            'number|20.1-10': 1,
                        }),
                    20,
                ),
            (res) =>
                res.forEach(({ number: i }) => {
                    let [int, d] = (i + '').split('.');
                    int = int - 0;
                    d = d - 0;
                    expect(int).toBe(20);
                    expect(d).toBeGreaterThanOrEqual(0);
                    expect(d).toBeLessThanOrEqual(10 ** 10);
                }),
        ],
        [
            '随机范围（小数位）3',
            () =>
                loop(
                    () =>
                        Mock.mock({
                            'number|20.2': 1,
                        }),
                    20,
                ),
            (res) =>
                res.forEach(({ number: i }) => {
                    let [int, d] = (i + '').split('.');
                    int = int - 0;
                    d = d - 0;
                    expect(int).toBe(20);
                    expect(d).toBeGreaterThanOrEqual(10);
                    expect(d).toBeLessThanOrEqual(99);
                }),
        ],
    ],
    Boolean: [],
});
