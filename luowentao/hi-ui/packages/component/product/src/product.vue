<template>
  <div class="img-box" :style="{'opacity': isDark && !black && 0.86}">
    <img :src="bigUrl" class="img-logo-big">
    <br/>
    <img :src="miniUrl" class="img-logo-mini">
  </div>
</template>

<script>
export default {
  name: "HiProduct",
  props:{
    bigUrl:{
      type:String,
      default:''
    },
    miniUrl:{
      type:String,
      default:''
    },
    black:{
      type:Boolean,
      default:false
    }
  },
  data(){
    return{
      isDark:false
    }
  },
  mounted() {
    window.addEventListener('selectTheme',(e)=>{
      this.isDark = e.detail.type == 'dark';
    })
  }
}
</script>

<style scoped>

</style>
