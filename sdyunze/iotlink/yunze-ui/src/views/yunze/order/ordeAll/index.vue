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
          />
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
            @node-click="handleNodeClick"
            :props="defaultProps" />

        </div>
      </el-col>
      <!--数据-->
      <el-col :span="mainwidth" :xs="24" >
        <el-form :model="queryParams" ref="queryForm" :inline="true" v-show="showSearch" label-width="68px">

          <el-form-item label="交易类型" prop="status">
            <el-select
              v-model="queryParams.ord_type"
              placeholder="交易类型"
              clearable
              size="small"
              style="width: 110px"
            >
              <el-option
                v-for="dict in orderTypeOptions"
                :key="dict.dictValue"
                :label="dict.dictLabel"
                :value="dict.dictValue"
              />
            </el-select>
          </el-form-item>


          <el-form-item label="支付状态" prop="status">
            <el-select
              v-model="queryParams.status"
              placeholder="支付状态"
              clearable
              size="small"
              style="width: 110px"
            >
              <el-option
                v-for="dict in OrderStateOptions"
                :key="dict.dictValue"
                :label="dict.dictLabel"
                :value="dict.dictValue"
              />
            </el-select>
          </el-form-item>


          <el-form-item label="资费选择" prop="status">
            <el-select
              v-hasPermi="['yunze:cardFlow:queryPackageSimple']"
              v-model="queryParams.package_id"
              placeholder="资费组"
              clearable
              ref="packageSel"
              @change="packageSelChange"
              size="small"
              style="width: 210px"
            >
              <el-option
                v-for="dict in packageOptions"
                :key="dict.package_id"
                :label="dict.package_agentname"
                :value="dict.package_id"
              />
            </el-select>
          </el-form-item>

          <el-form-item  >
            <el-select
              v-hasPermi="['yunze:cardFlow:queryPackageSimple']"
              v-model="queryParams.packet_id"
              placeholder="资费计划"
              clearable
              size="small"
              style="width: 210px"
            >
              <el-option
                v-for="dict in PacketSelOptions"
                :key="dict.dictValue"
                :label="dict.dictLabel"
                :value="dict.dictValue"
              />
            </el-select>
          </el-form-item>

          <el-form-item label="支付方式" prop="status" >
            <el-select
              v-model="queryParams.pay_type"
              placeholder="选择支付方式"
              clearable
              size="small"
              style="width: 110px"
            >
              <el-option
                v-for="dict in Payment_method"
                :key="dict.dictValue"
                :label="dict.dictLabel"
                :value="dict.dictValue"
              />
            </el-select>
          </el-form-item>

          <el-form-item label="加包执行" prop="status" >
            <el-select
              v-model="queryParams.Addpackage"
              placeholder="请选择"
              clearable
              size="small"
              style="width: 110px"
            >
              <el-option
                v-for="dict in Whether_to_add_package"
                :key="dict.dictValue"
                :label="dict.dictLabel"
                :value="dict.dictValue"
              />
            </el-select>
          </el-form-item>

        </el-form>


        <el-form :model="queryParams" ref="queryForm" :inline="true" v-show="showSearch">
          <el-form-item  prop="status"   >
            <span class="el-form-item__label" style="font-weight: 700;" >检索类型</span>
            <el-select
              v-model="queryParams.StartAndEndtype"
              placeholder="起止条件"
              clearable
              size="small"
              style="width: 110px"
            >
              <el-option

                v-for="dict in OrderStartAndEndTypeOptions"
                :key="dict.dictValue"
                :label="dict.dictLabel"
                :value="dict.dictValue"
              />
            </el-select>
          </el-form-item>
          <el-form-item>
            <el-input
              v-model="queryParams.StartValue"
              placeholder="开始号段(小)"
              clearable
              size="small"
              style="width: 200px"
              @keyup.enter.native="handleQuery"
            />
          </el-form-item>
          <el-form-item  >
            <el-input
              v-model="queryParams.EndValue"
              placeholder="结束号段(大)"
              clearable
              size="small"
              style="width: 200px"
              @keyup.enter.native="handleQuery"
            />
          </el-form-item>
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

        <el-form :model="queryParams" ref="queryForm" :inline="true" label-width="68px" style="margin-bottom: -10px;line-height:35px">
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
            <el-col :span="1.5" >
              <el-form-item label="显示删除" prop="status" v-hasPermi="['yunze:order:dellist']" style="margin-bottom: 10px;line-height:35px">
                <el-select
                  v-model="queryParams.del_flag"
                  placeholder="显示删除订单"
                  clearable
                  size="small"
                  style="width: 110px"
                >
                  <el-option
                    v-for="dict in customize_whether"
                    :key="dict.dictValue"
                    :label="dict.dictLabel"
                    :value="dict.dictValue"
                  />
                </el-select>
              </el-form-item>
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
                type="warning"
                plain
                icon="el-icon-download"
                size="mini"
                :loading="exportLoading"
                @click="handleExport"
                v-hasPermi="['yunze:card:exportData']"
              >导出</el-button>
            </el-col>
            <right-toolbar :showSearch.sync="showSearch"   v-hasPermi="['yunze:card:list']" @queryTable="getList" :columns="columns"></right-toolbar>
          </el-row>
        </el-form>


        <el-table v-loading="loading" :data="cardList" @selection-change="handleSelectionChange">
          <!--<el-table-column type="selection" width="50" align="center" />-->
          <el-table-column label="交易名称" align="center" key="ord_name" prop="ord_name" v-if="columns[0].visible" :show-overflow-tooltip="true" width="150"  />
          <el-table-column label="订单号" align="center" v-if="columns[1].visible"  :show-overflow-tooltip="true" width="190"  >
            <template slot-scope="scope">
                <span  @click="ViewDetails(scope.row)" class="link-type"
                       v-hasPermi="['yunze:order:find']" >{{ scope.row.ord_no }}</span>
            </template>
          </el-table-column>
          <el-table-column label="交易类型" align="center" key="ord_type"  v-if="columns[2].visible" :show-overflow-tooltip="true" width="80"  >
            <template slot-scope="scope" >
              {{tools.getDkeyValue(orderTypeOptions,scope.row.ord_type)}}
            </template>
          </el-table-column>
          <el-table-column label="ICCID" align="center" key="iccid" prop="iccid" v-if="columns[3].visible" :show-overflow-tooltip="true" width="190" />
          <el-table-column label="微信订单号" align="center" key="wx_ord_no" prop="wx_ord_no" v-if="columns[4].visible"  width="190"  />
          <el-table-column label="支付状态" align="center" key="status"  v-if="columns[5].visible" :show-overflow-tooltip="true" width="80"  >
            <template slot-scope="scope" >
              {{tools.getDkeyValue(orderStatusOptions,scope.row.status)}}
            </template>
          </el-table-column>
          <el-table-column label="交易金额" align="center" key="price" prop="price" v-if="columns[6].visible" :show-overflow-tooltip="true"  width="80" />
          <el-table-column label="交易后金额" align="center" key="account" prop="account" v-if="columns[7].visible" width="90" />
          <el-table-column label="支付方式" align="center" key="pay_type" prop="pay_type" v-if="columns[8].visible" width="80" >
            <template slot-scope="scope" >
              {{tools.getDkeyValue(payTypeOptions,scope.row.pay_type)}}
            </template>
          </el-table-column>
          <el-table-column label="生成方式" align="center" key="cre_type" prop="cre_type" v-if="columns[9].visible"  >
            <template slot-scope="scope" >
              {{tools.getDkeyValue(creTypeOptions,scope.row.cre_type)}}
            </template>
          </el-table-column>
          <el-table-column label="所属公司" align="center" key="agent_id" prop="agent_id" v-if="columns[10].visible"  >
            <template slot-scope="scope" >
              {{tools.getDeptNamekeyValue(DeptOptions,scope.row.agent_id)}}
            </template>
          </el-table-column>
          <el-table-column label="创建时间" align="center" key="create_time" prop="create_time" v-if="columns[11].visible"/>
          <el-table-column label="备注" align="center" key="info" prop="info" v-if="columns[12].visible" :show-overflow-tooltip="true"  width="80" />
          <el-table-column label="生效类型" align="center" key="validate_type" prop="validate_type" v-if="columns[13].visible"  >
            <template slot-scope="scope" >
              {{tools.getDkeyValue(validateTypeOptions,scope.row.validate_type)}}
            </template>
          </el-table-column>

          <el-table-column label="资费计划" align="center" key="packet_id" prop="packet_id" v-if="columns[14].visible"  width="200">
            <template slot-scope="scope" >
              {{tools.getDkeyValue(PacketOptions,scope.row.packet_id)}}
            </template>
          </el-table-column>
          <el-table-column label="已分润" align="center" key="is_profit" prop="is_profit" v-if="columns[15].visible"  >
            <template slot-scope="scope" >
              {{tools.getDkeyValue(customize_whether,scope.row.is_profit)}}
            </template>
          </el-table-column>
          <el-table-column label="分润类型" align="center" key="profit_type" prop="profit_type" v-if="columns[16].visible"  >
            <template slot-scope="scope" >
              {{tools.getDkeyValue(OrderProfitType,scope.row.profit_type)}}
            </template>
          </el-table-column>
          <el-table-column label="已加包" align="center" key="add_package" prop="add_package" v-if="columns[17].visible" >
            <template slot-scope="scope" >
              {{tools.getDkeyValue(customize_whether,scope.row.add_package)}}
            </template>
          </el-table-column>
          <el-table-column label="C端显示" align="center" key="show_status" prop="show_status" v-if="columns[18].visible" >
            <template slot-scope="scope" >
              {{tools.getDkeyValue(customize_whether,scope.row.show_status)}}
            </template>
          </el-table-column>
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
                  <el-dropdown-item   v-hasPermi="['yunze:order:manualaddPackage']"  @click.native="manualaddPackage(scope.row.iccid)">执行加包</el-dropdown-item>
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



    <!-- 详情查看 -->
    <el-dialog :title="title" :visible.sync="show_ds" width="960px" append-to-body>
      <el-form ref="form" :model="form" label-width="80px">
        <el-row>
          <el-col :span="8">
            <el-form-item label="资费计划" prop="packetName">
              <el-input v-model="form.packetName" placeholder="资费计划" />

            </el-form-item>
          </el-col>
          <el-col :span="8">
            <el-form-item label="加包时间" prop="add_package_time">
              <el-input v-model="form.add_package_time" placeholder="加包时间" />
            </el-form-item>
          </el-col>
          <el-col :span="8">
            <el-form-item label="已加包" prop="add_package">
              <el-input v-model="form.add_package" placeholder="已加包" />
            </el-form-item>
          </el-col>
        </el-row>
        <el-row>
          <el-col :span="24">
            <el-form-item label="加包参数" prop="cd_code">
              <el-input type="textarea" style="min-height: 300px;" v-model="form.add_parameter" placeholder="加包参数" />
            </el-form-item>
          </el-col>
        </el-row>
      </el-form>
    </el-dialog>




  </div>
