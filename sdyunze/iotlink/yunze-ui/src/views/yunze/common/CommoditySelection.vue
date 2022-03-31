<template>
      <div >
          <!--商品分类 下商品 多选框加载 组件 -->

            <el-form  @submit.native.prevent  :inline="true" label-width="60px" style="margin-left: 5%">
              <el-checkbox :indeterminate="isIndeterminate" v-model="checkAll" @change="handleCheckAllChange" style="float: left;margin-top: 10px;"> 全选</el-checkbox>
              <el-form-item label="名称" >
                <el-input v-model="selName" placeholder="名称" size="mini" style="width: 200px;"  @keyup.enter.native="selCheckboxOPtion"/>
              </el-form-item>
              <el-form-item>
                <el-button  style="margin-left: 30px;" type="warning" icon="el-icon-search" size="mini" @click="selCheckboxOPtion">搜索</el-button>
                <el-button style="margin-left: 30px;"  type="primary" size="mini" plain @click="preservationadd">确认载入</el-button>
              </el-form-item>
            </el-form>
            <div style="margin-left: 5%">
              <el-checkbox-group v-model="ThischeckedCities" @change="handleCheckedCitiesChange" >
                <el-checkbox v-for="city in cities" :label="city.id" :key="city.id"  >{{city.c_name}}</el-checkbox>
              </el-checkbox-group>
            </div>
      </div>
</template>

<script>
  export default {
    /*获取传值
    * 官方文档
    * https://cn.vuejs.org/v2/guide/components-props.html#%E4%BC%A0%E5%85%A5%E4%B8%80%E4%B8%AA%E5%AF%B9%E8%B1%A1%E7%9A%84%E6%89%80%E6%9C%89-property
    * String Number Boolean Array Object Date Function Symbol
    * */
    props: {
      checkedCities: Array,// 商品选择
      innerDrawer: Boolean,// 回显展示
      preservationadd: Function,//确认载入 商品 信息 从 父窗体 过来
      cities: Array,// 复选框 数组
      cityOptions: Array,//  复选框 检索临时缓存
    },
    data() {
      return {
        ThischeckedCities:[],//当前界面 商品选择 arr
        checkAll: false,//全选按钮 绑定值
        isIndeterminate: false,//全选按钮 是否选中
        selName: '',//商品 名称检索
      };
    },
    methods: {

      handleCheckedCitiesChange(value) {
        let checkedCount = value.length;
        this.checkAll = checkedCount === this.cities.length;
        this.isIndeterminate = checkedCount > 0 && checkedCount < this.cities.length;
        this.$emit("update:checkedCities",this.ThischeckedCities);
      },


      //全选
      handleCheckAllChange(val) {
        // console.log(this.checkedCities)
        if(val){
          let arr = [];
          for (let i = 0; i < this.cityOptions.length; i++) {
            arr.push(this.cityOptions[i].id);
          }
          this.ThischeckedCities = arr;
          this.$emit("update:checkedCities",arr);
        }else{
          this.ThischeckedCities =  [];
          this.$emit("update:checkedCities",[]);
        }
        this.isIndeterminate = false;
      },
      //查询 多选框
      selCheckboxOPtion(){

        let value = this.selName;
        let arr = [];
        if(value!=null && value!=''){
          for (let i = 0; i < this.cityOptions.length; i++) {
            if(this.cityOptions[i].c_name.indexOf(value)!=-1){
              arr.push(this.cityOptions[i]);
            }
          }
        }else{
          arr = JSON.parse(JSON.stringify(this.cityOptions));
        }
        //console.log(arr)
        this.$emit("update:cities",arr);
      },

    }
  };
</script>
