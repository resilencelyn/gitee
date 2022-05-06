<template>
  <hi-dialog
      :title="title"
      :bottom="16"
      :visible="visible">
    <div slot="dialog-body">
      <hi-row :padding="0"  style="padding: 0 -1.5rem;">
        <hi-col :span="1">
          <hi-picker type="hour" v-model="h"></hi-picker>
        </hi-col>
        <hi-col :span="1">
          <hi-picker type="minute" v-model="m"></hi-picker>
        </hi-col>
      </hi-row>
    </div>
    <div slot="dialog-foot">
      <hi-row is-vertical-line :padding="0">
        <hi-col :span="1" class="hi-btn-color-blue">
          <span @click="confirm('confirm')">取消</span>
        </hi-col>
        <hi-col :span="1" class="hi-btn-color-blue">
          <span @click="confirm('success')">确定</span>
        </hi-col>
      </hi-row>
    </div>
  </hi-dialog>
</template>

<script>
import HiDialog from "../../dialog"
import HiRow from "../../row";
import HiCol from "../../col";
import HiPicker from "./picker";
export default {
  name: "HiTimePicker",
  components: {HiPicker, HiCol, HiRow, HiDialog},
  model: {
    prop: 'modelVal',//指向props的参数名
    event: 'change'//事件名称
  },
  props:{
    title:{
      type:String,
      default:''
    },
    visible:{
      type:Boolean,
      default: false
    },
    modelVal: ''
  },
  watch:{
    visible(val){

    },
    modelVal(val){
      this.getVal(val)
    }
  },
  data(){
    return{
      m:0,
      h:0,
      m1:0,
      h1:0
    }
  },
  mounted() {
    this.getVal(this.modelVal)
  },
  methods:{
    getVal(val){
      let arr = val.split(':');
      this.h = parseInt(arr[0]);
      this.m = (parseInt(arr[1])/10);
      this.h1 = parseInt(arr[0]);
      this.m1 = (parseInt(arr[1])/10);
    },
    moveHandle(){},
    confirm(type){
      if (type == 'success'){
        this.$emit('change',(this.h)+':'+(((this.m)*10)==0?'00':((this.m)*10)));
      }else{
        this.h = this.h1;
        this.m = this.m1;
        console.log(this.h,this.m)
      }
      this.$emit('confirm',type,type=='success'?{m:(this.m+1)*10,h:(this.h+1)}:{});
      // this.visible = false;

    }
  }
}
</script>

<style scoped>
.picker-enter,
.picker-leave-active {
  transform: translateY(100%);
}

.picker-enter-active,
.picker-leave-active {
  transition: all .3s
}
</style>
