<template>
  <div class="mask" v-if="visible" @touchmove.stop.prevent="moveHandle">
    <transition name="loading">
      <div class="card loading text-color-one" v-show="visible">
        {{msg}}
        <img src="../../../img/loading.png" class="loading-icon"/>
      </div>
    </transition>
  </div>
</template>

<script>
export default {
  name: "loading",
  data(){
    return{
      visible:false,
      msg:''
    }
  },
  methods:{
    moveHandle(){},
    close(){
      this.visible = false;
    }
  }
}
</script>

<style scoped>
  .loading{
    padding: 1.5rem;
    position: fixed;
    bottom: 1rem;
    left: 1rem;
    right: 1rem;
    z-index: 999;
    line-height: 2.5rem;
    text-align: left;
  }
  .loading-icon{
    width: 2.5rem;
    height: 2.5rem;
    float: right;
  }

  .loading-enter,
  .loading-leave-active {
    transform: translateY(100%);
  }

  .loading-enter-active,
  .loading-leave-active {
    transition: all .3s
  }
</style>
