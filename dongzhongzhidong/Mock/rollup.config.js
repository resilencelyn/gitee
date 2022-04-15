import resolve from 'rollup-plugin-node-resolve';
import commonjs from 'rollup-plugin-commonjs';
import json from '@rollup/plugin-json';
import condition from '@konghayao/rollup-plugin-conditional';
export default [
    {
        input: './src/mock.js', // 打包入口
        output: [
            {
                // 打包出口
                file: './dist/mock.browser.esm.js',
                format: 'es',
            },
        ],
        plugins: [
            condition({ env: 'browser' }),
            json(),
            resolve({
                browser: true,
            }),
            commonjs(),
        ],
    },
    {
        input: './src/mock.js', // 打包入口
        output: [
            {
                // 打包出口
                file: './dist/mock.esm.js',
                format: 'es',
                name: 'Mock',
            },
        ],
        plugins: [condition({ env: 'node' }), json()],
    },
];
