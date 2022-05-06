<template>
  <div style="width: 100%;" class="header bg-color">
      <img :src="backUrl" class="back" v-show="back" @click="toBack">
      <div class="content text-color-one">
        <slot name="title"></slot>
      </div>
      <img :src="moreUrl" class="more" v-show="more" @click="toMore">
  </div>
</template>

<script>

export default {
  name: "HiHeader",
  props:{
    back:{
      type:Boolean,
      default:true
    },
    more:{
      type:Boolean,
      default:true
    }
  },
  data(){
    return{
      backUrl:require('../../../img/icon_return.png') ,
      moreUrl:require('../../../img/icon_more.png')
    }
  },
  mounted() {
    window.addEventListener('selectTheme',(e)=>{
      this.backUrl = e.detail.type == 'dark'?require('../../../img/icon_return_Dark.png'):require('../../../img/icon_return.png')
      this.moreUrl = e.detail.type == 'dark'?require('../../../img/icon_more_Dark.png'):require('../../../img/icon_more.png')
    })
  },
  methods:{
    toBack(){
      this.$emit('toBack');
    },
    toMore(){
      this.$emit('toMore');
    }
  }
}
</script>

<style scoped>

</style>
