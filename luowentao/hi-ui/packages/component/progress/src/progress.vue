<template>
  <div>
    <input type="range" :min="min" :max="max" v-model="rangeValue" style="width: 100%" ref="progress">
  </div>
</template>

<script>
export default {
  name: "HiProgress",
  model: {
    prop: 'modelVal',//指向props的参数名
    event: 'change'//事件名称
  },
  props:{
    min:{
      type:Number,
      default:0
    },
    max:{
      type:Number,
      default:0
    },
    modelVal: ''
  },
  watch:{
    rangeValue(val,oldVal){
      let progress = this.$refs.progress;
      let size = (val/this.max) * 100;
      progress.style.backgroundSize = `${size}% 100%`;
      this.$emit('change',this.rangeValue);
    }
  },
  data(){
    return{
      rangeValue:0
    }
  },
  mounted() {

    let progress = this.$refs.progress;
    let size = (this.modelVal/this.max) * 100;
    progress.style.backgroundSize = `${size}% 100%`;
    this.rangeValue = this.modelVal;
  },
  methods:{
  }
}
</script>

<style scoped>
  input[type=range]::-webkit-slider-thumb {
    -webkit-appearance: none;
    -moz-appearance: none;
    width: 1.56rem;
    height: 1.56rem;
    border-radius: 50%;
    position: relative;
    top: -0.625rem;
    background: #fff;
    border: 0 solid rgba(0,0,0,0.04);
    pointer-events:none;
    box-shadow: 0 3px 8px 0 rgb(0 0 0 / 15%), 0 1px 1px 0 rgb(0 0 0 / 16%);
  }
</style>
