<template>
  <div class="app-container">
    <el-row :gutter="20">

      <!-- 面板 -->
      <el-dialog :title="title" :visible.sync="editShow" width="670px" append-to-body>
        <el-form ref="form" :model="form" label-width="100px">
          <el-row>
            <el-form-item label="分类名称：" prop="category_name">
              <el-input v-model="form.category_name" placeholder="请输入 分类名称 "/>
            </el-form-item>
          </el-row>
          <el-row>
            <el-form-item label="分类编码：" prop="category_code">
              <el-input v-model="form.category_code" placeholder="请输入 分类编码 如： 10000000 【8位数字】"/>
            </el-form-item>
          </el-row>
          <el-row>
            <el-form-item label="状态：" prop="category_status">
              <el-select
                v-model="form.category_status"
                placeholder="起止条件"
                clearable
                size="small"
              >
                <el-option
                  v-for="dict in EnabledStateOptions"
                  :key="dict.dictValue"
                  :label="dict.dictLabel"
                  :value="dict.dictValue"
                />
              </el-select>
            </el-form-item>
          </el-row>
        </el-form>
        <div slot="footer" class="dialog-footer">
          <el-button v-hasPermi="['yunze:prdCategory:upd']" v-if="MyeditBtn" type="primary" @click="prdCategoryUpd">编
            辑
          </el-button>
          <el-button v-hasPermi="['yunze:prdCategory:save']" v-if="MyaddBtn" type="primary" @click="prdCategorySave">新
            增
          </el-button>
        </div>
      </el-dialog>


      <!-- :span="mainwidth" -->
      <el-col :span="4" :xs="24" v-show="option_show">

        <el-row :gutter="10" class="mb8">
          <el-col :span="1.5">
            <el-button
              type="primary"
              plain
              icon="el-icon-circle-plus-outline"
              size="mini"
              @click="save"
              v-hasPermi="['yunze:prdCategory:save']"
            >新增
            </el-button>
          </el-col>
          <right-toolbar :showSearch.sync="showSearch" v-hasPermi="['yunze:card:list']"
                         @queryTable="getList"></right-toolbar>
        </el-row>

        <div class="custom-tree-container">
          <div class="block">
            <el-tree
              :data="data"
              node-key="category_id"
              default-expand-all
              :expand-on-click-node="false">
                <span class="custom-tree-node" slot-scope="{ node, data }">
                  <span>{{ data.category_code + "  " + data.category_name }}</span>
                  <span style="margin-left: 40px;">
                    <el-button
                      type="text"
                      @click="edit(data)">
                      编辑
                    </el-button>

                    <!-- <el-button
                       type="text"
                       size="mini"
                       @click="() => append(data)">
                       Append
                     </el-button>
                     <el-button
                       type="text"
                       size="mini"
                       @click="() => remove(node, data)">
                       Delete
                     </el-button>
                     <el-button
                       type="text"
                       size="mini"
                       @click="() => add(data)">
                       add
                     </el-button>-->
                  </span>
                </span>
            </el-tree>
          </div>
        </div>
      </el-col>


      <!--用户数据-->
      <el-col :span="mainwidth" :xs="24">
        <el-form :model="queryParams" ref="queryForm" :inline="true" v-show="showSearch" label-width="68px">
          <!--分类状态   -->
          <el-form-item label="分类状态" prop="status">
            <el-select
              v-model="queryParams.category_status"
              placeholder="分类状态"
              clearable
              size="small"
              style="width: 110px"
            >
              <el-option
                v-for="dict in EnabledStateOptions"
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
          <el-form-item>
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

        <el-form :model="queryParams" ref="queryForm" :inline="true" label-width="68px" style="margin-bottom: 10px">
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
                    v-for="dict in Commodity_query"
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
              <el-button icon="el-icon-s-ticket" type="warning" plain size="mini" @click="agentShow">竖型展示</el-button>
            </el-col>
            <right-toolbar :showSearch.sync="showSearch" @queryTable="getList" :columns="columns"></right-toolbar>
          </el-row>
        </el-form>

        <el-table v-loading="loading" :data="cardList" @selection-change="handleSelectionChange">
          <el-table-column label="分类ID" align="center" prop="category_id" v-if="columns[0].visible"/>
          <el-table-column label="分类名称" align="center" prop="category_name" v-if="columns[1].visible"/>
          <el-table-column label="分类编码" align="center" prop="category_code" v-if="columns[2].visible"/>
          <el-table-column label="分类状态" align="center" prop="category_status" v-if="columns[5].visible">
            <template slot-scope="scope">
              {{ tools.getDkeyValue(EnabledStateOptions, scope.row.category_status) }}
            </template>
          </el-table-column>
          <el-table-column label="分类层级" align="center" prop="category_level" v-if="columns[3].visible"/>
          <el-table-column label="最后修改时间" align="center" prop="modified_time" v-if="columns[4].visible"/>

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

  </div>
