<template>
  <div class="app-container">
    <el-row :gutter="20">
      <!--部门数据-->
      <el-col :span="4" :xs="24" v-show="option_show">
        <div class="head-container">
          <el-input
            v-model="deptName"
            placeholder="请输入公司名称"
            clearable
            size="small"
            prefix-icon="el-icon-search"
            style="margin-bottom: 20px"
          />·
        </div>
        <div class="head-container">
          <el-checkbox v-model="deptExpand" @change="handleCheckedTreeExpand($event, 'dept')">展开/折叠</el-checkbox>
          <el-checkbox v-model="deptNodeAll" @change="handleCheckedTreeNodeAll($event, 'dept')">全选/全不选</el-checkbox>
          <el-checkbox v-model="deptCheckStrictly" @change="handleCheckedTreeConnect($event, 'dept')">父子联动</el-checkbox>



          <el-tree
            :data="deptOptions"
            show-checkbox
            :filter-node-method="filterNode"
            default-expand-all
            node-key="id"
            ref="dept"
            highlight-current
            :check-strictly="!deptCheckStrictly"
            :props="defaultProps" />


        </div>
      </el-col>
      <!--用户数据-->
      <el-col :span="mainwidth" :xs="24" >
        <el-form :model="queryParams" ref="queryForm" :inline="true" v-show="showSearch" label-width="68px">
          <el-form-item  v-hasPermi="['yunze:cardRoute:remove']"  >
           <!-- <el-radio-group >
              <label class="radio " v-for="(item,index) in channelStatusOptions">
                                    <span class="el-radio__inner my_radio__inner">
                                    <input type="radio" v-model="queryParams.cd_status"
                                           :value="item.dictValue"/>
                                     </span>
                <span class="el-radio__label my_radio__label">{{item.dictLabel}}</span>
              </label>
            </el-radio-group>-->

          </el-form-item>

        </el-form>

        <el-form :model="queryParams" ref="queryForm" :inline="true" label-width="68px" style="margin-bottom: 10px">
          <el-row :gutter="10" class="mb8">
            <el-col :span="1.5">
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
              <el-button size="mini" v-hasPermi="['yunze:card:listAgent']"  @click="agentShow" >公司所属</el-button>
            </el-col>
            <el-col :span="1.5">
              <el-button
                type="primary"
                plain
                icon="el-icon-plus"
                size="mini"
                @click="handleAdd"
                v-hasPermi="['yunze:cardRoute:add']"
              >新增</el-button>
            </el-col>
            <el-col :span="1.5">
              <el-button
                type="warning"
                plain
                icon="el-icon-download"
                size="mini"
                :loading="exportLoading"
                @click="handleExport"
                v-hasPermi="['yunze:cardRoute:exportData']"
              >导出</el-button>
            </el-col>
            <right-toolbar :showSearch.sync="showSearch"   v-hasPermi="['yunze:cardRoute:list']" @queryTable="getList" :columns="columns"></right-toolbar>
          </el-row>
        </el-form>



        <el-table v-loading="loading" :data="cardList" @selection-change="handleSelectionChange">
          <el-table-column label="cd_id" align="center" key="cd_id" prop="cd_id"  v-if="columns[0].visible"   />
          <el-table-column label="通道编号" align="center" key="cd_code" prop="cd_code"  v-if="columns[0].visible"   >
            <template slot-scope="scope">
              {{tools.getDkeyValue(channelCodeOptions,scope.row.cd_code)}}
            </template>
          </el-table-column>
          <el-table-column label="通道名称" align="center" key="cd_name" prop="cd_name"  v-if="columns[1].visible">
            <template slot-scope="scope">
              <router-link :to="'/polling/type/index/' + scope.row.cd_id" class="link-type">
                <span>{{ scope.row.cd_name }}</span>
              </router-link>
            </template>
          </el-table-column>
          <el-table-column label="通道别名" align="center" key="cd_alias" prop="cd_alias"  v-if="columns[2].visible"   />
          <el-table-column label="运营类型" align="center" v-if="columns[3].visible" :show-overflow-tooltip="true"  width="150"  >

            <template slot-scope="scope">
              {{tools.getDkeyValue(operators_type,scope.row.cd_operator_type)}}
            </template>
          </el-table-column>
          <el-table-column label="通道状态" align="center" key="cd_status"  v-if="columns[4].visible" :show-overflow-tooltip="true" width="150"  >
            <template slot-scope="scope" >
              {{tools.getDkeyValue(channelStatusOptions,scope.row.cd_status)}}
            </template>
          </el-table-column>
          <el-table-column label="是否轮训" align="center" key="cd_lunxun"  v-if="columns[5].visible" :show-overflow-tooltip="true" width="150"  >
            <template slot-scope="scope" >
              {{tools.getDkeyValue(customize_whether,scope.row.cd_lunxun)}}
            </template>
          </el-table-column>


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
              >修改</el-button>
              <el-button
                v-if="scope.row.userId !== 1"
                size="mini"
                type="text"
                icon="el-icon-delete"
                @click="handleDelete(scope.row)"
                v-hasPermi="['yunze:cardRoute:remove']"
              >删除</el-button>
              <el-button
                size="mini"
                type="text"
                icon="el-icon-s-order"
                @click="ViewDetails(scope.row)"
                v-hasPermi="['yunze:cardRoute:find']"
              >详情</el-button>
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
      <el-form ref="form" :model="form" :rules="rules" label-width="80px">
        <el-row>
          <el-col :span="8">
            <el-form-item label="通道名称" prop="cd_code">
              <el-input v-model="form.cd_name" placeholder="请输入通道名称" />
            </el-form-item>
          </el-col>
          <el-col :span="8">
            <el-form-item label="通道别名" prop="cd_alias">
              <el-input v-model="form.cd_alias" placeholder="请输入通道别名" />
            </el-form-item>
          </el-col>
          <el-col :span="8">
            <el-form-item label="通道编码" prop="cd_code">
              <el-select v-model="form.cd_code"   placeholder="请选择">
                <el-option
                  v-for="item in channelCodeOptions"
                  :key="item.dictValue"
                  :label="item.dictLabel"
                  :value="item.dictValue"
                ></el-option>
              </el-select>
            </el-form-item>
          </el-col>
        </el-row>
        <el-row>
          <el-col :span="8">
            <el-form-item label="运营商类型" prop="cd_code" label-width="130">
              <el-select v-model="form.cd_operator_type"   placeholder="请选择">
                <el-option
                  v-for="item in operators_type"
                  :key="item.dictValue"
                  :label="item.dictLabel"
                  :value="item.dictValue"
                ></el-option>
              </el-select>
            </el-form-item>
          </el-col>
          <el-col :span="8">
            <el-form-item label="是否轮训" prop="cd_code" >
              <el-select v-model="form.cd_lunxun"   placeholder="请选择">
                <el-option
                  v-for="item in customize_whether"
                  :key="item.dictValue"
                  :label="item.dictLabel"
                  :value="item.dictValue"
                ></el-option>
              </el-select>
            </el-form-item>
          </el-col>
          <el-col :span="8">
            <el-form-item label="通道状态" prop="cd_code" >
              <el-select v-model="form.cd_status"   placeholder="请选择">
                <el-option
                  v-for="item in channelStatusOptions"
                  :key="item.dictValue"
                  :label="item.dictLabel"
                  :value="item.dictValue"
                ></el-option>
              </el-select>
            </el-form-item>
          </el-col>
        </el-row>
        <el-row>
          <el-col :span="8">
            <el-form-item label="通道算法" prop="cd_code" >
              <el-select v-model="form.cd_algorithm"   placeholder="请选择">
                <el-option
                  v-for="item in route_algorithm"
                  :key="item.dictValue"
                  :label="item.dictLabel"
                  :value="item.dictValue"
                ></el-option>
              </el-select>
            </el-form-item>
          </el-col>
          <el-col :span="8">

          </el-col>
          <el-col :span="8">

          </el-col>
        </el-row>
        <el-row>
          <el-col :span="24">
            <el-form-item label="用户名" >
              <el-input v-model="form.cd_username" placeholder="请输入用户名" />
            </el-form-item>
          </el-col>
        </el-row>
        <el-row>
          <el-col :span="12">
            <el-form-item label="密码" >
              <el-input v-model="form.cd_pwd" placeholder="请输入密码" />
            </el-form-item>
          </el-col>
          <el-col :span="12">
            <el-form-item label="key" >
              <el-input v-model="form.cd_key" placeholder="请输入key" />
            </el-form-item>
          </el-col>
        </el-row>
      </el-form>
      <div slot="footer" class="dialog-footer">
        <el-input v-model="form.cd_id" v-if="false" />
        <el-button v-if="cardRouteAdd"   v-hasPermi="['yunze:cardRoute:add']" type="primary" @click="Add">新 增</el-button>
        <el-button v-if="cardRouteEdie"  v-hasPermi="['yunze:cardRoute:edit']"   type="primary" @click="Edie">保 存</el-button>
      </div>
    </el-dialog>




  </div>
