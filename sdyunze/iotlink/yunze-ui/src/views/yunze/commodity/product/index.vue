<template>
  <div class="app-container">
    <el-row :gutter="20">

      <el-col :span="mainwidth" :xs="24">
        <el-form :model="queryParams" ref="queryForm" :inline="true" v-show="showSearch" label-width="68px">


        </el-form>

      <el-form :model="queryParams" ref="queryForm" :inline="true"  label-width="68px" style="margin-bottom: 10px">
        <el-row :gutter="10" class="mb8">
          <el-col :span="1.5">
            <!--条件选择-->
              <el-input
                v-model="queryParams.value"
                placeholder="查询值"
                clearable
                size="small"
                style="width: 350px"
                @keyup.enter.native="handleQuery"
              >
                <el-select
                  v-model="queryParams.type"
                  placeholder="查询条件"
                  clearable
                  slot="prepend"
                  style="width: 110px"
                >
                  <el-option
                    v-for="dict in typeOptions"
                    :key="dict.dictValue"
                    :label="dict.dictLabel"
                    :value="dict.dictValue"
                  />
                </el-select>
              </el-input>
          </el-col>
          <el-col :span="1.5">
            <el-button type="primary" icon="el-icon-search" size="mini" @click="handleQuery">搜索</el-button>
          </el-col>
          <el-col :span="1.5">
            <el-button icon="el-icon-refresh" size="mini" @click="resetQuery">重置</el-button>
          </el-col>
          <el-col :span="1.5">
            <el-button
              type="primary"
              plain
              icon="el-icon-plus"
              size="mini"
              @click="handleAdd"
              v-hasPermi="['yunze:product:add']"
            >新增
            </el-button>
          </el-col>
          <el-col :span="1.5">
            <el-button
              type="primary"
              plain
              icon="el-icon-user"
              size="mini"
              @click="handleDivide"
              v-hasPermi="['yunze:productAgent:edit']"
            >商品代理设置</el-button>
          </el-col>
          <right-toolbar :showSearch.sync="showSearch" v-hasPermi="['yunze:cardRoute:list']" @queryTable="getList"
                         :columns="columns"></right-toolbar>
        </el-row>
      </el-form>

        <el-table v-loading="loading" :data="cardList" @selection-change="handleSelectionChange">
          <el-table-column label="编码" align="center" key="product_core" prop="product_core" v-if="columns[0].visible"/>
          <el-table-column label="名称" align="center" key="product_name" prop="product_name" v-if="columns[1].visible"/>
          <el-table-column label="所属分类" align="center" key="one_category_id" prop="one_category_id"
                           v-if="columns[2].visible">
            <template slot-scope="scope">
              {{tools.getkeyValue(PrdcategoryOption,scope.row.one_category_id,"category_id","category_name")}}
            </template>
          </el-table-column>
          <el-table-column label="售价" align="center" key="price" prop="price" v-if="columns[3].visible"/>
          <el-table-column label="售卖状态" align="center" key="publish_status" v-if="columns[4].visible"
                           :show-overflow-tooltip="true" width="150">
            <template slot-scope="scope">
              {{tools.getDkeyValue(commoditySellOption,scope.row.publish_status)}}
            </template>
          </el-table-column>
          <el-table-column label="描述" align="center" key="descript" prop="descript" v-if="columns[5].visible"/>
          <el-table-column label="录入时间" align="center" key="indate" prop="indate" v-if="columns[6].visible"/>
          <el-table-column label="修改时间" align="center" key="modified_time" prop="modified_time"
                           v-if="columns[7].visible"/>
          <el-table-column
            label="操作"
            align="center"
            width="160"
            class-name="small-padding fixed-width"
          >
            <template slot-scope="scope">

              <el-button
                size="mini"
                type="text"
                icon="el-icon-edit"
                @click="handleUpdate(scope.row)"
                v-hasPermi="['yunze:cardRoute:edit']"
              >修改
              </el-button>

              <el-button
                size="mini"
                type="text"
                icon="el-icon-edit"
                @click="onPreview(scope.row)"
              >图片查看
              </el-button>

            </template>
          </el-table-column>
        </el-table>

        <pagination
          v-show="total>0"
          :total="total"
          :page.sync="queryParams.pageNum"
          :limit.sync="queryParams.pageSize"
          @pagination="getList"
        />
      </el-col>
    </el-row>

    <!-- 详情查看 -->

    <el-dialog :title="title" :visible.sync="show_ds" width="960px" append-to-body>
      <el-form ref="form" :model="form"  label-width="80px">
        <el-row>
          <el-col :span="12">
            <el-form-item label="商品名称" prop="cd_code">
              <el-input v-model="form.product_name" placeholder="请输入商品名称"/>
            </el-form-item>
          </el-col>
          <el-col :span="12">
            <el-form-item label="销售价格" prop="price">
              <el-input v-model="form.price"  @keydown.native="handleInput2"  placeholder="请输入销售价格"/>
            </el-form-item>
          </el-col>
        </el-row>

        <el-row>
          <el-col :span="8">
            <el-form-item label="分类选择" prop="deptId">
              <treeselect v-model="form.one_category_id" :options="TreePrdcategoryOption" placeholder="请选择分类"/>
            </el-form-item>
          </el-col>
          <el-col :span="8">
            <el-form-item label="售卖状态" prop="cd_code">
              <el-select v-model="form.publish_status" placeholder="请选择">
                <el-option
                  v-for="item in commoditySellOption"
                  :key="item.dictValue"
                  :label="item.dictLabel"
                  :value="item.dictValue"
                ></el-option>
              </el-select>
            </el-form-item>
          </el-col>
          <el-col :span="8" v-if="cardRouteEdie" v-hasPermi="['yunze:product:AddImg']" >
            <el-form-item label="第一张为主图" label-width="150px">
              <el-radio-group>
                <label class="el-radio " v-for="(item,index) in customize_whether">
                                    <span class="el-radio__input my_checkbox__inner">
                                    <input type="radio" v-model="form.is_master"
                                           name="in_stock"
                                           @change="SelectChange()"
                                           :value="item.dictValue"/>
                                     </span>
                  <span class="el-radio__label ">{{item.dictLabel}}</span>
                </label>
              </el-radio-group>
            </el-form-item>
          </el-col>
        </el-row>

        <el-row>
          <el-col :span="24">
            <el-form-item label="商品描述">
              <el-input v-model="form.descript" placeholder="请输入商品描述"/>
            </el-form-item>
          </el-col>
        </el-row>
        <!-- -->
        <el-row  v-if="cardRouteEdie" v-hasPermi="['yunze:product:AddImg']" >
          <el-col :span="24">
            <ul class="el-upload-list__item  is-ready image-container" >
              <vuedraggable  >
                <li tabindex="0" class="Mylist__item-thumbnail is-ready draggable-item" v-for="(image, index) in sortImages" :key="index" style=" position: relative;">
                  <img :src="image.uri" alt=""  style="width:100%;" class="my_el-upload-list__item-thumbnail"   :preview-src-list="getImgList(index)"  />
                  <span class="My_el-upload-list__item-actions">
                          <span class="my_upload-list__item-actions">
                            <i class="el-icon-zoom-in"  ></i>
                          </span>
                          <span  class="my_upload-list__item-actions-left">
                            <i class="el-icon-download"></i>
                          </span>
                          <span  class="my_upload-list__item-actions-left">
                            <i class="el-icon-delete"></i>
                          </span>
                        </span>
                </li>
              </vuedraggable>
              <li class="Mylist__item-thumbnail">
                <el-upload
                  ref="uploadPrdImg"
                  action="#"
                  :headers="upload.headers"
                  :action="upload.url"
                  :disabled="upload.isUploading"
                  :multiple="multiple"
                  :accept="acceptConfig"
                  list-type="picture-card"
                  style="border: 1px dashed #c0ccda;"
                  :on-change="customUpload"
                  :on-progress="handleFileUploadProgress"
                  :on-success="handleFileSuccess"
                  :data="{Pstr:this.form.Pstr}"
                  :auto-upload="false" id="MyuploadImg" >
                  <div slot="file" slot-scope="{file}"  >
                    <img
                      class="el-upload-list__item-thumbnail"
                      :src="file.url" alt=""
                    >
                    <span
                      v-if="!disabled"
                      @click="handleRemove(file)"
                    >
                        <i class="el-icon-delete"></i>
                      </span>
                  </div>
                </el-upload>
              </li>
            </ul>
          </el-col>
        </el-row>



      </el-form>
      <div slot="footer" class="dialog-footer">
        <el-input v-model="form.cd_id" v-if="false"/>
        <el-button v-if="cardRouteAdd" v-hasPermi="['yunze:product:add']" type="primary" @click="Add">新 增</el-button>
        <el-button v-if="cardRouteEdie" v-hasPermi="['yunze:product:AddImg']" type="primary" @click="AddImg">图片新增</el-button>
        <el-button v-if="cardRouteEdie" v-hasPermi="['yunze:product:edit']" type="primary" @click="Edie">保 存
        </el-button>
      </div>
    </el-dialog>


    <el-dialog :visible.sync="dialogVisible">
      <img width="100%" :src="dialogImageUrl" alt="">
    </el-dialog>



    <!-- 商品售卖代理划分 -->
    <el-dialog :title="title" :visible.sync="Divide_show" width="960px" append-to-body>
      <el-form ref="form" :model="form"  label-width="80px">
        <el-row>
          <el-col :span="12">
            <el-checkbox v-model="deptExpand" @change="handleCheckedTreeExpand($event, 'dept')">展开/折叠</el-checkbox>
            <el-checkbox v-model="deptNodeAll" @change="handleCheckedTreeNodeAll($event, 'dept')">全选/全不选</el-checkbox>
            <el-checkbox v-model="deptCheckStrictly" @change="handleCheckedTreeConnect($event, 'dept')">父子联动</el-checkbox>

            <div class="head-container">
              <el-input
                v-model="treeName"
                placeholder="请输入企业名称"
                clearable
                size="small"
                prefix-icon="el-icon-search"
                style="margin-bottom: 20px"
              />
            </div>
            <div class="head-container">
              <el-tree :data="deptOptions"  ref="dept" :filter-node-method="filterNode"
                       show-checkbox  :check-strictly="!deptCheckStrictly"
                       default-expand-all node-key="id"
                       :props="defaultProps"  empty-text="暂无数据">
              </el-tree>
            </div>
          </el-col>
        </el-row>

      </el-form>
      <div slot="footer" class="dialog-footer">
        <el-button   v-hasPermi="['yunze:productAgent:edit']" v-if="Dividebtn"  type="primary" @click="Divide">保 存</el-button>
      </div>
    </el-dialog>


    <el-image
      v-show="false"
      ref="preview"
      class="hideImgDiv"
      :src="url"
      :preview-src-list="srcList"></el-image>

  </div>
