import Vue from 'vue'
import App from './App.vue'
import HiLinkUi from '../packages/index'
import '../packages/style/index.scss'

Vue.use(HiLinkUi);

Vue.config.productionTip = false

new Vue({
  render: h => h(App),
}).$mount('#app')
