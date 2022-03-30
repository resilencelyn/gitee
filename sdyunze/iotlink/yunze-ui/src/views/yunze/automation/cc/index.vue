<template>
  <div class="app-container">
    <el-row :gutter="20">
      <!--用户数据-->
      <el-col :span="mainwidth" :xs="24" >
        <el-form :model="queryParams" ref="queryForm" :inline="true" v-show="showSearch" label-width="68px">
          <!--条件选择-->
          <el-form-item label="条件选择">
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
          </el-form-item>
          <!--触发类型   -->
          <el-form-item label="触发类型" prop="status" >
            <el-select
              v-model="queryParams.trigger_type"
              placeholder="触发类型"
              clearable
              size="small"
              style="width: 130px"
            >
              <el-option
                v-for="dict in TaskOptions"
                :key="dict.dictValue"
                :label="dict.dictLabel"
                :value="dict.dictValue"
              />
            </el-select>
          </el-form-item>
          <!--运营商类型-->
          <el-form-item label="运营商类型" prop="status" label-width="100px">
            <!--  多选属性   multiple-->
            <el-select
              v-model="queryParams.operation_type"
              placeholder="运营商类型"
              clearable
              size="small"
              style="width: 280px"
            >
              <el-option
                v-for="dict in operation_type"
                :key="dict.dictValue"
                :label="dict.dictLabel"
                :value="dict.dictValue"
              />
            </el-select>
          </el-form-item>
          <!--执行模板-->
          <el-form-item label="执行模板" prop="status">
            <el-select
              v-model="queryParams.execution_template"
              placeholder="执行模板"
              clearable
              size="small"
              style="width: 95px"
            >
              <el-option
                v-for="dict in ExecutionTemplate"
                :key="dict.dictValue"
                :label="dict.dictLabel"
                :value="dict.dictValue"
              />
            </el-select>
          </el-form-item>
          <!--执行动作类型-->
          <el-form-item label="执行动作类型" prop="status" label-width="100px">
            <el-select
              v-model="queryParams.execution_type"
              placeholder="执行动作类型"
              clearable
              size="small"
              style="width: 110px"
            >
              <el-option
                v-for="dict in ExecuteAction"
                :key="dict.dictValue"
                :label="dict.dictLabel"
                :value="dict.dictValue"
              />
            </el-select>
          </el-form-item>
          <el-form-item >
            <el-button type="primary" icon="el-icon-search" size="mini" @click="handleQuery">搜索</el-button>
            <el-button icon="el-icon-refresh" size="mini" @click="resetQuery">重置</el-button>
          </el-form-item>

          <!--自动化状态-->
          <el-form-item label="自动化状态" prop="status" label-width="100px">
            <el-select
              v-model="queryParams.status"
              placeholder="自动化状态"
              clearable
              size="small"
              style="width: 95px"
            >
              <el-option
                v-for="dict in AutomationStatus"
                :key="dict.dictValue"
                :label="dict.dictLabel"
                :value="dict.dictValue"
              />
            </el-select>
          </el-form-item>

          <!--条件类型-->
          <el-form-item label="条件类型" prop="status">
            <el-select
              v-model="queryParams.conditions_type"
              placeholder="条件类型"
              clearable
              size="small"
              style="width: 95px"
            >
              <el-option
                v-for="dict in ConditionType"
                :key="dict.dictValue"
                :label="dict.dictLabel"
                :value="dict.dictValue"
              />
            </el-select>
          </el-form-item>
          <!--  时间类型-->
          <el-form-item label="时间类型" prop="status">
            <el-select
              v-model="queryParams.timetype"
              placeholder="时间类型"
              clearable
              size="small"
              style="width: 110px"
            >
              <el-option
                v-for="dict in timetypeOptions"
                :key="dict.dictValue"
                :label="dict.dictLabel"
                :value="dict.dictValue"
              />
            </el-select>
          </el-form-item>
          <el-form-item >
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
        </el-form>
        <el-row :gutter="10" class="mb8">
          <el-col :span="1.5">
            <el-dropdown-item>
              <el-button
                type="primary"
                plain
                icon="el-icon-plus"
                size="mini"
                @click="AddCc1"
              >新增</el-button>
            </el-dropdown-item>
          </el-col>
        </el-row>

        <el-table v-loading="loading" :data="cardList" @selection-change="handleSelectionChange">

          <el-table-column label="通道 id" align="center" key="channel_id" prop="channel_id" v-if="columns[0].visible " :show-overflow-tooltip="true">
            <template slot-scope="scope">
              <div v-if="scope.row.channel_id!=null && scope.row.channel_id!=''  && scope.row.channel_id.length>0 ">
                <div v-if="scope.row.channel_id.indexOf(',')!=-1">
                  <div  v-for="dict in (scope.row.channel_id).split(',')">
                    {{
                      "["+tools.getDkeyValue(findCr_RouteOptions,dict)+"]"
                    }}
                  </div>
                </div>
                <div v-if="scope.row.channel_id.indexOf(',')==-1">
                  {{
                    "["+tools.getDkeyValue(findCr_RouteOptions,scope.row.channel_id)+"]"
                  }}
                </div>
              </div>
            </template>
          </el-table-column>
          <el-table-column   label="自动化抄送名称" align="center"  prop="cc_name" v-if="columns[1].visible" />
          <el-table-column   label="触发类型" align="center" key="trigger_type" v-if="columns[2].visible" :show-overflow-tooltip="true" width="100">
            <template slot-scope="scope" >
              {{tools.getDkeyValue(TaskOptions,scope.row.trigger_type)}}
            </template>
          </el-table-column>
          <el-table-column   label="判断条件" align="center"  prop="triggering_conditions" v-if="columns[3].visible" />
          <el-table-column   label="运营商类型" align="center" key="operation_type"  prop="operation_type" v-if="columns[4].visible">
            <template slot-scope="scope">
              <div v-if="scope.row.operation_type!=null && scope.row.operation_type!=''  && scope.row.operation_type.length>0 ">
                <div v-if="scope.row.operation_type.indexOf(',')!=-1">
                  <div  v-for="dict in (scope.row.operation_type).split(',')">
                    {{
                      "["+tools.getDkeyValue(operation_type,dict)+"]"
                    }}
                  </div>
                </div>
                <div v-if="scope.row.operation_type.indexOf(',')==-1">
                  {{
                    "["+tools.getDkeyValue(operation_type,scope.row.operation_type)+"]"
                  }}
                </div>
              </div>
            </template>
          </el-table-column>
          <el-table-column   label="执行模板" align="center"  prop="execution_template" v-if="columns[5].visible">
            <template slot-scope="scope" >
              {{tools.getDkeyValue(ExecutionTemplate,scope.row.execution_template)}}
            </template>
          </el-table-column>
          <el-table-column   label="执行动作类型" align="center"  prop="execution_type" v-if="columns[5].visible">
            <template slot-scope="scope" >
              {{tools.getDkeyValue(ExecuteAction,scope.row.execution_type)}}
            </template>
          </el-table-column>
          <el-table-column   label="自动化状态" align="center"  prop="status" v-if="columns[5].visible">
            <template slot-scope="scope" >
              {{tools.getDkeyValue(AutomationStatus,scope.row.status)}}
            </template>
          </el-table-column>
          <el-table-column   label="所属企业" align="center"  prop="dep_id" v-if="columns[6].visible" >
            <template slot-scope="scope" >
              {{tools.getDeptNamekeyValue(DeptOptions,scope.row.dep_id)}}
            </template>
          </el-table-column>
          <el-table-column   label="备注" align="center"  prop="remark" v-if="columns[7].visible" />
          <el-table-column   label="创建时间" align="center"  prop="create_time" v-if="columns[8].visible" />
          <el-table-column
            label="操作"
            align="center"
            width="90"
            class-name="small-padding fixed-width"
          >
            <template slot-scope="scope">
              <el-dropdown >
                <el-button type="primary"  size="small" >
                  操作<i class="el-icon-arrow-down el-icon--right"></i>
                </el-button>
                <el-dropdown-menu slot="dropdown">

                  <template>
                    <el-dropdown-item>
                      <el-button type="text" @click="UpdateCc(scope.row)">修改</el-button>
                    </el-dropdown-item>
                  </template>

                  <el-dropdown-item >
                    <router-link :to="'/ccurl/type/index/'+ scope.row.channel_id" class="link-type">
                      <span>配置跳转</span>
                    </router-link>
                  </el-dropdown-item>

                  <el-dropdown-item   v-hasPermi="['yunze:cchis:list']">
                    <router-link :to="'/cchis/type/index/'+ scope.row.channel_id" class="link-type">
                      <span>推送记录</span>
                    </router-link>
                  </el-dropdown-item>

                  <template>
                    <el-dropdown-item>
                      <el-button type="text"  @click="handleAdd(scope.row)">新增配置</el-button>
                    </el-dropdown-item>
                  </template>

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

    <!-- 新增配置话框 -->
    <el-dialog :title="title" :visible.sync="open" width="500px" append-to-body>
      <el-form ref="form" :model="form" :rules="rules" label-width="80px">
        <br>
        <el-form-item label="推送 API" prop="url">
          <el-input v-model="form.url" placeholder="请输入API" />
        </el-form-item>
        <br>
        <el-form-item label="推送邮件" prop="email">
          <el-input v-model="form.email" placeholder="请输入邮件" />
        </el-form-item>
      </el-form>
      <div slot="footer" class="dialog-footer">
        <el-button type="primary" @click="submitForm">确 定</el-button>
        <el-button @click="cancel">取 消</el-button>
      </div>
    </el-dialog>

    <!-- 新增自动化 -->
    <el-dialog :visible.sync="automation" width="75%" append-to-body @close="automationClose">

      <el-row :gutter="20">

        <!-- 树型展示 -->
        <el-col :span="8"  v-show="Tree_type">
          <div class="head-container">
            <el-input
              v-model="deptName"
              placeholder="请输通道名称"
              clearable
              size="small"
              prefix-icon="el-icon-search"
              style="margin-bottom: 20px"
            />
          </div>
          <el-checkbox v-model="deptExpand" @change="passagewayTreeExpand($event, 'dept')">展开/折叠</el-checkbox>
          <el-checkbox v-model="deptNodeAll" @change="passagewayTreeNodeAll($event, 'dept')">全选/全不选</el-checkbox>
          <el-checkbox v-model="deptCheckStrictly" @change="passagewayTreeConnect($event, 'dept')">父子联动</el-checkbox>
          <el-row >
            <el-col :span="12">
              <div class="head-container">
                <el-tree           :data="passageway_id"
                                   v-model="forArr.channel_id"
                                   show-checkbox
                                   :filter-node-method="DeptNode"
                                   default-expand-all
                                   node-key="cd_id"
                                   ref="dept"
                                   highlight-current
                                   @node-click="handleNodeClick"
                                   :props="deptProps"
                                   empty-text="没有数据了~~~">
                </el-tree>
              </div>
            </el-col>
          </el-row>
        </el-col>


        <el-col :span="8" v-show="option_show">
          <div class="head-container">
            <el-input
              v-model="treeName"
              placeholder="请输入公司名称"
              clearable
              size="small"
              prefix-icon="el-icon-search"
              style="margin-bottom: 20px"
            />
          </div>
          <el-checkbox v-model="treeExpand" @change="handleCheckedTreeExpand($event, 'tree')">展开/折叠</el-checkbox>
          <el-checkbox v-model="treeNodeAll" @change="handleCheckedTreeNodeAll($event, 'tree')">全选/全不选</el-checkbox>
          <el-checkbox v-model="treeCheckStrictly" @change="handleCheckedTreeConnect($event, 'tree')">父子联动</el-checkbox>

          <el-tree :data="deptOptions" :check-on-click-node="false" ref="tree" :filter-node-method="filterNode"
                   show-checkbox :check-strictly="true" default-expand-all node-key="id"
                   :props="defaultProps" @check-change="treeNodeClick" empty-text="暂无数据">
          </el-tree>


        </el-col>

        <el-col :span="8">

          <el-form ref="form" :model="forArr" :rules="rules" label-width="80px">
            <el-form-item label="抄送名称" prop="cc_name" label-width="100px" >
              <el-input v-model="forArr.cc_name" placeholder="请输入抄送名称" />
            </el-form-item>
            <el-form-item  prop="status" label-width="100px">
               <span slot="label">
                触发类型
               </span>
              <el-select
                v-model="forArr.trigger_type"
                placeholder="请选择触发类型"
                clearable
                ref="packageSel"
                size="small"
                style="width: 360px"
              >
                <el-option
                  v-for="dict in TaskOptions"
                  :key="dict.dictValue"
                  :label="dict.dictLabel"
                  :value="dict.dictValue"
                />
              </el-select>
            </el-form-item>
            <el-form-item  prop="status" label-width="100px">
               <span slot="label">
                运营商类型
               </span>
              <el-select
                multiple
                v-model="forArr.operation_type"
                placeholder="请选择运营商类型"
                clearable
                ref="packageSel"
                size="small"
                style="width: 360px"
              >
                <el-option
                  v-for="dict in operation_type"
                  :key="dict.dictValue"
                  :label="dict.dictLabel"
                  :value="dict.dictValue"
                />
              </el-select>
            </el-form-item>
            <el-form-item  prop="status" label-width="100px">
               <span slot="label">
                执行动作类型
               </span>
              <el-select
                v-model="forArr.execution_type"
                placeholder="请选择执行动作类型"
                clearable
                ref="packageSel"
                size="small"
                style="width: 360px"
              >
                <el-option
                  v-for="dict in ExecuteAction"
                  :key="dict.dictValue"
                  :label="dict.dictLabel"
                  :value="dict.dictValue"
                />
              </el-select>
            </el-form-item>
            <el-form-item  prop="status" label-width="100px">
               <span slot="label">
                状态
               </span>
              <el-select
                v-model="forArr.status"
                placeholder="请选择状态"
                clearable
                ref="packageSel"
                size="small"
                style="width: 360px"
              >
                <el-option
                  v-for="dict in AutomationStatus"
                  :key="dict.dictValue"
                  :label="dict.dictLabel"
                  :value="dict.dictValue"
                />
              </el-select>
            </el-form-item>
            <el-form-item  prop="status" label-width="100px">
               <span slot="label">
                条件类型
               </span>
              <el-select
                v-model="forArr.conditions_type"
                placeholder="请选择条件类型"
                clearable
                ref="packageSel"
                size="small"
                style="width: 360px"
              >
                <el-option
                  v-for="dict in ConditionType"
                  :key="dict.dictValue"
                  :label="dict.dictLabel"
                  :value="dict.dictValue"
                />
              </el-select>
            </el-form-item>
            <el-form-item  prop="status" label-width="100px">
               <span slot="label">
                执行模板
               </span>
              <el-select
                v-model="forArr.execution_template"
                placeholder="请选择执行模板"
                clearable
                ref="packageSel"
                size="small"
                style="width: 360px"
              >
                <el-option
                  v-for="dict in ExecutionTemplate"
                  :key="dict.dictValue"
                  :label="dict.dictLabel"
                  :value="dict.dictValue"
                />
              </el-select>
            </el-form-item>
            <el-form-item label-width="100px">
              <el-button icon="el-icon-s-unfold" type="primary" plain size="mini"  @click="passageway">通道 id</el-button>
              <el-button icon="el-icon-shopping-bag-1" type="primary" plain size="mini"  @click="Operator" >公司所属</el-button>
            </el-form-item>
            <el-form-item label="备注" prop="remark" label-width="100px">
              <el-input v-model="forArr.remark"  type="textarea" placeholder="请输入内容"></el-input>
            </el-form-item>
          </el-form>
          <el-button v-if="cardAdd" type="primary" @click="submitAdd">确 定</el-button>
          <el-button v-if="cardEdie" type="primary" @click="Edie">保 存</el-button>
          <el-button @click="cancel">取 消</el-button>
        </el-col>
      </el-row>

    </el-dialog>


  </div>