</template>


<script>
import {listProduct, getCategory,save,edit,findData,findAgentList,AgentEdit,PrdImgfindList,readImage} from "@/api/yunze/commodity/product";
import tools from "@/utils/yunze/tools";
import Treeselect from "@riophae/vue-treeselect";
import { treeselect } from "@/api/system/dept";
import "@riophae/vue-treeselect/dist/vue-treeselect.css";
import {getToken} from "@/utils/auth";
import vuedraggable from 'vuedraggable'
import $ from 'jquery'



export default {
  name: "product",
  components: {Treeselect,vuedraggable},
  data() {
    return {
      images:[],
      sortImages: !this.images ? [] : [].concat(this.images),
      acceptConfig: 'image/jpeg,image/png,image/jpg,image/gif',
      fileList:{},

      url: '',
      srcList: [],


      Divide_show: false,//卡划分
      Dividebtn:true,//商品代理 btn
      //商品代理企业
      treeName: undefined,
      formDivide: {
        dept_id:'',
        user_id:'',
      },


      // 导入参数
      upload: {
        // 是否显示弹出层（用户导入）
        open: false,
        // 弹出层标题（用户导入）
        title: "",
        // 是否禁用上传
        isUploading: false,
        // 是否更新已经存在的用户数据
        updateSupport: 0,
        // 设置上传的请求头部
        headers: { Authorization: "Bearer " + getToken() },
        // 上传的地址
        url: process.env.VUE_APP_BASE_API + "/yunze/productPic/Prdedit",
        Pstr:'',//携带参数
      },

      Is_notAdd : false,
      dialogImageUrl: '',
      dialogVisible: false,
      disabled: false,

      //保存 编辑 按钮
      cardRouteAdd: false,
      cardRouteEdie: false,
      tools: tools,
      deptCheckStrictly: false,
      deptExpand: true,
      deptNodeAll: false,
      show_ds: false,//详情查看
      table_Online: [],//在线状态信息
      DeptOptions: [],// 公司信息
      customize_whether: [],//自定义是否

      mainwidth: 24,//宽度
      option_show: false, //公司所属查询
      // 遮罩层
      loading: true,
      // 导出遮罩层
      exportLoading: false,
      // 选中数组
      ids: [],
      // 非单个禁用
      single: true,
      // 非多个禁用
      multiple: true,
      // 显示搜索条件
      showSearch: false,
      // 总条数
      total: 0,
      // 用户表格数据
      cardList: null,
      // 弹出层标题
      title: "",
      // 部门树选项
      deptOptions: undefined,
      // 是否显示弹出层
      open: false,
      // 部门名称
      deptName: undefined,
      // 默认密码
      initPassword: undefined,
      // 日期范围
      dateRange: [],
      // 状态数据字典
      typeOptions: [],
      PrdcategoryOption: [],//商品分类
      TreePrdcategoryOption: [],//商品分类 树
      //详情
      tablefrom: [],
      commoditySellOption: [],//加载 商品出售状态
      imgArr: [],//上传图片
      SelectImgArr: [],//  商品图片查看图片

      checkStateOption: [], //加载 商品出售状态
      FlowData: [],
      selIccid: [{FlowData: ''}],
      activeNames: [0],

      //卡状态
      status_type: "",
      // 卡状态 字典
      stateOptions: [
        {
          dictLabel: ''
        }
      ],
      // 运营商类别 字典
      operators_type: [],
      // 通道状态 字典
      channelStatusOptions: [],
      // 通道编码 字典
      channelCodeOptions: [],
      // 表单参数
      form: {
        product_id:'',
        product_name:'',
        price:'',
        one_category_id:'',
        publish_status:'',
        descript:'',
        is_master:'',
        Pstr:'',
      },
      defaultProps: {
        children: "children",
        label: "label"
      },
      //当前查询参数
      sel: {
        iccid: '',
        agent_id: '',
        package_id: '',
      },
      // 查询参数
      queryParams: {
        pageNum: 1,
        pageSize: 10,
        type: null,
        value: null,
        cd_status: [],
      },
      // 列信息
      columns: [
        {key: 0, label: `编码`, visible: true},
        {key: 1, label: `名称`, visible: true},
        {key: 2, label: `所属分类`, visible: true},
        {key: 3, label: `售价`, visible: true},
        {key: 4, label: `售卖状态`, visible: true},
        {key: 5, label: `描述`, visible: true},
        {key: 6, label: `录入时间`, visible: true},
        {key: 7, label: `修改时间`, visible: true},
      ],
      // 表单校验
      rules: {}
    };
  },
  watch: {},

  created() {


    //加载 自定义 是否 0 1 否 是
    if(window['customize_whether']!=undefined &&  window['customize_whether']!=null && window['customize_whether']!=''){
      this.customize_whether = window['customize_whether'];
    }else{
      this.getDicts("yunze_customize_whether").then(response => {
        window['customize_whether'] = response.data;
        this.customize_whether = window['customize_whether'];
      });
    }




    //加载 查询条件
    if (window['Bcommodity_selType'] != undefined && window['Bcommodity_selType'] != null && window['Bcommodity_selType'] != '') {
      this.typeOptions = window['Bcommodity_selType'];
    } else {
      this.getDicts("yz_Bcommodity_selType").then(response => {
        window['Bcommodity_selType'] = response.data;
        this.typeOptions = window['Bcommodity_selType'];
      });
    }


    //加载 商品分类
    if (window['PrdcategoryOption'] != undefined && window['PrdcategoryOption'] != null && window['PrdcategoryOption'] != '') {
      this.PrdcategoryOption = window['PrdcategoryOption'];
      this.MysetTreePrdcategoryOption();
    } else {
      let Pwd_Str = tools.encrypt(JSON.stringify({}));
      getCategory(Pwd_Str).then(response => {
        let jsonobj = JSON.parse(tools.Decrypt(response));
        window['PrdcategoryOption'] = jsonobj.Data;
        this.PrdcategoryOption = window['PrdcategoryOption'];
        this.MysetTreePrdcategoryOption();
      });
    }


    //加载 商品出售状态
    if (window['commoditySellOption'] != undefined && window['commoditySellOption'] != null && window['commoditySellOption'] != '') {
      this.commoditySellOption = window['commoditySellOption'];
    } else {
      this.getDicts("yz_commodity_sell").then(response => {
        window['commoditySellOption'] = response.data;
        this.commoditySellOption = window['commoditySellOption'];
      });
    }


    //加载 商品审核状态
    if (window['checkStateOption'] != undefined && window['checkStateOption'] != null && window['checkStateOption'] != '') {
      this.checkStateOption = window['checkStateOption'];
    } else {
      this.getDicts("yz_check_state").then(response => {
        window['checkStateOption'] = response.data;
        this.checkStateOption = window['checkStateOption'];
      });
    }
    this.getTreeselect();
    this.getList();
  },
  methods: {
    onPreview(row) {
      this.SelectImgArr = [];
      let Name = 'PrdImg_'+row.product_id;
      //console.log(Name)
      if(window[Name] != undefined && window[Name] != null && window[Name] != ''){
        this.SelectImgArr = window[Name];
        this.$refs.preview.clickHandler();
      }else{
        this.findImg(row.product_id);
      }
    },

    findImg(product_id){
      let map = {};
      map.product_id = product_id;
      map.pic_status = "1";
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
      PrdImgfindList(Pwd_Str).then(response => {
        let jsonobj = JSON.parse(tools.Decrypt(response));
        if (jsonobj.code == 200) {
          let Arrimg = [];
          //console.log(jsonobj)
          if(jsonobj.Data.length>0){
            for (let i = 0; i < jsonobj.Data.length; i++) {
              let Pwd_Str = tools.encrypt(JSON.stringify({"imagePath":jsonobj.Data[i].pic_url}));
          console.log(process.env.VUE_APP_BASE_API)

              Arrimg.push(process.env.VUE_APP_BASE_API+"/yunze/productPic/readImage?map="+Pwd_Str);
              if(i==jsonobj.Data.length-1){
                let Name = 'PrdImg_'+ product_id;
                window[Name] = Arrimg;
                this.SelectImgArr = Arrimg;
                this.srcList = Arrimg;
                //调用image组件中的大图浏览图片方法
                this.$refs.preview.clickHandler();
              }
            }
          }else{
            this.msgError("该商品暂未上传商品图片！");
          }
        }else{
          this.msgError(jsonobj.msg);
        }
      })


    },



    /**保存商品代理*/
    Divide(){
      //this.Dividebtn = false;
      let dept_id = this.$refs.dept.getCheckedKeys();
      if(dept_id.length>0){
        let map = {};
        map.IdArr = dept_id;
        let Pwd_Str = tools.encrypt(JSON.stringify(map));
        AgentEdit(Pwd_Str).then(response => {
          let jsonobj = JSON.parse(tools.Decrypt(response));
          //console.log(jsonobj);
          if (jsonobj.code == 200) {
            this.msgSuccess(jsonobj.msg);
            this.getAgentSel();
          }else{
            this.msgError(jsonobj.msg);
          }
        })
      }else{
        this.msgError("请选择售卖代理商！");
      }
    },
    /**划卡*/
    handleDivide(){
      this.Divide_show = true;
      this.getAgentSel();
    },
    //获取商品代理 选中
    getAgentSel(){
      let map = {};
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
      findAgentList(Pwd_Str).then(response => {
        let jsonobj = JSON.parse(tools.Decrypt(response));
        //console.log(jsonobj);
        if (jsonobj.code == 200) {
          let AgentSelArr = jsonobj.Data;
          if(AgentSelArr.length>0){
            console.log(AgentSelArr);
            this.$refs.dept.setCheckedKeys(AgentSelArr);
          }
        }else{
          this.msgError(jsonobj.msg);
        }
      })
    },

    SelectChange(){
      this.getPwd_Str();
    },
    // 加密数据
    getPwd_Str(){
      if(this.sortImages.length>0){
        let imgArr = [];
        for (let i = 0; i < this.sortImages.length; i++) {
          let obj = this.sortImages[i];
          obj.pic_order = i;
          imgArr.push(obj);
        }
        this.imgArr = imgArr;
      }
      let map ={};
      map.is_master = this.form.is_master;
      map.product_id = this.form.product_id;
      map.imgArr = this.imgArr;
      console.log(map)
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
      this.form.Pstr = Pwd_Str;
    },
    // 文件上传中处理
    handleFileUploadProgress(event, file, fileList) {
      this.upload.isUploading = true;
      this.Is_notAdd = true;
    },



    // 文件上传成功处理
    handleFileSuccess(response, file, fileList) {
      this.upload.open = false;
      this.upload.isUploading = false;
      //this.$refs.uploadPrdImg.clearFiles();
      // this.$message.info('导入结果'+ JSON.toString({ dangerouslyUseHTMLString: true }));
      // console.log({dangerouslyUseHTMLString: true})
      // this.$alert(response.msg, "导入结果", { dangerouslyUseHTMLString: true });
      this.$message.success(response.msg)

    },
    AddImg(){
      this.SelectChange();
      if(this.sortImages.length<10){
        let _this = this;
        if (tools.VerificationsText(_this, _this.form.is_master, "请选择 第一张是否主图！") == true &&
          tools.VerificationsText(_this, _this.form.product_id, "必要参数缺失刷新后重试！") == true ) {
          this.sortImages = [];
          this.images = [];
          _this.$refs.uploadPrdImg.submit();
        }
      }else{
        this.$message.error('最多上传10张图片！');
      }
    },

    handleInput2(e) {
      // 通过正则过滤小数点后两位
      e.target.value = (e.target.value.match(/^\d*(\.?\d{0,1})/g)[0]) || null
    },

    /* 操作--查看 图片预览 */
    getImgList(index){
      let images = JSON.parse(JSON.stringify(this.sortImages));
      for(let i=0;i<index;i++){
        images.push(images[0]);
        images.splice(0,1);
      }
      return images
    },



    customUpload(file) {
      if(!this.Is_notAdd){
        const that = this;
        const isLt10M = file.size / 1024 / 1024 < 10;
        if (!isLt10M) {
          this.$message.error('上传图片大小不能超过10M');
          return false
        }
        if (['image/jpeg', 'image/png', 'image/jpg', 'image/gif'].indexOf(file.raw.type) === -1) {
          this.$message.error('格式为JPG、JPEG、PNG、GIF');
          return false
        }
        const name = file.name.split('.')[0];
        const type = file.name.split('.')[1];
        const copyFile = new File([file.raw], `${name}.${type}`);
        that.sortImages.push({
          uri: file.url,
          fileName: file.name,
          fileSize: file.size / 1024 / 1024
        })
        //console.log(that.sortImages);
        // this.$refs.uploadPrdImg.clearFiles();

        //延迟 清除显示 解决 自配置组件的界面显示问题
        setTimeout(function(){
          $('#MyuploadImg ul').html("");
        },400);
        //延迟 清除显示
        setTimeout(function(){
          $('#MyuploadImg ul').html("");
        },1000);
      }else{
        this.Is_notAdd = false;
      }
    },

    changeImages(){

    },
    handleRemove(file) {
      console.log(file);

    },
    handlePictureCardPreview(file) {
      this.dialogImageUrl = file.url;
      this.dialogVisible = true;
    },
    handleDownload(file) {
      console.log(file);
    },


    MysetTreePrdcategoryOption() {
      if (this.PrdcategoryOption != null && this.PrdcategoryOption.length > 0) {
        for (let i = 0; i < this.PrdcategoryOption.length; i++) {
          let obj = this.PrdcategoryOption[i];
          let map = {};
          map.id = obj.category_id;
          map.label = obj.category_name;
          this.TreePrdcategoryOption.push(map);
        }
      }
    },


    /** 查询企业下拉树结构 */
    getTreeselect() {
      treeselect().then(response => {
        this.deptOptions = response.data;
      });
    },
    // 筛选节点
    filterNode(value, data) {
      if (!value) return true;
      return data.label.indexOf(value) !== -1;
    },
    // 树权限（展开/折叠）
    handleCheckedTreeExpand(value, type) {
      if (type == 'dept') {
        let treeList = this.deptOptions;
        for (let i = 0; i < treeList.length; i++) {
          this.$refs.dept.store.nodesMap[treeList[i].id].expanded = value;
        }
      }
    },
    // 树权限（全选/全不选）
    handleCheckedTreeNodeAll(value, type) {
      if (type == 'dept') {
        //console.log(value);
        this.$refs.dept.setCheckedNodes(value ? this.deptOptions : []);
      }
    },
    // 树权限（父子联动）
    handleCheckedTreeConnect(value, type) {
      if (type == 'menu') {
        this.menuCheckStrictly = value ? true : false;
      } else if (type == 'dept') {
        this.$refs.dept.setCheckedNodes(false ? this.deptOptions : []);
        this.deptCheckStrictly = value ? true : false;

      }
    },


    /*获取查询参数*/
    getParams() {

      if (this.$refs.dept != undefined & this.$refs.dept != null & this.$refs.dept != 'undefined') {
        //console.log(this.$refs.dept.getCheckedKeys());
        if (this.$refs.dept.getCheckedKeys().length > 0) {
          this.queryParams.agent_id = this.$refs.dept.getCheckedKeys();
        }
      }

    },


    /** 查询通道列表 */
    getList() {
      this.loading = true;
      this.getParams();
      this.queryParams.page = 1;
      //console.log(this.queryParams);
      let Pwd_Str = tools.encrypt(JSON.stringify(this.queryParams));
      listProduct(Pwd_Str).then(response => {
          let jsonobj = JSON.parse(tools.Decrypt(response));
          //console.log(jsonobj);
          if (jsonobj.code == 200) {
            this.cardList = jsonobj.Data.Data;
            this.total = jsonobj.Data.Pu.rowCount;
          } else {
            this.msgError("获取数据异常，请联系管理员！");
          }
          this.loading = false;
        }
      );
    },
    /*代理查询拓展*/
    agentShow() {
      let _this = this;
      _this.option_show = !_this.option_show;
      _this.mainwidth = _this.option_show ? 20 : 24;
    },


    // 取消按钮
    cancel() {
      this.open = false;
    },
    /** 搜索按钮操作 */
    handleQuery() {
      this.queryParams.page = 1;
      this.getList();
    },
    /** 重置按钮操作 */
    resetQuery() {
      this.queryParams = {};
      this.queryParams.pageNum = 1;
      this.queryParams.pageSize = 10;
      this.selTime = null;
      this.handleQuery();
    },
    // 多选框选中数据
    handleSelectionChange(selection) {
      this.ids = selection.map(item => item.userId);
      this.single = selection.length != 1;
      this.multiple = !selection.length;
    },
    /** 新增按钮操作 */
    handleAdd() {

      this.form = {};//清空数据
      this.show_ds = true;
      this.cardRouteAdd = true;
      this.cardRouteEdie = false;
    },
    /** 修改按钮操作 */
    handleUpdate(row) {
      this.form = {};//清空数据
      this.show_ds = true;
      this.cardRouteAdd = false;
      this.cardRouteEdie = true;

      this.finddata(row);



    },
    /** 详情按钮操作 */
    ViewDetails(row) {
      //console.log(row);
      this.cardRouteAdd = false;
      this.cardRouteEdie = false;
      this.finddata(row);
    },


    /*获取详细信息*/
    finddata(row) {
      this.form = {};//清空数据
      let map = {};
      map.product_id = row.product_id;
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
      findData(Pwd_Str).then(response => {
          let jsonobj = JSON.parse(tools.Decrypt(response));
          if (jsonobj.code == 200) {
            this.form = jsonobj.Data;
            this.form.publish_status = ""+this.form.publish_status;
            this.show_ds = true;
          } else {
            this.msgError("获取数据异常，请联系管理员！");
          }
          this.loading = false;

        }
      );

    },
    //新增
    Add() {
      let _this = this;
      if (tools.VerificationsText(_this, _this.form.product_name, "商品名称不能为空！") == true &&
        tools.VerificationsText(_this, _this.form.price, "售价不能为空！") == true &&
        tools.VerificationsText(_this, _this.form.one_category_id, "请选择商品分类！") == true &&
        tools.VerificationsText(_this, _this.form.publish_status, "请选择售卖状态！") == true) {
        let Pwd_Str = tools.encrypt(JSON.stringify(this.form));
        save(Pwd_Str).then(response => {
          let jsonobj = JSON.parse(tools.Decrypt(response));
          //console.log(jsonobj);
          if (jsonobj.code == 200) {
            this.msgSuccess(jsonobj.msg);
            this.show_ds = false;
            //更新数据
            this.queryParams.page = 1;
            this.getList();
          } else {
            let msg = jsonobj.Data != null && jsonobj.Data != '' ? jsonobj.Data : jsonobj.msg;
            this.msgError(msg);
          }
        })
      }
    },
    //编辑
    Edie() {
      let _this = this;
      if (tools.VerificationsText(_this, _this.form.product_name, "商品名称不能为空！") == true &&
        tools.VerificationsText(_this, _this.form.price, "售价不能为空！") == true &&
        tools.VerificationsText(_this, _this.form.one_category_id, "请选择商品分类！") == true &&
        tools.VerificationsText(_this, _this.form.product_id, "缺失必要参数刷新后重试！") == true &&
        tools.VerificationsText(_this, _this.form.publish_status, "请选择售卖状态！") == true) {
        let Pwd_Str = tools.encrypt(JSON.stringify(_this.form));
        edit(Pwd_Str).then(response => {
          let jsonobj = JSON.parse(tools.Decrypt(response));
          //console.log(jsonobj);
          if (jsonobj.code == 200) {
            this.msgSuccess(jsonobj.msg);
            this.show_ds = false;
            //更新数据
            this.queryParams.page = 1;
            this.getList();
          } else {
            let msg = jsonobj.Data != null && jsonobj.Data != '' ? jsonobj.Data : jsonobj.msg;
            this.msgError(msg);
          }
        })
      }
    },





  }
};
</script>


