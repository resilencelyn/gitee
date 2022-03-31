import { createApp, ComponentPublicInstance } from 'vue';
import App from './app.vue';
import { version } from '../package.json';

// web路由
import webRouter from './router/web';
// wap路由
import wapRouter from './router/wap';
// ua工具类
import { isWap } from './utils/ua';
// 引入vuex
import store from './store';

// 引入全局异常处理函数
import { globalErrorHandler } from './utils/global-error-handler';
// 引入markdown编译插件样式
import 'highlight.js/styles/monokai.css';
import component from './components';

// 安装百度统计
import './utils/baidu-tongji.ts';

// 打印版本，用于调试
console.debug(`版本：${version}`);
// 判断设备
console.debug(isWap(), navigator.userAgent);

import './global.less';

// 判断当前路由
const router = isWap() ? wapRouter : webRouter;

const app = createApp(App);
app.use(component);
app.use(router);
app.use(store);


// 注册全局异常处理方法
app.config.errorHandler = (err, instance, info) =>
  globalErrorHandler(err as Error, instance, info, router, store);
app.config.globalProperties.$throw = (
  err: Error,
  instance: ComponentPublicInstance | null,
) => globalErrorHandler(err, instance, null, router, store);

app.mount('#app');
