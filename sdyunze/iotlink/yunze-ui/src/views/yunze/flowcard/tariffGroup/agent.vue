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
            :props="defaultProps" />


        </div>
      </el-col>
      <!--用户数据-->
      <el-col :span="mainwidth" :xs="24" >
        <el-form :model="queryParams" ref="queryForm" :inline="true" v-show="showSearch" label-width="68px">
          <el-form-item label="资费计划" prop="status">
            <el-select
              v-model="queryParams.packetType"
              placeholder="查询条件"
              clearable
              size="small"
              style="width: 110px"
            >
              <el-option
                v-for="dict in packetValueOptions"
                :key="dict.dictValue"
                :label="dict.dictLabel"
                :value="dict.dictValue"
              />
            </el-select>
          </el-form-item>
          <el-form-item  >
            <el-input
              v-model="queryParams.packetValue"
              placeholder="查询值"
              clearable
              size="small"
              style="width: 200px"
              @keyup.enter.native="handleQuery"
            />
          </el-form-item>
          <el-form-item   label="微信支付" >
            <el-checkbox-group >
              <label class="el-checkbox my_checkbox" v-for="(item,index) in customize_whether">
                                    <span class="el-checkbox__inner my_checkbox__inner">
                                    <input type="checkbox" v-model="queryParams.wechat_pay"
                                           name="queryParams_wechat_pay"
                                           :value="item.dictValue"/>
                                     </span>
                <span class="el-checkbox__label my_checkbox__label">{{item.dictLabel}}</span>
              </label>
            </el-checkbox-group>
          </el-form-item>
          <el-form-item   label="余额支付" >
            <el-checkbox-group >
              <label class="el-checkbox my_checkbox" v-for="(item,index) in customize_whether">
                                    <span class="el-checkbox__inner my_checkbox__inner">
                                    <input type="checkbox" v-model="queryParams.balance_pay"
                                           name="queryParams_balance_pay"
                                           :value="item.dictValue"/>
                                     </span>
                <span class="el-checkbox__label my_checkbox__label">{{item.dictLabel}}</span>
              </label>
            </el-checkbox-group>
          </el-form-item>
          <el-form-item   label="分月到账" >
            <el-checkbox-group >
              <label class="el-checkbox my_checkbox" v-for="(item,index) in customize_whether">
                                    <span class="el-checkbox__inner my_checkbox__inner">
                                    <input type="checkbox" v-model="queryParams.is_month"
                                           name="queryParams_is_month"
                                           :value="item.dictValue"/>
                                     </span>
                <span class="el-checkbox__label my_checkbox__label">{{item.dictLabel}}</span>
              </label>
            </el-checkbox-group>
          </el-form-item>
          <el-form-item   label="在售" >
            <el-checkbox-group >
              <label class="el-checkbox my_checkbox" v-for="(item,index) in customize_whether">
                                    <span class="el-checkbox__inner my_checkbox__inner">
                                    <input type="checkbox" v-model="queryParams.in_stock"
                                           name="queryParams_in_stock"
                                           :value="item.dictValue"/>
                                     </span>
                <span class="el-checkbox__label my_checkbox__label">{{item.dictLabel}}</span>
              </label>
            </el-checkbox-group>
          </el-form-item>
          <el-form-item   label="生效类型" >
            <el-checkbox-group >
              <label class="el-checkbox my_checkbox" v-for="(item,index) in card_takeEffect_typeOptions">
                                    <span class="el-checkbox__inner my_checkbox__inner">
                                    <input type="checkbox" v-model="queryParams.packet_valid_type"
                                           name="queryParams_packet_valid_type"
                                           :value="item.dictValue"/>
                                     </span>
                <span class="el-checkbox__label my_checkbox__label">{{item.dictLabel}}</span>
              </label>
            </el-checkbox-group>
          </el-form-item>


        </el-form>

        <el-form :model="queryParams" ref="queryForm" :inline="true" label-width="68px" style="margin-bottom: -10px">
            <el-row :gutter="10" class="mb8">
              <el-col :span="1.5" >
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
              <right-toolbar :showSearch.sync="showSearch"   v-hasPermi="['yunze:tariffGroup:list']" @queryTable="getList" :columns="columns"></right-toolbar>
            </el-row>
        </el-form>

        <el-row :gutter="10" class="mb8">
          <!-- <el-col :span="1.5">
             <el-button
               type="primary"
               plain
               icon="el-icon-plus"
               size="mini"
               @click="handleAdd"
               v-hasPermi="['yunze:tariffGroup:add']"
             >新增资费组</el-button>
           </el-col>
           <el-col :span="1.5">
             <el-button
               type="primary"
               plain
               icon="el-icon-plus"
               size="mini"
               @click="handlePlanAdd()"
               v-hasPermi="['yunze:tariffPlan:add']"
             >新增资费计划</el-button>
           </el-col>
           <el-col :span="1.5">
             <el-button
               type="warning"
               plain
               icon="el-icon-download"
               size="mini"
               :loading="exportLoading"
               @click="handleExport"
               v-hasPermi="['yunze:tariffGroup:exportData']"
             >导出</el-button>
           </el-col>-->
        </el-row>
        <el-table
          :data="tariffGroupList"
        >
          <el-table-column type="expand">
            <template slot-scope="scope">
              <el-table
                :height="350"
                :key="scope.row.index"
                :data="scope.row.data"
              >
                <el-table-column  label="编号" prop="packet_id" />
                <el-table-column  label="名称" prop="packet_wx_name" />
                <el-table-column  label="流量(MB)" prop="packet_flow" />
                <el-table-column  label="售价" prop="packet_price" />
                <el-table-column  label="成本" prop="packet_cost" />
                <el-table-column  label="可微信" prop="wechat_pay" >
                  <template slot-scope="scope" >
                    {{tools.getDkeyValue(customize_whether,scope.row.wechat_pay)}}
                  </template>
                </el-table-column>
                <el-table-column  label="可余额" prop="balance_pay" >
                  <template slot-scope="scope" >
                    {{tools.getDkeyValue(customize_whether,scope.row.balance_pay)}}
                  </template>
                </el-table-column>
                <el-table-column  label="周期" prop="packet_valid_time" />
                <el-table-column  label="规则" prop="packet_valid_name" />
                <el-table-column  label="分月" prop="is_month" >
                  <template slot-scope="scope" >
                    {{tools.getDkeyValue(customize_whether,scope.row.is_month)}}
                  </template>
                </el-table-column>

                <el-table-column  label="自缴费" prop="deduction" >
                  <template slot-scope="scope" >
                    {{tools.getDkeyValue(customize_whether,scope.row.deduction)}}
                  </template>
                </el-table-column>
                <el-table-column label="在售" align="center" key="in_stock" >
                  <template slot-scope="scope" >
                    {{tools.getDkeyValue(customize_whether,scope.row.in_stock)}}
                  </template>
                </el-table-column>
                <el-table-column  label="生效类型" prop="packet_valid_type" >
                  <template slot-scope="scope" >
                    {{tools.getDkeyValue(card_takeEffect_typeOptions,scope.row.packet_valid_type)}}
                  </template>
                </el-table-column>
                <!--<el-table-column
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
                      @click="handlePlanUpdate(scope.row)"
                      v-hasPermi="['yunze:tariffGroup:edit']"
                    >修改</el-button>
                    <el-button
                      size="mini"
                      type="text"
                      icon="el-icon-s-order"
                      @click="ViewPlanDetails(scope.row)"
                      v-hasPermi="['yunze:tariffGroup:find']"
                    >详情</el-button>
                  </template>
                </el-table-column>-->
              </el-table>
            </template>
          </el-table-column>
          <el-table-column label="资费组编号" align="center" key="package_id" prop="package_id"  v-if="columns[0].visible"   />
          <el-table-column label="资费组名称" align="center" key="package_agentname" prop="package_agentname"  v-if="columns[2].visible"   />
          <el-table-column label="所属公司" align="center" key="dept_name" prop="dept_name"  v-if="columns[3].visible"   />
          <el-table-column label="所属人" align="center" key="nick_name" prop="nick_name"  v-if="columns[4].visible"   />
          <el-table-column label="创建时间" align="center" key="create_time" prop="create_time"  v-if="columns[5].visible"   />
          <!--

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
                          v-hasPermi="['yunze:tariffGroup:edit']"
                        >修改</el-button>
                        <el-button
                          size="mini"
                          type="text"
                          icon="el-icon-plus"
                          @click="handlePlanAdd(scope.row.package_id)"
                          v-hasPermi="['yunze:tariffPlan:add']"
                        >新增资费计划</el-button>
                      </template>
                    </el-table-column>
          -->

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

    <!-- 资费组 -->
    <el-dialog title="资费组" :visible.sync="show_ds" width="600px" append-to-body>
      <el-form ref="form" :model="form" :rules="rules" label-width="80px">
        <el-form-item  label="资费组名称" prop="package_name" label-width="150">
          <el-input @blur="KeyUP_Cler(form.package_name,'')"  v-model="form.package_name"  placeholder="请输入资费组名称" />
        </el-form-item>
        <el-form-item id="addpackage_agentname" label="资费组别名" prop="package_agentname" label-width="150">
          <el-input @blur="KeyUP_Cler(form.package_agentname,'')"  v-model="form.package_agentname" placeholder="请输入资费组别名" />
        </el-form-item>
      </el-form>
      <div slot="footer" class="dialog-footer">
        <el-input v-model="form.id" v-if="false" />
        <el-button v-if="tariffGroupAdd"   v-hasPermi="['yunze:tariffGroup:add']" type="primary" @click="Add">新 增</el-button>
        <el-button v-if="tariffGroupEdie"  v-hasPermi="['yunze:tariffGroup:edit']"   type="primary" @click="Edie">保 存</el-button>

      </div>
    </el-dialog>



    <!-- 资费计划 -->
    <el-dialog title="资费计划" :visible.sync="showPlan_ds" width="1060px" append-to-body>



      <el-form ref="Planform" :model="Planform" label-width="100px">
        <el-row>
          <el-col :span="8">
            <el-form-item label="资费组" prop="cd_code">
              <el-select v-model="Planform.package_id"   placeholder="请选择">
                <el-option
                  v-for="dict in packageOptions"
                  :key="dict.package_id"
                  :label="dict.package_agentname"
                  :value="dict.package_id"
                />
              </el-select>
            </el-form-item>
          </el-col>
          <el-col :span="8">
            <el-form-item  label="计划名称" prop="package_name" >
              <el-input @blur="KeyUP_Cler(Planform.packet_name,'')"  v-model="Planform.packet_name"  placeholder="请输入资费计划 名称" />
            </el-form-item>
          </el-col>
          <el-col :span="8">
            <el-form-item label="计划别称" prop="package_agentname" >
              <el-input @blur="KeyUP_Cler(Planform.packet_wx_name,'')"  v-model="Planform.packet_wx_name" placeholder="请输入资费计划 别称" />
            </el-form-item>
          </el-col>
        </el-row>
        <el-row>
          <el-col :span="8">
            <el-form-item label="售价" prop="packet_price" >
              <el-input @keydown.native="handleInput2"   v-model="Planform.packet_price"  placeholder="请输入资费计划 售价" />
            </el-form-item>
          </el-col>
          <el-col :span="8">
            <el-form-item label="成本" prop="packet_cost" >
              <el-input @keydown.native="handleInput2"  v-model="Planform.packet_cost"  placeholder="请输入资费计划 成本" />
            </el-form-item>
          </el-col>
          <el-col :span="8">
            <el-form-item label="在售" prop="packet_cost" >
              <el-radio-group>
                <label class="el-radio " v-for="(item,index) in customize_whether">
                                    <span class="el-radio__input my_checkbox__inner">
                                    <input type="radio" v-model="Planform.in_stock"
                                           name="in_stock"
                                           :value="item.dictValue"/>
                                     </span>
                  <span class="el-radio__label ">{{item.dictLabel}}</span>
                </label>
              </el-radio-group>
            </el-form-item>
          </el-col>
        </el-row>

        <el-row>
          <el-col :span="8">
            <el-form-item label="流量(MB)" prop="packet_flow" >
              <el-input  @keydown.native="handleInput3(Planform.packet_flow)"   v-model="Planform.packet_flow"   placeholder="请输入资费计划流量单位(MB)" />
            </el-form-item>
          </el-col>
          <el-col :span="8">
            <el-form-item label="系数" prop="error_so" >
              <el-input  @blur="handleInput3" @keydown.native="handleInput3"    v-model="Planform.error_so"   placeholder="请输入资费计划 系数" />
            </el-form-item>
          </el-col>
          <el-col :span="8">
            <el-form-item label="用量(MB)" prop="error_flow" >
              <el-input readonly ref="error_flow"  v-model="Planform.error_flow"  placeholder="自动计算 资费计划用量 (MB)" />
            </el-form-item>
          </el-col>
        </el-row>

        <el-row>
          <el-col :span="8">
            <el-form-item label="可微信支付" prop="wechat_pay" >
              <el-radio-group>
                <label class="el-radio " v-for="(item,index) in customize_whether">
                                    <span class="el-radio__input my_checkbox__inner">
                                    <input type="radio" v-model="Planform.wechat_pay"
                                           name="wechat_pay"
                                           :value="item.dictValue"/>
                                     </span>
                  <span class="el-radio__label ">{{item.dictLabel}}</span>
                </label>
              </el-radio-group>
            </el-form-item>
          </el-col>
          <el-col :span="8">
            <el-form-item label="可余额支付" prop="balance_pay" >
              <el-radio-group>
                <label class="el-radio " v-for="(item,index) in customize_whether">
                                    <span class="el-radio__input my_checkbox__inner">
                                    <input type="radio" v-model="Planform.balance_pay"
                                           name="balance_pay"
                                           :value="item.dictValue"/>
                                     </span>
                  <span class="el-radio__label ">{{item.dictLabel}}</span>
                </label>
              </el-radio-group>
            </el-form-item>
          </el-col>
          <el-col :span="6">
            <el-form-item label="自动缴费" prop="deduction" title="自动扣款 所属公司企业">
              <el-radio-group>
                <label class="el-radio " v-for="(item,index) in customize_whether">
                                    <span class="el-radio__input my_checkbox__inner">
                                    <input type="radio" v-model="Planform.deduction"
                                           name="deduction"
                                           :value="item.dictValue"/>
                                     </span>
                  <span class="el-radio__label ">{{item.dictLabel}}</span>
                </label>
              </el-radio-group>
            </el-form-item>
          </el-col>
        </el-row>

        <el-row>
          <el-col :span="6">
            <el-form-item label="计划周期" prop="packet_valid_time" >
              <el-input @keydown.native="handleInput"  v-model="Planform.packet_valid_time"  placeholder="请输入资费计划 计划周期" />
            </el-form-item>
          </el-col>
          <el-col :span="6">
            <el-form-item label="周期规则" prop="packet_valid_name">

              <el-select v-model="Planform.packet_valid_name"   placeholder="请选择">
                <el-option
                  v-for="dict in packetValidNameOptions"
                  :key="dict.dictLabel"
                  :label="dict.dictLabel"
                  :value="dict.dictLabel"
                />
              </el-select>
            </el-form-item>
          </el-col>
          <el-col :span="6">
            <el-form-item label="生效类型" prop="packet_valid_type">
              <el-select v-model="Planform.packet_valid_type" name="packet_valid_type"  placeholder="请选择">
                <el-option
                  v-for="dict in card_takeEffect_typeOptions"
                  :key="dict.dictValue"
                  :label="dict.dictLabel"
                  :value="dict.dictValue"
                />
              </el-select>
            </el-form-item>
          </el-col>
          <el-col :span="6">
            <el-form-item label="是否分月" prop="is_month" >
              <el-radio-group>
                <label class="el-radio " v-for="(item,index) in customize_whether">
                                    <span class="el-radio__input my_checkbox__inner">
                                    <input type="radio" v-model="Planform.is_month"
                                           name="is_month"
                                           :value="item.dictValue"/>
                                     </span>
                  <span class="el-radio__label ">{{item.dictLabel}}</span>
                </label>
              </el-radio-group>
            </el-form-item>
          </el-col>
        </el-row>
      </el-form>
      <div slot="footer" class="dialog-footer">
        <el-input v-model="Planform.id" v-if="false" />
        <el-button v-if="TariffPlanAdd"   v-hasPermi="['yunze:tariffPlan:add']" type="primary" @click="AddPlan">新 增</el-button>
        <el-button v-if="TariffPlanEdie"  v-hasPermi="['yunze:tariffPlan:edit']"   type="primary" @click="EdiePlan">保 存</el-button>

      </div>
    </el-dialog>


  </div>
