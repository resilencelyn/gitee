<template>
  <div class="app-container">
    <el-row :gutter="20">
      <!--企业数据-->
      <el-col :span="4" :xs="24" v-show="option_show">
        <div class="head-container">
          <el-input
            v-model="dept_name"
            placeholder="请输入企业名称"
            clearable
            size="small"
            prefix-icon="el-icon-search"
            style="margin-bottom: 20px"
          />

        </div>
        <div class="head-container">
          <el-tree
            :data="deptOptions"
            :props="defaultProps"
            :expand-on-click-node="false"
            :filter-node-method="filterNode"
            ref="tree"
            @node-click="handleNodeClick"
            default-expand-all
          />
        </div>
      </el-col>
      <!--用户数据-->
      <el-col :span="mainwidth" :xs="24">
          <el-form :model="queryParams" ref="queryForm" :inline="true" v-show="showSearch"  style="margin-bottom: -10px;">
            <el-form-item label="状态" prop="status">
              <el-select
                v-model="queryParams.status"
                placeholder="用户状态"
                clearable
                size="small"
                style="width: 240px"
              >
                <el-option
                  v-for="dict in statusOptions"
                  :key="dict.dictValue"
                  :label="dict.dictLabel"
                  :value="dict.dictValue"
                />
              </el-select>
            </el-form-item>
            <el-form-item label="创建时间">
              <el-date-picker
                v-model="selTime"
                type="datetimerange"
                :picker-options="pickerOptions"
                range-separator="至"
                value-format="yyyy-MM-dd"
                start-placeholder="开始日期"
                end-placeholder="结束日期"
                style="width: 360px"
                align="right">
              </el-date-picker>
            </el-form-item>
          </el-form >

          <el-form :model="queryParams" ref="queryForm" :inline="true"  style="margin-bottom: 10px;">
            <el-col :span="1.5">
              <el-input
                v-model="queryParams.value"
                placeholder="查询值"
                clearable
                size="small"
                style="width: 350px;"
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
                v-hasPermi="['yunze:agent:add']"
              >新增
              </el-button>
            </el-col>
            <!--<el-col :span="1.5">
              <el-button
                type="success"
                plain
                icon="el-icon-edit"
                size="mini"
                :disabled="single"
                @click="handleUpdate"
                v-hasPermi="['yunze:agent:edit']"
              >修改
              </el-button>
            </el-col>-->
            <el-col :span="1.5">
              <el-button
                type="danger"
                plain
                icon="el-icon-delete"
                size="mini"
                :disabled="multiple"
                @click="handleDelete"
                v-hasPermi="['yunze:agent:remove']"
              >删除
              </el-button>
            </el-col>
            <el-col :span="1.5">
              <el-button
                type="info"
                plain
                icon="el-icon-upload2"
                size="mini"
                @click="handleImport"
                v-hasPermi="['yunze:agent:import']"
              >导入
              </el-button>
            </el-col>
            <el-col :span="1.5">
              <el-button
                type="warning"
                plain
                icon="el-icon-download"
                size="mini"
                :loading="exportLoading"
                @click="handleExport"
                v-hasPermi="['yunze:agent:export']"
              >导出
              </el-button>
            </el-col>
            <el-col :span="1.5">
              <el-button size="mini" v-hasPermi="['yunze:sys:internal']" @click="agentShow">公司所属</el-button>
            </el-col>
            <right-toolbar :showSearch.sync="showSearch" @queryTable="getList" :columns="columns"></right-toolbar>
          </el-form>


            <el-table v-loading="loading" :data="userList" @selection-change="handleSelectionChange" >
              <!--<el-table-column type="selection" width="50" align="center"/>-->
              <el-table-column label="用户编号" align="center" key="user_id" prop="user_id" v-if="columns[0].visible"/>
              <el-table-column label="用户账号" align="center" key="user_name" prop="user_name" v-if="columns[1].visible"
                               :show-overflow-tooltip="true"/>
              <el-table-column label="用户昵称" align="center" key="nick_name" prop="nick_name" v-if="columns[2].visible"
                               :show-overflow-tooltip="true"/>
              <el-table-column label="企业" align="center" key="dept_name" prop="dept_name" v-if="columns[3].visible"
                               :show-overflow-tooltip="true"/>
              <el-table-column label="手机号码" align="center" key="phonenumber" prop="phonenumber" v-if="columns[4].visible"
                               width="120"/>
              <el-table-column label="状态" align="center" key="status" v-if="columns[5].visible">
                <template slot-scope="scope">
                  <el-switch
                    v-model="scope.row.status"
                    active-value="0"
                    inactive-value="1"
                    @change="handleStatusChange(scope.row)"
                  ></el-switch>
                </template>
              </el-table-column>
              <el-table-column label="创建时间" align="center" prop="create_time" v-if="columns[6].visible" width="160">
                <template slot-scope="scope">
                  <span>{{ parseTime(scope.row.create_time) }}</span>
                </template>
              </el-table-column>
              <el-table-column
                label="操作"
                align="center"
                width="160"
                class-name="small-padding fixed-width"
              >
                <template slot-scope="scope" v-if="scope.row.user_name !=='admin' ">
                  <el-dropdown >
                    <el-button type="primary" size="small">
                      操作<i class="el-icon-arrow-down el-icon--right"></i>
                    </el-button>
                    <el-dropdown-menu slot="dropdown">
                      <div v-if="scope.row.user_name !== tools.getCookie('username')">
                        <el-dropdown-item v-hasPermi="['yunze:agent:edit']"   @click.native="handleUpdate(scope.row)">修改</el-dropdown-item>
                        <el-dropdown-item v-hasPermi="['yunze:agent:remove']"  v-if="scope.row.user_name !== tools.getCookie('username')"  @click.native="handleDelete(scope.row)">删除</el-dropdown-item>
                        <el-dropdown-item v-hasPermi="['yunze:agent:resetPwd']"  @click.native="handleResetPwd(scope.row)">密码重置</el-dropdown-item>
                      </div>
                      <div v-hasPermi="['yunze:sys:internal']">
                        <el-dropdown-item v-hasPermi="['yunze:sys:DividePlan']"  v-if="scope.row.dept_id !== 100 && scope.row.user_name !== tools.getCookie('username')" @click.native="handleDividePlan(scope.row,true)">平台划分资费</el-dropdown-item>
                      </div>
                      <el-dropdown-item v-hasPermi="['yunze:agent:DividePlan']"  v-if="scope.row.dept_id !== 100 && scope.row.user_name !== tools.getCookie('username')" @click.native="handleDividePlan(scope.row,false)">企业划分资费</el-dropdown-item>
                      <el-dropdown-item v-hasPermi="['yunze:XsglCustomer:add']" v-if="scope.row.dept_id !== 100 && scope.row.user_name !== tools.getCookie('username')" >
                        <router-link
                          :to="'/customer/xsgl/customer?str='+tools.encrypt(JSON.stringify({'dept_id':scope.row.dept_id,'dept_name':scope.row.dept_name})) " class="link-type">
                          <span>新增客户</span>
                        </router-link>
                      </el-dropdown-item>
                      <el-dropdown-item v-hasPermi="['yunze:card:diagnosis']" v-if="scope.row.dept_id !== 100 " >
                        <router-link :to="'/system/deptPrestore/'+ tools.encrypt(JSON.stringify({'deptName':scope.row.dept_name,'deptId':scope.row.dept_id}))" class="link-type">
                          <span>企业预存</span>
                        </router-link>
                      </el-dropdown-item>
                    </el-dropdown-menu>
                  </el-dropdown>
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

    <!-- 添加或修改参数配置对话框 -->
    <el-dialog :title="title" :visible.sync="open" width="600px" append-to-body>
      <el-form ref="form" :model="form" :rules="rules" label-width="80px">
        <el-row>
          <el-col :span="12">
            <el-form-item label="用户昵称" prop="nickName">
              <el-input v-model="form.nickName" placeholder="如 云则费总"/>
            </el-form-item>
          </el-col>
          <el-col :span="12">
            <el-form-item label="归属企业" prop="deptId">
              <treeselect v-model="form.deptId" :options="deptOptions" :show-count="true" placeholder="请选择归属企业"/>
            </el-form-item>
          </el-col>
        </el-row>
        <el-row>
          <el-col :span="12">
            <el-form-item label="手机号码" prop="phonenumber">
              <el-input v-model="form.phonenumber" placeholder="请输入手机号码" maxlength="11"/>
            </el-form-item>
          </el-col>
          <el-col :span="12">
            <el-form-item label="邮箱" prop="email">
              <el-input v-model="form.email" placeholder="请输入邮箱" maxlength="50"/>
            </el-form-item>
          </el-col>
        </el-row>
        <el-row>
          <el-col :span="12">
            <el-form-item v-if="form.userId == undefined" label="用户账号" prop="userName">
              <el-input v-model="form.userName" placeholder="yunzefeizong"/>
            </el-form-item>
          </el-col>
          <el-col :span="12">
            <el-form-item v-if="form.userId == undefined" label="用户密码" prop="password">
              <el-input v-model="form.password" placeholder="请输入用户密码" type="password"/>
            </el-form-item>
          </el-col>
        </el-row>
        <el-row>
          <el-col :span="12">
            <el-form-item label="用户性别">
              <el-select v-model="form.sex" placeholder="请选择">
                <el-option
                  v-for="dict in sexOptions"
                  :key="dict.dictValue"
                  :label="dict.dictLabel"
                  :value="dict.dictValue"
                ></el-option>
              </el-select>
            </el-form-item>
          </el-col>
          <el-col :span="12">
            <el-form-item label="状态">
              <el-radio-group v-model="form.status">
                <el-radio
                  v-for="dict in statusOptions"
                  :key="dict.dictValue"
                  :label="dict.dictValue"
                >{{ dict.dictLabel }}
                </el-radio>
              </el-radio-group>
            </el-form-item>
          </el-col>
        </el-row>
        <el-row>
          <el-col :span="12">
            <el-form-item label="岗位">
              <el-select v-model="form.postIds" multiple placeholder="请选择">
                <el-option
                  v-for="item in postOptions"
                  :key="item.postId"
                  :label="item.postName"
                  :value="item.postId"
                  :disabled="item.status == 1"
                ></el-option>
              </el-select>
            </el-form-item>
          </el-col>
          <el-col :span="12">
            <el-form-item label="角色" prop="roleIds">
              <el-select v-model="form.roleIds" multiple placeholder="请选择">
                <el-option
                  v-for="item in roleOptions"
                  :key="item.roleId"
                  :label="item.roleName"
                  :value="item.roleId"
                  :disabled="item.status == 1"
                ></el-option>
              </el-select>
            </el-form-item>
          </el-col>
        </el-row>
        <el-row>
          <el-col :span="24">
            <el-form-item label="备注">
              <el-input v-model="form.remark" type="textarea" placeholder="请输入内容"></el-input>
            </el-form-item>
          </el-col>
        </el-row>
      </el-form>
      <div slot="footer" class="dialog-footer">
        <el-button type="primary" @click="submitForm">确 定</el-button>
        <el-button @click="cancel">取 消</el-button>
      </div>
    </el-dialog>

    <!-- 用户导入对话框 -->
    <el-dialog :title="upload.title" :visible.sync="upload.open" width="400px" append-to-body>
      <el-upload
        ref="upload"
        :limit="1"
        accept=".xlsx, .xls"
        :headers="upload.headers"
        :action="upload.url + '?updateSupport=' + upload.updateSupport"
        :disabled="upload.isUploading"
        :on-progress="handleFileUploadProgress"
        :on-success="handleFileSuccess"
        :auto-upload="false"
        drag
      >
        <i class="el-icon-upload"></i>
        <div class="el-upload__text">
          将文件拖到此处，或
          <em>点击上传</em>
        </div>
        <div class="el-upload__tip" slot="tip">
          <el-checkbox v-model="upload.updateSupport"/>
          是否更新已经存在的用户数据
          <el-link type="info" style="font-size:12px" @click="importTemplate">下载模板</el-link>
        </div>
        <div class="el-upload__tip" style="color:red" slot="tip">提示：仅允许导入“xls”或“xlsx”格式文件！</div>
      </el-upload>
      <div slot="footer" class="dialog-footer">
        <el-button type="primary" @click="submitFileForm">确 定</el-button>
        <el-button @click="upload.open = false">取 消</el-button>
      </div>
    </el-dialog>


    <!-- 划分资费计划 -->
    <el-dialog title="划分资费计划" :visible.sync="showPlan_ds" width="80%" append-to-body>

      <el-form ref="Planform" :model="Planform" label-width="100px">
        <el-row>
          <el-col :span="24">
            <el-form-item label="资费组" prop="cd_code">
              <el-select v-model="Planform.package_id" @change="packageIdChange" placeholder="请选择">
                <el-option
                  v-for="dict in packageOptions"
                  :key="dict.package_id"
                  :label="dict.package_agentname"
                  :value="dict.package_id"

                />
              </el-select>
            </el-form-item>
          </el-col>
        </el-row>
        <el-input v-model="Planform.dept_id" v-if="false"/>
        <el-input v-model="Planform.user_id" v-if="false"/>
      </el-form>

      <el-table
        :data="tariffPlanList"
      >
        <el-table-column label="编号" prop="packet_id"/>
        <el-table-column label="名称" prop="packet_name"/>
        <el-table-column label="别称" prop="packet_wx_name"/>

        <el-table-column label="成本" prop="packet_cost"/>
        <el-table-column label="售价" prop="packet_price"/>
        <el-table-column label="流量(MB)" prop="packet_flow"/>
        <el-table-column label="已划分" align="center" key="IsitDivided">
          <template slot-scope="scope">
            {{ tools.getDkeyValue(customize_whether, "" + scope.row.IsitDivided) }}
          </template>
        </el-table-column>
        <el-table-column min-width="100" prop="packet_cost" :width="90" label="客户成本">
          <template slot-scope="scope">
            <el-input v-model="scope.row.set_packet_cost" @keydown.native="tools.handleInput2" placeholder="成本价"/>
          </template>
        </el-table-column>

        <el-table-column min-width="100" prop="set_packet_price" :width="90" label="客户售价">
          <template slot-scope="scope">
            <el-input v-model="scope.row.set_packet_price" @keydown.native="tools.handleInput2" placeholder="售价"/>
          </template>
        </el-table-column>
        <el-table-column min-width="100" prop="errorTime" label="系数" v-if="showSo" >
          <template slot-scope="scope">
                 <el-input v-model="scope.row.set_error_so" @keydown.native="tools.handleInput2"  placeholder="系数" />
          </template>
        </el-table-column>
        <el-table-column label="已设置用量(MB)" v-if="error_flowShow" prop="error_flow"/>
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
              icon="el-icon-setting"
              @click="sysHandletariffDivision(scope.row)"
              v-hasPermi="['yunze:tariffGroup:tariffDivision']"
            >平台划分</el-button>

            <el-button
              size="mini"
              type="text"
              icon="el-icon-edit"
              @click="handletariffDivision(scope.row)"
              v-hasPermi="['yunze:tariffGroup:agentTariffDivision']"
            >企业划分
            </el-button>

          </template>
        </el-table-column>
      </el-table>


    </el-dialog>


  </div>