</template>

<script>
import { listCc,AddCc,UpdateCc} from "@/api/yunze/automation/cc";
import { getToken } from "@/utils/auth";
import { treeselect } from "@/api/system/dept";
import { AddCcUrl } from "@/api/yunze/automation/ccurl";
import tools from "@/utils/yunze/tools";
import {findCr, findDeptUser} from "@/api/yunze/flowcard/card/card";

export default {
  name: "cc",
  data() {
    return {
      centerDialogVisible: false,
      cardAdd: false,
      cardEdie: false,
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
      DeptOptions:[],//公司所属
      // 定义 tools
      tools:tools,
      //通道
      deptCheckStrictly: true,
      deptExpand: true,
      deptNodeAll: false,
      //公司所属
      treeCheckStrictly: true,
      treeExpand: true,
      treeNodeAll: false,

      customize_whether: [],//自定义是否
      ExecutionTask_OutType : [],// 执行任务导出类别
      SetMealImport:false, //详情查询 套餐信息
      show_ds:false, //详情查询
      selTime:'',//时间选择
      mainwidth:24,//宽度

      option_show:false, //公司所属查询
      Tree_type: false, //树型展示

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
      showSearch: true,
      // 总条数
      total: 0,
      // 用户表格数据
      cardList: null,
      // 部门树选项
      deptOptions: undefined,
      // 弹出层标题
      title: "",
      // 部门树选项
      passageway_id: undefined,
      // 是否显示弹出层
      open: false,
      automation: false,
      // 部门名称
      deptName: undefined,
      treeName: undefined,
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
      //任务类别
      category_type: "",
      //执行动作类型
      ExecuteAction: [],
      //自动化状态
      AutomationStatus: [],
      // 运营商类别 字典
      operators_type: [],
      // 执行任务状态 字典
      channelStatusOptions: [],
      // 执行任务编码 字典
      channelCodeOptions: [],
      //条件类型、
      ConditionType: [],
      selectList:[],
      splice: [],
      //执行模板
      ExecutionTemplate:[],
      userArr: [],
      operation_type:[],//运营商类型
      // 表单参数
      form: {
        cc_id:null,
        url:null,
        cc_name:null,
        email:null,
        trigger_typeArr:[],
        operation_type:[],
      },
      defaultProps: {
        children: "children",
        label: "label"
      },
      //通道
      deptProps: {
        children: "children",
        label: "label"
      },
      trigger_type: '',
      TaskOptions: [
      ],
      //当前查询参数
      sel:{
        iccid:'',
        agent_id:'',
        package_id:'',
      },
      formDivide: {
        dept_id:'',
        user_id:'',
      },
      // 查询参数
      queryParams: {
        pageNum: 1,
        pageSize: 10,
        type: null,
        value: null,
        cd_status: [],
        staTime: null,
        endTime: null,
        timetype: null,

      },
      forArr: {
        trigger_type: null,
        operation_type: null,
        execution_type: null,
        status: null,
        conditions_type: null,
        execution_template: null,
        dep_id: null,
        channel_id: null,
      },

      findCr_RouteOptions: [],// API通道信息
      // 列信息
      columns: [
        { key: 0, label: `通道id`, visible: true },
        { key: 1, label: `自动化抄送名称`, visible: true },
        { key: 2, label: `触发类型`, visible: true },
        { key: 3, label: `判断条件`, visible: true },
        { key: 4, label: `运营商类型`, visible: true },
        { key: 5, label: `执行模板`, visible: true },
        { key: 6, label: `所属企业`, visible: true },
        { key: 6, label: `所属企业`, visible: true },
        { key: 6, label: `所属企业`, visible: true },
        { key: 7, label: `备注`, visible: true },
        { key: 8, label: `创建时间`, visible: true },
      ],
      // 表单校验
      rules: {
        url: [
          { required: true, message: "APL不能为空", trigger: "blur" }
        ],
        email: [
          { required: true, message: "邮件不能为空", trigger: "blur" }
        ],
        cc_name: [
          { required: true, message: "抄送名称不能为空", trigger: "blur" }
        ]
      }
    };
  },
  watch: {
    // 根据名称筛选企业树
    deptName(val) {
      this.$refs.dept.filter(val);
    },
    treeName(val) {
      this.$refs.tree.filter(val);
    }
  },
  created() {
    this.getTreeselect();
    this.getList();





    //加载 查询条件
    if(window['typeOptions']!=undefined &&  window['typeOptions']!=null && window['typeOptions']!=''){
      this.typeOptions = window['typeOptions'];
    }else{
      this.getDicts("cc_query_criteria").then(response => {
        window['typeOptions'] = response.data;
        this.typeOptions = window['typeOptions'];
      });
    }

    //加载 触发类型
    if(window['TaskOptions']!=undefined &&  window['TaskOptions']!=null && window['TaskOptions']!=''){
      this.TaskOptions = window['TaskOptions'];
    }else{
      this.getDicts("yz_cc_trigger_type").then(response => {

        window['TaskOptions'] = response.data;
        this.TaskOptions = window['TaskOptions'];
      });
    }

    //加载 运营商类型
    if(window['operation_type']!=undefined &&  window['operation_type']!=null && window['operation_type']!=''){
      this.operation_type = window['operation_type'];
    }else{
      this.getDicts("yunze_card_operators_type").then(response => {

        window['operation_type'] = response.data;
        this.operation_type = window['operation_type'];
      });
    }



    //加载 执行动作类型
    if(window['ExecuteAction']!=undefined &&  window['ExecuteAction']!=null && window['ExecuteAction']!=''){
      this.ExecuteAction = window['ExecuteAction'];
    }else{
      this.getDicts("yz_cc_execution_type").then(response => {

        window['ExecuteAction'] = response.data;
        this.ExecuteAction = window['ExecuteAction'];
      });
    }

    //加载 自动化状态
    if(window['AutomationStatus']!=undefined &&  window['AutomationStatus']!=null && window['AutomationStatus']!=''){
      this.AutomationStatus = window['AutomationStatus'];
    }else{
      this.getDicts("yz_cc_status").then(response => {

        window['AutomationStatus'] = response.data;
        this.AutomationStatus = window['AutomationStatus'];
      });
    }

    //加载 条件类型
    if(window['ConditionType']!=undefined &&  window['ConditionType']!=null && window['ConditionType']!=''){
      this.ConditionType = window['ConditionType'];
    }else{
      this.getDicts("yz_cc_condition").then(response => {

        window['ConditionType'] = response.data;
        this.ConditionType = window['ConditionType'];
      });
    }

    //加载 执行模板
    if(window['ExecutionTemplate']!=undefined &&  window['ExecutionTemplate']!=null && window['ExecutionTemplate']!=''){
      this.ExecutionTemplate = window['ExecutionTemplate'];
    }else{
      this.getDicts("yz_cc_template").then(response => {

        window['ExecutionTemplate'] = response.data;
        this.ExecutionTemplate = window['ExecutionTemplate'];
      });
    }

    //加载 时间赛选查询条件
    if(window['timetypeOptions']!=undefined &&  window['timetypeOptions']!=null && window['timetypeOptions']!=''){
      this.timetypeOptions = window['timetypeOptions'];
    }else{
      this.getDicts("Automation_time").then(response => {
        window['timetypeOptions'] = response.data;
        this.timetypeOptions = window['timetypeOptions'];
      });
    }

    //加载 企业名称
    if(window['DeptOptions']!=undefined &&  window['DeptOptions']!=null && window['DeptOptions']!=''){
      this.DeptOptions = window['DeptOptions'];
    }else{
      this.getDeptName().then(response => {
        let jsonobj =  JSON.parse(tools.Decrypt(response));
        window['DeptOptions'] = jsonobj.Data;
        this.DeptOptions = window['DeptOptions'];
      });
    }

    //加载 执行任务导出类别
    if(window['ExecutionTask_OutType']!=undefined &&  window['ExecutionTask_OutType']!=null && window['ExecutionTask_OutType']!=''){
      this.ExecutionTask_OutType = window['ExecutionTask_OutType'];
    }else{
      this.getDicts("yunze_ExecutionTask_OutType").then(response => {
        window['ExecutionTask_OutType'] = response.data;
        this.ExecutionTask_OutType = window['ExecutionTask_OutType'];
      });
    }


    //加载 API通道名称
    if(window['findCr_RouteOptions']!=undefined &&  window['findCr_RouteOptions']!=null && window['findCr_RouteOptions']!=''){
      this.findCr_RouteOptions = window['findCr_RouteOptions'];
      this.setTreeApi();
    }else{
      findCr().then(response => {
        let jsonobj =  JSON.parse(tools.Decrypt(response));
        window['findCr_RouteOptions'] = jsonobj.Data;
        this.findCr_RouteOptions = window['findCr_RouteOptions'];
        this.setTreeApi();
      });
    }





  },
  methods: {

    // tree单选getNode(data) {

    treeNodeClick (data, checked) {
      //console.log(data)
      this.userArr = [];
      const node = this.$refs.tree.getNode(data);
      //console.log(node)
      const str = [];
      /*function filterName (node) {
        if (node) {
          if (node.parent) {
            str.push(node.data.label);
            filterName(node.parent);
          }
        }
      }*/
      // 获取当前选择的id在数组中的索引
      const indexs = this.selectList.indexOf(data.id);
      // 如果不存在数组中，并且数组中已经有一个id并且checked为true的时候，代表不能再次选择。
      if (indexs < 0 && this.selectList.length === 1 && checked) {
        this.msgError("单选操作请勿多选！");
        // 设置已选择的节点为false 很重要
        this.$refs.tree.setChecked(data, false);
      } else if (this.selectList.length === 0 && checked) {
        // 发现数组为空 并且是已选择
        // 防止数组有值，首先清空，再push
        this.selectList = [];
        this.formDivide.dept_id = '';
        this.selectList.push(data.id);
        //filterName(node);

        //this.formDivide.dept_id = str.reverse().join('/');
        //this.formDivide.dept_id = str[0];
        this.formDivide.dept_id = data.id;
        this.formDivide.dept_name = data.label;
        //获取企业下用户
        this.userArr = [];
        this.formDivide.user_id = null;
        let map ={};
        map.dept_id = this.formDivide.dept_id;
        let Pwd_Str = tools.encrypt(JSON.stringify(map));
        findDeptUser(Pwd_Str).then(response => {
            let jsonobj =  JSON.parse(tools.Decrypt(response));
            //console.log(jsonobj);
            if(jsonobj.code==200){
              this.userArr = jsonobj.Data;
            }else{
              this.msgError(jsonobj.msg);
            }
            this.loading = false;
          }
        );

        //console.log(this.formDivide.dept_id);
      } else if (indexs >= 0 && this.selectList.length === 1 && !checked) {
        // 再次直接进行赋值为空操作
        this.selectList = [];
        this.formDivide.dept_id = '';
        this.forArr.dep_id = '';
      }
    },

    setTreeApi(){
      this.passageway_id = [];
      //console.log(this.findCr_RouteOptions)
      for (let i = 0; i < this.findCr_RouteOptions.length ; i++) {
        let obj = findCr_RouteOptions[i];
        let map = {};
        map.id = obj.dictValue;
        map.label = obj.dictLabel;
        this.passageway_id.push(map);
      }
    },

    // 通道 树权限（展开/折叠）
    passagewayTreeExpand(value, type) {
      if (type == 'dept') {
        let deptList = this.passageway_id;
        for (let i = 0; i < deptList.length; i++) {
          this.$refs.dept.store.nodesMap[deptList[i].id].expanded = value;
        }
      }
    },
    // 通道 树权限（全选/全不选）
    passagewayTreeNodeAll(value, type) {
      if (type == 'dept') {
        //console.log(value);
        this.$refs.dept.setCheckedNodes(value ? this.passageway_id: []);
      }
    },
    // 通道 树权限（父子联动）
    passagewayTreeConnect(value, type) {
      if (type == 'menu') {
        this.menuCheckStrictly = value ? true: false;
      } else if (type == 'dept') {
        this.$refs.dept.setCheckedNodes(false ? this.passageway_id: []);
        this.deptCheckStrictly = value ? true: false;

      }
    },

    // 树权限（展开/折叠）
    handleCheckedTreeExpand(value, type) {
      if (type == 'tree') {
        let treeList = this.deptOptions;
        for (let i = 0; i < treeList.length; i++) {
          this.$refs.tree.store.nodesMap[treeList[i].id].expanded = value;
        }
      }
    },
    // 树权限（全选/全不选）
    handleCheckedTreeNodeAll(value, type) {
      if (type == 'tree') {
        //console.log(value);
        this.$refs.tree.setCheckedNodes(value ? this.deptOptions: []);
      }
    },
    // 树权限（父子联动）
    handleCheckedTreeConnect(value, type) {
      if (type == 'menu') {
        this.menuCheckStrictly = value ? true: false;
      } else if (type == 'tree') {
        this.$refs.tree.setCheckedNodes(false ? this.deptOptions: []);
        this.treeCheckStrictly = value ? true: false;

      }
    },


    /*获取查询参数*/
    getParams(){

      if(this.$refs.tree!=undefined & this.$refs.tree!=null & this.$refs.tree!='undefined' ){
        // console.log(this.$refs.dept.getCheckedKeys());
        if(this.$refs.tree.getCheckedKeys().length>0){
          this.queryParams.dep_id = this.$refs.tree.getCheckedKeys();
        }
      }
      // 通道 id
      if(this.$refs.dept!=undefined & this.$refs.dept!=null & this.$refs.dept!='undefined' ){
        // console.log(this.$refs.dept.getCheckedKeys());
        if(this.$refs.dept.getCheckedKeys().length>0){
          this.queryParams.channel_id = this.$refs.dept.getCheckedKeys();
        }
      }

      if(tools.Is_null(this.queryParams.timetype) && this.selTime !=null){
        //console.log(this.selTime);
        this.queryParams.staTime = this.selTime[0];
        this.queryParams.endTime = this.selTime[1];
      }


    },
    /*下载执行任务*/
    getDownloadExecutionTask(row){
      let map = {};
      map.path = row.value;
      map.token = getToken();
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
      //console.log(map);
      window.open(process.env.VUE_APP_BASE_API+"/yunze/ExecutionTask/download?Pstr="+Pwd_Str, '_blank');

    },

    //下载 xls 文件 格式
    getdownloadConversionk(row){
      let map = {};
      map.path = row.value;
      map.token = getToken();
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
      //console.log(map);
      window.open(process.env.VUE_APP_BASE_API+"/yunze/ExecutionTask/downloadConversion?Pstr="+Pwd_Str, '_blank');

    },




    /** 查询执行任务列表 */
    getList() {
      this.loading = true;
      this.getParams();
      this.queryParams.page = 1;
      //console.log(this.queryParams);
      let Pwd_Str = tools.encrypt(JSON.stringify(this.queryParams));
      //console.log(Pwd_Str);
      listCc(Pwd_Str).then(response => {
          let jsonobj =  JSON.parse(tools.Decrypt(response));
          //console.log(jsonobj);
          if(jsonobj.code==200){
            this.cardList = jsonobj.Data.Data;
            this.total = jsonobj.Data.Pu.rowCount;
          }else{
            this.msgError("获取数据异常，请联系管理员！");
          }
          this.loading = false;
        }
      );
    },
    //通道 ids
    passageway(){
      let _this = this;
      _this.Tree_type=!_this.Tree_type;
    },
    //运营商类型
    Operator(){
      let _this = this;
      _this.option_show=!_this.option_show;
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
    // 通道筛选节点
    DeptNode(value, data) {
      if (!value) return true;
      return data.label.indexOf(value) !== -1;
    },
    // 节点单击事件
    handleNodeClick(data) {
      // console.log(data);
      this.queryParams.agent_id = [data.id];
      this.getList();
    },

    // 取消按钮
    cancel() {
      this.open = false;
      this.automation = false;
    },


    /** 搜索按钮操作 */
    handleQuery() {
      this.queryParams.page = 1;
      this.getList();
    },

    SetTree(){
      ///在这里做数据校验
      if(this.$refs.tree.getCheckedKeys().length>0){
        let arr = this.$refs.tree.getCheckedKeys();
        console.log(arr)
        let Str ="";
        for (let i = 0; i < arr.length; i++) {
          Str+=arr[i]+",";
        }
        console.log(Str)
        Str = Str.substring(0,Str.length-1);
        this.forArr.dep_id =Str;
      }
      // 通道 id

      if(this.$refs.dept.getCheckedKeys().length>0){
        let arr = this.$refs.dept.getCheckedKeys();
        console.log(arr)
        let Str ="";
        for (let i = 0; i < arr.length; i++) {
          Str+=arr[i]+",";
        }
        console.log(Str)
        Str = Str.substring(0,Str.length-1);
        this.forArr.channel_id = Str;
      }else {
        this.forArr.channel_id = '';
      }

    },

    /** 自动化啊新增 提交按钮 */

    submitAdd() {
      this.SetTree();
      let _this = this;
      if (tools.VerificationsText(_this, _this.forArr.trigger_type, "触发类型不能为空！") == true &&
        tools.VerificationsText(_this, _this.forArr.execution_type, "执行动作类型不能为空！") == true &&
        tools.VerificationsText(_this, _this.forArr.status, "状态不能为空！") == true &&
        tools.VerificationsText(_this, _this.forArr.execution_template, "执行模板不能为空！") == true &&
        tools.VerificationsText(_this, _this.forArr.dep_id, "所属企业不能为空！") == true &&
        tools.VerificationsText(_this, _this.forArr.cc_name, "自动化抄送名称不能为空！") == true

      ){
        this.forArr.operation_type = this.forArr.operation_type.join(',') //把数组转换成字符串 插入数据
        let Pwd_Str = tools.encrypt(JSON.stringify(this.forArr));
        console.log(Pwd_Str)
        AddCc(Pwd_Str).then(response => {
          let jsonobj =  JSON.parse(tools.Decrypt(response));
          console.log(jsonobj);
          if(jsonobj.code==200){
            this.msgSuccess(jsonobj.msg);
            this.automation = false;
          }else{
            this.msgError("获取数据异常，请联系管理员！");
          }
        });
      }
    },


    /**修改自动化 保存按钮*/
    Edie(){
      //  运营商类型
      if(this.forArr.operation_type!=undefined & this.forArr.operation_type !='undefined' & this.forArr.operation_type!=null & this.forArr.operation_type!='' & this.forArr.operation_type.length>0 ){
        // this.forArr.operation_type.join(',')
        this.forArr.operation_type = this.forArr.operation_type.join(',')
        // }else {
        //   let Str = [];
        //   Str.push(this.forArr.operation_type);
        //   this.forArr.operation_type=Str;
        //   this.forArr={};
      }
      this.SetTree();
      console.log(this.forArr)
      let _this = this;
      if (tools.VerificationsText(_this, _this.forArr.trigger_type, "触发类型不能为空！") == true &&
        tools.VerificationsText(_this, _this.forArr.execution_type, "执行动作类型不能为空！") == true &&
        tools.VerificationsText(_this, _this.forArr.status, "状态不能为空！") == true &&
        tools.VerificationsText(_this, _this.forArr.execution_template, "执行模板不能为空！") == true &&
        tools.VerificationsText(_this, _this.forArr.dep_id, "所属企业不能为空！") == true &&
        tools.VerificationsText(_this, _this.forArr.cc_name, "自动化抄送名称不能为空！") == true

      ){
        let Pwd_Str = tools.encrypt(JSON.stringify(this.forArr));
        //console.log(Pwd_Str)
        UpdateCc(Pwd_Str).then(response => {
          let jsonobj =  JSON.parse(tools.Decrypt(response));
          console.log(jsonobj);
          if(jsonobj.code==200){
            this.msgSuccess(jsonobj.msg);
            this.automation = false;
          }else{
            this.msgError("获取数据异常，请联系管理员！");
          }
        });
      }
    },


    /** 提交按钮 */
    submitForm: function() {
      let Pwd_Str = tools.encrypt(JSON.stringify(this.form));
      AddCcUrl(Pwd_Str).then(response => {
        let jsonobj =  JSON.parse(tools.Decrypt(response));
        console.log(jsonobj);
        if(jsonobj.code==200){
          this.msgSuccess(jsonobj.msg);
          this.open = false;
        }else{
          this.msgError("获取数据异常，请联系管理员！");
        }
      });
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
    handleAdd(row) {
      // console.log(row)
      this.form = {};//清空数据
      this.open = true;
      this.title = "添加自动化推送类型";
      //初始化 新增 数据
      this.form.cc_id= row.id;
    },
    //新增按钮
    AddCc1(){
      this.automation = true;
      this.cardAdd = true;
      this.cardEdie = false;
      this.option_show = false;
      this.Tree_type = false;
      this.forArr = {};//清空数据

    },

    //关闭 自动化修改 清除 terr 选中
    automationClose(){
      this.$refs.dept.setCheckedKeys([]);
      this.$refs.tree.setCheckedKeys([]);
    },


    /**修改自动化按钮*/
    UpdateCc(row){
      //console.log(row)
      this.Tree_type = true;
      this.option_show = true;



      //this.getList();
      //转义
      row = JSON.parse(JSON.stringify(row));
      this.automation = true;
      if (row.channel_id !=null && row.channel_id != '' && row.channel_id.length>0){
        this.Tree_type = true;
        if(row.channel_id.indexOf(',')!= -1){
          row.channel_id =row.channel_id.split(',');
        }else {
          let Arr = [];
          Arr.push(row.channel_id)
          row.channel_id = Arr;
        }
        this.$nextTick(() => {
          this.$refs.dept.setCheckedKeys(row.channel_id);
        });
      }else{
        this.$nextTick(() => {
          this.$refs.dept.setCheckedKeys([]);
        });
        this.Tree_type = false;
      }

      if (row.dep_id !=null && row.dep_id != ''){
        let Arr = [];
        Arr.push(row.dep_id)
        row.dep_id = Arr;
        this.$nextTick(() => {
          this.$refs.tree.setCheckedKeys(row.dep_id);
        });
      }

      if (row.operation_type !=null && row.operation_type != '' && row.operation_type.length>0){
        if(row.operation_type.indexOf(',')!= -1){
          row.operation_type =row.operation_type.split(',');
        }else {
          let Arr = [];
          Arr.push(row.operation_type);
          row.operation_type = Arr;
        }
      }

      row.trigger_type = ""+row.trigger_type;
      row.conditions_type = ""+row.conditions_type;
      row.execution_type = ""+row.execution_type;
      row.status = ""+row.status;
      row.execution_template = ""+row.execution_template;
      this.forArr = row;
      this.cardAdd = false;
      this.cardEdie = true;
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

