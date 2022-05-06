<template>
  <div class="toast" :style="{top:statusBarHeight+'rem'}" v-show="visible">
    <img class="toast-icon" :src="url"/>
    <div class="toast-msg">{{msg}}</div>
  </div>
</template>

<script>
export default {
  name: "toast",
  data(){
    return{
      visible:false,
      msg:'',
      statusBarHeight:3.5,
      url: require('../../../img/ic_warn.png')
    }
  },
  mounted() {
    console.log(window.hilink);
    window.getStatusBarHeight = res => {
      let data = JSON.parse(res);
      this.statusBarHeight = data.statusBarHeight + 3.5;
    }
    try {
      window.hilink.getStatusBarHeight('getStatusBarHeight');
    }catch (e) {
      console.error('当前不在hilink环境下');
    }
    window.addEventListener('selectTheme',(e)=>{
      this.url = e.detail.type == 'dark'?require('../../../img/ic_warn_dark.png'):require('../../../img/ic_warn.png')
    })
  },
  methods:{
  }
}
</script>

<style scoped>

</style>