</template>
<script>
import {
  listUser,
  delUser,
  addUser,
  updateUser,
  exportUser,
  resetUserPwd,
  changeUserStatus,
  importTemplate,
  findPackage,
  findPacket,
  agentTariffDivision,
  tariffDivision
} from "@/api/yunze/user/agent";

import {
  getUser,
}from "@/api/system/user";
import {getToken} from "@/utils/auth";
import {treeselect} from "@/api/system/dept";
import Treeselect from "@riophae/vue-treeselect";
import "@riophae/vue-treeselect/dist/vue-treeselect.css";
import tools from "@/utils/yunze/tools";

export default {
  name: "agentUser",
  components: {Treeselect},
  data() {
    return {
      activeName:'allUser',
      showSo:false,
      Planform: {},
      showPlan_ds: false,//划分 资费计划
      mainwidth: 24,//宽度
      option_show:false, //公司所属查询
      tools: tools,
      tariffPlanList: [],//划分资费计划 List
      packageOptions: [],//资费组
      customize_whether: [],//自定义是否
      error_flowShow: false,//是否显示 已经划分 设置好的用量
      typeOptions: [],//查询条件
      pickerOptions: {
        shortcuts: [{
          text: '最近一周',
          onClick(picker) {
            const end = new Date();
            const start = new Date();
            start.setTime(start.getTime() - 3600 * 1000 * 24 * 7);
            picker.$emit('pick', [start, end]);
          }
        }, {
          text: '最近一个月',
          onClick(picker) {
            const end = new Date();
            const start = new Date();
            start.setTime(start.getTime() - 3600 * 1000 * 24 * 30);
            picker.$emit('pick', [start, end]);
          }
        }, {
          text: '最近三个月',
          onClick(picker) {
            const end = new Date();
            const start = new Date();
            start.setTime(start.getTime() - 3600 * 1000 * 24 * 90);
            picker.$emit('pick', [start, end]);
          }
        }]
      },

      selTime: '',//时间选择
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
      userList: null,
      // 弹出层标题
      title: "",
      // 企业树选项
      deptOptions: undefined,
      // 是否显示弹出层
      open: false,
      // 企业名称
      dept_name: undefined,
      // 默认密码
      initPassword: undefined,
      // 日期范围
      dateRange: [],
      // 状态数据字典
      statusOptions: [],
      // 性别状态字典
      sexOptions: [],
      // 岗位选项
      postOptions: [],
      // 角色选项
      roleOptions: [],
      // 表单参数
      form: {},
      defaultProps: {
        children: "children",
        label: "label"
      },
      // 用户导入参数
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
        headers: {Authorization: "Bearer " + getToken()},
        // 上传的地址
        url: process.env.VUE_APP_BASE_API + "/system/user/importData"
      },
      // 查询参数
      queryParams: {
        pageNum: 1,
        pageSize: 10,
        user_name: undefined,
        staTime: null,
        endTime: null,
        timetype: null,
        phonenumber: undefined,
        status: undefined,
        deptId: undefined
      },
      // 列信息
      columns: [
        {key: 0, label: `用户编号`, visible: true},
        {key: 1, label: `用户账号`, visible: true},
        {key: 2, label: `用户昵称`, visible: true},
        {key: 3, label: `企业`, visible: true},
        {key: 4, label: `手机号码`, visible: true},
        {key: 5, label: `状态`, visible: true},
        {key: 6, label: `创建时间`, visible: true}
      ],
      // 表单校验
      rules: {
        userName: [
          {required: true, message: "用户账号不能为空", trigger: "blur"}
        ],
        nickName: [
          {required: true, message: "用户昵称不能为空", trigger: "blur"}
        ],
        password: [
          {required: true, message: "用户密码不能为空", trigger: "blur"}
        ],
        roleIds: [
          {required: true, message: "请选择所属角色", trigger: "blur"}
        ],
        deptId: [
          {required: true, message: "请选择所属企业", trigger: "blur"}
        ],


        email: [
          {
            type: "email",
            message: "'请输入正确的邮箱地址",
            trigger: ["blur", "change"]
          }
        ],
        phonenumber: [
          {
            pattern: /^1[3|4|5|6|7|8|9][0-9]\d{8}$/,
            message: "请输入正确的手机号码",
            trigger: "blur"
          }
        ]
      }
    };
  },
  watch: {
    // 根据名称筛选企业树
    dept_name(val) {
      this.$refs.tree.filter(val);
    }
  },
  created() {

    this.getDicts("sys_normal_disable").then(response => {
      this.statusOptions = response.data;
    });
    //加载 系统 性别
    if (window['SexTypeOption'] != undefined && window['SexTypeOption'] != null && window['SexTypeOption'] != '') {
       this.sexOptions = window['SexTypeOption'];
    } else {
      this.getDicts("sys_user_sex").then(response => {
        window['SexTypeOption'] = response.data;
        this.sexOptions = window['SexTypeOption'];
      });
    }
    this.getConfigKey("sys.user.initPassword").then(response => {
      this.initPassword = response.msg;
    });

    //加载 查询条件
    if (window['sysUser_selType'] != undefined && window['sysUser_selType'] != null && window['sysUser_selType'] != '') {
      this.typeOptions = window['sysUser_selType'];
    } else {
      this.getDicts("yz_sysUser_selType").then(response => {
        window['sysUser_selType'] = response.data;
        this.typeOptions = window['sysUser_selType'];
      });
    }


    //加载 自定义 是否 0 1 否 是
    if (window['customize_whether'] != undefined && window['customize_whether'] != null && window['customize_whether'] != '') {
      this.customize_whether = window['customize_whether'];
    } else {
      this.getDicts("yunze_customize_whether").then(response => {
        window['customize_whether'] = response.data;
        this.customize_whether = window['customize_whether'];
      });
    }




    this.getTreeselect();
    this.getList();

  },

  methods: {

    /*代理查询拓展*/
    agentShow() {
      let _this = this;
      _this.option_show = !_this.option_show;
      _this.mainwidth = _this.option_show ? 20 : 24;
    },

    userHandleClick(){

    },

    /*获取查询参数*/
    getParams() {
      this.queryParams.staTime = null;
      this.queryParams.endTime = null;
      if (this.selTime != null) {
        //console.log(this.selTime);
        this.queryParams.staTime = this.selTime[0];
        this.queryParams.endTime = this.selTime[1];
      }
    },
    /** 查询用户列表 */
    getList() {
      this.loading = true;
      this.getParams();
      let Pwd_Str = tools.encrypt(JSON.stringify(this.queryParams));
      //console.log(Pwd_Str);
      listUser(Pwd_Str).then(response => {
          let jsonobj = JSON.parse(tools.Decrypt(response));
          //console.log(jsonobj);
          if (jsonobj.code == 200) {
            this.userList = jsonobj.Data.Data;
            this.total = jsonobj.Data.Pu.rowCount;
          } else {
            this.msgError("获取数据异常，请联系管理员！");
          }
          this.loading = false;
        }
      );
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
    // 节点单击事件
    handleNodeClick(data) {
      this.queryParams.deptId = data.id;
      this.getList();
    },
    // 用户状态修改
    handleStatusChange(row) {
      let text = row.status === "0" ? "启用" : "停用";
      this.$confirm('确认要"' + text + '""' + row.user_name + '"用户吗?', "警告", {
        confirmButtonText: "确定",
        cancelButtonText: "取消",
        type: "warning"
      }).then(function () {
        return changeUserStatus(row.user_id, row.status);
      }).then(() => {
        this.msgSuccess(text + "成功");
      }).catch(function () {
        row.status = row.status === "0" ? "1" : "0";
      });
    },
    // 取消按钮
    cancel() {
      this.open = false;
      this.reset();
    },
    // 表单重置
    reset() {
      this.form = {
        user_id: undefined,
        deptId: undefined,
        user_name: undefined,
        nick_name: undefined,
        password: undefined,
        phonenumber: undefined,
        email: undefined,
        sex: undefined,
        status: "0",
        remark: undefined,
        postIds: [],
        roleIds: []
      };
      this.resetForm("form");
    },
    /** 搜索按钮操作 */
    handleQuery() {
      this.queryParams.page = 1;
      this.getList();
    },
    /** 重置按钮操作 */
    resetQuery() {
      this.dateRange = [];
      this.resetForm("queryForm");
      this.handleQuery();
    },
    // 多选框选中数据
    handleSelectionChange(selection) {
      this.ids = selection.map(item => item.user_id);
      this.single = selection.length != 1;
      this.multiple = !selection.length;
    },
    /** 新增按钮操作 */
    handleAdd() {
      this.reset();
      this.getTreeselect();
      getUser().then(response => {
        console.log(response)
        this.postOptions = response.posts;
        this.roleOptions = response.roles;
        this.open = true;
        this.title = "添加用户";
        this.form.password = this.initPassword;
      });
    },
    /** 修改按钮操作 */
    handleUpdate(row) {
      this.reset();
      this.getTreeselect();
      const user_id = row.user_id || this.ids;
      getUser(user_id).then(response => {
        this.form = response.data;
        this.postOptions = response.posts;
        this.roleOptions = response.roles;
        this.form.postIds = response.postIds;
        this.form.roleIds = response.roleIds;
        this.open = true;
        this.title = "修改用户";
        this.form.password = "";
      });
    },
    /** 重置密码按钮操作 */
    handleResetPwd(row) {
      this.$prompt('请输入"' + row.user_name + '"的新密码', "提示", {
        confirmButtonText: "确定",
        cancelButtonText: "取消"
      }).then(({value}) => {
        resetUserPwd(row.user_id, value).then(response => {
          this.msgSuccess("修改成功，新密码是：" + value);
        });
      }).catch(() => {
      });
    },

    /** 划分资费 按钮操作 */
    handleDividePlan(row,showSo) {
      this.showSo = showSo;
      console.log(showSo);
      this.packageOptions = [];
      this.tariffPlanList = [];
      this.Planform = {};
      this.Planform.dept_id = row.dept_id;
      this.Planform.user_id = row.user_id;
      //console.log(row);
      this.showPlan_ds = true;
      let map = {};
      //map.agent_id = ['100'];
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
      findPackage(Pwd_Str).then(response => {
          let jsonobj = JSON.parse(tools.Decrypt(response));
          //console.log(jsonobj);
          if (jsonobj.code == 200) {

            this.packageOptions = jsonobj.Data;
            this.showPlan_ds = true;
          } else {
            this.msgError(jsonobj.msg);
          }
          this.loading = false;

        }
      );


    },

    /**切换资费组 查询资费计划 */
    packageIdChange() {
      let package_id = this.Planform.package_id;
      if (tools.Is_null(package_id)) {
        this.tariffPlanList = [];
        let map = {};
        this.error_flowShow = false;
        map.findDept_id = this.Planform.dept_id;
        map.package_id = [package_id];
        let Pwd_Str = tools.encrypt(JSON.stringify(map));
        findPacket(Pwd_Str).then(response => {
            let jsonobj = JSON.parse(tools.Decrypt(response));
            //console.log(jsonobj);
            if (jsonobj.code == 200) {

              let arr = jsonobj.Data.Packet;
              let agentPacketarr = jsonobj.Data.agentPacket;
              var new_map = {},
                dest = [];
              if (agentPacketarr != null && agentPacketarr.length > 0 && arr != null && arr.length > 0) {
                for (var i = 0; i < arr.length; i++) {
                  var ai = arr[i];
                  for (let j = 0; j < agentPacketarr.length > 0; j++) {
                    var agentPacket = agentPacketarr[j];
                    if (ai.packet_id == agentPacket.packet_id) {
                      ai.set_packet_cost = agentPacket.set_packet_cost;
                      ai.set_error_so = agentPacket.set_error_so;
                      ai.set_packet_price = agentPacket.set_packet_price;
                      ai.IsitDivided = 1;
                      if (ai.error_flow != undefined && ai.error_flow != null && ai.error_flow != '') {
                        ai.error_flow = agentPacket.error_flow;
                        this.error_flowShow = true;
                      }
                      break;
                    } else {
                      ai.IsitDivided = 0;
                    }
                  }
                  dest.push(ai);
                }
                this.tariffPlanList = dest;
                this.showPlan_ds = true;
              } else {

                for (var i = 0; i < arr.length; i++) {
                  var ai = arr[i];
                  ai.IsitDivided = 0;
                  dest.push(ai);
                }

                this.tariffPlanList = dest;
                this.showPlan_ds = true;
              }
              //console.log(dest);
              //this.tariffPlanList = jsonobj.Data;

            } else {
              this.msgError(jsonobj.msg);
            }
            this.loading = false;

          }
        );
      }
    },



    //平台给企业划分资费
    sysHandletariffDivision(row){
      let _this = this;
      if (tools.VerificationsText(_this,row.set_error_so,"系数 不能为空！") == true &&
        tools.VerificationsText(_this, row.set_packet_cost, "成本 不能为空！") == true &&
        tools.VerificationsText(_this, row.set_packet_price, "售价 不能为空！") == true &&
        tools.VerificationsText(_this, _this.Planform.dept_id, "操作参数缺失刷新后重试！") == true &&
        tools.VerificationsText(_this, _this.Planform.user_id, "操作参数缺失刷新后重试！") == true ) {

        if(row.set_packet_cost>=row.packet_cost){
          if(row.set_packet_cost<=row.set_packet_price){
            let map = {};
            map.agent_id = this.Planform.dept_id;
            map.dept_id = this.Planform.dept_id;
            map.user_id = this.Planform.user_id;
            map.set_error_so = row.set_error_so;
            map.set_packet_cost = row.set_packet_cost;
            map.set_packet_price = row.set_packet_price;
            map.user_id = this.Planform.user_id;
            map.id = row.id;
            map.package_id = this.Planform.package_id;
            map.packet_id = row.packet_id;

            //console.log(map);
            let Pwd_Str = tools.encrypt(JSON.stringify(map));
            tariffDivision(Pwd_Str).then(response => {
                let jsonobj = JSON.parse(tools.Decrypt(response));
                //console.log(jsonobj);
                if (jsonobj.code == 200) {
                  let msg = jsonobj.Data != null && jsonobj.Data != '' ? jsonobj.Data : jsonobj.msg;
                  if(jsonobj.Data){
                    this.msgSuccess(jsonobj.msg);
                    this.packageIdChange();//刷新当前资费计划
                  }else{this.msgError(jsonobj.msg);}
                } else {
                  this.msgError(jsonobj.msg);
                }
                this.loading = false;
              }
            );
          }else{
            this.msgError("划分的成本不能大于划分的售价！");
          }
        }else{
          this.msgError("划分的成本不能小于当前成本！");
        }
      }
    },





    /** 划分资费保存 - 企业给 终端用户划分资费 */
    handletariffDivision(row) {
      //console.log(row);
      //console.log(this.Planform.dept_id);
      //console.log(this.Planform.user_id);
      let _this = this;
      if (tools.VerificationsText(_this, row.set_error_so, "操作参数缺失刷新后重试！") == true &&
        tools.VerificationsText(_this, row.set_packet_cost, "成本 不能为空！") == true &&
        tools.VerificationsText(_this, row.set_packet_price, "售价 不能为空！") == true &&
        tools.VerificationsText(_this, _this.Planform.dept_id, "操作参数缺失刷新后重试！") == true &&
        tools.VerificationsText(_this, _this.Planform.user_id, "操作参数缺失刷新后重试！") == true) {

        if (row.set_packet_cost >= row.packet_cost) {
          if (row.set_packet_cost <= row.set_packet_price) {
            let map = {};
            map.agent_id = this.Planform.dept_id;
            map.dept_id = this.Planform.dept_id;
            map.user_id = this.Planform.user_id;
            map.set_error_so = row.set_error_so;
            map.set_packet_cost = row.set_packet_cost;
            map.set_packet_price = row.set_packet_price;
            map.user_id = this.Planform.user_id;
            map.id = row.id;
            map.package_id = this.Planform.package_id;
            map.packet_id = row.packet_id;

            //console.log(map);
            let Pwd_Str = tools.encrypt(JSON.stringify(map));
            agentTariffDivision(Pwd_Str).then(response => {
                let jsonobj = JSON.parse(tools.Decrypt(response));
                //console.log(jsonobj);
                if (jsonobj.code == 200) {
                  let msg = jsonobj.Data != null && jsonobj.Data != '' ? jsonobj.Data : jsonobj.msg;
                  if (jsonobj.Data) {
                    this.msgSuccess(jsonobj.msg);
                    this.packageIdChange();//刷新当前资费计划
                  } else {
                    this.msgError(jsonobj.msg);
                  }
                } else {
                  this.msgError(jsonobj.msg);
                }
                this.loading = false;
              }
            );

          } else {
            this.msgError("划分的成本不能大于划分的售价！");
          }
        } else {
          this.msgError("划分的成本不能小于当前成本！");
        }
      }
    },


    /** 提交按钮 */
    submitForm: function () {
      this.$refs["form"].validate(valid => {
        if (valid) {
          if (this.form.userId != undefined) {
            updateUser(this.form).then(response => {
              this.msgSuccess("修改成功");
              this.open = false;
              this.getList();
            });
          } else {
            addUser(this.form).then(response => {
              this.msgSuccess("新增成功");
              this.open = false;
              this.getList();
            });
          }
        }
      });
    },
    /** 删除按钮操作 */
    handleDelete(row) {
      const user_ids = row.user_id || this.ids;
      this.$confirm('是否确认删除用户编号为"' + user_ids + '"的数据项?', "警告", {
        confirmButtonText: "确定",
        cancelButtonText: "取消",
        type: "warning"
      }).then(function () {
        return delUser(user_ids);
      }).then(() => {
        this.getList();
        this.msgSuccess("删除成功");
      }).catch(() => {
      });
    },
    /** 导出按钮操作 */
    handleExport() {
      const queryParams = this.queryParams;
      this.$confirm('是否确认导出所有用户数据项?', "警告", {
        confirmButtonText: "确定",
        cancelButtonText: "取消",
        type: "warning"
      }).then(() => {
        this.exportLoading = true;
        return exportUser(queryParams);
      }).then(response => {
        this.download(response.msg);
        this.exportLoading = false;
      }).catch(() => {
      });
    },
    /** 导入按钮操作 */
    handleImport() {
      this.upload.title = "用户导入";
      this.upload.open = true;
    },
    /** 下载模板操作 */
    importTemplate() {
      importTemplate().then(response => {
        this.download(response.msg);
      });
    },
    // 文件上传中处理
    handleFileUploadProgress(event, file, fileList) {
      this.upload.isUploading = true;
    },
    // 文件上传成功处理
    handleFileSuccess(response, file, fileList) {
      this.upload.open = false;
      this.upload.isUploading = false;
      this.$refs.upload.clearFiles();
      this.$alert(response.msg, "导入结果", {dangerouslyUseHTMLString: true});
      this.getList();
    },
    // 提交上传文件
    submitFileForm() {
      this.$refs.upload.submit();
    }
  }
};
</script>


