<template>
  <hi-dialog
      :visible="isShow"
      :top="230"
      :title="title">
    <div slot="dialog-body">
      {{msg}}
    </div>
    <div slot="dialog-foot">
      <hi-row is-vertical-line>
        <hi-col :span="1" :style="{color:cancelColor}" :class="cancelColor==''?'hi-btn-color-blue':''">
          <span @click="cancel">{{cancelText}}</span>
        </hi-col>
        <hi-col :span="1" :style="{color:confirmColor}" :class="confirmColor==''?'hi-btn-color-blue':''">
          <span @click="confirm">{{confirmText}}</span>
        </hi-col>
      </hi-row>
    </div>
  </hi-dialog>
</template>

<script>
import HiRow from "../../row";
import HiCol from "../../col";
import HiDialog from "../../dialog";
export default {
  name: "HiConfirm",
  components: { HiCol, HiRow, HiDialog},
  data(){
    return{
      isShow:false,
      title:'提示',
      msg:'',
      cancelText:'取消',
      confirmText:'确认',
      cancelColor:'',
      confirmColor:''
    }
  },
  methods:{
    cancel(){

    },
    confirm(){

    }
  }
}
</script>

<style scoped>

</style>
