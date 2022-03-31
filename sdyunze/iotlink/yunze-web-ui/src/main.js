import Vue from 'vue'
import App from './app.vue'
import router from './router'

import axios from 'axios'
Vue.prototype.$ajax =axios
axios.defaults.withCredentials=true

import tools from './utils/tools'
Vue.use(tools)

import check from './static/js/check'
Vue.prototype.$check=check

import './static/vant/index'
import 'vant/lib/index.css'

import './static/elemnet-ui/index';
import 'element-ui/lib/theme-chalk/index.css'

import store from './vuex/store'
Vue.use(store)

import VueClipBoard from 'vue-clipboard2'
Vue.use(VueClipBoard)


import { Cascader,Divider,Area,Picker,Checkbox,CheckboxGroup,NumberKeyboard,Stepper  } from 'vant';

Vue.use(Stepper);
Vue.use(NumberKeyboard);
Vue.use(Checkbox);
Vue.use(CheckboxGroup);
Vue.use(Cascader);
Vue.use(Divider);
Vue.use(Picker);
Vue.use(Area);
Vue.config.productionTip = false;


/**
 * 路由拦截器
 */
router.beforeEach((to,from,next)=>{

  if(to.matched.some(function (item) {
    return item.meta.loginRequire
  })) {
    if (to.path.startsWith("/web") || to.path.startsWith("/wechat") || to.path.startsWith("/Shopping")) {
      //公众号拦截
      let webToken = localStorage.getItem("web-token")
      //console.log(webToken)
      if (webToken == null) {
        //console.log(to.path)
        //console.log(to.path.startsWith("/Shopping"))
        if (to.path.startsWith("/wechat") || to.path.startsWith("/Shopping") || to.path.startsWith("/byCard")) {
          next()
        } else {
          next("/wechat")
        }
      } else {
        next()
      }
    }

  }else{
    next()
  }
  next()

});
/**
 * 请求拦截器
 */
axios.interceptors.request.use((res)=>{
  //判断请求是从哪来的
  let param=res.url.split("/")[3];//请求去处
  let wech=res.url.split("/")[5];//请求来源
  if(param==="wechat"){
    let sysToken =sessionStorage.getItem("sys-token");
    if(null!=sysToken){
      let userId=JSON.parse(sessionStorage.getItem("loginUser"))
      res.headers.token=sysToken
      res.headers.userId=userId.id
    }
  }
  else{
    //只能在微信打开
    let ua = navigator.userAgent.toLowerCase()
    let isWeixin = ua.indexOf('micromessenger') !== -1
    if (!isWeixin) {
      window.location.href="/WxError.html"
    }

    let webToken =localStorage.getItem("web-token")
    if(null!=webToken){
      res.headers.token=webToken
    }else{
      //console.log("处理")
    }
  }
  return res;
},(error => {
  return Promise.reject(error);
}))

/**
 * 响应拦截器
 */
axios.interceptors.response.use(function (response) {
  // 对响应数据做点什么
  if(response.data==="500-lanjie"){
    alert("当前账户已在另一处登录，您已被迫下线")
    sessionStorage.clear()
  }

  if(response.data==="500-weblanjie"||response.data==="500-webnolg"){
    router.push({
      name: "bind",
      params: {"openid": store.state.openid}
    })
  }
  if(response.data==="401-auth"){
    window.location.href="/error-401.html"
  }
  //console.log(response);
  return response
}, function (error) {
  // 对响应错误做点什么
  return Promise.reject(error)
});

new Vue({
  store,
  router,
  render: h => h(App),
}).$mount('#app')