<style scoped>

.el-table .cell {
  line-height: 17px;
  padding-left: 10px;
  padding-right: 0px;
}

.el-form-item {
  margin-bottom: 10px;
}

.my_checkbox__inner {
  border: 1px;
}


.Mylist__item-thumbnail{
  overflow: hidden;
  background-color: #fff;
  border: 1px solid #c0ccda;
  border-radius: 6px;
  -webkit-box-sizing: border-box;
  box-sizing: border-box;
  width: 148px;
  height: 148px;
  margin: 0 8px 8px 0;
  display: inline-block;
  font-size: 14px;
  color: #606266;
  line-height: 1.8;
}



.My_el-upload-list__item-actions{
  position: absolute;
  width: 100%;
  height: 100%;
  left: 0;
  top: 0;
  cursor: default;
  text-align: center;
  color: #fff;
  opacity: 0;
  font-size: 20px;
  background-color: rgba(0, 0, 0, 0.5);
  -webkit-transition: opacity 0.3s;
  transition: opacity 0.3s;
}
.My_el-upload-list__item-actions:hover{
  opacity: 1;
}


.My-upload-list__item{
  overflow: hidden;
  background-color: #fff;
  border: 1px solid #c0ccda;
  border-radius: 6px;
  -webkit-box-sizing: border-box;
  box-sizing: border-box;
  width: 148px;
  height: 148px;
  margin: 0 8px 8px 0;
  display: inline-block;
}

.my_upload-list__item-actions-left{
  margin-left: 15px;
}
.my_upload-list__item-actions-left,.my_upload-list__item-actions{
  display: inline-block;cursor: pointer;text-align: center;color: #fff;font-size: 20px;line-height: 1.8;margin-top: 35%;

}

</style>

