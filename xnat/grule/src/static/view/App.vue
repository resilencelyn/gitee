<template>
<!--    :style="{backgroundImage: 'url(' + require('@/img/bg.png') +')'}" -->
    <div id="app">
        <router-view @init="init" />
    </div>
</template>

<script>

export default {
    name: 'App',
    created() {
        // 添加响应拦截器
        axios.interceptors.response.use(resp => {
            if (resp.code !== '00') this.$message.error(resp.msg)
            else if (resp.msg) this.$message.info(resp.msg)
            return resp;
        }, (error) => {
            if (error.response.status === 401) {
                this.$user.id = null
                this.$router.push(`/login`);
            } else {
                this.$message.error(error.response.status +': '+ error.response.statusText)
            }
            return Promise.reject(error);
        });
        this.init()
    },
    watch: {
        // $route(to, from) {
        //     console.log('$route', to, from, this.$router.currentRoute)
        // },
    },
    methods: {
      init() {
        const loading = this.$loading({target: document.body, text: '初始化中...'})
        axios.get('currentUser').then(resp => {
          loading.close()
          if (resp.code === '00') {
            // 设置当前用户的信息
            for (let k in resp.data) this.$user[(k)] = resp.data[(k)]
            // 当前用户权限函数
            this.$initUserPrivilege(this.$user)
            // 已经登录了, 自动跳到主页
            if (this.$router.currentRoute.value.name === 'login') this.$router.push('/');
            this.initCfg()
            // 初始化websocket
            this.initWs()
          }
        }).catch(e => loading.close())
      },
      // 从后端拿一些配置
      initCfg() {
        axios.get('./webConfig').then(resp => {
          for (let k in resp.data) this.$cfg[(k)] = resp.data[(k)]
          if (this.$cfg.title) document.title = this.$cfg.title;
        })
      },
      initWs() {
        ws({path: '/ws', onMsg(e) {
            const jo = toJSON(e.data);
            if (jo) {
              if (jo.type === 'notify' && jo.msg) this.$message.info(jo.msg)
            } else {
              this.$message.info(e.data)
            }
          }})
      }
    }
}
</script>

<style scoped>
</style>