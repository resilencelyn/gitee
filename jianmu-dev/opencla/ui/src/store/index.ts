import { createStore } from 'vuex';
import { IRootState } from '@/model';

const store = createStore<IRootState>({
  // 开发环境开启严格模式，在严格模式下，无论何时发生了状态变更且不是由 mutation 函数引起的，将会抛出错误
  strict: process.env.NODE_ENV !== 'production',
  // 根mutation
  mutations: {},
  // 根action
  actions: {},
});

// 动态加载模块
Object.values(import.meta.globEager('./modules/*.ts')).forEach(({ default: module, namespace }) =>
  store.registerModule(namespace, module));

export default store;