</template>

<script>
import {listCategory, save, upd} from "@/api/yunze/commodity/commodityCategory";
import {CategoryList} from "@/api/yunze/commodity/category/category";
import {getToken} from "@/utils/auth";
import {treeselect} from "@/api/system/dept";
import tools from "@/utils/yunze/tools";

export default {
  name: "Channel",
  data() {
    return {
      tools: tools,
      //保存 编辑 按钮
      MyeditBtn: false,
      MyaddBtn: false,
      editShow: false,

      Urls: [],
      cardLists: [],
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

      data: [],


      EnabledStateOptions: [],// 系统通用启用状态
      findCr_RouteOptions: [],

      // 连接管理时间查询类型 数据字典
      flowHisTime: [],


      deptCheckStrictly: true,
      deptExpand: true,
      deptNodeAll: false,

      customize_whether: [],//自定义是否
      ExecutionTask_OutType: [],// 执行日志导出类别

      SetMealImport: false, //详情查询 套餐信息
      show_ds: false, //详情查询
      selTime: '',//时间选择
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
      // 连接管理时间查询类型 数据字典
      timetypeOptions: [],
      //商品 条件查询
      Commodity_query: [],
      // 简要套餐名称
      packageOptions: [],
      //分类状态
      Classification_status: [],
      yunze_polling_type: [],
      cd_id: undefined,
      //详情
      tablefrom: [],

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
      //任务类别
      category_type: "",
      // 任务类别 字典
      TaskOptions: [],

      // 运营商类别 字典
      operators_type: [],
      // 执行日志状态 字典
      channelStatusOptions: [],
      // 执行日志编码 字典
      channelCodeOptions: [],
      // 表单参数
      form: {
        category_status: '1',
        category_code: '',
        category_name: '',
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
        staTime: null,
        endTime: null,
        timetype: null,
        taskType: null,
        channel_id: null,
        polling_type: null,
      },
      //通过id查询
      getByIds: {
        id: ''
      },

      // 列信息
      columns: [
        {key: 1, label: `分类ID`, visible: true},
        {key: 2, label: `分类名称`, visible: true},
        {key: 3, label: `分类编码`, visible: true},
        {key: 4, label: `分类层级`, visible: true},
        {key: 5, label: `最后修改时间`, visible: true},
        {key: 6, label: `分类状态`, visible: true},
      ],
      // 表单校验
      rules: {}
    };
  },

  created() {


    //加载 查询条件
    if (window['Commodity_query'] != undefined && window['Commodity_query'] != null && window['Commodity_query'] != '') {
      this.Commodity_query = window['Commodity_query'];
    } else {
      this.getDicts("yz_BcommoditySort_selType").then(response => {
        window['Commodity_query'] = response.data;
        this.Commodity_query = window['Commodity_query'];
      });
    }

    //加载 时间赛选查询条件
    if (window['timetypeOptions'] != undefined && window['timetypeOptions'] != null && window['timetypeOptions'] != '') {
      this.timetypeOptions = window['timetypeOptions'];
    } else {
      this.getDicts("prdCategory_timet").then(response => {
        window['timetypeOptions'] = response.data;
        this.timetypeOptions = window['timetypeOptions'];
      });
    }

    //加载 通用启用状态
    if (window['EnabledStateOptions'] != undefined && window['EnabledStateOptions'] != null && window['EnabledStateOptions'] != '') {
      this.EnabledStateOptions = window['EnabledStateOptions'];
    } else {
      this.getDicts("yz_enabled_state").then(response => {
        window['EnabledStateOptions'] = response.data;
        this.EnabledStateOptions = window['EnabledStateOptions'];
      });
    }

    this.Verticaltype();


    this.getTreeselect();
    this.getList();
  },
  methods: {

    Verticaltype() {
      this.data = [];
      this.loading = true;
      let Pwd_Str = tools.encrypt(JSON.stringify({}));
      listCategory(Pwd_Str).then(response => {
          let jsonobj = JSON.parse(tools.Decrypt(response));
          if (jsonobj.code == 200) {
            this.data = jsonobj.Data;
          } else {
            this.msgError(jsonobj.msg);
          }
          this.loading = false;
        }
      );
    },

    prdCategoryUpd() {
      let _this = this;
      if (tools.VerificationsText(_this, _this.form.category_name, "请输入分类名称！") == true &&
        tools.VerificationsText(_this, _this.form.category_status, "请选择分类状态！") == true &&
        tools.VerificationsText(_this, _this.form.category_id, "必要参数缺失刷新后重试！") == true &&
        tools.VerificationsText(_this, _this.form.category_code, "请输入分类编码！") == true) {
        _this.form.parent_id = "0";//默认 分类 0
        _this.form.category_level = "1";//分层等级 1
        let Pwd_Str = tools.encrypt(JSON.stringify(_this.form));
        upd(Pwd_Str).then(response => {
            let jsonobj = JSON.parse(tools.Decrypt(response));
            if (jsonobj.code == 200) {
              let msg = jsonobj.Data;
              this.msgSuccess(msg);
              this.form = {};
              this.editShow = false;
              this.Verticaltype();
            } else {
              this.msgError(jsonobj.msg);
            }
            this.loading = false;
          }
        );

      }
    },
    prdCategorySave() {
      let _this = this;
      if (tools.VerificationsText(_this, _this.form.category_name, "请输入分类名称！") == true &&
        tools.VerificationsText(_this, _this.form.category_code, "请输入分类编码！") == true) {
        _this.form.parent_id = "0";//默认 分类 0
        _this.form.category_level = "1";//分层等级 1
        let Pwd_Str = tools.encrypt(JSON.stringify(_this.form));
        save(Pwd_Str).then(response => {
            let jsonobj = JSON.parse(tools.Decrypt(response));
            if (jsonobj.code == 200) {
              let msg = jsonobj.Data;
              this.msgSuccess(msg);
              this.form = {};
              this.editShow = false;
              this.Verticaltype();
            } else {
              this.msgError(jsonobj.msg);
            }
            this.loading = false;
          }
        );

      }

    },

    save() {
      this.form = {};
      this.title = "新增分类";
      this.MyeditBtn = false;
      this.MyaddBtn = true;
      this.editShow = true;

    },
    edit(data) {
      this.form = {};
      this.form = data;
      this.form.category_status = "" + this.form.category_status;
      this.title = "修改分类";
      this.MyeditBtn = true;
      this.MyaddBtn = false;
      this.editShow = true;

    },
    add(data) {
      const newChild = {id: id++, label: 'testtest', children: []};
      if (!data.children) {
        this.$set(data, 'children', []);
      }
      this.data.push(newChild);
    },

    append(data) {
      const newChild = {id: id++, label: 'testtest', children: []};
      if (!data.children) {
        this.$set(data, 'children', []);
      }
      data.children.push(newChild);
    },

    remove(node, data) {
      const parent = node.parent;
      const children = parent.data.children || parent.data;
      const index = children.findIndex(d => d.id === data.id);
      children.splice(index, 1);
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
      if (tools.Is_null(this.queryParams.timetype) && this.selTime != null) {
        //console.log(this.selTime);
        this.queryParams.staTime = this.selTime[0];
        this.queryParams.endTime = this.selTime[1];
      }


    },
    /*下载执行日志*/
    getDownloadExecutionTask(row) {
      let map = {};
      map.path = row.value;
      map.token = getToken();
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
      //console.log(map);
      window.open(process.env.VUE_APP_BASE_API + "/yunze/ExecutionTask/download?Pstr=" + Pwd_Str, '_blank');

    },

    //下载 xls 文件 格式
    getdownloadConversionk(row) {
      let map = {};
      map.path = row.value;
      map.token = getToken();
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
      //console.log(map);
      window.open(process.env.VUE_APP_BASE_API + "/yunze/ExecutionTask/downloadConversion?Pstr=" + Pwd_Str, '_blank');

    },


    /** 查询执行日志列表 */
    getList() {
      this.loading = true;
      this.getParams();
      this.queryParams.page = 1;
      //console.log(this.queryParams);
      let Pwd_Str = tools.encrypt(JSON.stringify(this.queryParams));
      //console.log(Pwd_Str);
      CategoryList(Pwd_Str).then(response => {
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

    agentShow() {
      let _this = this;
      _this.option_show = !_this.option_show;
      _this.mainwidth = _this.option_show ? 20 : 24;
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
      this.Verticaltype();
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
      this.ids = selection.map(item => item.cd_id);
      this.ids = selection.map(item => item.userId);
      this.single = selection.length != 1;
      this.multiple = !selection.length;
    },
    /** 新增按钮操作 */
    handleAdd() {

      this.form = {};//清空数据
      //初始化 新增 数据
      this.form.cd_code = "YiDong_EC";
      this.form.cd_operator_type = "1";
      this.form.cd_lunxun = "1";
      this.form.cd_status = "1";
      this.show_ds = true;
      this.executionTaskAdd = true;
      this.executionTaskEdie = false;
    },
    /** 修改按钮操作 */
    handleUpdate(row) {
      this.executionTaskAdd = false;
      this.executionTaskEdie = true;
      this.finddata(row);
    },
    /** 详情按钮操作 */
    ViewDetails(row) {
      //console.log(row);
      this.executionTaskAdd = false;
      this.executionTaskEdie = false;
      this.finddata(row);
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

</style>

