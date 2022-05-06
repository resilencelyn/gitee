<template>
  <div style="position: relative;width: 100%;text-align: center">
    <div class="picker-column" style="height: 220px">
      <ul class="picker-column__wrapper" style="transform: translate3d(0px, 88px, 0px); transition-duration: 200ms; transition-property: all;">
        <li class="picker-column__item" v-for="(item,index) in (type=='hour'?timeLang:7)">
          <div class="ellipsis text-color-one model-title-main-size">{{type=='hour'?(index<10?'0'+index:index):(index==0?'0'+index:index*10)}}</div>
        </li>
      </ul>
    </div>
    <div class="picker__mask" style="background-size: 100% 88px;">
      <div style="margin-top: 88px;height: 44px;line-height: 44px;margin-left: 60px;" class="text-color-one model-title-main-size">
        {{type=='hour'?'小时':'分钟'}}
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: "HiPicker",
  model: {
    prop: 'modelVal',//指向props的参数名
    event: 'change'//事件名称
  },
  props:{
    type:{
      type: String,
      default:'minute'
    },
    timeLang:{
      type: Number,
      default: 24
    },
    modelVal:0
  },
  watch:{
    modelVal(val){
      // if (this.isWatch){
        this.getVal(val)
      // }
      // this.isWatch = false;
    }
  },
  data(){
    return{
      startY:0,
      endY:0,
      moveY:0,
      offsetY:0,
      height:44,
      nowItemId:0,
      lang:7,
      isWatch:true
    }
  },
  mounted() {
    this.lang = this.type=='hour'?this.timeLang:7;
    this.getVal(this.modelVal);
    setTimeout(()=>{
      this.init();
    },1000)
  },
  methods:{

    getVal(val){
      let picker = this.$el.getElementsByClassName('picker-column__wrapper').item(0);
      this.nowItemId = val;
      picker.style.transform = `translate3d(0px, ${88-(this.nowItemId*this.height)}px, 0px)`;
    },
    init(){
      let picker = this.$el.getElementsByClassName('picker-column__wrapper').item(0);
      picker.addEventListener('touchstart',(e)=>{
        this.reset();
        this.startY = e.touches[0].pageY;
      },false)
      picker.addEventListener('touchmove',(e)=>{
        this.endY = e.touches[0].clientY;
        this.moveY = this.endY - this.startY;
        this.offsetY = Math.abs(this.moveY);
        picker.style.transitionDuration = '0ms';
        picker.style.transitionProperty = 'none';
        this.move(picker,false)
      },false)
      picker.addEventListener('touchend',(e)=>{
        const shouldSwipe = this.offsetY > this.height / 5;
        picker.style.transitionDuration = '200ms';
        picker.style.transitionProperty = 'all';
        if (shouldSwipe){
          let num = 0;
          if (this.moveY > 0 && this.nowItemId > 0){
            num = this.nowItemId - Math.round(this.offsetY/44);
            this.nowItemId = num<0?0:num
          }else if (this.moveY < 0 && this.nowItemId < (this.lang)){
            num = this.nowItemId + Math.round(this.offsetY/44);
            this.nowItemId = parseInt((num<0?0:(num<this.lang)?num:(this.lang-1)))
          }
          this.move(picker,shouldSwipe)
        }else {
          this.offsetY = 0;
          this.move(picker,shouldSwipe)
        }
      },false)
    },
    reset() {
      this.startY = 0;
      this.endY = 0;
      this.moveY = 0;
      this.offsetY = 0;
    },
    move(picker,isSwitch){
      if (isSwitch){
        console.log('nowItemId',this.nowItemId);
        picker.style.transform = `translate3d(0px, ${88-(this.nowItemId*this.height)}px, 0px)`;
        this.$emit('change',this.nowItemId<10?'0'+this.nowItemId:this.nowItemId);
        console.log(this.nowItemId<10?'0'+this.nowItemId:this.nowItemId)
      }else{
        picker.style.transform = `translate3d(0px, ${(88-(this.nowItemId*this.height))+this.moveY}px, 0px)`;
      }
    }
  }
}
</script>

<style scoped>

</style>