</template>

<script>
import { listCardRoute, getCardRoute, addCardRoute,  editCardRoute, exportcardRoute,delCardRoute } from "@/api/yunze/API/Channel";
import { treeselect } from "@/api/system/dept";
import Treeselect from "@riophae/vue-treeselect";
import "@riophae/vue-treeselect/dist/vue-treeselect.css";
import tools from "@/utils/yunze/tools";



export default {
  name: "Channel",
  components: { Treeselect },
  data() {
    return {
      //保存 编辑 按钮
      cardRouteAdd: false,
      cardRouteEdie: false,
      tools:tools,
      deptCheckStrictly: true,
      deptExpand: true,
      deptNodeAll: false,
      show_ds: false,//详情查看
      table_Online: [],//在线状态信息
      DeptOptions: [],// 公司信息
      customize_whether: [],//自定义是否
      route_algorithm: [],//通道算法类型
      mainwidth:24,//宽度
      option_show:false, //公司所属查询
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
      // 连接管理时间查询类型 数据字典
      timetypeOptions: [],
      // 简要套餐名称
      packageOptions: [],

      //详情
      tablefrom: [],

      FlowData : [],
      selIccid:[{FlowData:''}],
      activeNames:[0],

      //卡状态
      status_type: "",
      // 卡状态 字典
      stateOptions: [
        {
          dictLabel:''
        }
      ],
      // 运营商类别 字典
      operators_type: [],
      // 通道状态 字典
      channelStatusOptions: [],
      // 通道编码 字典
      channelCodeOptions: [],
      // 表单参数
      form: {},
      defaultProps: {
        children: "children",
        label: "label"
      },
      //当前查询参数
      sel:{
        iccid:'',
        agent_id:'',
        package_id:'',
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
        { key: 0, label: `通道编号`, visible: true },
        { key: 1, label: `通道名称`, visible: true },
        { key: 2, label: `通道别名`, visible: true },
        { key: 3, label: `运营类型`, visible: true },
        { key: 4, label: `通道状态`, visible: true },
        { key: 5, label: `是否轮训`, visible: true },
        { key: 6, label: `创建时间`, visible: true },

      ],
      // 表单校验
      rules: {
      }
    };
  },
  watch: {
    // 根据名称筛选部门树
    deptName(val) {
      //console.log(val);
      this.$refs.dept.filter(val);

    }
  },

  created() {
    this.getTreeselect();
    this.getList();



    //加载 查询条件
    if(window['channel_selType']!=undefined &&  window['channel_selType']!=null && window['channel_selType']!=''){
      this.typeOptions = window['channel_selType'];
    }else{
      this.getDicts("yunze_channel_selType").then(response => {
        window['channel_selType'] = response.data;
        this.typeOptions = window['channel_selType'];
      });
    }




    //加载 运营商类型
    if(window['operators_type']!=undefined &&  window['operators_type']!=null && window['operators_type']!=''){
      this.operators_type = window['operators_type'];
    }else{
      this.getDicts("yunze_card_operators_type").then(response => {
        window['operators_type'] = response.data;
        this.operators_type = window['operators_type'];
      });
    }



    //加载 自定义 是否 0 1 否 是
    if(window['customize_whether']!=undefined &&  window['customize_whether']!=null && window['customize_whether']!=''){
      this.customize_whether = window['customize_whether'];
    }else{
      this.getDicts("yunze_customize_whether").then(response => {
        window['customize_whether'] = response.data;
        this.customize_whether = window['customize_whether'];
      });
    }


    //加载 通道轮询算法
    if (window['route_algorithm'] != undefined && window['route_algorithm'] != null && window['route_algorithm'] != '') {
      this.route_algorithm = window['route_algorithm'];
    } else {
      this.getDicts("yz_route_algorithm").then(response => {
        window['route_algorithm'] = response.data;
        this.route_algorithm = window['route_algorithm'];
      });
    }


    //加载 通道状态
    if(window['channelStatusOptions']!=undefined &&  window['channelStatusOptions']!=null && window['channelStatusOptions']!=''){
      this.channelStatusOptions = window['channelStatusOptions'];
    }else{
      this.getDicts("channel_status").then(response => {
        window['channelStatusOptions'] = response.data;
        this.channelStatusOptions = window['channelStatusOptions'];
      });
    }


    //加载 通道编码
    if(window['channelCodeOptions']!=undefined &&  window['channelCodeOptions']!=null && window['channelCodeOptions']!=''){
      this.channelCodeOptions = window['channelCodeOptions'];
    }else{
      this.getDicts("channel_code").then(response => {
        window['channelCodeOptions'] = response.data;
        this.channelCodeOptions = window['channelCodeOptions'];
      });
    }



    /*this.getDicts("sys_user_sex").then(response => {
      this.sexOptions = response.data;
    });
    this.getConfigKey("sys.user.initPassword").then(response => {
      this.initPassword = response.msg;
    });*/
  },
  methods: {


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
        this.$refs.dept.setCheckedNodes(value ? this.deptOptions: []);
      }
    },
    // 树权限（父子联动）
    handleCheckedTreeConnect(value, type) {
      if (type == 'menu') {
        this.menuCheckStrictly = value ? true: false;
      } else if (type == 'dept') {
        this.$refs.dept.setCheckedNodes(false ? this.deptOptions: []);
        this.deptCheckStrictly = value ? true: false;

      }
    },


    /*获取查询参数*/
    getParams(){

      if(this.$refs.dept!=undefined & this.$refs.dept!=null & this.$refs.dept!='undefined' ){
        //console.log(this.$refs.dept.getCheckedKeys());
        if(this.$refs.dept.getCheckedKeys().length>0){
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
      //console.log(Pwd_Str);
      listCardRoute(Pwd_Str).then(response => {
          let jsonobj =  JSON.parse(tools.Decrypt(response));
          //console.log(jsonobj);
          if(jsonobj.code==200){
            this.cardList = jsonobj.Data.Data;
            this.total = jsonobj.Data.Pu.rowCount;
          }else{
             if(jsonobj.deptId=="100"){
            this.msgError(jsonobj.msg);
          }else{
            this.msgError("网络异常");
          }
          }
          this.loading = false;
        }
      );
    },
    /*代理查询拓展*/
    agentShow(){
      let _this = this;
      _this.option_show=!_this.option_show;
      _this.mainwidth = _this.option_show?20:24;
    },
    /** 查询部门下拉树结构 */
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
    // 节点单击事件
    handleNodeClick(data) {
      //console.log(data);
      this.getList();
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
      this.queryParams.pageNum= 1;
      this.queryParams.pageSize= 10;
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
      //初始化 新增 数据
      /* this.form.cd_code= "YiDong_EC";
       this.form.cd_operator_type= "1";
       this.form.cd_lunxun= "1";
       this.form.cd_status= "1";*/
      this.show_ds = true;
      this.cardRouteAdd = true;
      this.cardRouteEdie = false;
    },
    /** 修改按钮操作 */
    handleUpdate(row) {
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
    finddata(row){
      this.form = {};//清空数据
      let map ={};
      map.cd_id = row.cd_id;
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
      getCardRoute(Pwd_Str).then(response => {
          let jsonobj =  JSON.parse(tools.Decrypt(response));
          //console.log(jsonobj);
          if(jsonobj.code==200){

            jsonobj.Data.cd_algorithm = ''+jsonobj.Data.cd_algorithm;
            this.form = jsonobj.Data;

            this.show_ds = true;
          }else{
             if(jsonobj.deptId=="100"){
            this.msgError(jsonobj.msg);
          }else{
            this.msgError("网络异常");
          }
          }
          this.loading = false;

        }
      );

    },


    //新增
    Add(){
      let _this = this;
      if (tools.VerificationsText(_this, _this.form.cd_code, "通道编号不能为空！") == true &&
        tools.VerificationsText(_this, _this.form.cd_name, "通道名称不能为空！") == true &&
        tools.VerificationsText(_this, _this.form.cd_username, "通道注册用户名不能为空！") == true &&
        tools.VerificationsText(_this, _this.form.cd_pwd, "通道用户登录密码不能为空！") == true &&
        tools.VerificationsText(_this, _this.form.cd_key, "通道key不能为空！") == true &&
        tools.VerificationsText(_this, _this.form.cd_operator_type, "请选择运营商类型！") == true &&
        tools.VerificationsText(_this, _this.form.cd_lunxun, "请选择是否轮训！") == true &&
        tools.VerificationsText(_this, _this.form.cd_algorithm, "请选择 算法类型！") == true &&

        tools.VerificationsText(_this, _this.form.cd_status, "请选择通道状态！") == true ) {
        let Pwd_Str = tools.encrypt(JSON.stringify(this.form));
        addCardRoute(Pwd_Str).then(response => {
          let jsonobj = JSON.parse(tools.Decrypt(response));
          //console.log(jsonobj);
          if (jsonobj.code == 200) {
            this.msgSuccess(jsonobj.msg);
            this.show_ds = false;
            //更新数据
            this.queryParams.page = 1;
            this.getList();

          }else{
            let msg = jsonobj.Data!=null && jsonobj.Data!=''?jsonobj.Data:jsonobj.msg;
            this.msgError(msg);
          }
        })
      }
    },
    //编辑
    Edie(){

      let _this = this;
      if (tools.VerificationsText(_this, _this.form.cd_code, "通道编号不能为空！") == true &&
        tools.VerificationsText(_this, _this.form.cd_name, "通道名称不能为空！") == true &&
        tools.VerificationsText(_this, _this.form.cd_username, "通道注册用户名不能为空！") == true &&
        tools.VerificationsText(_this, _this.form.cd_pwd, "通道用户登录密码不能为空！") == true &&
        tools.VerificationsText(_this, _this.form.cd_key, "通道key不能为空！") == true &&
        tools.VerificationsText(_this, _this.form.cd_operator_type, "请选择运营商类型！") == true &&
        tools.VerificationsText(_this, _this.form.cd_lunxun, "请选择是否轮训！") == true &&
        tools.VerificationsText(_this, _this.form.cd_id, "操作参数缺失刷新后重试！") == true &&
        tools.VerificationsText(_this, _this.form.cd_algorithm, "请选择 算法类型！") == true &&

        tools.VerificationsText(_this, _this.form.cd_status, "请选择通道状态！") == true ) {
        let Pwd_Str = tools.encrypt(JSON.stringify(this.form));
        editCardRoute(Pwd_Str).then(response => {
          let jsonobj = JSON.parse(tools.Decrypt(response));
          //console.log(jsonobj);
          if (jsonobj.code == 200) {
            this.msgSuccess(jsonobj.msg);
            this.show_ds = false;
            //更新数据
            this.queryParams.page = 1;
            this.getList();

          }else{
            let msg = jsonobj.Data!=null && jsonobj.Data!=''?jsonobj.Data:jsonobj.msg;
            this.msgError(msg);
          }
        })
      }

    },


    /** 删除按钮操作 */
    handleDelete(row) {
      let map = {};
      map.cd_id = row.cd_id;
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
      tools.openAsk(this,'warning', "确定要删除通道名称为 '"+row.cd_name+"' 的这条数据吗？", this.delRouteFun, Pwd_Str);

    },
    //删除通道
    delRouteFun(Pwd_Str){
      delCardRoute(Pwd_Str).then(response => {
        let jsonobj = JSON.parse(tools.Decrypt(response));
        //console.log(jsonobj);
        if (jsonobj.code == 200) {
          this.msgSuccess(jsonobj.msg);
          this.getList();
        }else{
          let msg = jsonobj.Data!=null && jsonobj.Data!=''?jsonobj.Data:jsonobj.msg;
          this.msgError(msg);
        }
      })
    },




    /** 导出按钮操作 */
    handleExport() {

      this.getParams();
      let Pwd_Str = tools.encrypt(JSON.stringify(this.queryParams));
      exportcardRoute(Pwd_Str).then(response => {
        let jsonobj = JSON.parse(tools.Decrypt(response));
        //console.log(jsonobj);
        if (jsonobj.code == 200) {
          this.msgSuccess(jsonobj.Data);
        }else{
          let msg = jsonobj.Data!=null && jsonobj.Data!=''?jsonobj.Data:jsonobj.msg;
          this.msgError(msg);
        }

      })

    },
    /** 导入按钮操作 */
    handleImport() {
      this.upload.title = "用户导入";
      this.upload.open = true;
    },


  }
};
</script>

<style scoped>

.el-table .cell{
  line-height: 17px;
  padding-left: 10px;
  padding-right: 0px;
}

.el-form-item {
  margin-bottom: 10px;
}

.my_checkbox__inner{
  border: 1px;
}

</style>

