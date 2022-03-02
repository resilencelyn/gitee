import { defineConfig } from 'umi';

export default defineConfig({
  mfsu: {},
  nodeModulesTransform: {
    type: 'none',
    exclude: [],
  },
  devtool: 'source-map',
  routes: [
    { path: '/index.html', redirect: '/ide_registry' },
    { path: '/', redirect: '/ide_registry' },
    {
      path: '/ide_registry',
      name: '镜像源',
      component: '@/pages/IdeRegistry',
    },
    { path: '/ide_image', name: '镜像', component: '@/pages/IdeImage' },
    {
      path: '/ide_enviroment',
      name: '资源配额',
      component: '@/pages/IdeEnvironment',
    },
    { path: '/ide', name: '开发环境', component: '@/pages/Ide' },
    { path: '/node', name: '节点管理', component: '@/pages/Node' },
    { component: '@/pages/404' },
  ],
  fastRefresh: {},
  layout: {
    name: 'MegaIde',
    locale: true,
    layout: 'top',
  },
  define: {
    'process.env': process.env
  },
  proxy: {
    '/api': {
      target: 'http://megaide.lixulife.com',
      changeOrigin: true,
    },
  },
});