</template>

<script>
import { listagentTariffGroup,addagentTariffGroup,addtariffGroup,findagentTariffGroup,editagentTariffGroup } from "@/api/yunze/flowcard/agentTariffGroup";
import { AddtariffPlan,findagentTariffPlan,editagentTariffPlan } from "@/api/yunze/flowcard/agentTariffPlan";
import { queryPackageSimple } from "@/api/yunze/flowcard/card/card";
import { treeselect } from "@/api/system/dept";
import Treeselect from "@riophae/vue-treeselect";
import "@riophae/vue-treeselect/dist/vue-treeselect.css";
import tools from "@/utils/yunze/tools";
import $ from 'jquery';


export default {
  name: "agentTariffGroup",
  components: { Treeselect },
  data() {
    return {
      //保存 编辑 按钮 资费组
      tariffGroupAdd: false,
      tariffGroupEdie: false,
      //保存 编辑 按钮 资费计划
      TariffPlanAdd: false,
      TariffPlanEdie: false,
      tools:tools,
      error_flow: 0,

      deptCheckStrictly: true,
      deptExpand: true,
      deptNodeAll: false,
      show_ds: false,//详情查看 资费组
      showPlan_ds: false,//详情查看 资费计划

      DeptOptions: [],// 公司信息
      customize_whether: [],//自定义是否
      packet_valid_nameOptions: [],//资费计划周期
      card_takeEffect_typeOptions: [], //资费计划生效类型


      mainwidth:20,//宽度
      option_show:true, //公司所属查询
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
      tariffGroupList: null,
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
      // 查询类型数据字典
      typeOptions: [],
      //资费计划 查询类别
      packetValueOptions: [],

      // 连接管理时间查询类型 数据字典
      timetypeOptions: [],
      // 简要套餐名称
      packageOptions: [],
      //资费计划周期
      packetValidNameOptions: [],

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
      Planform: {
        error_flow:0,
        packet_valid_type:'',
        packet_valid_name:'',
      },//资费计划

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
        packetType: null,
        packetValue: null,
        packet_valid_type: [],
        in_stock: [],
        is_month: [],
        balance_pay: [],
        wechat_pay: [],
        deptId:[],
      },
      // 列信息
      columns: [
        { key: 0, label: `资费组编号`, visible: true },
        { key: 1, label: `资费组名称`, visible: true },
        { key: 2, label: `资费组名称`, visible: true },
        { key: 3, label: `所属公司`, visible: true },
        { key: 4, label: `所属人`, visible: true },
        { key: 5, label: `创建时间`, visible: true },

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
  mounted:function(){
    /* tools.Textfilter($("#addpackage_name"),"");
     tools.Textfilter($("#addpackage_agentname"),"");*/

  },
  created() {
    this.getTreeselect();
    this.getList();


    //加载 查询条件
    if(window['tariffGroup_selType']!=undefined &&  window['tariffGroup_selType']!=null && window['tariffGroup_selType']!=''){
      this.typeOptions = window['tariffGroup_selType'];
    }else{
      this.getDicts("yunze_tariffGroup_selType").then(response => {
        window['tariffGroup_selType'] = response.data;
        this.typeOptions = window['tariffGroup_selType'];
      });
    }

    //加载 查询条件
    if(window['tariffPlan_selType']!=undefined &&  window['tariffPlan_selType']!=null && window['tariffPlan_selType']!=''){
      this.packetValueOptions = window['tariffPlan_selType'];
    }else{
      this.getDicts("yunze_tariffPlan_selType").then(response => {
        window['tariffPlan_selType'] = response.data;
        this.packetValueOptions = window['tariffPlan_selType'];
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


    //加载 资费计划周期
    if(window['packetValidNameOptions']!=undefined &&  window['packetValidNameOptions']!=null && window['packetValidNameOptions']!=''){
      this.packetValidNameOptions = window['packetValidNameOptions'];
    }else{
      this.getDicts("yunze_packet_valid_name").then(response => {
        window['packetValidNameOptions'] = response.data;
        this.packetValidNameOptions = window['packetValidNameOptions'];
      });
    }



    /* //加载 资费计划生效类型
     if(window['card_takeEffect_type']!=undefined &&  window['card_takeEffect_type']!=null && window['card_takeEffect_type']!=''){
       this.card_takeEffect_type = window['card_takeEffect_type'];
     }else{
       this.getDicts("yunze_card_takeEffect_type").then(response => {
         window['card_takeEffect_type'] = response.data;
         this.card_takeEffect_type = window['card_takeEffect_type'];
       });
     }*/

    //加载 资费计划周期
    if(window['packet_valid_nameOptions']!=undefined &&  window['packet_valid_nameOptions']!=null && window['packet_valid_nameOptions']!=''){
      this.packet_valid_nameOptions = window['packet_valid_nameOptions'];
    }else{
      this.getDicts("yunze_packet_valid_time").then(response => {
        window['packet_valid_nameOptions'] = response.data;
        this.packet_valid_nameOptions = window['packet_valid_nameOptions'];
      });
    }


    //加载 资费计划生效类型
    if(window['card_takeEffect_typeOptions']!=undefined &&  window['card_takeEffect_typeOptions']!=null && window['card_takeEffect_typeOptions']!=''){
      this.card_takeEffect_typeOptions = window['card_takeEffect_typeOptions'];
    }else{
      this.getDicts("yunze_card_takeEffect_type").then(response => {
        window['card_takeEffect_typeOptions'] = response.data;
        this.card_takeEffect_typeOptions = window['card_takeEffect_typeOptions'];
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
    // 在售状态修改
    handleStatusChange(row) {
      let text = row.status === "0" ? "启用" : "停用";
      this.$confirm('确认要"' + text + '""' + row.userName + '"用户吗?', "警告", {
        confirmButtonText: "确定",
        cancelButtonText: "取消",
        type: "warning"
      }).then(function() {
        return changeUserStatus(row.userId, row.status);
      }).then(() => {
        this.msgSuccess(text + "成功");
      }).catch(function() {
        row.status = row.status === "0" ? "1" : "0";
      });
    },

    handleInput(e) {
      e.target.value=e.target.value.replace(/[^\d]/g,'');
    },
    handleInput2(e) {
      // 通过正则过滤小数点后两位
      e.target.value = (e.target.value.match(/^\d*(\.?\d{0,1})/g)[0]) || null
    },

    handleInput3(e) {
      // 通过正则过滤小数点后两位
      //this.$emit('update:value', (e.target.value.match(/^\d*(\.?\d{0,1})/g)[0]) || null);
      e.target.value = (e.target.value.match(/^\d*(\.?\d{0,1})/g)[0]) || null;
      //this.$emit("on-change-grant", val);
      this.FlowCalculation();
    },



    /** 查询列表 */
    getList() {
      this.loading = true;
      this.getParams();
      this.queryParams.page = 1;
      //console.log(this.queryParams);
      let Pwd_Str = tools.encrypt(JSON.stringify(this.queryParams));
      //console.log(Pwd_Str);
      listagentTariffGroup(Pwd_Str).then(response => {
          let jsonobj =  JSON.parse(tools.Decrypt(response));
          //console.log(jsonobj);
          if(jsonobj.code==200){
            let arr = jsonobj.Data.Data;
            let Dsarr = jsonobj.Data.PackeData;
            let new_map = {},dest = [];
            for (let j = 0; j < arr.length ; j++) {
              var ai = arr[j];
              var package_id = ai.package_id;
              var data = [];
              for(var i = 0; i < Dsarr.length; i++) {
                var aiDs = Dsarr[i];
                if(package_id==aiDs.package_id) {
                  data.push(aiDs);
                }
              }
              ai.data = data;
              dest.push(ai);
            }
            this.tariffGroupList = dest;
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
    KeyUP_Cler($val,df_val){
      let val = tools.KeyUP_Cler(this,$val,df_val);
      return val;
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
      this.show_ds = true;
      this.tariffGroupAdd = true;
      this.tariffGroupEdie = false;

    },
    /** 新增按钮操作 资费计划 */
    handlePlanAdd(package_id) {
      this.TariffPlanAdd = true;
      this.TariffPlanEdie = false;


      this.Planform = {};//清空数据 赋值 默认数据
      if(package_id!=undefined && package_id!=null && package_id!=''){
        this.Planform.package_id = package_id;
      }
      this.Planform.balance_pay = '1';
      this.Planform.wechat_pay = '1';
      this.Planform.is_month = '0';
      this.Planform.in_stock = '1';
      /* this.Planform.packet_valid_type = '1';
       this.Planform.packet_valid_name = "月";*/
      this.Planform.deduction = '0';
      //this.Planform.packet_valid_time = 1;
      //this.Planform.error_so = 1.0;

      //初始化 新增 数据
      this.showPlan_ds = true;
      this.TariffPlanAdd = true;
      this.TariffPlanEdie = false;
    },

    /**详情 资费计划 查询按钮*/
    ViewPlanDetails(row){
      this.TariffPlanAdd = false;
      this.TariffPlanEdie = false;
      this.Planform = {};//清空数据
      let map ={};
      map.id = row.id;
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
      findagentTariffPlan(Pwd_Str).then(response => {
          let jsonobj =  JSON.parse(tools.Decrypt(response));
          //console.log(jsonobj);
          if(jsonobj.code==200){
            jsonobj.Data.packet_valid_type = ""+jsonobj.Data.packet_valid_type;
            this.Planform = jsonobj.Data;
            this.showPlan_ds = true;
          }else{
            this.msgError("获取数据异常，请联系管理员！");
          }
          this.loading = false;

        }
      );
    },



    /** 编辑按钮操作 资费计划 */
    handlePlanUpdate(row){
      this.TariffPlanAdd = false;
      this.TariffPlanEdie = true;
      this.Planform = {};//清空数据
      let map ={};
      map.id = row.id;
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
      findagentTariffPlan(Pwd_Str).then(response => {
          let jsonobj =  JSON.parse(tools.Decrypt(response));
          //console.log(jsonobj);
          if(jsonobj.code==200){
            jsonobj.Data.packet_valid_type = ""+jsonobj.Data.packet_valid_type;
            this.Planform = jsonobj.Data;
            this.showPlan_ds = true;
          }else{
            this.msgError("获取数据异常，请联系管理员！");
          }
          this.loading = false;

        }
      );

    },


    /** 用量计算 */
    FlowCalculation(){
      if(tools.Is_null(this.Planform.packet_flow)==true && tools.Is_null(this.Planform.error_so)==true){
        let error_flow = tools.NumberDiv(this.Planform.packet_flow,this.Planform.error_so,0);
        this.Planform.error_flow = error_flow;
        this.$refs.error_flow.value = error_flow;
      }
    },


    /** 修改按钮操作 */
    handleUpdate(row) {
      this.tariffGroupAdd = false;
      this.tariffGroupEdie = true;
      this.finddata(row);
    },
    /** 详情按钮操作 */
    ViewDetails(row) {
      //console.log(row);
      this.tariffGroupAdd = false;
      this.tariffGroupEdie = false;
      this.finddata(row);
    },
    /** 资费计划新增*/
    AddTariffPlan(row) {
      this.TariffPlanAdd = false;
      this.TariffPlanEdie = true;
      this.finddata(row);
    },

    /*获取详细信息*/
    finddata(row){
      this.form = {};//清空数据
      let map ={};
      map.id = row.id;
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
      findagentTariffGroup(Pwd_Str).then(response => {
          let jsonobj =  JSON.parse(tools.Decrypt(response));
          //console.log(jsonobj);
          if(jsonobj.code==200){
            this.form = jsonobj.Data;
            this.show_ds = true;
          }else{
            this.msgError("获取数据异常，请联系管理员！");
          }
          this.loading = false;

        }
      );

    },




    /*获取字典数据中的匹对key value*/
    getDeptNamekeyValue(arr,key){
      let value = '';
      key = ""+key+"";
      for (let i = 0; i < arr.length; i++) {
        let map =  arr[i];
        if(map.dept_id==key){
          value = map.dept_name;
          break;
        }
      }
      return value;
    },

    //新增  资费组
    Add(){
      let _this = this;
      if (tools.VerificationsText(_this, _this.form.package_name, "资费组名称不能为空！") == true &&
        tools.VerificationsText(_this, _this.form.package_agentname, "资费组别称不能为空！") == true) {

        if(tools.KeyUP_Cler(_this,_this.form.package_name,'','',"资费组名称中 不能包含特殊字符！") == true &&
          tools.KeyUP_Cler(_this,_this.form.package_agentname,'','',"资费组别称中 不能包含特殊字符！") == true){
          let Pwd_Str = tools.encrypt(JSON.stringify(this.form));
          addagenttariffGroup(Pwd_Str).then(response => {
            let jsonobj = JSON.parse(tools.Decrypt(response));
            //console.log(jsonobj);
            if (jsonobj.code == 200) {
              this.msgSuccess(jsonobj.msg);
              this.show_ds = false;
              //更新数据
              queryPackageSimple().then(response => {
                let jsonobj =  JSON.parse(tools.Decrypt(response));
                window['packageOptions'] = jsonobj.Data;
                this.packageOptions = window['packageOptions'];
              });
              this.queryParams.page = 1;
              this.getList();

            }else{
              let msg = jsonobj.Data!=null && jsonobj.Data!=''?jsonobj.Data:jsonobj.msg;
              this.msgError(msg);
            }
          })
        }
      }
    },
    //编辑
    Edie(){
      let _this = this;
      if (tools.VerificationsText(_this, _this.form.package_name, "资费组名称不能为空！") == true &&
        tools.VerificationsText(_this, _this.form.id, "必要参数缺失，请刷新后重试！") == true &&
        tools.VerificationsText(_this, _this.form.package_agentname, "资费组别称不能为空！") == true) {

        if (tools.KeyUP_Cler(_this, _this.form.package_name, '', '', "资费组名称中 不能包含特殊字符！") == true &&
          tools.KeyUP_Cler(_this, _this.form.package_agentname, '', '', "资费组别称中 不能包含特殊字符！") == true) {
          let Pwd_Str = tools.encrypt(JSON.stringify(this.form));
          editagentTariffGroup(Pwd_Str).then(response => {
            let jsonobj = JSON.parse(tools.Decrypt(response));
            //console.log(jsonobj);
            if (jsonobj.code == 200) {
              this.msgSuccess(jsonobj.msg);
              this.show_ds = false;
              //更新数据
              queryPackageSimple().then(response => {
                let jsonobj =  JSON.parse(tools.Decrypt(response));
                window['packageOptions'] = jsonobj.Data;
                this.packageOptions = window['packageOptions'];
              });
              this.queryParams.page = 1;
              this.getList();

            } else {
              let msg = jsonobj.Data != null && jsonobj.Data != '' ? jsonobj.Data : jsonobj.msg;
              this.msgError(msg);
            }
          })
        }
      }

    },
    //资费计划 新增 编辑

    AddPlan(){
      let _this = this;
      if (tools.VerificationsText(_this, _this.Planform.package_id, "请选择 对应择资费组！") == true &&
        tools.VerificationsText(_this, _this.Planform.packet_name, "资费计划 名称 不能为空！") == true &&
        tools.VerificationsText(_this, _this.Planform.packet_wx_name, "资费计划 别称 不能为空！") == true &&
        tools.VerificationsText(_this, _this.Planform.packet_price, "资费计划 售价 不能为空！") == true &&
        tools.VerificationsText(_this, _this.Planform.packet_cost, "资费计划 成本 不能为空！") == true &&
        tools.VerificationsText(_this, _this.Planform.in_stock, "请选择 是否在售！") == true &&
        tools.VerificationsText(_this, _this.Planform.packet_flow, "资费计划 流量 不能为空！") == true &&
        tools.VerificationsText(_this, _this.Planform.error_so, "资费计划 系数 不能为空！") == true &&
        tools.VerificationsText(_this, _this.Planform.error_flow, "资费计划 用量 不能为空！") == true &&
        tools.VerificationsText(_this, _this.Planform.wechat_pay, "请选择 是否可微信支付 ！") == true &&
        tools.VerificationsText(_this, _this.Planform.balance_pay, "请选择 是否可余额支付！") == true &&
        tools.VerificationsText(_this, _this.Planform.deduction, "请选择  是否自动缴费！") == true &&

        tools.VerificationsText(_this, _this.Planform.packet_valid_time, "资费计划 周期 不能为空！") == true &&
        tools.VerificationsText(_this, _this.Planform.packet_valid_name, "资费计划 周期规则 不能为空！") == true &&
        tools.VerificationsText(_this, _this.Planform.packet_valid_type, "请选择 生效类型！") == true &&
        tools.VerificationsText(_this, _this.Planform.is_month, "请选择 是否分月！") == true ) {

        if(tools.KeyUP_Cler(_this,_this.form.package_name,'','',"资费计划名称中 不能包含特殊字符！") == true &&
          tools.KeyUP_Cler(_this,_this.form.package_agentname,'','',"资费计划别称中 不能包含特殊字符！") == true){

          //系数不能小于 1
          if(_this.Planform.error_so>=1){
            let Pwd_Str = tools.encrypt(JSON.stringify(this.Planform));
            AddtariffPlan(Pwd_Str).then(response => {
              let jsonobj = JSON.parse(tools.Decrypt(response));
              //console.log(jsonobj);
              if (jsonobj.code == 200) {
                this.msgSuccess(jsonobj.msg);
                this.showPlan_ds = false;
                //更新数据
                this.queryParams.page = 1;
                this.getList();

              }else{
                let msg = jsonobj.Data!=null && jsonobj.Data!=''?jsonobj.Data:jsonobj.msg;
                this.msgError(msg);
              }
            })
          }
        }
      }
    },


    EdiePlan(){
      let _this = this;
      if (tools.VerificationsText(_this, _this.Planform.package_id, "请选择 对应择资费组！") == true &&
        tools.VerificationsText(_this, _this.Planform.packet_name, "资费计划 名称 不能为空！") == true &&
        tools.VerificationsText(_this, _this.Planform.packet_wx_name, "资费计划 别称 不能为空！") == true &&
        tools.VerificationsText(_this, _this.Planform.packet_price, "资费计划 售价 不能为空！") == true &&
        tools.VerificationsText(_this, _this.Planform.packet_cost, "资费计划 成本 不能为空！") == true &&
        tools.VerificationsText(_this, _this.Planform.in_stock, "请选择 是否在售！") == true &&
        tools.VerificationsText(_this, _this.Planform.packet_flow, "资费计划 流量 不能为空！") == true &&
        tools.VerificationsText(_this, _this.Planform.error_so, "资费计划 系数 不能为空！") == true &&
        tools.VerificationsText(_this, _this.Planform.error_flow, "资费计划 用量 不能为空！") == true &&
        tools.VerificationsText(_this, _this.Planform.wechat_pay, "请选择 是否可微信支付 ！") == true &&
        tools.VerificationsText(_this, _this.Planform.balance_pay, "请选择 是否可余额支付！") == true &&
        tools.VerificationsText(_this, _this.Planform.deduction, "请选择  是否自动缴费！") == true &&
        tools.VerificationsText(_this, _this.Planform.id, "必要参数缺失 请刷新后重试！") == true &&
        tools.VerificationsText(_this, _this.Planform.packet_valid_time, "资费计划 周期 不能为空！") == true &&
        tools.VerificationsText(_this, _this.Planform.packet_valid_name, "资费计划 周期规则 不能为空！") == true &&
        tools.VerificationsText(_this, _this.Planform.packet_valid_type, "请选择 生效类型！") == true &&
        tools.VerificationsText(_this, _this.Planform.is_month, "请选择 是否分月！") == true ) {

        if(tools.KeyUP_Cler(_this,_this.form.package_name,'','',"资费计划名称中 不能包含特殊字符！") == true &&
          tools.KeyUP_Cler(_this,_this.form.package_agentname,'','',"资费计划别称中 不能包含特殊字符！") == true){

          //系数不能小于 1
          if(_this.Planform.error_so>=1){
            let Pwd_Str = tools.encrypt(JSON.stringify(this.Planform));
            editTariffPlan(Pwd_Str).then(response => {
              let jsonobj = JSON.parse(tools.Decrypt(response));
              //console.log(jsonobj);
              if (jsonobj.code == 200) {
                this.msgSuccess(jsonobj.msg);
                this.showPlan_ds = false;
                //更新数据
                this.queryParams.page = 1;
                this.getList();

              }else{
                let msg = jsonobj.Data!=null && jsonobj.Data!=''?jsonobj.Data:jsonobj.msg;
                this.msgError(msg);
              }
            })
          }
        }
      }
    },




    /** 提交按钮 */
    submitForm: function() {
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
      let map = {};
      map.cd_id = row.cd_id;
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
    },





    /** 导出按钮操作 */
    handleExport() {

      this.getParams();
      let Pwd_Str = tools.encrypt(JSON.stringify(this.queryParams));
      exporttariffGroup(Pwd_Str).then(response => {
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

