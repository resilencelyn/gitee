<template>
  <div :class="'switch '+(modelVal?'switch-on':'switch-off')" :style="{backgroundColor:modelVal?noColor:offColor}">
    <div class="switch-btn" @click="switchType"></div>
  </div>
</template>

<script>
export default {
  name: "HiSwitch",
  model: {
    prop: 'modelVal',//指向props的参数名
    event: 'change'//事件名称
  },
  props:{
    noColor:{
      type:String,
      default:''
    },
    offColor:{
      type:String,
      default:''
    },
    modelVal:false
  },
  data(){
    return{
      dom:null
    }
  },
  mounted() {
    this.dom = this.$el.getElementsByClassName('switch-btn').item(0);
    this.dom.style.transitionDuration = '0ms';
    this.dom.style.transform = `translateX(0)`;
  },
  methods:{
    switchType(){
      this.dom.style.transitionDuration = '200ms';
      this.dom.style.transform = `translateX(${(!this.modelVal)?1.875:0}rem)`;
      this.$emit('change',!this.modelVal)
    }
  }
}
</script>

<style scoped>

</style>