</template>

<script>
import { listOrder,queryPacketSimple,Exportallorders } from "@/api/yunze/order/order";
import { queryPackageSimple} from "@/api/yunze/flowcard/card/card";
import { getToken } from "@/utils/auth";
import { treeselect } from "@/api/system/dept";
import Treeselect from "@riophae/vue-treeselect";
import "@riophae/vue-treeselect/dist/vue-treeselect.css";
import tools from "@/utils/yunze/tools";



export default {
  name: "OrderAll",
  components: { Treeselect },
  data() {
    return {


      CardBindingForm:{
        bind_type:'2',
        imei:null,
        contactName:null,
        iccid:null,
        contactPhone:null
      },

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

      tools:tools,
      deptCheckStrictly: true,
      deptExpand: true,
      deptNodeAll: false,
      Divide_show: false,//卡划分
      cardDivide: false,//卡划分 保存
      CardBindingShow: false,//机卡绑定 操作界面
      dialogVisible: false,
      treeData: [],
      selectList: [],
      userArr:[],//企业下用户数组




      orderStatusOptions: [],//订单状态
      payTypeOptions: [],//支付类型
      creTypeOptions: [],//生成方式
      customize_whether: [],//自定义是否
      Payment_method: [], // 支付方式
      Whether_to_add_package: [], //加包执行
      validateTypeOptions: [],//生效类型
      PacketOptions: [],//资费计划
      PacketSelOptions:[],//资费计划查询
      OrderStartAndEndTypeOptions:[],//订单查询起止条件
      DeptOptions:[],//所属企业
      OrderStateOptions:[],//订单支付状态
      OrderProfitType:[],//订单 分润类型


      SetMealImport:false, //详情查询 套餐信息
      show_ds:false, //详情查询
      selTime:[],//时间选择
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
      // 企业名称
      deptName: undefined,
      //划卡企业
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

      //订单状态
      status_type: "",
      // 订单交易类型 字典
      orderTypeOptions: [],
      // 运营商类别 字典
      operators_type: [],

      // 性别状态字典
      sexOptions: [],
      // 岗位选项
      postOptions: [],
      // 角色选项
      roleOptions: [],
      // 表单参数
      form: {},
      formDivide: {
        dept_id:'',
        user_id:'',
      },
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
        headers: { Authorization: "Bearer " + getToken() },
        // 上传的地址
        url: process.env.VUE_APP_BASE_API + "/yunze/card/importData"
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
        timetype: null,
        staTime: null,
        endTime: null,
        agent_id: null,
        is_pool: null,
        pool_id: null,
        channel_id: null,
        del_flag: "0",
        Addpackage: null,
        pay_type: null,
        status_id:null,
        package_id:null,
        StartAndEndtype:null,
        StartValue:null,
        EndValue:null,
        cd_operator_type:null,
        selType:'Tariff',
      },
      // 列信息
      columns: [
        { key: 0, label: `交易名称`, visible: true },
        { key: 1, label: `订单号`, visible: true },
        { key: 2, label: `交易类型`, visible: true },
        { key: 3, label: `ICCID`, visible: true },
        { key: 4, label: `微信订单号`, visible: true },
        { key: 5, label: `支付状态`, visible: true },
        { key: 6, label: `交易金额`, visible: true },
        { key: 7, label: `交易后金额`, visible: true },
        { key: 8, label: `支付方式`, visible: true },
        { key: 9, label: `生成方式`, visible: true },
        { key: 10, label: `所属公司`, visible: true },
        { key: 11, label: `创建时间`, visible: true },
        { key: 12, label: `备注`, visible: true },
        { key: 13, label: `生效类型`, visible: false },
        { key: 14, label: `资费计划`, visible: false },
        { key: 15, label: `是否分润`, visible: false },
        { key: 16, label: `分润类型`, visible: false },
        { key: 17, label: `是否已加包`, visible: false },
        { key: 18, label: `是否C端显示`, visible: false },
        { key: 19, label: `open_id`, visible: false },

      ],
      // 表单校验
      rules: {
        userName: [
          { required: true, message: "用户名称不能为空", trigger: "blur" }
        ],
        nickName: [
          { required: true, message: "用户昵称不能为空", trigger: "blur" }
        ],
        password: [
          { required: true, message: "用户密码不能为空", trigger: "blur" }
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
    deptName(val) {
      this.$refs.dept.filter(val);

    },
    // 根据名称筛选企业树
    treeName(val) {
      this.$refs.tree.filter(val);
    }
  },

  created() {

    let str = this.$route.params.Pstr;

    //加载 查询条件
    if(window['order_seltype']!=undefined &&  window['order_seltype']!=null && window['order_seltype']!=''){
      this.typeOptions = window['order_seltype'];
    }else{
      this.getDicts("yunze_order_seltype").then(response => {
        window['order_seltype'] = response.data;
        this.typeOptions = window['order_seltype'];
      });
    }


    //加载 资费组名称
    if(window['packageOptions']!=undefined &&  window['packageOptions']!=null && window['packageOptions']!=''){
      this.packageOptions = window['packageOptions'];
    }else{
      queryPackageSimple().then(response => {
        let jsonobj =  JSON.parse(tools.Decrypt(response));
        //console.log(jsonobj);
        window['packageOptions'] = jsonobj.Data;
        this.packageOptions = window['packageOptions'];
      });
    }




    //加载 时间赛选查询条件
    if(window['order_sel_timetype']!=undefined &&  window['order_sel_timetype']!=null && window['order_sel_timetype']!=''){
      this.timetypeOptions = window['order_sel_timetype'];
    }else{
      this.getDicts("yunze_order_sel_timetype").then(response => {
        window['order_sel_timetype'] = response.data;
        this.timetypeOptions = window['order_sel_timetype'];
      });
    }



    //加载 订单交易类型
    if(window['orderTypeOptions']!=undefined &&  window['orderTypeOptions']!=null && window['orderTypeOptions']!=''){
      this.orderTypeOptions = window['orderTypeOptions'];
    }else{
      this.getDicts("yunze_order_type").then(response => {
        window['orderTypeOptions'] = response.data;
        this.orderTypeOptions = window['orderTypeOptions'];
      });
    }

    //加载 订单状态
    if(window['orderStatusOptions']!=undefined &&  window['orderStatusOptions']!=null && window['orderStatusOptions']!=''){
      this.orderStatusOptions = window['orderStatusOptions'];
    }else{
      this.getDicts("yunze_order_status").then(response => {
        window['orderStatusOptions'] = response.data;
        this.orderStatusOptions = window['orderStatusOptions'];
      });
    }

    //加载 支付类型
    if(window['payTypeOptions']!=undefined &&  window['payTypeOptions']!=null && window['payTypeOptions']!=''){
      this.payTypeOptions = window['payTypeOptions'];
    }else{
      this.getDicts("yunze_card_pay_type").then(response => {
        window['payTypeOptions'] = response.data;
        this.payTypeOptions = window['payTypeOptions'];
      });
    }

    //加载 生成方式
    if(window['creTypeOptions']!=undefined &&  window['creTypeOptions']!=null && window['creTypeOptions']!=''){
      this.creTypeOptions = window['creTypeOptions'];
    }else{
      this.getDicts("yunze_order_creType").then(response => {
        window['creTypeOptions'] = response.data;
        this.creTypeOptions = window['creTypeOptions'];
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

    // 支付方式：w微信支付-b余额支付-s平台记录
    if(window['Payment_method']!=undefined &&  window['Payment_method']!=null && window['Payment_method']!=''){
      this.Payment_method = window['Payment_method'];
    }else{
      this.getDicts("yunze_card_pay_type").then(response => {
        window['Payment_method'] = response.data;
        this.Payment_method = window['Payment_method'];
      });
    }

    // 加包执行 是否已经执行过加包 0否 1 是
    if(window['Whether_to_add_package']!=undefined &&  window['Whether_to_add_package']!=null && window['Whether_to_add_package']!=''){
      this.Whether_to_add_package = window['Whether_to_add_package'];
    }else{
      this.getDicts("yunze_customize_whether").then(response => {
        window['Whether_to_add_package'] = response.data;
        this.Whether_to_add_package = window['Whether_to_add_package'];
      });
    }


    //加载 资费计划生效类别
    if(window['validateTypeOptions']!=undefined &&  window['validateTypeOptions']!=null && window['validateTypeOptions']!=''){
      this.validateTypeOptions = window['validateTypeOptions'];
    }else{
      this.getDicts("yunze_card_takeEffect_type").then(response => {
        window['validateTypeOptions'] = response.data;
        this.validateTypeOptions = window['validateTypeOptions'];
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



    //加载 资费计划
    if(window['PacketOptions']!=undefined &&  window['PacketOptions']!=null && window['PacketOptions']!=''){
      this.PacketOptions = window['PacketOptions'];
    }else{
      queryPacketSimple().then(response => {
        let jsonobj =  JSON.parse(tools.Decrypt(response));
        window['PacketOptions'] = jsonobj.Data;
        let arr = jsonobj.Data;
        var new_map = {},
          dest = [];
        if(arr!=null && arr.length>0){
          for(var i = 0; i < arr.length; i++) {
            var ai = arr[i];
            if(!new_map[ai.package_id]) { //依赖分组字段可自行更改！
              dest.push({
                package_id: ai.package_id, //依赖分组字段可自行更改！
                data: [ai]
              });
              new_map[ai.package_id] = ai; //依赖分组字段可自行更改！
            } else {
              for(var j = 0; j < dest.length; j++) {
                var dj = dest[j];
                if(dj.package_id == ai.package_id) { //依赖分组字段可自行更改！
                  dj.data.push(ai);
                  break;
                }
              }
            }
          }
          //存储资费计划
          for (let i = 0; i < dest.length; i++) {
            let obj = dest[i];
            window['PackageOptions_'+obj.package_id] = obj.data;
            //存储生效类型
            if(dest[i]!=null && dest[i].length>0){
              for (let j = 0; j < dest[i].length; j++) {
                let Pobj = dest[i];
                window['validateTypeOptions_'+Pobj.dictValue] = Pobj.packet_valid_type;
              }
            }
          }
        }

        this.PacketOptions = window['PacketOptions'];
      });
    }



    //加载 起止条件查询类别
    if(window['OrderStartAndEndTypeOptions']!=undefined &&  window['OrderStartAndEndTypeOptions']!=null && window['OrderStartAndEndTypeOptions']!=''){
      this.OrderStartAndEndTypeOptions = window['OrderStartAndEndTypeOptions'];
    }else{
      this.getDicts("yunze_order_StartAndEnd_type").then(response => {
        window['OrderStartAndEndTypeOptions'] = response.data;
        this.OrderStartAndEndTypeOptions = window['OrderStartAndEndTypeOptions'];
      });
    }



    //加载 订单状态
    if(window['OrderStateOptions']!=undefined &&  window['OrderStateOptions']!=null && window['OrderStateOptions']!=''){
      this.OrderStateOptions = window['OrderStateOptions'];
    }else{
      this.getDicts("yunze_order_status").then(response => {
        window['OrderStateOptions'] = response.data;
        this.OrderStateOptions = window['OrderStateOptions'];
      });
    }



    //加载 分润类型
    if(window['OrderProfitType']!=undefined &&  window['OrderProfitType']!=null && window['OrderProfitType']!=''){
      this.OrderProfitType = window['OrderProfitType'];
    }else{
      this.getDicts("yunze_order_profitType").then(response => {
        window['OrderProfitType'] = response.data;
        this.OrderProfitType = window['OrderProfitType'];
      });
    }



    this.getTreeselect();

    if(str!=null && str!='' && str.length>0){
      var reg_1 = new RegExp( "%2F" , "g" );
      var reg_2 = new RegExp( " " , "g" );
      str = str.replace(reg_1, "/");//转义 /
      str = str.replace(reg_2, "+");//转义 [ ]  》 +
      //console.log(str);
      let jsonobj =  JSON.parse(tools.Decrypt(str));
      //console.log(jsonobj);
      if(jsonobj!=null ){
        this.queryParams = jsonobj;
        if(jsonobj.staTime!=undefined && jsonobj.staTime!=null && jsonobj.staTime.length>0){
          this.selTime[0] = jsonobj.staTime;
          this.selTime[1] = jsonobj.endTime;
        }
      }
      this.getList();
    }else{
      this.getList();
    }

  },
  methods: {

    //手动执行加包[未完成]
    manualaddPackage(){

    },




    /*获取查询参数*/
    getParams(){
      this.queryParams.staTime = null;
      this.queryParams.endTime = null;
      this.queryParams.agent_id = null;
      if(tools.Is_null(this.queryParams.timetype) && this.selTime !=null){
        //console.log(this.selTime);
        this.queryParams.staTime = this.selTime[0];
        this.queryParams.endTime = this.selTime[1];
      }
      if(this.$refs.dept!=undefined & this.$refs.dept!=null & this.$refs.dept!='undefined' ){
        //console.log(this.$refs.dept.getCheckedKeys());
        if(this.$refs.dept.getCheckedKeys().length>0){
          this.queryParams.agent_id = this.$refs.dept.getCheckedKeys();
        }
      }

      this.queryParams.StartValue = tools.Is_null(this.queryParams.StartValue)?this.queryParams.StartValue:null;
      this.queryParams.EndValue = tools.Is_null(this.queryParams.EndValue)?this.queryParams.EndValue:null;


    },


    /** 查询列表 */
    getList() {
      this.loading = true;
      this.getParams();
      let Pwd_Str = tools.encrypt(JSON.stringify(this.queryParams));
      //console.log(Pwd_Str);
      listOrder(Pwd_Str).then(response => {
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
    /*代理查询拓展*/
    agentShow(){
      let _this = this;
      _this.option_show=!_this.option_show;
      _this.mainwidth = _this.option_show?20:24;
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
      this.queryParams.agent_id = [data.id];
      this.getList();
    },
    // 取消按钮
    cancel() {
      this.open = false;
      this.reset();
    },
    // 表单重置
    reset() {
      this.form = {
        userId: undefined,
        deptId: undefined,
        userName: undefined,
        nickName: undefined,
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
    /** 详情按钮操作 */
    ViewDetails(row) {
      //console.log(row);
      this.show_ds = true;
      this.form.add_package = tools.getDkeyValue(customize_whether,row.add_package);
      this.form.packetName = tools.getDkeyValue(PacketOptions,row.packet_id);
      this.form.add_package_time = row.add_package_time;
      this.form.add_parameter = tools.formatJson(row.add_parameter);
    },


    /** 导出按钮操作 */
    handleExport() {




      this.getParams();
      let Pwd_Str = tools.encrypt(JSON.stringify(this.queryParams));
      Exportallorders(Pwd_Str).then(response => {
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
    /**资费组选择*/
    packageSelChange(){
      if(tools.Is_null(this.queryParams.package_id)){
        //console.log(window['PackageOptions_'+this.queryParams.package_id]);
        this.PacketSelOptions = window['PackageOptions_'+this.queryParams.package_id];
      }
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

</style>

