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
          <el-form-item label="资费组" prop="status">
            <el-select
              v-model="queryParams.type"
              placeholder="查询条件"
              clearable
              size="small"
              style="width: 110px"
            >
              <el-option
                v-for="dict in typeOptions"
                :key="dict.dictValue"
                :label="dict.dictLabel"
                :value="dict.dictValue"
              />
            </el-select>
          </el-form-item>
          <el-form-item  >
            <el-input
              v-model="queryParams.value"
              placeholder="查询值"
              clearable
              size="small"
              style="width: 200px"
              @keyup.enter.native="handleQuery"
            />
          </el-form-item>
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
          <el-form-item   label="是否拆分" >
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


          <el-form-item >
            <el-button type="primary" icon="el-icon-search" size="mini" @click="handleQuery">搜索</el-button>
            <el-button icon="el-icon-refresh" size="mini" @click="resetQuery">重置</el-button>
            <el-button size="mini" v-hasPermi="['yunze:card:listAgent']"  @click="agentShow" >公司所属</el-button>
          </el-form-item>
        </el-form>


        <el-row :gutter="10" class="mb8">
          <el-col :span="1.5">
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
          </el-col>
          <right-toolbar :showSearch.sync="showSearch"   v-hasPermi="['yunze:tariffGroup:list']" @queryTable="getList" :columns="columns"></right-toolbar>
        </el-row>
        <el-table
          :data="tariffGroupList"
        >
          <el-table-column type="expand">
            <template slot-scope="scope">
              <el-table
                :height="550"
                :key="scope.row.index"
                :data="scope.row.data"
              >
                <el-table-column  label="编号" prop="packet_id" />
                <el-table-column  label="名称" prop="packet_name" />
                <el-table-column  label="别称" prop="packet_wx_name" />

                <el-table-column  label="流量(MB)" prop="packet_flow" />
                <el-table-column  label=" / 系数" prop="error_so" />
                <el-table-column  label="用量(MB)" prop="error_flow" />
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
                </el-table-column>
              </el-table>
            </template>
          </el-table-column>
          <el-table-column label="资费组编号" align="center" key="package_id" prop="package_id"  v-if="columns[0].visible"   />
          <el-table-column label="资费组名称" align="center" key="package_name" prop="package_name"  v-if="columns[1].visible"   />
          <el-table-column label="资费组别称" align="center" key="package_agentname" prop="package_agentname"  v-if="columns[2].visible"   />
          <el-table-column label="所属公司" align="center" key="dept_name" prop="dept_name"  v-if="columns[3].visible"   />
          <el-table-column label="所属人" align="center" key="nick_name" prop="nick_name"  v-if="columns[4].visible"   />
          <el-table-column label="创建时间" align="center" key="create_time" prop="create_time"  v-if="columns[5].visible"   />

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
            <el-form-item  label="资费类型"  prop="packet_valid_name" >
              <span slot="label">
                资费类型
                <el-tooltip placement="right">
                  <div slot="content">
                    为 【基础资费】 类型 类型规则 为 【仅一次】 时，每个周期规则内 只允许充值一次；类型规则 为 【可叠加】 时，C端充值默认为周期延顺。<br/>
                    为 【叠加资费】 类型 类型规则 时，类型规则 隐藏不允许选中 类型规则 为【可叠加】。
                  </div>
                  <i class="el-icon-question"></i>
                </el-tooltip>
              </span>
              <el-select v-model="Planform.packet_type"
                         @change="packetTypeOptionsSelChange"
                         placeholder="请选择">
                <el-option
                  v-for="dict in packetTypeOptions"
                  :key="dict.dictValue"
                  :label="dict.dictLabel"
                  :value="dict.dictValue"
                />
              </el-select>
            </el-form-item>
          </el-col>
          <el-col :span="8" v-if="showBase_packet_type">
            <el-form-item label="类型规则" prop="packet_valid_name">
              <el-select  v-model="Planform.base_packet_type"
                          @change="BasePacketTypeOptionsSelChange"
                          class="el-select .el-input"   placeholder="请选择">
                <el-option
                  v-for="dict in BasePacketTypeOptions"
                  :key="dict.dictValue"
                  :label="dict.dictLabel"
                  :value="dict.dictValue"
                />
              </el-select>
            </el-form-item>
          </el-col>
          <el-col :span="8">
            <el-form-item label="禁止充值日期" prop="packet_valid_name">
              <span slot="label">
                禁充值日期
                <el-tooltip placement="right">
                  <div slot="content">
                    禁止充值日期 指的是在指定日期 【天】时 ，<br/>
                    C端用户不能充值该资费计划；<br/>
                    如指定 日期 为 【8】 ，每个月 8 号 则C端用户 不能充值该资费计划。
                    如指定 日期 为 【8,9】 ，每个月 8 号、9 号 则C端用户 不能充值该资费计划。
                  </div>
                  <i class="el-icon-question"></i>
                </el-tooltip>
              </span>
              <el-input   v-model="Planform.date_limit" placeholder="选填禁止充值日期" />
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
              <span slot="label">
                在售
                <el-tooltip placement="right">
                  <div slot="content">
                    默认为 是  <span class="ffba00">选择 否时 充值下架</span>
                  </div>
                  <i class="el-icon-question"></i>
                </el-tooltip>
              </span>
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
              <el-input  @blur="handleInput3" @keydown.native="handleInput3"   v-model="Planform.packet_flow"   placeholder="请输入资费计划流量单位(MB)" />
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
          <el-col :span="6">
            <el-form-item label="计划周期" prop="packet_valid_time" >
              <el-input @keydown.native="handleInput"  v-model="Planform.packet_valid_time"  placeholder="请输入 计划周期" />
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
          <el-col :span="6" v-if="packetValidTypeShow" >
            <el-form-item label="生效类型" prop="packet_valid_type">
               <span slot="label">
                生效类型
                <el-tooltip placement="right">
                  <div slot="content">
                      [当月生效] : 订单生产时间开始当月生效资费计划，<span class="ffba00">默认选择项</span>;<br/>
                      [次月生效] : 订单生产时间往后推一个月次月一号开始推算开始时间;<br/>
                      [周期延顺] : 当前平台记录最后到期时间往后进行推算开始时间;<br/>
                    <span class="ffba00">[激活生效] : 根据上游查询出激活时间后开始推算开始时间; 注意：该类型生效类型C端用户，不展示且不能充值！</span>
                  </div>
                  <i class="el-icon-question"></i>
                </el-tooltip>
              </span>
              <el-select v-model="Planform.packet_valid_type"  name="packet_valid_type"  placeholder="请选择">
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
            <el-form-item label="是否拆分" prop="is_month" >
              <span slot="label">
                是否拆分
                <el-tooltip placement="right">
                  <div slot="content">
                     根据 选择 的 [周期规则] 拆分   <br /> 周期计划 大于 1 时 按[周期规则]拆分 <br /> 如：计划周期 【12】， 周期规则 【月】，是否拆分 【是】，流量(MB) 【30MB】 则拆分为 12个资费计划 每月到账 设置流量 30MB
                  </div>
                  <i class="el-icon-question"></i>
                </el-tooltip>
              </span>
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
  import { listTariffGroup,addtariffGroup,findTariffGroup,editTariffGroup } from "@/api/yunze/flowcard/TariffGroup";
  import { AddtariffPlan,findTariffPlan,editTariffPlan } from "@/api/yunze/flowcard/TariffPlan";
  import { queryPackageSimple } from "@/api/yunze/flowcard/card/card";
  import { treeselect } from "@/api/system/dept";
  import Treeselect from "@riophae/vue-treeselect";
  import "@riophae/vue-treeselect/dist/vue-treeselect.css";
  import tools from "@/utils/yunze/tools";



  export default {
    name: "tariffGroup",
    components: { Treeselect },
    data() {
      return {
        time:'',


        //保存 编辑 按钮 资费组
        tariffGroupAdd: false,
        tariffGroupEdie: false,
        packetValidTypeShow: true,
        //保存 编辑 按钮 资费计划
        TariffPlanAdd: false,
        TariffPlanEdie: false,
        showBase_packet_type: true,
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
        showSearch: true,
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
        //资费计划类别
        packetTypeOptions: [],
        //资费类型规则
        BasePacketTypeOptions: [],


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
          date_limit:'',
          base_packet_type:'',
          packet_type:'',
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
      this.getList();
    },
    created() {
      this.getTreeselect();
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



      //加载 资费计划类别
      if(window['packetTypeOptions']!=undefined &&  window['packetTypeOptions']!=null && window['packetTypeOptions']!=''){
        this.packetTypeOptions = window['packetTypeOptions'];
      }else{
        this.getDicts("yunze_packet_type").then(response => {
          window['packetTypeOptions'] = response.data;
          this.packetTypeOptions = window['packetTypeOptions'];
        });
      }

      //加载 资费类型规则
      if(window['BasePacketTypeOptions']!=undefined &&  window['BasePacketTypeOptions']!=null && window['BasePacketTypeOptions']!=''){
        this.BasePacketTypeOptions = window['BasePacketTypeOptions'];
      }else{
        this.getDicts("yunze_base_packet_type").then(response => {
          window['BasePacketTypeOptions'] = response.data;
          this.BasePacketTypeOptions = window['BasePacketTypeOptions'];
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


      BasePacketTypeSelChange(){
        console.log(this.Planform.base_packet_type);
        this.Planform.base_packet_type = this.Planform.base_packet_type;
        try {
          this.$forceupdate();
        }catch (e) {

        }
      },



      /**资费类型 选择*/
      packetTypeOptionsSelChange(){
        if(tools.Is_null(this.Planform.packet_type)){
          if(this.Planform.packet_type=='1'){
            this.showBase_packet_type = false;
            this.packetValidTypeShow = false;
          } else{
            this.packetValidTypeShow = true;
            this.showBase_packet_type = true;
          }
        }
      },



      /*类型规则选择
      * */
      BasePacketTypeOptionsSelChange(){
        if(tools.Is_null(this.Planform.packet_type)==true && tools.Is_null(this.Planform.base_packet_type)==true){
          if(this.Planform.packet_type=='0' && this.Planform.base_packet_type=='1'){
            this.packetValidTypeShow = false;
          }else if(this.Planform.packet_type=='1'){
            this.packetValidTypeShow = false;
          } else{
            this.packetValidTypeShow = true;
          }
        }
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
        listTariffGroup(Pwd_Str).then(response => {
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
        this.Planform.is_month = '1';
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
        findTariffPlan(Pwd_Str).then(response => {
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
        findTariffPlan(Pwd_Str).then(response => {
            let jsonobj =  JSON.parse(tools.Decrypt(response));
            //console.log(jsonobj);
            if(jsonobj.code==200){
              jsonobj.Data.packet_valid_type = ""+jsonobj.Data.packet_valid_type;
              jsonobj.Data.packet_type = jsonobj.Data.packet_type+"";
              jsonobj.Data.base_packet_type = jsonobj.Data.base_packet_type+"";
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
        findTariffGroup(Pwd_Str).then(response => {
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
            addtariffGroup(Pwd_Str).then(response => {
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
            editTariffGroup(Pwd_Str).then(response => {
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
        let base_packet_type = _this.Planform.base_packet_type;
        let packet_type = _this.Planform.packet_type;
        if(tools.Is_null(packet_type)){
          //叠加包 默认赋值 类型
          if(packet_type=='1'){
            _this.Planform.base_packet_type = '1';//叠加
            _this.Planform.packet_valid_type = '1';//当月生效
          }else if(packet_type=='0'){
            //基础 可叠加
            if(tools.Is_null(base_packet_type)){
              if(base_packet_type=='1'){
                _this.Planform.packet_valid_type = '3';//周期延顺
              }
            }
          }
        }
        if (tools.VerificationsText(_this, _this.Planform.package_id, "请选择 对应择资费组！") == true &&
          tools.VerificationsText(_this, _this.Planform.packet_name, "资费计划 名称 不能为空！") == true &&
          tools.VerificationsText(_this, _this.Planform.packet_wx_name, "资费计划 别称 不能为空！") == true &&
          tools.VerificationsText(_this, _this.Planform.packet_price, "资费计划 售价 不能为空！") == true &&
          tools.VerificationsText(_this, _this.Planform.packet_cost, "资费计划 成本 不能为空！") == true &&
          tools.VerificationsText(_this, _this.Planform.in_stock, "请选择 是否在售！") == true &&
          tools.VerificationsText(_this, _this.Planform.packet_type, "请选择 资费类型！") == true &&
          tools.VerificationsText(_this, _this.Planform.packet_flow, "资费计划 流量 不能为空！") == true &&
          tools.VerificationsText(_this, _this.Planform.error_so, "资费计划 系数 不能为空！") == true &&
          tools.VerificationsText(_this, _this.Planform.error_flow, "资费计划 用量 不能为空！") == true &&
          tools.VerificationsText(_this, _this.Planform.wechat_pay, "请选择 是否可微信支付 ！") == true &&
          tools.VerificationsText(_this, _this.Planform.balance_pay, "请选择 是否可余额支付！") == true &&
          tools.VerificationsText(_this, _this.Planform.deduction, "请选择  是否自动缴费！") == true &&

          tools.VerificationsText(_this, _this.Planform.packet_type, "请选择  资费类型！") == true &&
          tools.VerificationsText(_this, _this.Planform.base_packet_type, "请选择  类型规则！") == true &&
          tools.VerificationsText(_this, _this.Planform.packet_valid_time, "资费计划 周期 不能为空！") == true &&
          tools.VerificationsText(_this, _this.Planform.packet_valid_name, "资费计划 周期规则 不能为空！") == true &&
          tools.VerificationsText(_this, _this.Planform.packet_valid_type, "请选择 生效类型！") == true &&
          tools.VerificationsText(_this, _this.Planform.is_month, "请选择 是否拆分！") == true ) {

          let date_limit = _this.Planform.date_limit;
          if(tools.Is_null(date_limit)){
            if(date_limit.length>2){
              if(date_limit.indexOf(",")==-1){
                this.msgError("禁止充值日期格式有误！多个日期需用英文符号 逗号 [,]隔开");
                return ;
              }
            }else{
              if(date_limit.indexOf(",")>0){
                this.msgError("禁止充值日期格式有误！,但个日期时不需要填写 [,] ");
                return ;
              }
            }
          }


          if(tools.KeyUP_Cler(_this,_this.form.package_name,'','',"资费计划名称中 不能包含特殊字符！") == true &&
            tools.KeyUP_Cler(_this,_this.form.package_agentname,'','',"资费计划别称中 不能包含特殊字符！") == true){

            if(tools.VerificationsText(_this,_this.Planform.base_packet_type, "请选择 类型规则！") == true){
              //系数不能小于 1 不能大于 5
              if(_this.Planform.error_so>=1 && _this.Planform.error_so<=5){
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
              }else {
                this.msgError("系数不能小于 1 不能大于 5！");
              }
            }
          }
        }
      },


      unique(arr) {
        return Array.from(new Set(arr))
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
          tools.VerificationsText(_this, _this.Planform.is_month, "请选择 是否拆分！") == true ) {

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
        tools.openAsk(this,'warning', "确定要删除通道名称为 '"+row.cd_name+"' 的这条数据吗？", this.deltariffGroupFun, Pwd_Str);

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


  .ffba00{
    color: #ffba00;
  }

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

