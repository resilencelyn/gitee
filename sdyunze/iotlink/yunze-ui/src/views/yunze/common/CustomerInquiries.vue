<template>
      <div >
          <!-- 客户 赛选查询条件  组件-->

        <el-row v-if="loadingSelOption">
          <el-col :span="6">
            <el-form-item prop="cd_code">
                 <span slot="label">
                    一级分类：<span class="redColor">*</span>
                    <el-tooltip placement="right">
                      <div slot="content">
                        企业定位 所属 一级分类
                      </div>
                      <i class="el-icon-question"></i>
                    </el-tooltip>
                  </span>
              <el-select v-model="Sel.oneIdArr" multiple placeholder="请选择">
                <el-option
                  v-for="item in CustomerOneCategoryOption"
                  :key="item.dictValue"
                  :label="item.dictLabel"
                  :value="item.dictValue"
                ></el-option>
              </el-select>
            </el-form-item>
          </el-col>
          <el-col :span="6">
            <el-form-item prop="cd_code">
                 <span slot="label">
                    二级分类：<span class="redColor">*</span>
                    <el-tooltip placement="right">
                      <div slot="content">
                        企业定位 所属 二级分类
                      </div>
                      <i class="el-icon-question"></i>
                    </el-tooltip>
                  </span>
              <el-select v-model="Sel.TowIdArr" multiple placeholder="请选择">
                <el-option
                  v-for="item in CustomerTowCategoryOption"
                  :key="item.dictValue"
                  :label="item.dictLabel"
                  :value="item.dictValue"
                ></el-option>
              </el-select>
            </el-form-item>
          </el-col>
          <el-col :span="6">
            <el-form-item prop="cd_code">
                <span slot="label">
                    客户级别：<span class="redColor">*</span>
                    <el-tooltip placement="right">
                      <div slot="content">
                        客户级别： 如创建 最新级别 需字典管理 客户级别 新增 客户级别详情；
                      </div>
                      <i class="el-icon-question"></i>
                    </el-tooltip>
                  </span>
              <el-select v-model="Sel.gradeIdArr" multiple placeholder="请选择">
                <el-option
                  v-for="item in CustomerGradeOption"
                  :key="item.dictValue"
                  :label="item.dictLabel"
                  :value="item.dictValue"
                ></el-option>
              </el-select>
            </el-form-item>
          </el-col>
        </el-row>

        <el-row v-if="loadingSelOption">
          <el-col :span="8">
            <el-form-item>
                <span slot="label">
                    地址信息：<span class="redColor">*</span>
                    <el-tooltip placement="right">
                      <div slot="content">
                        企业 所在地址信息
                      </div>
                      <i class="el-icon-question"></i>
                    </el-tooltip>
                  </span>
              <template>
                <div id="app">
                  <el-cascader
                    size="large"
                    :options="selFindoptions"
                    v-model="selectedOptions"
                    style="width: 400px;"
                    @change="handleChange">
                  </el-cascader>
                </div>
              </template>
            </el-form-item>
          </el-col>
          <el-col :span="8">
            <el-form-item>
                <span slot="label">
                    企业关键字：<span class="redColor">*</span>
                    <el-tooltip placement="right">
                      <div slot="content">
                        企业 名字中包含的关键字 检索企业信息加载 甲方信息
                      </div>
                      <i class="el-icon-question"></i>
                    </el-tooltip>
                  </span>
              <el-input placeholder="请输入 企业关键字" v-model="Sel.c_name" @keyup.enter.native="setCustomerArr(null)"
                        class="input-with-select">
                <el-button slot="append" icon="el-icon-search" @click="setCustomerArr(null)"></el-button>
              </el-input>

            </el-form-item>
          </el-col>
        </el-row>




      </div>
</template>

<script>
  import {CodeToText} from "element-china-area-data";

  export default {
    /*获取传值
    * 官方文档
    * https://cn.vuejs.org/v2/guide/components-props.html#%E4%BC%A0%E5%85%A5%E4%B8%80%E4%B8%AA%E5%AF%B9%E8%B1%A1%E7%9A%84%E6%89%80%E6%9C%89-property
    * String Number Boolean Array Object Date Function Symbol
    * */
    props: {
      CustomerOneCategoryOption: Array,// 客户一级分类
      CustomerTowCategoryOption: Array, // 客户二级分类
      Sel: Object,//客户信息查询参数
      loadingSelOption: Boolean,//是否 加载 查询客户信息框
      CustomerGradeOption: Array,// 客户级别
      selFindoptions: Array,//是省市区三级联动数据（带“全部”选项）
      setCustomerArr: Function,//获取公司信息
      SetSel: Function, //set 客户信息 查询参数
    },
    data() {
      return {
        selectedOptions: [], //地址选项
      };
    },
    methods: {
      handleChange(value) {//省市区 获取
        //console.log(value)
        let Map = JSON.parse(JSON.stringify(this.Sel));
        for (let i = 0; i < value.length; i++) {
          if (i == 0) {
            Map.province = CodeToText[value[i]];
          } else if (i == 1) {
            Map.city = CodeToText[value[i]];
          } else if (i == 2) {
            Map.district = CodeToText[value[i]];
          }
        }
        //选择 非省 市 区 时  重新赋值 市 区
        if(value.length==2){
          Map.district = null;
        }else if(value.length==1){
          Map.city = null;
          Map.district = null;
        }
        this.$emit("SetSel",Map);
      },



    }
  };
</script>
