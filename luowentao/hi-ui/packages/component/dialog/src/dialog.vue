<template>
  <div class="mask" v-show="visible" @touchmove.stop.prevent="moveHandle" v-if="visible">
    <transition name="dialog">
      <div class="card dialog" :style="{bottom:bottom+'px',top:top+'px'}" v-show="visible">
        <div class="dialog-title text-color-one model-title-main-size">{{title}}</div>
        <div class="dialog-body dialog-content-size text-color-two">
          <slot name="dialog-body"></slot>
        </div>
        <div class="dialog-foot">
          <slot name="dialog-foot"></slot>
        </div>
      </div>
    </transition>
  </div>
</template>

<script>
export default {
  name: "HiDialog",
  props:{
    title:{
      type:String,
      default:''
    },
    visible:{
      type:Boolean,
      default: false
    },
    bottom:{
      type:Number,
      default:null
    },
    top:{
      type:Number,
      default:null
    }
  },
  watch:{
    visible(val){
      this.$emit(val?'openDialog':'closeDialog');
    }
  },
  data(){
    return{}
  },
  methods:{
    moveHandle(){}
  }
}
</script>

<style scoped>

.dialog-enter,
.dialog-leave-active {
  transform: translateY(100%);
}

.dialog-enter-active,
.dialog-leave-active {
  transition: all .3s
}
</style>
