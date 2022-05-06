<template>
  <div ref="dropdown">

    <div @click.stop="showDropdown">
      <slot name="content"></slot>
    </div>
<!--    <transition name="dropdown">-->
      <ul class="dropdown" v-show="isShow" ref="dropdownList">
        <li v-for="(item,index) in list" :key="index" :class="modelVal==item.id?'dropdown-item-select':''" @click="select(item.id)">
          <div class="dropdown-item text-color-one">{{item.name}}</div>
        </li>
      </ul>
<!--    </transition>-->
  </div>
</template>

<script>
export default {
  name: "HiDropdown",
  model: {
    prop: 'modelVal',//指向props的参数名
    event: 'change'//事件名称
  },
  props:{
    list:{
      type:Array,
      default:[]
    },
    modelVal: '',
  },
  computed:{
    style(){
      return {height:this.$parent};
    }
  },
  data() {
    return {
      isShow: false,
      selectItemId:''
    }
  },
  watch:{
    modelVal(val,oldVal){
      console.log(val);
    },
    isShow(val){
      let num = this.list.length;
      let kjHeight = (num*48)+(num-1);//控件高度
      kjHeight = kjHeight > 240 ? 240 : kjHeight;
      let height = kjHeight+16+64;
      let top = this.$refs.dropdown.getBoundingClientRect().top
      this.$refs.dropdownList.style.top = ((top-height)>0?(0 - height):0) + 'px';
    }
  },
  mounted() {
    document.addEventListener('click',()=>{
      this.isShow = false
    })
  },
  methods:{
    showDropdown(){
      this.isShow = !this.isShow;
      console.log(this.isShow)
    },
    select(id){
      this.$emit('change',id);
      this.isShow = false;
    }
  }
}
</script>

<style scoped>
.dropdown-enter,
.dropdown-leave-active {
  transform: translateY(-80%);
}

.dropdown-enter-active,
.dropdown-leave-active {
  transition: all .4s;
}
.dropdown{
  margin-top: 8px;
}
.dropdown li:last-child .dropdown-item{
  border-bottom: none;
}
.dropdown li:last-child{
  border-bottom-left-radius: 8px;
  border-bottom-right-radius: 8px;
}

.dropdown li:first-child{
  border-top-left-radius: 8px;
  border-top-right-radius: 8px;
}
ul{
  list-style: none;
  margin: 0;
  padding: 0;
}
</style>
