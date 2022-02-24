import { defineConfig } from 'umi';

export default defineConfig({
  mfsu: {},
  nodeModulesTransform: {
    type: 'none',
    exclude: [],
  },
  devtool: 'source-map',
  routes: [
    { path: '/', redirect: '/ide_registry' },
    {
      path: '/ide_registry',
      name: '镜像源',
      component: '@/pages/IdeRegistry/index',
    },
    { path: '/ide_image', name: '镜像', component: '@/pages/IdeImage/index' },
    {
      path: '/ide_enviroment',
      name: '资源配置',
      component: '@/pages/IdeEnvironment/index',
    },
    { path: '/ide', name: '开发环境', component: '@/pages/Ide/index' },
  ],
  fastRefresh: {},
  layout: {
    name: 'MegaIde',
    locale: true,
    layout: 'top',
  },
  proxy: {
    '/api': {
      target: 'http://127.0.0.1:8001',
      changeOrigin: true,
    },
  },
});
