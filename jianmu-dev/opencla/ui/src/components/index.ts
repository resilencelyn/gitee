import { App } from 'vue';
import { ElLoading, ElMessage, ElMessageBox } from 'element-plus';
import JmMarkdown from './markdown/index.vue';

// 引入vant样式
import 'vant/lib/index.css';
// vant自定义样式
import './theme/wap/custom-vant.less';
// 按需引入vant组件
import { Button, Cell, CellGroup, Checkbox, Dialog, Field, Popup, Radio, RadioGroup, Tag, Toast } from 'vant';
// 引入element样式
import './theme/web/custom-element-plus.scss';

export default {
  // app.use()触发install的调用
  install: (app: App) => {
    app.component(JmMarkdown.name, JmMarkdown);
    app.use(Button);
    app.use(Checkbox);
    app.use(RadioGroup);
    app.use(Radio);
    app.use(Cell);
    app.use(CellGroup);
    app.use(Field);
    app.use(Popup);
    app.use(Tag);
    // 全局注册变量
    app.config.globalProperties.$loading = ElLoading.service;
    app.config.globalProperties.$success = ElMessage.success;
    app.config.globalProperties.$warning = ElMessage.warning;
    app.config.globalProperties.$info = ElMessage.info;
    app.config.globalProperties.$error = ElMessage.error;
    app.config.globalProperties.$alert = ElMessageBox.alert;
    app.config.globalProperties.$confirm = ElMessageBox.confirm;
    app.config.globalProperties.$prompt = ElMessageBox.prompt;
    app.config.globalProperties.$toast = Toast;
    app.config.globalProperties.$dialog = Dialog.alert;
  },
};
