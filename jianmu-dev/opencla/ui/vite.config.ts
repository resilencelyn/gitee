import { ConfigEnv, UserConfigExport } from 'vite';
import vue from '@vitejs/plugin-vue';
import path, { resolve } from 'path';
import { name, version } from './package.json';

import AutoImport from 'unplugin-auto-import/vite';
import Components from 'unplugin-vue-components/vite';
import { ElementPlusResolver } from 'unplugin-vue-components/resolvers';
import ElementPlus from 'unplugin-element-plus/vite';
// 引入vant配置
// import styleImport, { VantResolve } from 'vite-plugin-style-import';

const target = 'http://localhost:8080';

const changeOrigin = true;

// https://vitejs.dev/config/
export default ({ command, mode }: ConfigEnv): UserConfigExport => {
  return {
    plugins: [
      vue(),
      AutoImport({
        resolvers: [ElementPlusResolver()],
      }),
      Components({
        resolvers: [
          ElementPlusResolver({
            importStyle: 'sass',
          }),
        ],
      }),
      ElementPlus({
        useSource: true,
      }),
    ],
    // base public path
    base:
      command === 'build' && mode === 'cdn'
        ? `https://jianmu-cla.assets.dghub.cn/${name}/v${version}/`
        : '/',
    resolve: {
      alias: {
        '@': resolve(__dirname, 'src'),
      },
    },
    css: {
      preprocessorOptions: {
        scss: {
          additionalData: '@use "./src/components/theme/common/vars.scss" as *;',
        },
        less: {
          additionalData: `@import "${path.resolve(__dirname, './src/components/theme/common/vars.less')}";`,
        },
      },
      // vite内置的postcss插件
      // px转换为vw响应适配
      postcss: {
        plugins: [
          // 导入依赖autoprefixer，用来给不同的浏览器自动添加相应前缀，比如-webkit-,-moz-等
          require('autoprefixer'),
          // 导入依赖postcss-px-to-viewport
          require('./src/components/postcss-px-to-viewport')({
            // 要转换的单位
            unitToConvert: 'px',
            // 视窗的宽度
            viewportWidth: 750,
            // 转换后的精度，小数点保留几位
            unitPrecision: 3,
            // 指定转换的css属性的单位，*表示全部css属性的单位都要进行转换
            propList: ['*'],
            // 指定需要转换成的视窗单位，默认vw
            viewportUnit: 'vw',
            // 指定字体需要转换成的视窗单位，默认vw
            fontViewportUnit: 'vw',
            // 指定不转换为视窗单位的类名
            selectorBlackList: ['ignore-'],
            // 最小值，默认1，小于等于1px的不转换
            minPixelValue: 1,
            // 是否在媒体查询的css代码中也进行转换，默认false
            mediaQuery: false,
            // 是否在转换后直接更换属性值
            replace: true,
            // 设置忽略文件，用正则做目录名匹配，[/node_modules/]
            exclude: [/web/, /node_modules/, /element-plus/],
            // 是否做横屏处理
            landscape: false,
            // landscape横屏处理后的单位
            landscapeUnit: 'vw',
            // landscape横屏处理的视窗宽度
            landscapeWidth: 750,
          }),
        ],
      },
    },
    server: {
      proxy: {
        '/auth': { target, changeOrigin },
        '/agreements': { target, changeOrigin },
        '/emails': { target, changeOrigin },
      },
    },
  };
};