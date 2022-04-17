import resolve from '@rollup/plugin-node-resolve';
import commonjs from '@rollup/plugin-commonjs';
import json from '@rollup/plugin-json';
import condition from '@konghayao/rollup-plugin-conditional';
import babel from '@rollup/plugin-babel';
export default [
    {
        input: './src/mock.ts', // 打包入口
        external: ['@faker-js/faker'],
        output: [
            {
                // 打包出口
                file: './dist/mock.browser.esm.js',
                format: 'es',
                paths: {
                    '@faker-js/faker':
                        'https://cdn.skypack.dev/pin/@faker-js/faker@v6.1.2-XXqPDwHPH8U4a4bdHNrb/mode=imports,min/optimized/@faker-js/faker.js',
                },
            },
        ],
        plugins: [
            condition({ env: 'browser' }),

            resolve({
                browser: true,
                extensions: ['.ts', '.json', ''],
            }),

            commonjs(),
            json({ compact: true }),
            babel({
                extensions: ['.ts', '.json'],
            }),
        ],
    },
    // {
    //     input: './src/mock.js', // 打包入口
    //     output: [
    //         {
    //             // 打包出口
    //             file: './dist/mock.esm.js',
    //             format: 'es',
    //             name: 'Mock',
    //         },
    //     ],
    //     plugins: [condition({ env: 'node' }), json()],
    // },
];
