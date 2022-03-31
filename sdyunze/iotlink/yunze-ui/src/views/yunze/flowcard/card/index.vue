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
            :props="defaultProps"/>

        </div>
      </el-col>
      <!--用户数据-->
      <el-col :span="mainwidth" :xs="24">
        <el-form :model="queryParams" ref="queryForm" :inline="true" v-show="showSearch" label-width="68px">

          <el-form-item label="卡状态" prop="status">
            <el-select
              v-model="queryParams.status_ShowId"
              placeholder="卡状态"
              clearable
              size="small"
              style="width: 110px"
            >
              <el-option
                v-for="dict in stateShowId"
                :key="dict.dictValue"
                :label="dict.dictLabel"
                :value="dict.dictValue"
              />
            </el-select>
          </el-form-item>

          <el-form-item label="状态描述" prop="status" v-hasPermi="['yunze:sys:internal']">
            <el-select
              v-model="queryParams.status_id"
              placeholder="状态描述"
              clearable
              size="small"
              style="width: 110px"
            >
              <el-option
                v-for="dict in stateOptions"
                :key="dict.dictValue"
                :label="dict.dictLabel"
                :value="dict.dictValue"
              />
            </el-select>
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
          <el-form-item label="已达停机阈值" label-width="100px">
            <el-switch @change="changeExceedsThreshold" active-value="1" inactive-value="0"
                       v-model="queryParams.exceedsThreshold" active-color="#13ce66">
            </el-switch>
          </el-form-item>
          <br>


        </el-form>

        <el-form :model="queryParams" ref="queryForm" :inline="true" v-show="showSearch">
          <el-form-item prop="status">
            <span class="el-form-item__label" style="font-weight: 700;"
                  title="联通卡号选择ICCID查询时 需选择 运营商类型 为 '联通' ! ">起止条件 </span>
            <el-select
              title="联通卡号选择ICCID查询时 需选择 运营商类型 为 '联通' ! "
              v-model="queryParams.StartAndEndtype"
              placeholder="起止条件"
              clearable
              size="small"
              style="width: 110px"
            >
              <el-option
                title="联通卡号选择ICCID查询时 需选择 运营商类型 为 '联通' ! "
                v-for="dict in StartAndEndOptions"
                :key="dict.dictValue"
                :label="dict.dictLabel"
                :value="dict.dictValue"
              />
            </el-select>
          </el-form-item>
          <el-form-item>
            <el-input
              v-model="queryParams.StartValue"
              placeholder="开始号段"
              clearable
              size="small"
              style="width: 200px"
              @keyup.enter.native="handleQuery"
            />
          </el-form-item>
          <el-form-item>
            <el-input
              v-model="queryParams.EndValue"
              placeholder="结束号段"
              clearable
              size="small"
              style="width: 200px"
              @keyup.enter.native="handleQuery"
            />
          </el-form-item>

          <el-form-item>
            <span class="el-form-item__label" style="font-weight: 700;">分组</span>
            <el-select
              v-model="queryParams.customize_grouping"
              placeholder="所属分组"
              clearable
              size="small"
              style="width: 110px"
            >
              <el-option
                v-for="dict in CardGrouping"
                :label="dict"
                :value="dict"
              />
            </el-select>
          </el-form-item>


          <el-form-item label="多维度">
            <el-select
              v-model="queryParams.dimensionField"
              placeholder="维度字段"
              clearable
              style="width: 110px"
            >
              <el-option
                v-for="dict in dimensionFieldOptions"
                :key="dict.dictValue"
                :label="dict.dictLabel"
                :value="dict.dictValue"
              />
            </el-select>
          </el-form-item>
          <el-form-item>
            <el-input
              v-model="queryParams.dimensionValue"
              placeholder="维度值"
              clearable
              size="small"
              style="width: 350px"
              @keyup.enter.native="handleQuery"
            >
              <el-select
                v-model="queryParams.dimensionType"
                placeholder="维度条件"
                clearable
                slot="prepend"
                style="width: 110px"
              >
                <el-option
                  v-for="dict in dimensionTypeOptions"
                  :key="dict.dictValue"
                  :label="dict.dictLabel"
                  :value="dict.dictValue"
                />
              </el-select>
            </el-input>
          </el-form-item>


          <el-row :gutter="10" class="mb8" style="margin-left: 15px;">
            <el-form-item prop="status">
            <span class="el-form-item__label" style="font-weight: 700;"
                  title="联通卡号选择ICCID查询时 需选择 运营商类型 为 '联通' ! ">运营商</span>
              <el-select
                title="联通卡号选择ICCID查询时 需选择 运营商类型 为 '联通' ! "
                v-model="queryParams.cd_operator_type"
                placeholder="运营商"
                clearable
                multiple
                size="small"
                style="width: 270px"
              >
                <el-option
                  v-for="dict in operators_type"
                  :key="dict.dictValue"
                  :label="dict.dictLabel"
                  :value="dict.dictValue"
                />
              </el-select>
            </el-form-item>

            <el-form-item>
            <span class="el-form-item__label" style="font-weight: 700;"
                  v-hasPermi="['yunze:card:queryParamsChannelId']">所属通道</span>
              <el-select
                v-model="queryParams.channel_id"
                placeholder="所属通道"
                clearable
                multiple
                v-hasPermi="['yunze:card:queryParamsChannelId']"
                size="small"
                style="width: 550px"
              >
                <el-option
                  v-for="dict in RouteOptions"
                  :key="dict.cd_id"
                  :label="dict.cd_alias"
                  :value="dict.cd_id"
                />
              </el-select>
            </el-form-item>

            <el-form-item label="资费组" prop="status">
              <el-select
                v-hasPermi="['yunze:cardFlow:queryPackageSimple']"
                v-model="queryParams.package_id"
                placeholder="资费组"
                clearable
                multiple
                size="small"
                style="width: 550px"
              >
                <el-option
                  v-for="dict in packageOptions"
                  :key="dict.package_id"
                  :label="dict.package_agentname"
                  :value="dict.package_id"
                />
              </el-select>
            </el-form-item>

          </el-row>

        </el-form>

        <el-form :model="queryParams" ref="queryForm" :inline="true" style="margin-bottom: 10px;">
          <el-row :gutter="10">
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
              <el-dropdown>
              <el-button type="primary" icon="el-icon-search" size="mini" @click="handleQuery">搜索</el-button>
                <el-dropdown-menu slot="dropdown">
                  <el-dropdown-item>
                    <el-button
                      type="primary"
                      size="mini"
                      @click="Importquery"
                      v-hasPermi="['yunze:card:ImportQuery']"
                    >导入查询
                    </el-button>
                  </el-dropdown-item>
                </el-dropdown-menu>
              </el-dropdown>
            </el-col>
            <el-col :span="1.5">
              <el-button icon="el-icon-refresh" size="mini" @click="resetQuery">重置</el-button>
            </el-col>
            <el-col :span="1.5">
              <el-button size="mini" v-hasPermi="['yunze:card:listAgent']" @click="agentShow">公司所属</el-button>
            </el-col>

            <el-col :span="1.5">
              <el-button
                type="info"
                plain
                icon="el-icon-upload2"
                size="mini"
                @click="handleImport"
                v-hasPermi="['yunze:card:import']"
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
                v-hasPermi="['yunze:card:exportData']"
              >导出
              </el-button>
            </el-col>
            <el-col :span="1.5">
              <el-button
                type="danger"
                plain
                icon="el-icon-link"
                size="mini"
                @click="handleDivide"
                v-hasPermi="['yunze:card:divide']"
              >划卡
              </el-button>
            </el-col>
            <!--批量操作-->
            <el-col :span="1.5">
              <el-dropdown>
                <el-button
                  type="warning"
                  plain
                  icon="el-icon-user-solid"
                  size="mini"
                  style="width: 110px;"
                >批量操作
                </el-button>
                <el-dropdown-menu slot="dropdown">
                  <router-link :to="'/specialOperation/type/cardInfoEdit'" class="link-type">
                    <el-dropdown-item>
                      <el-button
                        type="success"
                        plain
                        icon="el-icon-edit-outline"
                        style="width: 150px;margin-bottom:10px;"
                        size="mini"
                        v-hasPermi="['yunze:card:importSetCardInfo']"
                      >批量分组及备注
                      </el-button>
                    </el-dropdown-item>
                  </router-link>
                  <router-link :to="'/specialOperation/type/selImei'" class="link-type">
                    <el-dropdown-item>
                      <el-button
                        type="success"
                        plain
                        icon="el-icon-mobile-phone"
                        style="width: 150px;margin-bottom:10px;"
                        size="mini"
                        v-hasPermi="['yunze:card:importSelImei']"
                      >查询IMEI
                      </el-button>
                    </el-dropdown-item>
                  </router-link>
                  <router-link :to="'/specialOperation/type/Selbatch'" class="link-type">
                    <el-dropdown-item>
                      <el-button
                        type="danger"
                        plain
                        icon="el-icon-setting"
                        style="width: 150px;margin-bottom:10px;"
                        size="mini"
                        v-hasPermi="['yunze:card:ApiUpdBatch']"
                      >批量停复机
                      </el-button>
                    </el-dropdown-item>
                  </router-link>
                  <router-link :to="'/specialOperation/type/realname'" class="link-type">
                    <el-dropdown-item>
                      <el-button
                        type="danger"
                        plain
                        icon="el-icon-user"
                        style="width: 150px;margin-bottom:10px;"
                        size="mini"
                        v-hasPermi="['yunze:card:cancelrealname']"
                      >批量取消实名
                      </el-button>
                    </el-dropdown-item>
                  </router-link>
                    <router-link :to="'/specialOperation/type/cardInfoReplace'" class="link-type"  >
                      <el-dropdown-item>
                        <el-button
                          type="warning"
                          plain
                          icon="el-icon-setting"
                          size="mini"
                          v-hasPermi="['yunze:card:replace']"
                        >批量更新卡信息
                        </el-button>
                      </el-dropdown-item>
                    </router-link>

                </el-dropdown-menu>
              </el-dropdown>
            </el-col>

            <el-col :span="1.5">
              <el-button
                type="success"
                plain
                icon="el-icon-check"
                size="mini"
                @click="ShowUpdInfo()"
              >勾选操作
              </el-button>
            </el-col>
<!--            <el-col :span="1.5">-->
<!--              <el-button-->
<!--                type="success"-->
<!--                plain-->
<!--                v-if="IsUpdInfo"-->
<!--                icon="el-icon-edit-outline"-->
<!--                size="mini"-->
<!--                @click="Updatenotfilledin()"-->
<!--                v-hasPermi="['yunze:card:importSetCardInfo']"-->
<!--              >变更分组及备注-->
<!--              </el-button>-->
<!--            </el-col>-->

            <!--勾选操作-->
            <el-dropdown v-if="IsUpdInfo">
              <el-button
                type="success"
                plain
                icon="el-icon-user-solid"
                size="mini"
                style="width: 110px;"
              >操作
              </el-button>
              <el-dropdown-menu slot="dropdown">
                <el-dropdown-item @click.native="PublicMethod"/>
                <el-dropdown-item v-hasPermi="['yunze:card:ConsumptionArr']" @click.native="Consumption">同步用量</el-dropdown-item>
                <el-dropdown-item v-hasPermi="['yunze:card:PublicMethodArr']" @click.native="State">同步状态</el-dropdown-item>
                <el-dropdown-item v-hasPermi="['yunze:card:ConsumptionAndStateArr']"@click.native="ConsumptionAndState">同步用量和状态</el-dropdown-item>
                <el-dropdown-item  v-hasPermi="['yunze:card:importSetCardInfo']" @click.native="Updatenotfilledin()"  >变更分组及备注</el-dropdown-item>
                <el-dropdown-item v-hasPermi="['yunze:card:Stopped']" @click.native="Stopped">停机</el-dropdown-item>
                <el-dropdown-item v-hasPermi="['yunze:card:MachineArr']" @click.native="Machine">复机</el-dropdown-item>
                <el-dropdown-item v-hasPermi="['yunze:card:DisconnectNetworkArr']" @click.native="DisconnectNetwork">断网
                </el-dropdown-item>
                <el-dropdown-item v-hasPermi="['yunze:card:OpenNetworkArr']" @click.native="OpenNetwork">开网
                </el-dropdown-item>
              </el-dropdown-menu>
            </el-dropdown>

            <right-toolbar :showSearch.sync="showSearch" v-hasPermi="['yunze:card:list']" @queryTable="getList"
                           :columns="columns"></right-toolbar>
          </el-row>
        </el-form>


        <el-table v-loading="loading" :data="cardList" ref="multipleTable" @selection-change="handleSelectionChange">
          <el-table-column type="selection" v-if="IsUpdInfo" width="55" align="center"/>
          <el-table-column :label="columns[0].label" align="center" v-if="columns[0].visible" width="110">
            <template slot-scope="scope">
                <span @click="ViewDetails(scope.row)" class="link-type"
                      v-hasPermi="['yunze:card:find']">{{ scope.row.vid }}</span>
            </template>
          </el-table-column>
          <el-table-column :label="columns[1].label" align="center" v-if="columns[1].visible" key="msisdn"
                           prop="msisdn"
                           :show-overflow-tooltip="true" width="130"/>
          <el-table-column :label="columns[2].label" align="center" key="iccid" prop="iccid" v-if="columns[2].visible"
                           :show-overflow-tooltip="true" width="200"/>
          <el-table-column :label="columns[3].label" align="center" key="imsi" prop="imsi" v-if="columns[3].visible"
                           :show-overflow-tooltip="true" width="140"/>
          <el-table-column :label="columns[4].label" align="center" key="status_ShowId" prop="status_ShowId"
                           v-if="columns[4].visible" width="140">
            <template slot-scope="scope">
              {{ tools.getDkeyValue(stateShowId, scope.row.status_ShowId) }}
            </template>
          </el-table-column>
          <el-table-column :label="columns[16].label" align="center" key="status_id" v-if="columns[16].visible"
                           width="100">
            <template slot-scope="scope" v-if="internalBool==true">
              {{ tools.getDkeyValue(stateOptions, scope.row.status_id) }}
            </template>
          </el-table-column>
          <el-table-column :label="columns[12].label" align="center" key="channel_id" prop="channel_id"
                           v-show="columns[12].visible " :show-overflow-tooltip="true">
            <template slot-scope="scope">
              <span
                v-if="internalBool==true">{{
                  tools.getDkeyValue(findCr_RouteOptions, '' + scope.row.channel_id)
                }}</span>
              <span
                v-if="internalBool==false">{{
                  tools.getkeyValue(RouteOptions, '' + scope.row.channel_id, "cd_id", "cd_alias")
                }}</span>
            </template>
          </el-table-column>
          <el-table-column :label="columns[6].label" key="open_date" prop="open_date" v-if="columns[6].visible"
                           width="100"/>
          <el-table-column :label="columns[11].label" align="center" key="customize_grouping"
                           prop="customize_grouping"
                           v-if="columns[11].visible" :show-overflow-tooltip="true" width="100"/>
          <el-table-column :label="columns[13].label" align="center" key="deliver_date" prop="deliver_date"
                           v-if="columns[13].visible" width="150"/>
          <el-table-column :label="columns[7].label" align="center" key="agent_id" prop="agent_id"
                           v-if="columns[7].visible">
            <template slot-scope="scope">
              {{ tools.getDeptNamekeyValue(DeptOptions, scope.row.agent_id) }}
            </template>
          </el-table-column>
          <el-table-column :label="columns[15].label" align="center" prop="package_id" v-if="columns[15].visible">
            <template slot-scope="scope">
              {{ tools.getkeyValue(packageOptions, scope.row.package_id, "package_id", "package_agentname") }}
            </template>
          </el-table-column>
          <el-table-column :label="columns[8].label" align="center" key="batch_date" prop="batch_date"
                           v-if="columns[8].visible" width="150"/>
          <el-table-column :label="columns[9].label" align="center" key="syn_Time" prop="syn_Time"
                           v-if="columns[9].visible" width="150"/>
          <el-table-column :label="columns[10].label" align="center" key="used" prop="used" v-if="columns[10].visible"
                           width="150"/>
          <el-table-column :label="columns[17].label" align="center" key="activate_date" prop="activate_date"
                           v-if="columns[17].visible"/>
          <el-table-column :label="columns[14].label" align="center" key="due_expire_time" prop="due_expire_time"
                           v-if="columns[14].visible"/>
          <el-table-column :label="columns[5].label" align="center" key="remarks" prop="remarks"
                           v-if="columns[5].visible" :show-overflow-tooltip="true" width="150"/>
          <el-table-column
            label="操作"
            align="center"
            width="90"
            class-name="small-padding fixed-width"
          >
            <template slot-scope="scope">
              <el-dropdown>
                <el-button type="primary" size="small">
                  操作<i class="el-icon-arrow-down el-icon--right"></i>
                </el-button>
                <el-dropdown-menu slot="dropdown">
                  <el-dropdown-item v-hasPermi="['yunze:card:SynFlow']" @click.native="SynFlow(scope.row.iccid)">同步用量
                  </el-dropdown-item>
                  <el-dropdown-item v-hasPermi="['yunze:card:SynStatus']" @click.native="SynStatus(scope.row.iccid)">
                    同步状态
                  </el-dropdown-item>
                  <el-dropdown-item v-hasPermi="['yunze:card:diagnosis']">
                    <router-link :to="'/card/diagnosis/'+ tools.encrypt(JSON.stringify({'cardNo':scope.row.iccid}))"
                                 class="link-type">
                      <span>智能诊断</span>
                    </router-link>
                  </el-dropdown-item>
                  <el-dropdown-item v-hasPermi="['yunze:card:SynActivateDate']"
                                    @click.native="SynActivateDate(scope.row.iccid)">同步激活时间
                  </el-dropdown-item>
                  <el-dropdown-item v-hasPermi="['yunze:card:CardBinding']"
                                    @click.native="CardBinding(scope.row.iccid)">机卡解绑
                  </el-dropdown-item>
                  <el-dropdown-item v-hasPermi="['yunze:flow:list']">
                    <router-link :to="'/card/type/cardFlow/'+ scope.row.iccid" class="link-type">
                      <span>订购资费查询</span>
                    </router-link>
                  </el-dropdown-item>
                  <el-dropdown-item v-hasPermi="['yunze:flowhis:list']">
                    <router-link :to="'/card/type/cardFlowHis/'+ scope.row.iccid" class="link-type">
                      <span>用量详情</span>
                    </router-link>
                  </el-dropdown-item>
                  <el-dropdown-item>
                    <router-link :to="'/card/type/cardChange/'+ scope.row.iccid" class="link-type">
                      <span>卡信息变更查询</span>
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


    <!-- 详情查看 -->
    <el-drawer
      :with-header="false"
      :visible.sync="show_ds"
      :close-on-click-modal="false"
      size="90%">
      <el-divider content-position="left">{{ cardInfoTitle }}
        <el-button @click="Close" size="mini" type="primary" style="margin-left: 20px">关闭</el-button>
      </el-divider>
      <el-card shadow="always">
        <el-table
          :data="tablefrom"
          :show-header="false"
          border>
          <el-table-column prop="label1">
            <template slot-scope="scope">
              <b>
                <div :style="{'color':scope.row.label1? 'black':'black'}">
                  {{ scope.row.label1 }}
                </div>
              </b>
            </template>
          </el-table-column>
          <el-table-column
            prop="value1"
          />
          <el-table-column prop="label2">
            <template slot-scope="scope">
              <b>
                <div :style="{'color':scope.row.label2? 'black':'black'}">
                  {{ scope.row.label2 }}
                </div>
              </b>
            </template>
          </el-table-column>
          <el-table-column
            prop="value2"
          />
          <el-table-column prop="label3">
            <template slot-scope="scope">
              <b>
                <div :style="{'color':scope.row.label3? 'black':'black'}">
                  {{ scope.row.label3 }}
                </div>
              </b>
            </template>
          </el-table-column>
          <el-table-column
            prop="value3"
          />

        </el-table>
        <el-table
          :data="tablefrom_remarks"
          :show-header="false"
          border>
          <el-table-column prop="label1" width="285px">
            <template slot-scope="scope">
              <b>
                <div :style="{'color':scope.row.label1? 'black':'black'}">
                  {{ scope.row.label1 }}
                </div>
              </b>
            </template>
          </el-table-column>
          <el-table-column
            prop="value1"
          />
        </el-table>


      </el-card>


      <el-divider content-position="left">{{ OnlineTitle }}   <!--【{{form.}}}】-->

        <el-button @click="getOnlineStatus" size="mini" type="primary" style="margin-left: 40px">刷新</el-button>
      </el-divider>


      <el-card shadow="always">

        <el-table
          :data="table_Online"
          :show-header="false"
          border>
          <el-table-column prop="label1" width="285px">
            <template slot-scope="scope">
              <b>
                <div :style="{'color':scope.row.label1? 'black':'black'}">
                  {{ scope.row.label1 }}
                </div>
              </b>
            </template>
          </el-table-column>
          <el-table-column
            prop="value1"
          />
          <el-table-column prop="label2" width="285px">
            <template slot-scope="scope">
              <b>
                <div :style="{'color':scope.row.label2? 'black':'black'}">
                  {{ scope.row.label2 }}
                </div>
              </b>
            </template>
          </el-table-column>
          <el-table-column
            prop="value2"
          />
        </el-table>
      </el-card>
      <!--      {{Changequery}}-->
      <el-divider content-position="left">

        <!--        <el-button @click="getOnlineStatus"  type="primary" style="margin-left: 40px" >展现变更</el-button>-->
        <!--        <el-button @click="getOnlineStatus" style="margin-left: 20px" >查看更多</el-button>-->
      </el-divider>
      <br><br><br><br>

      <el-steps align-center :active="cardChangeList.length"
      >
        <el-step :title="item.createTime" :description="item.remark+'\n'+'变更前 '+item.cbefore+' 变更后 '+item.cafterward"
                 v-for="item in  cardChangeList">

          <h4>{{ item.remark }}</h4>remark
          <p>{{}}</p>


        </el-step>

        <!--  :description="'变更前 '+item.cbefore+' 变更后 '+item.cafterward"       <el-step :title="o.createTime" :description="o.cbefore"></el-step>-->
      </el-steps>

    </el-drawer>


    <!-- 卡板导入对话框 -->
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
          <!--<el-checkbox v-model="upload.updateSupport" />是否更新已经存在的用户数据-->
          <el-link type="info" style="font-size:12px" @click="importTemplate">下载模板</el-link>
        </div>
        <div class="el-upload__tip" style="color:red" slot="tip">提示：仅允许导入“xls”或“xlsx”格式文件！</div>
      </el-upload>
      <div slot="footer" class="dialog-footer">
        <el-button type="primary" v-if="submitFileFormBtn" @click="submitFileForm">确 定</el-button>
        <el-button @click="upload.open = false">取 消</el-button>
      </div>
    </el-dialog>

    <!--是否更新未填写属性-->
    <el-dialog :title="notfilledin" :visible.sync="fill" width="950px" append-to-body>
      <el-form ref="form" :model="UpdInfo" label-width="80px">
        <el-divider></el-divider>
        <template>
          <el-form-item label="分组编辑 :" prop="customize_grouping" label-width="100px">
            <el-input v-model="UpdInfo.customize_grouping" placeholder="请输入内容"></el-input>
          </el-form-item>
          <br>
          <!--  发货日期-->
          <el-form-item label="发货日期" prop="status" label-width="100px">
            <el-date-picker
              v-model="UpdInfo.deliver_date"
              placeholder="选择日期"
              size="small"
              type="date"
              format="yyyy-MM-dd"
              value-format="yyyy-MM-dd"
              :picker-options="deliverOptions">
            </el-date-picker>
          </el-form-item>
          <br>
          <el-form-item label="备注 :" prop="remarks" label-width="100px">
            <el-input v-model="UpdInfo.remarks" type="textarea" placeholder="请输入内容"></el-input>
          </el-form-item>
          <el-form-item label="是否更新未填写属性 :" label-width="150px">
            <template>

              <el-radio-group>
                <label class="el-radio " v-for="(item,index) in buttonyesno">
                                  <span class="el-radio__input my_checkbox__inner">
                                  <input type="radio" v-model="UpdInfo.Button"
                                         name="Button"
                                         :value="item.dictValue"/>
                                   </span>
                  <span class="el-radio__label ">{{ item.dictLabel }}</span>
                </label>
              </el-radio-group>
            </template>
          </el-form-item>
        </template>
      </el-form>
      <div slot="footer" class="dialog-footer">
        <el-button type="primary" @click="fillForm">确 定</el-button>
        <el-button @click="cancel">取 消</el-button>
      </div>
    </el-dialog>

    <!-- 导入查询 -->
    <el-dialog :title="updatel.title" :visible.sync="updatel.Number" width="400px" append-to-body>
      <el-form ref="form" :model="form" label-width="80px">
        <el-col :span="35">
          <el-form-item prop="status">
            <span slot="label">
                  号码类型
              </span>
            <el-tooltip placement="right">
              <div slot="content">
                1:下载模板2:选择您的号码类型3:点击上传文件、然后确定4:列如你上传100个号码,可以在此界面进行筛选,
              </div>
              <div slot="content">
                添加筛选条件点击搜索,
                如想取消此页面,可以点击重置或者关闭页面
              </div>
              <i class="el-icon-question"></i>
            </el-tooltip>
            <el-select
              v-model="queryParams.UpType"
              placeholder="请输入您的卡号"
              clearable
              size="small"
              @change="SelectChange()"
              style="width: 200px"
            >
              <el-option
                v-for="dict in cardNumber"
                :key="dict.dictValue"
                :label="dict.dictLabel"
                :value="dict.dictValue"
              />
            </el-select>
          </el-form-item>

        </el-col>

      </el-form>

      <el-upload
        ref="number"
        :limit="1"
        accept=".xlsx, .xls"
        :headers="number.headers"
        :action="number.url"
        :disabled="number.isUploading"
        :on-progress="handleFileUploadProgress"
        :on-success="handlenumberFileSuccess"
        :auto-upload="false"
        :data="{Pstr:this.updatel.Pstr}"
        drag
      >
        <i class="el-icon-upload"></i>
        <div class="el-upload__text">
          将文件拖到此处，或
          <em>点击上传</em>
        </div>
        <div class="el-upload__tip" slot="tip">
          <el-link type="info" style="font-size:12px" @click="importCardSel">下载模板</el-link>
        </div>
        <div class="el-upload__tip" style="color:red" slot="tip">提示：仅允许导入“xls”或“xlsx”格式文件！</div>
      </el-upload>
      <div slot="footer" class="dialog-footer">
        <el-button type="primary" v-if="submitFileFormBtn" @click="submitNumber">确 定</el-button>
        <el-button @click="updatel.Number = false">取 消</el-button>
      </div>
    </el-dialog>


    <!-- 卡号划分 -->
    <el-dialog :title="title" :visible.sync="Divide_show" width="960px" append-to-body>
      <el-form ref="form" :model="form" label-width="80px">
        <el-row>
          <el-col :span="12">

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
              <el-tree :data="deptOptions" :check-on-click-node="false" ref="tree" :filter-node-method="filterNode"
                       show-checkbox :check-strictly="true" default-expand-all node-key="id"
                       :props="defaultProps" @check-change="treeNodeClick" empty-text="暂无数据">
              </el-tree>
            </div>
          </el-col>
          <el-col :span="12">
            <el-form-item label="所属用户" prop="cd_code">
              <el-select v-model="formDivide.user_id" placeholder="请选择">
                <el-option
                  v-for="item in userArr"
                  :key="item.user_id"
                  :label="item.nick_name"
                  :value="item.user_id"
                ></el-option>
              </el-select>
            </el-form-item>
          </el-col>
        </el-row>

      </el-form>
      <div slot="footer" class="dialog-footer">
        <el-button v-hasPermi="['yunze:card:divide']" v-if="Dividebtn" type="primary" @click="Divide">保 存</el-button>
      </div>
    </el-dialog>


    <!-- 解绑 -->
    <el-dialog title="机卡解绑" :visible.sync="CardBindingShow" width="660px" append-to-body>
      <el-form ref="form" :model="CardBindingForm" label-width="180px">
        <el-row>
          <el-form-item label="解绑联系人：" prop="cd_code">
            <el-input v-model="CardBindingForm.contactName" placeholder="请输入解绑联系人如 ：YunXi "/>
          </el-form-item>
        </el-row>
        <el-row>
          <el-form-item label="解绑联系人手机号：" prop="cd_code">
            <el-input v-model="CardBindingForm.contactPhone" placeholder="请输入解绑联系人手机号如：13378 "/>
          </el-form-item>
        </el-row>
      </el-form>
      <div slot="footer" class="dialog-footer">
        <el-button v-hasPermi="['yunze:card:CardBinding']" :v-if="CardBindingSaveBtn" type="primary"
                   @click="CardBindingSave">解 绑
        </el-button>
      </div>
    </el-dialog>


  </div>
</template>

<script>
import {
  listCard,
  getCard,
  queryflowSimple,
  queryOnlineStatus,
  queryPackageSimple,
  exportCard,
  queryflowData,
  importTemplate,
  findDeptUser,
  DivideCard,
  SynCarStatus,
  SynCarFlow,
  CarUntie,
  getCardGrouping,
  findCr,
  UpdateFill,
  StoppedArr,
  MachineArr,
  DisconnectNetworkArr,
  OpenNetworkArr,
  ConsumptionArr,
  PublicMethodArr,
  ConsumptionAndStateArr,
  SynCardActivateDate
} from "@/api/yunze/flowcard/card/card";
import {getToken} from "@/utils/auth";
import {treeselect} from "@/api/system/dept";
import Treeselect from "@riophae/vue-treeselect";
import "@riophae/vue-treeselect/dist/vue-treeselect.css";
import tools from "@/utils/yunze/tools";
import {getselTime} from "@/api/yunze/flowcard/card/change";


export default {
  name: "Card",
  components: {Treeselect},
  data() {
    return {

      findCr_RouteOptions: [],// API通道信息
      cardChangeList: [
        // {"cbefore":"正常","cafterward":"停机","createTime":"createTime","remark":"执行成功！"},
        // {"cbefore":"停机","cafterward":"正常","createTime":"createTime","remark":"执行失败……！"},
        // {"cbefore":"停机","cafterward":"正常","createTime":"createTime","remark":"执行失败……！"},
        // {"cbefore":"停机","cafterward":"正常","createTime":"createTime","remark":"执行失败……！"},

      ],//卡状态变更数组
      submitFileFormBtn: true,//导入 btn
      Dividebtn: true,//划卡 btn
      CardBindingSaveBtn: true,//机卡解绑 btn
      internalBool: false,//内部人员
      CardBindingForm: {
        bind_type: '2',
        imei: null,
        contactName: null,
        iccid: null,
        contactPhone: null
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

      deliverOptions: {
        shortcuts: [{
          text: '今天',
          onClick(picker) {
            picker.$emit('pick', tools.gitData());
          }
        }, {
          text: '昨天',
          onClick(picker) {
            picker.$emit('pick', tools.getBeforeDate(1));
          }
        }, {
          text: '一周前',
          onClick(picker) {
            picker.$emit('pick', tools.getBeforeDate(7));
          }
        }]
      },

      // 表单参数
      form: {},
      tools: tools,
      deptCheckStrictly: true,
      deptExpand: true,
      deptNodeAll: false,
      Divide_show: false,//卡划分
      cardDivide: false,//卡划分 保存
      CardBindingShow: false,//机卡绑定 操作界面
      dialogVisible: false,
      treeData: [],
      selectList: [],
      userArr: [],//企业下用户数组
      //降序
      ORDER_BY_type: [],
      StartAndEndOptions: [],// 起止条件查询类别
      table_Online: [],//在线状态信息
      DeptOptions: [],// 公司信息
      RouteOptions: [],// API通道信息
      tablefrom_remarks: [],//备注信息
      cardInfoTitle: '卡基础信息 (信息同步中)',
      cardPackageTitle: '资费信息 (信息同步中)',
      OnlineTitle: '在线信息 (信息同步中)',
      // Changequery: '卡信息变更记录查询记录',

      access_type: [],//卡板接入方式
      online_status: [],//卡板在线状态
      card_types: [],//卡类型
      network_types: [],//网络类型
      customize_whether: [],//自定义是否



      DxCMP_netModel_type: [],//电信CMP无线接入模式
      DxCMP_RAT: [],//电信CMP接入类型
      DxCMP_online: [],//电信CMP在线状态

      SetMealImport:false, //详情查询 套餐信息
      ifsel:false, //详情查询 只是只读
      show_ds:false, //详情查询
      selTime:[],//时间选择
      mainwidth:24,//宽度
      option_show:false, //公司所属查询

      // 遮罩层
      loading: true,
      // 导出遮罩层
      exportLoading: false,
      //是否更新未填写属性
      fill: false,
      notfilledin: "",
      // 选中数组
      ids: [],
      // 非单个禁用
      single: true,
      // 非多个禁用
      multiple: true,
      // 显示搜索条件
      showSearch: false,
      // 显示 变更按钮
      IsUpdInfo: false,
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
      CardGrouping: [],//分组
      createTime: [],

      dimensionFieldOptions: [],//维度字段
      dimensionTypeOptions: [],//维度条件

      //详情
      tablefrom: [],

      FlowData: [],
      selIccid: [{FlowData: []}],
      activeNames: [0],

      //卡状态
      status_type: "",
      //卡号类别
      cardNumber: "",
      //断开网状态
      cardConnectionTypeOptions: [],
      // 移动 归属地 字典
      YDcard_attributionOptions: [],
      // 卡状态描述 字典
      stateOptions: [
        {
          dictLabel: ''
        }
      ],
      // 卡状态
      stateShowId: [],
      // 运营商类别 字典
      operators_type: [],

      // 性别状态字典
      sexOptions: [],
      // 岗位选项
      postOptions: [],
      // 角色选项
      roleOptions: [],
      // 表单参数
      UpdInfo: {
        Button: "0",
        customize_grouping: null,
        remarks: null,
        iccids: null,
        deliver_date: null,
      },
      //操作参数
      Operation: {
        Stopped: null,
        iccids: null,
      },
      formDivide: {
        dept_id: '',
        user_id: '',
      },
      updatel: {
        title: '',
        Number: false,
        Pstr: null,
      },
      defaultProps: {
        children: "children",
        label: "label"
      },
      buttonyesno: [],
      statusOptions: [],
      // 用户导入参数
      upload: {
        // 是否显示弹出层（用户导入）
        open: false,
        Number: false,
        // 弹出层标题（用户导入）
        title: "",
        // 是否禁用上传
        isUploading: false,
        // 是否更新已经存在的用户数据
        updateSupport: 0,
        // 设置上传的请求头部
        headers: {Authorization: "Bearer " + getToken()},
        // 上传的地址
        url: process.env.VUE_APP_BASE_API + "/yunze/card/importData"
      },

      // 卡号 导入参数
      number: {
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
        url: process.env.VUE_APP_BASE_API + "/yunze/card/cardNumber",
        Pstr: '',//携带参数
      },

      //当前查询参数
      sel: {
        iccid: '',
        agent_id: '',
        package_id: '',
      },
      // 查询参数
      queryParams: {
        UpType: null,
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
        channel_id: [],
        del_flag: null,
        status_id: null,
        package_id: [],
        StartAndEndtype: null,
        StartValue: null,
        EndValue: null,
        cd_operator_type: [],
        cardNumber: null,
        UpArr: [],
        rat: null,
        customize_grouping: null,
        deliver_date: null,
        status_ShowId: null,
        dimensionField: null,
        dimensionType: null,
        dimensionValue: null,
        exceedsThreshold: 0,
      },
      columns: [
        {key: 0, label: `虚拟编号`, visible: true},
        {key: 1, label: `MSISDN`, visible: true},
        {key: 2, label: `ICCID`, visible: true},
        {key: 3, label: `IMSI`, visible: false},
        {key: 4, label: `卡状态`, visible: true},
        {key: 5, label: `备注信息`, visible: true},
        {key: 6, label: `开卡日期`, visible: false},
        {key: 7, label: `所属公司`, visible: true},
        {key: 8, label: `入库日期`, visible: false},
        {key: 9, label: `同步时间`, visible: false},
        {key: 10, label: `用量`, visible: true},
        {key: 11, label: `分组`, visible: true},
        {key: 12, label: `通道`, visible: true},
        {key: 13, label: `发货日期`, visible: true},
        {key: 14, label: `到期日期`, visible: true},
        {key: 15, label: `资费组`, visible: true},
        {key: 16, label: `卡状态描述`, visible: false},
        {key: 17, label: `激活日期`, visible: true},
      ],
      // 表单校验
      rules: {
        userName: [
          {required: true, message: "用户名称不能为空", trigger: "blur"}
        ],
        nickName: [
          {required: true, message: "用户昵称不能为空", trigger: "blur"}
        ],
        password: [
          {required: true, message: "用户密码不能为空", trigger: "blur"}
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
    //console.log(str)


    //加载 自定义 是否 1 2
    if (window['buttonyesno'] != undefined && window['buttonyesno'] != null && window['buttonyesno'] != '') {
      this.buttonyesno = window['buttonyesno'];
    } else {
      this.getDicts("yunze_customize_whether").then(response => {
        window['buttonyesno'] = response.data;
        this.buttonyesno = window['buttonyesno'];
      });
    }


    //加载 API通道名称 内部用
    if (this.internalBool == true) {
      if (window['findCr_RouteOptions'] != undefined && window['findCr_RouteOptions'] != null && window['findCr_RouteOptions'] != '') {
        this.findCr_RouteOptions = window['findCr_RouteOptions'];
      } else {
        findCr().then(response => {
          let jsonobj = JSON.parse(tools.Decrypt(response));
          window['findCr_RouteOptions'] = jsonobj.Data;
          this.findCr_RouteOptions = window['findCr_RouteOptions'];
        });
      }
    }


    //获取解绑 联系人信息
    this.getConfigKey("yunze.card.contactName").then(response => {
      this.CardBindingForm.contactName = response.msg;
    });
    this.getConfigKey("yunze.card.contactPhone").then(response => {
      this.CardBindingForm.contactPhone = response.msg;
    });

    //加载 查询条件
    if (window['card_seltype'] != undefined && window['card_seltype'] != null && window['card_seltype'] != '') {
      this.typeOptions = window['card_seltype'];
    } else {
      this.getDicts("yunze_card_seltype").then(response => {
        window['card_seltype'] = response.data;
        this.typeOptions = window['card_seltype'];
      });
    }


    //加载 资费组名称
    if (window['packageOptions'] != undefined && window['packageOptions'] != null && window['packageOptions'] != '') {
      this.packageOptions = window['packageOptions'];
      if (!tools.IsArrInside(this.packageOptions, "package_id", "IsNull")) {
        this.packageOptions.push({"package_id": "IsNull", "package_agentname": "未划分资费"});
      }
    } else {
      queryPackageSimple().then(response => {
        let jsonobj = JSON.parse(tools.Decrypt(response));
        //console.log(jsonobj);
        window['packageOptions'] = jsonobj.Data;
        this.packageOptions = window['packageOptions'];
        if (!tools.IsArrInside(this.packageOptions, "package_id", "IsNull")) {
          this.packageOptions.push({"package_id": "IsNull", "package_agentname": "未划分资费"});
        }
      });
    }


    //加载 卡号类别
    if (window['cardNumber'] != undefined && window['cardNumber'] != null && window['cardNumber'] != '') {
      this.cardNumber = window['cardNumber'];
    } else {
      this.getDicts("cardNumber").then(response => {
        window['cardNumber'] = response.data;
        this.cardNumber = window['cardNumber'];
      });
    }


    //加载 维度条件
    if (window['dimensionTypeOptions'] != undefined && window['dimensionTypeOptions'] != null && window['dimensionTypeOptions'] != '') {
      this.dimensionTypeOptions = window['dimensionTypeOptions'];
    } else {
      this.getDicts("yunze_card_dimensionType").then(response => {
        window['dimensionTypeOptions'] = response.data;
        this.dimensionTypeOptions = window['dimensionTypeOptions'];
      });
    }

    //加载 维度字段
    if (window['dimensionFieldOptions'] != undefined && window['dimensionFieldOptions'] != null && window['dimensionFieldOptions'] != '') {
      this.dimensionFieldOptions = window['dimensionFieldOptions'];
    } else {
      this.getDicts("yunze_card_dimensionField").then(response => {
        window['dimensionFieldOptions'] = response.data;
        this.dimensionFieldOptions = window['dimensionFieldOptions'];
      });
    }


    //加载 时间赛选查询条件
    if (window['timetypeOptions'] != undefined && window['timetypeOptions'] != null && window['timetypeOptions'] != '') {
      this.timetypeOptions = window['timetypeOptions'];
    } else {
      this.getDicts("yunze_card_sel_timetype").then(response => {
        window['timetypeOptions'] = response.data;
        this.timetypeOptions = window['timetypeOptions'];
      });
    }

    //加载 运营商类型
    if (window['operators_type'] != undefined && window['operators_type'] != null && window['operators_type'] != '') {
      this.operators_type = window['operators_type'];
    } else {
      this.getDicts("yunze_card_operators_type").then(response => {
        window['operators_type'] = response.data;
        this.operators_type = window['operators_type'];
      });
    }

    //加载 卡状态描述
    if (window['stateOptions'] != undefined && window['stateOptions'] != null && window['stateOptions'] != '') {
      this.stateOptions = window['stateOptions'];
    } else {
      this.getDicts("yunze_card_status_type").then(response => {
        window['stateOptions'] = response.data;
        this.stateOptions = window['stateOptions'];
      });
    }
    //加载 卡状态
    if (window['stateShowId'] != undefined && window['stateShowId'] != null && window['stateShowId'] != '') {
      this.stateShowId = window['stateShowId'];
    } else {
      this.getDicts("yunze_card_status_ShowId").then(response => {
        window['stateShowId'] = response.data;
        this.stateShowId = window['stateShowId'];
      });
    }

    //加载  断开网状态
    if (window['cardConnectionTypeOptions'] != undefined && window['cardConnectionTypeOptions'] != null && window['cardConnectionTypeOptions'] != '') {
      this.cardConnectionTypeOptions = window['cardConnectionTypeOptions'];
    } else {
      this.getDicts("yz_cardConnection_type").then(response => {
        window['cardConnectionTypeOptions'] = response.data;
        this.cardConnectionTypeOptions = window['cardConnectionTypeOptions'];
      });
    }


    //加载 卡类型
    if (window['card_types'] != undefined && window['card_types'] != null && window['card_types'] != '') {
      this.card_types = window['card_types'];
    } else {
      this.getDicts("yunze_card_card_type").then(response => {
        window['card_types'] = response.data;
        this.card_types = window['card_types'];
      });
    }

    //加载 卡类型
    if (window['network_types'] != undefined && window['network_types'] != null && window['network_types'] != '') {
      this.network_types = window['network_types'];
    } else {
      this.getDicts("yunze_card_network_type").then(response => {
        window['network_types'] = response.data;
        this.network_types = window['network_types'];
      });
    }

    //加载 卡板在线状态
    if (window['online_status'] != undefined && window['online_status'] != null && window['online_status'] != '') {
      this.online_status = window['online_status'];
    } else {
      this.getDicts("yunze_card_online_status").then(response => {
        window['online_status'] = response.data;
        this.online_status = window['online_status'];
      });
    }

    //加载 卡板接入方式
    if (window['access_type'] != undefined && window['access_type'] != null && window['access_type'] != '') {
      this.access_type = window['access_type'];
    } else {
      this.getDicts("yunze_card_access_type").then(response => {
        window['access_type'] = response.data;
        this.access_type = window['access_type'];
      });
    }

    //加载 移动 归属地
    if(window['YDcard_attributionOptions']!=undefined &&  window['YDcard_attributionOptions']!=null && window['YDcard_attributionOptions']!=''){
      this.YDcard_attributionOptions = window['YDcard_attributionOptions'];
    }else{
      this.getDicts("yz_YDcard_attribution").then(response => {
        window['YDcard_attributionOptions'] = response.data;
        this.YDcard_attributionOptions = window['YDcard_attributionOptions'];
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


    //加载 企业名称
    if (window['DeptOptions'] != undefined && window['DeptOptions'] != null && window['DeptOptions'] != '') {
      this.DeptOptions = window['DeptOptions'];
    } else {
      this.getDeptName().then(response => {
        let jsonobj = JSON.parse(tools.Decrypt(response));
        window['DeptOptions'] = jsonobj.Data;
        this.DeptOptions = window['DeptOptions'];
      });
    }

    //加载 API通道名称
    if (window['RouteOptions'] != undefined && window['RouteOptions'] != null && window['RouteOptions'] != '') {
      this.RouteOptions = window['RouteOptions'];
      if (!tools.IsArrInside(this.RouteOptions, "cd_id", "IsNull")) {
        this.RouteOptions.push({"cd_id": "IsNull", "cd_alias": "未划分通道"});
      }
    } else {
      this.findSp().then(response => {
        let jsonobj = JSON.parse(tools.Decrypt(response));
        window['RouteOptions'] = jsonobj.Data;
        this.RouteOptions = window['RouteOptions'];
        if (!tools.IsArrInside(this.RouteOptions, "cd_id", "IsNull")) {
          this.RouteOptions.push({"cd_id": "IsNull", "cd_alias": "未划分通道"});
        }
      });
    }

    //加载 电信CMP无线接入模式
    if(window['DxCMP_netModel_type']!=undefined &&  window['DxCMP_netModel_type']!=null && window['DxCMP_netModel_type']!=''){
      this.DxCMP_netModel_type = window['DxCMP_netModel_type'];
    }else{
      this.getDicts("DianXin_CMP_netModel").then(response => {
        window['DxCMP_netModel_type'] = response.data;
        this.DxCMP_netModel_type = window['DxCMP_netModel_type'];
      });
    }

    //加载 电信CMP接入类型
    if(window['DxCMP_RAT']!=undefined &&  window['DxCMP_RAT']!=null && window['DxCMP_RAT']!=''){
      this.DxCMP_RAT = window['DxCMP_RAT'];
    }else{
      this.getDicts("DianXin_CMP_RAT").then(response => {
        window['DxCMP_RAT'] = response.data;
        this.DxCMP_RAT = window['DxCMP_RAT'];
      });
    }

    //加载 电信CMP在线状态
    if(window['DxCMP_online']!=undefined &&  window['DxCMP_online']!=null && window['DxCMP_online']!=''){
      this.DxCMP_online = window['DxCMP_online'];
    }else{
      this.getDicts("DianXin_CMP_online").then(response => {
        window['DxCMP_online'] = response.data;
        this.DxCMP_online = window['DxCMP_online'];
      });
    }


    //加载 起止条件查询类别
    if (window['StartAndEndOptions'] != undefined && window['StartAndEndOptions'] != null && window['StartAndEndOptions'] != '') {
      this.StartAndEndOptions = window['StartAndEndOptions'];
    } else {
      this.getDicts("yunze_card_StartAndEnd_type").then(response => {
        window['StartAndEndOptions'] = response.data;
        this.StartAndEndOptions = window['StartAndEndOptions'];
      });
    }
    this.getTreeselect();


    if (str != null && str != '' && str.length > 0) {
      var reg_1 = new RegExp("%2F", "g");
      var reg_2 = new RegExp(" ", "g");
      str = str.replace(reg_1, "/");//转义 /
      str = str.replace(reg_2, "+");//转义 [ ]  》 +

      let jsonobj = JSON.parse(tools.Decrypt(str));
      //console.log(jsonobj)
      if (jsonobj != null) {
        if(tools.Is_null(jsonobj.UpType)){
          this.queryParams.UpType =jsonobj.UpType;
          this.showSearch = true;
        }
        if(tools.Is_null(jsonobj.type)){
          this.queryParams.type =jsonobj.type;
        }
        if(tools.Is_null(jsonobj.value)){
          this.queryParams.value =jsonobj.value;
        }
        if(tools.Is_null(jsonobj.timetype)){
          this.queryParams.timetype = jsonobj.timetype;
          this.showSearch = true;
        }
        if(tools.Is_null(jsonobj.agent_id)){
          this.queryParams.agent_id = jsonobj.agent_id;
          this.showSearch = true;
        }
        if(tools.Is_null(jsonobj.is_pool)){
          this.queryParams.is_pool = jsonobj.is_pool;
          this.showSearch = true;
        }
        if(tools.Is_null(jsonobj.pool_id)){
          this.queryParams.pool_id = jsonobj.pool_id;
          this.showSearch = true;
        }
        if (jsonobj.channel_id != undefined && jsonobj.channel_id != null && jsonobj.channel_id.length > 0) {
          this.queryParams.channel_id = jsonobj.channel_id;
          this.showSearch = true;
        }
        if(tools.Is_null(jsonobj.del_flag)){
          this.queryParams.del_flag = jsonobj.del_flag;
          this.showSearch = true;
        }
        if(tools.Is_null(jsonobj.status_id)){
          this.queryParams.status_id = jsonobj.status_id;
          this.showSearch = true;
        }
        if (jsonobj.package_id != undefined && jsonobj.package_id != null && jsonobj.package_id.length > 0) {
          this.queryParams.package_id = jsonobj.package_id;
          this.showSearch = true;
        }
        if(tools.Is_null(jsonobj.StartAndEndtype)){
          this.queryParams.StartAndEndtype = jsonobj.StartAndEndtype;
          this.showSearch = true;
        }
        if(tools.Is_null(jsonobj.StartValue)){
          this.queryParams.StartValue = jsonobj.StartValue;
          this.showSearch = true;
        }
        if(tools.Is_null(jsonobj.EndValue)){
          this.queryParams.EndValue = jsonobj.EndValue;
          this.showSearch = true;
        }
        if (jsonobj.cd_operator_type != undefined && jsonobj.cd_operator_type != null && jsonobj.cd_operator_type.length > 0) {
          this.queryParams.cd_operator_type = jsonobj.cd_operator_type;
          this.showSearch = true;
        }
        if(tools.Is_null(jsonobj.cardNumber)){
          this.queryParams.cardNumber = jsonobj.cardNumber;
          this.showSearch = true;
        }
        if (jsonobj.UpArr != undefined && jsonobj.UpArr != null && jsonobj.UpArr.length > 0) {
          this.queryParams.UpArr = jsonobj.UpArr;
          this.showSearch = true;
        }
        if(tools.Is_null(jsonobj.rat)){
          this.queryParams.rat = jsonobj.rat;
          this.showSearch = true;
        }
        if(tools.Is_null(jsonobj.customize_grouping)){
          this.queryParams.customize_grouping = jsonobj.customize_grouping;
          this.showSearch = true;
        }
        if(tools.Is_null(jsonobj.deliver_date)){
          this.queryParams.deliver_date = jsonobj.deliver_date;
          this.showSearch = true;
        }
        if(tools.Is_null(jsonobj.status_ShowId)){
          this.queryParams.status_ShowId = jsonobj.status_ShowId;
          this.showSearch = true;
        }
        if(tools.Is_null(jsonobj.dimensionField)){
          this.queryParams.dimensionField = jsonobj.dimensionField;
          this.showSearch = true;
        }
        if(tools.Is_null(jsonobj.dimensionType)){
          this.queryParams.dimensionType = jsonobj.dimensionType;
          this.showSearch = true;
        }
        if(tools.Is_null(jsonobj.dimensionValue)){
          this.queryParams.dimensionValue = jsonobj.dimensionValue;
          this.showSearch = true;
        }
        if(tools.Is_null(jsonobj.exceedsThreshold)){
          this.queryParams.exceedsThreshold = jsonobj.exceedsThreshold;
          this.showSearch = true;
        }






        if (jsonobj.staTime != undefined && jsonobj.staTime != null && jsonobj.staTime.length > 0) {
          this.selTime[0] = jsonobj.staTime;
          this.selTime[1] = jsonobj.endTime;
        }

        //加 if判断 是否获取到了 树形空间的
        if (jsonobj.trreAgent_id != undefined && jsonobj.trreAgent_id != null && jsonobj.trreAgent_id.length > 0) {
          this.queryParams.agent_id = jsonobj.trreAgent_id;
          console.log(this.queryParams)
          this.deptCheckStrictly = false;
          this.$nextTick(() => {
            this.$refs.dept.setCheckedKeys(jsonobj.trreAgent_id);
          });
        }
        // 把传过来的值放入到新的数组
        if (jsonobj.cArr != undefined && jsonobj.cArr != null && jsonobj.cArr.length > 0) {
          let cArrId = [];
          for (let i = 0; i < jsonobj.cArr.length; i++) {
            let obj = jsonobj.cArr[i];
            cArrId.push(Number(obj));
          }
          this.queryParams.channel_id = cArrId;
        }
        /* if(jsonobj.exceedsThreshold!=undefined && jsonobj.exceedsThreshold!=null && jsonobj.exceedsThreshold.length>0){
           this.loadexceedsThresholdBoolShow = false;
           this.queryParams.exceedsThresholdBool = jsonobj.exceedsThreshold=='1'?true:false;
           console.log(this.queryParams.exceedsThresholdBool)
         }*/
      }

      this.getList();
      // this.loadexceedsThresholdBoolShow = true;
    } else {
      this.getList();
    }


  },


  methods: {

    //已达量停机阈值 变更
    changeExceedsThreshold() {
      //console.log(this.queryParams.exceedsThreshold)
    },


    /*setInternal(){
      this.internalBool
    }*/


    /*setColumns(){
    if(this.internalBool){
      this.columns = [
        { key: 0, label: `虚拟编号`, visible: true },
        { key: 1, label: `msisdn`, visible: true },
        { key: 2, label: `iccid`, visible: true },
        { key: 3, label: `imsi`, visible: true },
        { key: 4, label: `卡状态`, visible: true },
        { key: 5, label: `备注信息`, visible: true },
        { key: 6, label: `开卡日期`, visible: false },
        { key: 7, label: `所属公司`, visible: true },
        { key: 8, label: `入库日期`, visible: false },
        { key: 9, label: `同步时间`, visible: true },
        { key: 10, label: `用量`, visible: true },
        { key: 11, label: `分组`, visible: true },
        { key: 12, label: `通道`, visible: false },
        { key: 13, label: `断开网状态`, visible: false },
        { key: 14, label: `未订购停机`, visible: false },
        { key: 15, label: `停机阈值`, visible: false },
      ];
      }else{
        this.columns = [
          { key: 0, label: `虚拟编号`, visible: true },
          { key: 1, label: `msisdn`, visible: true },
          { key: 2, label: `iccid`, visible: true },
          { key: 3, label: `imsi`, visible: true },
          { key: 4, label: `卡状态`, visible: true },
          { key: 5, label: `备注信息`, visible: true },
          { key: 6, label: `开卡日期`, visible: false },
          { key: 7, label: `所属公司`, visible: true },
          { key: 8, label: `入库日期`, visible: false },
          { key: 9, label: `同步时间`, visible: true },
          { key: 10, label: `用量`, visible: true },
          { key: 11, label: `分组`, visible: true },
          { key: 12, label: `无权限`, visible: false },
          { key: 13, label: `无权限`, visible: false },
          { key: 14, label: `无权限`, visible: false },
          { key: 15, label: `无权限`, visible: false },
        ];
      }
    },*/


    // tree单选
    treeNodeClick(data, checked) {
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
        let map = {};
        map.dept_id = this.formDivide.dept_id;
        let Pwd_Str = tools.encrypt(JSON.stringify(map));
        findDeptUser(Pwd_Str).then(response => {
            let jsonobj = JSON.parse(tools.Decrypt(response));
            //console.log(jsonobj);
            if (jsonobj.code == 200) {
              this.userArr = jsonobj.Data;
            } else {
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
      }
    },


    SelectChange() {
      this.getPwd_Str();
      //console.log("SelectChange");
    },
    // 加密数据
    getPwd_Str() {
      let Pwd_Str = tools.encrypt(JSON.stringify(this.queryParams));
      console.log(Pwd_Str)
      this.updatel.Pstr = Pwd_Str;
    },
    // 文件上传中处理
    handleFileUploadProgress(event, file, fileList) {
      this.updatel.isUploading = true;
    },


    //导入查询
    handlenumberFileSuccess(response, file, fileList) {
      let jsonobj = JSON.parse(tools.Decrypt(response.msg));
      if (response.code == 200) {
        this.cardList = jsonobj.Data;
        this.total = jsonobj.Pu.rowCount;
        this.queryParams = jsonobj.Pmap;
      } else {
        this.msgError("获取数据异常，请联系管理员！");
      }
      this.updatel.Number = false;
      this.updatel.isUploading = false;
      this.$refs.number.clearFiles();
      this.submitFileFormBtn = true;
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
    submitNumber() {
      let _this = this;
      this.SelectChange();
      this.submitFileFormBtn = false;
      if (tools.Is_null(this.queryParams.UpType)
      ) {
        this.titleUp(_this);
      } else {
        this.submitFileFormBtn = true;
        tools.open(this, "请选择需要设置更新的属性条件！")
      }

    },
    titleUp(_this) {
      let msg = '';
      if (tools.Is_null(_this.queryParams.UpType)) {
        msg += ' 导入查询 卡号类型  ' + tools.getPackageNNamekeyValue(packageOptions, _this.queryParams.UpType) + '  ';
      }

      msg += ' 是否确定以上操作 ？ ';
      tools.openAsk(this, 'warning', msg, this.uploadUp, _this, this.FalseFun, null);
    },
    uploadUp(_this) {
      // console.log("number")
      _this.$refs.number.submit();
    },

    FalseFun(Pwd_Str) {
      this.submitFileFormBtn = true;
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


    /*加载当前 套餐包*/
    loadCar(item, index, NotExpired) {
      //
      let _this = this;
      // console.log(item);
      //  console.log(index);
      let map = {};
      map.iccid = this.sel.iccid;
      map.$index = index;
      map.agent_id = this.sel.agent_id;
      map.package_id = this.sel.package_id;

      map.NotExpired = NotExpired != undefined && NotExpired != null ? '0' : '1';
      // console.log(map);
      _this.SetMealImport = false;
      _this.queryflow(map);

    },
    queryflow(map) {//查询现有流量包
      let _this = this;
      _this.FlowData = [];//重置
      //console.log(map);
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
      //console.log(Pwd_Str);
      queryflowData(Pwd_Str).then(response => {
        let jsonobj = JSON.parse(tools.Decrypt(response));
        //console.log(jsonobj);
        if (jsonobj.code == "200") {
          //tools.open(_this, response.data.data);
          let arr = jsonobj.Data;

          var new_map = {},
            dest = [];
          for (var i = 0; i < arr.length; i++) {
            var ai = arr[i];
            if (!new_map[ai.ord_no]) { //依赖分组字段可自行更改！
              dest.push({
                ord_no: ai.ord_no, //依赖分组字段可自行更改！
                packet_wx_name: ai.packet_wx_name,
                ord_status: ai.ord_status,
                pay_type: ai.pay_type,
                is_profit: ai.is_profit,
                validate_type: ai.validate_type,
                info: ai.info,
                ord_name: ai.ord_name,
                true_flow: ai.true_flow,
                error_flow: ai.error_flow,
                data: [ai]
              });
              new_map[ai.ord_no] = ai; //依赖分组字段可自行更改！
            } else {
              for (var j = 0; j < dest.length; j++) {
                var dj = dest[j];
                if (dj.ord_no == ai.ord_no) { //依赖分组字段可自行更改！
                  dj.data.push(ai);
                  break;
                }
              }
            }
          }
          //console.log(dest);
          _this.FlowData = dest;
          _this.data = dest;
          //console.log(_this.selIccid);
          _this.selIccid[0].iccid = map.iccid;
          _this.selIccid[0].FlowData = dest;
          _this.SetMealImport = true;
          //window[map.iccid] = _this.FlowData;
          this.cardPackageTitle = "资费信息 (已同步数据)";
        } else {
          //tools.open(_this, jsonobj.msg);
          tools.MessageShow(_this, jsonobj.msg, "error");
        }
      })
    },


    loadCardGrouping() {
      this.CardGrouping = [];
      let map = {};
      if (this.$refs.dept != undefined & this.$refs.dept != null & this.$refs.dept != 'undefined') {
        if (this.$refs.dept.getCheckedKeys().length > 0) {
          map.agent_id = this.$refs.dept.getCheckedKeys();
        }
      }
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
      getCardGrouping(Pwd_Str).then(response => {
          let jsonobj = JSON.parse(tools.Decrypt(response));
          if (jsonobj.code == 200) {
            this.CardGrouping = jsonobj.Data;
          } else {
            this.msgError(jsonobj.msg);
          }
          this.loading = false;
        }
      );

    },

    /*获取查询参数*/
    getParams() {

      this.queryParams.staTime = null;
      this.queryParams.endTime = null;
      //this.queryParams.agent_id = null;
      if (tools.Is_null(this.queryParams.timetype) && this.selTime != null) {
        //console.log(this.selTime);
        this.queryParams.staTime = this.selTime[0];
        this.queryParams.endTime = this.selTime[1];
      }
      if (this.$refs.dept != undefined & this.$refs.dept != null & this.$refs.dept != 'undefined') {
        //console.log(this.$refs.dept.getCheckedKeys());
        if (this.$refs.dept.getCheckedKeys().length > 0) {
          this.queryParams.agent_id = this.$refs.dept.getCheckedKeys();
        }
      }

      this.queryParams.StartValue = tools.Is_null(this.queryParams.StartValue) ? this.queryParams.StartValue : null;
      this.queryParams.EndValue = tools.Is_null(this.queryParams.EndValue) ? this.queryParams.EndValue : null;
    },


    /** 查询列表 */
    getList() {
      this.loading = true;
      this.loadCardGrouping();//加载 分组
      this.getParams();
      let Pwd_Str = tools.encrypt(JSON.stringify(this.queryParams));

      listCard(Pwd_Str).then(response => {
          let jsonobj = JSON.parse(tools.Decrypt(response));
          //console.log(jsonobj);
          if (jsonobj.code == 200) {
            this.internalBool = jsonobj.Data.Pmap.is_Internal;
            /*  this.setColumns();*/
            //加载 API通道名称 内部用
            if (this.internalBool == true) {
              if (window['findCr_RouteOptions'] != undefined && window['findCr_RouteOptions'] != null && window['findCr_RouteOptions'] != '') {
                this.findCr_RouteOptions = window['findCr_RouteOptions'];
              } else {
                findCr().then(response => {
                  let jsonobj = JSON.parse(tools.Decrypt(response));
                  window['findCr_RouteOptions'] = jsonobj.Data;
                  this.findCr_RouteOptions = window['findCr_RouteOptions'];
                });
              }
            }
            this.cardList = jsonobj.Data.Data;
            this.total = jsonobj.Data.Pu.rowCount;
          } else {
            this.msgError("获取数据异常，请联系管理员！");
          }
          this.loading = false;
        }
      );
    },

    Change(iccid) {
      this.loading = true;
      let Pwd_Str = tools.encrypt(JSON.stringify({'iccid': iccid}));
      getselTime(Pwd_Str).then(response => {
          let jsonobj = JSON.parse(tools.Decrypt(response));
          if (jsonobj.code == '200') {
            let cardLists = jsonobj.Data;
            ///console.log(cardLists);
            let arr = [];
            for (let i = 0; i < cardLists.length; i++) {
              arr.push(cardLists[cardLists.length - i - 1]);
            }
            //console.log(arr);
            this.cardChangeList = arr;
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
    /** 查询企业下拉树结构 */
    getTreeselect() {
      treeselect().then(response => {
        this.deptOptions = response.data;
        //console.log(this.deptOptions)
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
      this.fill = false;
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
    /**关闭按钮 */
    Close(){
      this.show_ds =  false;
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
    /** 详情按钮操作 */
    ViewDetails(row) {
      //console.log(row);
      const userId = row.userId;
      //this.form = row;
      /*this.postOptions = response.posts;
      this.roleOptions = response.roles;*/
      this.cardInfoTitle = "卡基础信息 (信息同步中)";
      this.cardPackageTitle = "资费信息 (信息同步中)";
      this.OnlineTitle = '在线信息 (信息同步中)';

      this.ifsel = true;
      this.finddata(row);
      this.show_ds = true;
      this.sel.iccid = row.iccid;
      this.sel.agent_id = row.agent_id;
      this.sel.package_id = row.package_id;

      this.Change(row.iccid);

      //详细套餐信息
      /*  let map = {};
      map.iccid = row.iccid;
      map.$index = 0;
      map.NotExpired = 1;
      map.package_id = row.package_id;
      map.agent_id = row.agent_id;
    // console.log(row);
      //console.log(_this.selIccid);
      this.queryflow(map);*/


      this.getOnlineStatus();


    },

    //在线信息 更新
    getOnlineStatus() {
      //  table_Online
      this.table_Online = [];//重置数据
      let map = {};
      map.iccid = this.sel.iccid;
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
      queryOnlineStatus(Pwd_Str).then(res => {
        let jsonobj = JSON.parse(tools.Decrypt(res));
        //console.log(jsonobj);

        if(jsonobj.code==200){
          let obj = jsonobj.Data.Data;
          let map1 = {};
          let map2 = {};
          let cd_code = jsonobj.Data.cd_code;

          let createDate = '';
          if (cd_code == "YiDong_EC" || cd_code == "YiDong_EC_TOKE_ShuoLang" || cd_code == "YiDong_EC_TengYu") {
            map1.value1 = tools.getDkeyValue(this.access_type, obj.rat);
            map1.value2 = tools.getDkeyValue(this.online_status, obj.status);
            map2.value1 = obj.ip;
            map2.value2 = obj.apnId;
            createDate = obj.createDate;
          } else if (cd_code == "YiDong_ECv2") {
            map1.value1 = tools.getDkeyValue(this.access_type, obj.RAT);
            map1.value2 = tools.getDkeyValue(this.online_status, obj.GPRSSTATUS);
            map2.value1 = obj.IP;
            map2.value2 = obj.APN;
            createDate = obj.createDate;
          }else if(cd_code == "ZCWL" ) {
            map1.value1 = tools.getDkeyValue(this.access_type, obj.rat);
            map1.value2 = tools.getDkeyValue(this.online_status, obj.status);
            map2.value1 = obj.ip;
            map2.value2 = obj.apnId;
            createDate = obj.createDate;
          }else if(cd_code == "DianXin_CMP") {
            //DxCMP_RAT: [],//电信CMP接入类型
            map1.value1 = tools.getDkeyValue(this.DxCMP_RAT, obj.rattype)+" 无线接入模式 : [ "+tools.getDkeyValue(this.DxCMP_netModel_type, obj.netModel)+" ]";
            map1.value2 = tools.getDkeyValue(this.DxCMP_online, obj.result);
            map2.value1 = obj.framedIpAddress;
            map2.value2 = obj.apn;
            createDate = obj.eventTimestamp;
          }
          map1.label1 = "接入方式";
          map1.label2 = "在线状态";
          this.table_Online.push(map1);

          map2.label1 = "IP地址";
          map2.label2 = "APN";
          this.table_Online.push(map2);
          if (tools.Is_null(createDate)) {
            this.OnlineTitle = '在线信息 (会话创建时间 : ' + createDate + ')';
          } else {
            this.OnlineTitle = '在线信息 (已同步信息)';
          }
        } else {
          let map1 = {};
          map1.label1 = "返回信息";
          map1.value1 = jsonobj.msg;
          map1.label2 = "请核对API信息（未产生用量不会有在线记录！）";
          map1.value2 = "目前仅支持移动EC,移动Pboos";
          this.table_Online.push(map1);
          this.msgError(jsonobj.msg);
          this.OnlineTitle = '在线信息 (已同步信息)';
        }

      })
    },


    /*获取单卡详细信息*/
    finddata(row) {
      this.tablefrom = [];//清空数据
      this.tablefrom_remarks = [];//清空数据
      this.reset();
      let map = {};
      map.iccid = row.iccid;
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
      getCard(Pwd_Str).then(response => {
          let jsonobj = JSON.parse(tools.Decrypt(response));
          //console.log(jsonobj);
          if (jsonobj.code == 200) {
            this.form = jsonobj.Data;
            this.form.status_id = "" + this.form.status_id + "";
            let map1 = {};
            map1.label1 = "MSISDN";
            map1.value1 = jsonobj.Data.msisdn;
            map1.label2 = "ICCID";
            map1.value2 = jsonobj.Data.iccid;
            map1.label3 = "IMSI";
            map1.value3 = jsonobj.Data.imsi;
            this.tablefrom.push(map1);
            let map2 = {};
            //console.log(this.operators_type);
            map2.label1 = "IMEI";
            map2.value1 = jsonobj.Data.imei;
            map2.label2 = "运营商";
            if (tools.Is_null(jsonobj.Data.cd_operator_type)) {
              map2.value2 = tools.getDkeyValue(this.operators_type, jsonobj.Data.cd_operator_type);
            } else {
              map2.value2 = "暂未划分API通道";
            }
            if (this.internalBool) {
              if (map2.value2 == '移动' && jsonobj.Data.iccid.length > 12) {
                let subStr = jsonobj.Data.iccid.substring(8, 12);
                let attributionCode = subStr.substring(0, 2);
                let YY = subStr.substring(2, subStr.length);
                let attribution = tools.getDkeyValue(this.YDcard_attributionOptions, attributionCode);
                map2.value2 += " [归属地：" + attribution + "] [ " + YY + " 年开卡] ";
              }
            }
            map2.label3 = "卡状态";
            map2.value3 = tools.getDkeyValue(this.stateShowId, jsonobj.Data.status_ShowId);

            this.tablefrom.push(map2);
            let map3 = {};
            map3.label1 = "GPRS";
            map3.value1 = tools.getDkeyValue(this.card_types, jsonobj.Data.gprs);
            map3.label2 = "卡类型";
            map3.value2 = tools.getDkeyValue(this.card_types, jsonobj.Data.type);
            map3.label3 = "网络类型";
            map3.value3 = tools.getDkeyValue(this.network_types, jsonobj.Data.network_type);
            this.tablefrom.push(map3);

            queryflowSimple(Pwd_Str).then(res => {
                let jBj = JSON.parse(tools.Decrypt(res));
                //console.log(jBj);
                //jBj.Data =

                let map4 = {};
                map4.label1 = "短信功能";
                map4.value1 = tools.getDkeyValue(this.customize_whether, jsonobj.Data.is_sms);
                map4.label2 = "短信服务号码";
                map4.value2 = jsonobj.Data.sms_number;
                map4.label3 = "计费";
                map4.value3 = jBj.Data != undefined && jBj.Data != null ? jBj.Data.packet_wx_name : '未订购';
                this.tablefrom.push(map4);


                let map5 = {};
                map5.label1 = "激活时间";
                map5.value1 = jsonobj.Data.activate_date;
                map5.label2 = "到期时间";
                map5.value2 = jBj.Data != undefined && jBj.Data != null ? jBj.Data.end_time : '';
                map5.label3 = jBj.Data != undefined && jBj.Data != null ? "服务周期  ( " + jBj.Data.packet_valid_name + " ) " : '服务周期';
                map5.value3 = jBj.Data != undefined && jBj.Data != null ? jBj.Data.packet_valid_time : '';
                this.tablefrom.push(map5);
                let map7 = {};
                let channelname = "";
                // console.log(this.internalBool)
                if (this.internalBool) {
                  channelname = tools.getDkeyValue(this.findCr_RouteOptions, '' + row.channel_id);
                } else {
                  channelname = tools.getkeyValue(this.RouteOptions, '' + row.channel_id, "cd_id", "cd_alias");

                }
                map7.label1 = "分组";
                map7.value1 = row.customize_grouping;
                map7.label2 = "所属通道";
                map7.value2 = channelname;
                map7.label3 = "断开网状态";
                map7.value3 = row.connection_status != null ? tools.getDkeyValue(this.cardConnectionTypeOptions, '' + row.connection_status) : "";
                this.tablefrom.push(map7);


                if (this.internalBool) {
                  let map8 = {};
                  map8.label1 = "未订购停机";
                  map8.value1 = tools.getDkeyValue(this.customize_whether, row.is_Disconnected);
                  map8.label2 = "停机阈值";
                  map8.value2 = row.remind_ratio;
                  map8.label3 = "卡状态描述";
                  map8.value3 = tools.getDkeyValue(this.stateOptions, jsonobj.Data.status_id);
                  this.tablefrom.push(map8);
                }

                let map6 = {};
                map6.label1 = "备注";
                map6.value1 = jsonobj.Data.remarks;
                this.tablefrom_remarks.push(map6);
              }
            );


            this.cardInfoTitle = "卡基础信息 (已同步数据)";
          } else {
            this.msgError("获取数据异常，请联系管理员！");
          }
          this.loading = false;
        }
      );

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
      const userIds = row.userId || this.ids;
      this.$confirm('是否确认删除用户编号为"' + userIds + '"的数据项?', "警告", {
        confirmButtonText: "确定",
        cancelButtonText: "取消",
        type: "warning"
      }).then(function () {
        return delUser(userIds);
      }).then(() => {
        this.getList();
        this.msgSuccess("删除成功");
      }).catch(() => {
      });
    },
    /** 导出按钮操作 */
    handleExport() {
      /*const queryParams = this.queryParams;
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
        }).catch(() => {});*/

      this.getParams();
      let Pwd_Str = tools.encrypt(JSON.stringify(this.queryParams));
      exportCard(Pwd_Str).then(response => {
        let jsonobj = JSON.parse(tools.Decrypt(response));
        //console.log(jsonobj);
        if (jsonobj.code == 200) {
          this.msgSuccess(jsonobj.Data);
        } else {
          let msg = jsonobj.Data != null && jsonobj.Data != '' ? jsonobj.Data : jsonobj.msg;
          this.msgError(msg);
        }

      })

    },
    /**划卡*/
    handleDivide() {
      this.Divide_show = true;

    },
    /**保存划卡*/
    Divide() {
      this.Dividebtn = false;
      //console.log(this.formDivide.dept_id+"   "+this.formDivide.user_id);
      if (tools.VerificationsText(this, this.formDivide.dept_id, "请选择所属企业！") == true &&
        tools.VerificationsText(this, this.formDivide.user_id, "请选择所属用户！") == true) {
        //1.先按当期筛选条件查询出数据 询问是否保存
        this.handleQuery();
        let map = this.queryParams;
        let user_name = tools.getUserNamekeyValue(this.userArr, this.formDivide.user_id);
        map.set_dept_id = this.formDivide.dept_id;
        map.set_user_id = this.formDivide.user_id;
        map.set_dept_name = this.formDivide.dept_name;
        map.set_user_name = user_name;
        let Pwd_Str = tools.encrypt(JSON.stringify(map));
        tools.openAsk(this, 'warning', "确定将筛选 [ " + this.total + " ] 条数据  划分给  [ " + this.formDivide.dept_name + " ]  [ " + user_name + " ] 吗？", this.DivideSave, Pwd_Str);
      }
    },


    //同步卡状态
    SynStatus(iccid) {
      let map = {};
      map.iccid = iccid;
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
      SynCarStatus(Pwd_Str).then(response => {
        let jsonobj = JSON.parse(tools.Decrypt(response));
        //console.log(jsonobj);
        if (jsonobj.code == 200) {
          this.msgSuccess(jsonobj.msg);
          this.getList();
        } else {
          this.msgError(jsonobj.msg);
        }
      })
    },
    //同步用量
    SynFlow(iccid) {
      this.loading = true;
      let map = {};
      map.iccid = iccid;
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
      SynCarFlow(Pwd_Str).then(response => {
        this.loading = false;
        let jsonobj = JSON.parse(tools.Decrypt(response));
        //console.log(jsonobj);
        if (jsonobj.code == 200) {
          this.msgSuccess(jsonobj.msg);
          this.getList();
        } else {
          this.msgError(jsonobj.msg);
        }
      })
    },

    //同步激活时间
    SynActivateDate(iccid) {
      this.loading = true;
      let map = {};
      map.iccid = iccid;
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
      SynCardActivateDate(Pwd_Str).then(response => {
        this.loading = false;
        let jsonobj = JSON.parse(tools.Decrypt(response));
        //console.log(jsonobj);
        if (jsonobj.code == 200) {
          this.msgSuccess(jsonobj.msg);
          this.getList();
        } else {
          this.msgError(jsonobj.msg);
        }
      })
    },


    //机卡解绑 界面打开
    CardBinding(iccid) {
      this.CardBindingShow = true;
      this.CardBindingForm.iccid = iccid;
    },

    //机卡解绑
    MyCarUntie() {
      let Pwd_Str = tools.encrypt(JSON.stringify(this.CardBindingForm));
      CarUntie(Pwd_Str).then(response => {
        let jsonobj = JSON.parse(tools.Decrypt(response));
        //console.log(jsonobj);
        if (jsonobj.code == 200) {
          this.msgSuccess(jsonobj.Data);
          this.getList();
        } else {
          this.msgError(jsonobj.msg);
        }
      })
    },


    //机卡解绑 保存
    CardBindingSave() {
      let _this = this;
      if (tools.VerificationsText(_this, _this.CardBindingForm.contactName, "解绑联系人不能为空！") == true &&
        tools.VerificationsText(_this, _this.CardBindingForm.contactPhone, "解绑联系人手机号不能为空！") == true) {
        //机卡解绑
        this.CardBindingSaveBtn = false;
        _this.MyCarUntie();
      }
    },


    //划卡保存
    DivideSave(Pwd_Str) {
      DivideCard(Pwd_Str).then(response => {
        this.Dividebtn = true;
        this.CardBindingSaveBtn = true;
        let jsonobj = JSON.parse(tools.Decrypt(response));
        //console.log(jsonobj);
        if (jsonobj.code == 200) {
          let msg = jsonobj.Data != null && jsonobj.Data != '' ? jsonobj.Data : jsonobj.msg;
          this.msgSuccess(msg);
          this.handleQuery();
        } else {
          let msg = jsonobj.Data != null && jsonobj.Data != '' ? jsonobj.Data : jsonobj.msg;
          this.msgError(msg);
        }
      })
    },


    /** 导入按钮操作 */
    handleImport() {
      this.upload.title = "用户导入";
      this.upload.open = true;
      this.submitFileFormBtn = true;
    },
    /** 导入查询按钮操作 */
    Importquery() {
      this.updatel.title = "导入查询";
      this.queryParams.UpArr = [];
      this.queryParams.UpType = "";
      this.updatel.Number = true;
    },

    ShowUpdInfo() {
      this.IsUpdInfo = !this.IsUpdInfo;
    },

    /**是否更新未填写属性*/
    Updatenotfilledin() {
      this.notfilledin = "更新基础信息";
      // this.fill=true;

      let _this = this;
      _this.selIccid = _this.$refs.multipleTable.selection;
      if (_this.selIccid.length > 0) {
        let max = 500;
        if (_this.selIccid.length <= max) {
          this.$refs.multipleTable.clearSelection();//清除选中
          let iccids = [];
          for (let i = 0; i < _this.selIccid.length; i++) {
            iccids.push(_this.selIccid[i].iccid);

          }

          _this.UpdInfo.iccids = iccids;
          _this.UpdInfo.customize_grouping = '';
          _this.UpdInfo.remarks = '';
          _this.UpdInfo.deliver_date = '';
          _this.fill = true;
          // console.log(iccids);
        } else {
          tools.open(_this, "一次最多操作更新" + max + "个号码！");
        }
      } else {
        tools.open(_this, "请勾选需要操作的卡号！");
      }
    },
    //修改提交确定按钮
    fillForm() {
      let _this = this;

      if (_this, _this.UpdInfo.iccids.length > 0) {
        //console.log(_this.form);
        let Button = _this.UpdInfo.Button;

        let customize_grouping = _this.UpdInfo.customize_grouping;
        let remarks = _this.UpdInfo.remarks;
        let deliver_date = _this.UpdInfo.deliver_date;

        let title = '';
        // 单选按钮 1 是
        if (Button == '1') {
          console.log(Button)
          if (customize_grouping.length > 0) {
            title += '  分组： [' + customize_grouping + ']';
          } else {
            title += '  分组： []';
            this.UpdInfo.customize_grouping = '';
          }
          if (remarks.length > 0) {
            title += '  备注： [' + remarks + ']';
          } else {
            title += '  备注： []';
            this.UpdInfo.remarks = '';
          }
          if (deliver_date.length > 0) {
            title += '  发货日期： [' + deliver_date + ']';
          } else {
            title += '  发货日期： []';
            this.UpdInfo.deliver_date = '';
          }
          _this.$confirm('批量更新卡信息 [' + _this.UpdInfo.iccids.length + '] 张, ' + title + '是否继续?', '提示', {
            confirmButtonText: '确定',
            cancelButtonText: '取消',
            type: 'warning'
          }).then(() => {
            //console.log("upd");
            console.log(_this.UpdInfo);
            _this.UpdateCardinfo(_this.UpdInfo);
          })
        } else {
          // 单选按钮 0 否
          console.log(Button)

          if (customize_grouping.length > 0 || remarks.length > 0 || deliver_date.length > 0) {
            customize_grouping = customize_grouping.length > 0 ? customize_grouping : null;
            remarks = remarks.length > 0 ? remarks : null;
            deliver_date = deliver_date.length > 0 ? deliver_date : null;
            title += customize_grouping != null ? '  分组： [' + customize_grouping + ']' : '';
            title += remarks != null ? '  备注： [' + remarks + ']' : '';
            title += deliver_date != null ? '  发货日期： [' + deliver_date + ']' : '';
            this.UpdInfo.customize_grouping = customize_grouping;
            this.UpdInfo.remarks = remarks;
            this.UpdInfo.deliver_date = deliver_date;
            _this.$confirm('批量更新卡信息 [' + _this.UpdInfo.iccids.length + '] 张, ' + title + '是否继续?', '提示', {
              confirmButtonText: '确定',
              cancelButtonText: '取消',
              type: 'warning'
            }).then(() => {
              //console.log("upd");
              console.log(_this.UpdInfo);
              _this.UpdateCardinfo(_this.UpdInfo);
            })

          } else {
            this.$message({
              type: 'info',
              message: '请填写需要更新的数据或选择更新未填写属性！'
            });
          }
        }
      } else {
        this.$message({
          type: 'info',
          message: '请选择需要操作的号码'
        });
      }

    },


    UpdateCardinfo(map) {

      let Pwd_Str = tools.encrypt(JSON.stringify(map));
      console.log(Pwd_Str);
      UpdateFill(Pwd_Str).then(response => {
        let jsonobj = JSON.parse(tools.Decrypt(response));
        console.log(jsonobj);
        if (jsonobj.code == 200) {
          this.msgSuccess(jsonobj.msg);
          this.getList();
        } else {
          let msg = jsonobj.Data.msg;
          this.msgError(msg);
        }
        this.fill = false;
      });
    },
    /**下面是勾选操作*/
    //下面公共的的方法
    PublicMethod(index) {
      let _this = this;
      _this.selIccid = _this.$refs.multipleTable.selection;
      let max = 500;
      if (_this.selIccid.length <= max) {
        this.$refs.multipleTable.clearSelection();//清除选中
        let iccids = [];
        for (let i = 0; i < _this.selIccid.length; i++) {
          iccids.push({"iccid": _this.selIccid[i].iccid});
        }
        _this.Operation.iccids = iccids;
        _this.Operation.iccid = '';
        // console.log(iccids);

      } else {
        tools.open(_this, "一次最多操作更新" + max + "个号码！");
      }
    },
    //停机
    Stopped() {
      this.PublicMethod();
      let _this = this;
      let title = '';
      if (_this.selIccid.length > 0) {
        _this.$confirm('批量【停机】-->卡信息 [' + _this.Operation.iccids.length + '] 张, ' + title + '是否继续?', '提示', {
          type: 'warning'
        }).then(() => {
          //console.log("upd");
          console.log(_this.Operation);
          _this.StoppedArrs();
        })
      } else {
        tools.open(_this, "请勾选需要停机的卡号！");
      }
    },
    StoppedArrs() {
      let Pwd_Str = tools.encrypt(JSON.stringify(this.Operation));
      StoppedArr(Pwd_Str).then(response => {
        let jsonobj = JSON.parse(tools.Decrypt(response));
        console.log(jsonobj)
        if (jsonobj.code == 200) {
          let msg = jsonobj.Data != null && jsonobj.Data != '' ? jsonobj.Data : jsonobj.msg;
          // this.cardList = msg;
          this.msgSuccess(msg);
        } else {
          this.msgError("获取数据异常，请联系管理员！");
        }
        this.handleQuery();
      });
    },
    //复机
    Machine() {
      this.PublicMethod();
      let _this = this;
      let title = '';
      if (_this.selIccid.length > 0) {
        _this.$confirm('批量【复机】-->卡信息 [' + _this.Operation.iccids.length + '] 张, ' + title + '是否继续?', '提示', {
          type: 'warning'
        }).then(() => {
          //console.log("upd");
          console.log(_this.Operation);
          _this.MachineArrs();
        })
      } else {
        tools.open(_this, "请勾选需要复机的卡号！");
      }
    },
    MachineArrs() {
      let Pwd_Str = tools.encrypt(JSON.stringify(this.Operation));
      MachineArr(Pwd_Str).then(response => {
        let jsonobj = JSON.parse(tools.Decrypt(response));
        if (jsonobj.code == 200) {
          let msg = jsonobj.Data != null && jsonobj.Data != '' ? jsonobj.Data : jsonobj.msg;
          // this.cardList = msg;
          this.msgSuccess(msg);
        } else {
          this.msgError("获取数据异常，请联系管理员！");
        }
        this.handleQuery();
      });
    },
    //断网
    DisconnectNetwork() {
      this.PublicMethod();
      let _this = this;
      let title = '';
      if (_this.selIccid.length > 0) {
        _this.$confirm('批量【断网】-->卡信息 [' + _this.Operation.iccids.length + '] 张, ' + title + '是否继续?', '提示', {
          type: 'warning'
        }).then(() => {
          //console.log("upd");
          console.log(_this.Operation);
          _this.DisconnectNetworkArrs();
        })
      } else {
        tools.open(_this, "请勾选需要断网的卡号！");
      }
    },
    DisconnectNetworkArrs() {
      let Pwd_Str = tools.encrypt(JSON.stringify(this.Operation));
      DisconnectNetworkArr(Pwd_Str).then(response => {
        let jsonobj = JSON.parse(tools.Decrypt(response));
        if (jsonobj.code == 200) {
          //this.cardList = jsonobj.Data.Data;
          let msg = jsonobj.Data != null && jsonobj.Data != '' ? jsonobj.Data : jsonobj.msg;
          this.msgSuccess(msg);
        } else {
          this.msgError("获取数据异常，请联系管理员！");
        }
        this.handleQuery();
      });
    },
    //开网
    OpenNetwork() {
      this.PublicMethod();
      let _this = this;
      let title = '';
      if (_this.selIccid.length > 0) {
        _this.$confirm('批量【开网】-->卡信息 [' + _this.Operation.iccids.length + '] 张, ' + title + '是否继续?', '提示', {
          type: 'warning'
        }).then(() => {
          //console.log("upd");
          console.log(_this.Operation);
          _this.OpenNetworkArrs();
        })
      } else {
        tools.open(_this, "请勾选需要开网的卡号！");
      }
    },
    OpenNetworkArrs() {
      let Pwd_Str = tools.encrypt(JSON.stringify(this.Operation));
      OpenNetworkArr(Pwd_Str).then(response => {
        let jsonobj = JSON.parse(tools.Decrypt(response));
        if (jsonobj.code == 200) {
          //this.cardList = jsonobj.Data.Data;
          let msg = jsonobj.Data != null && jsonobj.Data != '' ? jsonobj.Data : jsonobj.msg;
          this.msgSuccess(msg);
        } else {
          this.msgError("获取数据异常，请联系管理员！");
        }
        this.handleQuery();
      });
    },
    //同步用量
    Consumption() {
      this.PublicMethod();
      let _this = this;
      let title = '';
      if (_this.selIccid.length > 0) {
        _this.$confirm('批量【同步用量】-->卡信息 [' + _this.Operation.iccids.length + '] 张, ' + title + '是否继续?', '提示', {
          type: 'warning'
        }).then(() => {
          _this.ConsumptionArrs();
        })
      } else {
        tools.open(_this, "请勾选需要同步用量的卡号！");
      }
    },
    ConsumptionArrs(index) {
      let Pwd_Str = tools.encrypt(JSON.stringify(this.Operation));
      ConsumptionArr(Pwd_Str).then(response => {
        let jsonobj = JSON.parse(tools.Decrypt(response));
        if (jsonobj.code == 200) {
          //this.cardList = jsonobj.Data.Data;
          let msg = jsonobj.Data != null && jsonobj.Data != '' ? jsonobj.Data : jsonobj.msg;
          this.msgSuccess(msg);
        } else {
          this.msgError("获取数据异常，请联系管理员！");
        }
        this.handleQuery();
      });
    },
    //同步状态
    State() {
      this.PublicMethod();
      let _this = this;
      let title = '';
      if (_this.selIccid.length > 0) {
        _this.$confirm('批量【同步状态】-->卡信息 [' + _this.Operation.iccids.length + '] 张, ' + title + '是否继续?', '提示', {
          type: 'warning'
        }).then(() => {
          //console.log("upd");
          // console.log(_this.Operation);
          _this.PublicMethodArrs();
        })
      } else {
        tools.open(_this, "请勾选需要同步状态的卡号！");
      }
    },
    PublicMethodArrs() {
      let Pwd_Str = tools.encrypt(JSON.stringify(this.Operation));
      PublicMethodArr(Pwd_Str).then(response => {
        let jsonobj = JSON.parse(tools.Decrypt(response));
        if (jsonobj.code == 200) {
          //this.cardList = jsonobj.Data.Data;
          let msg = jsonobj.Data != null && jsonobj.Data != '' ? jsonobj.Data : jsonobj.msg;
          this.msgSuccess(msg);
        } else {
          this.msgError("获取数据异常，请联系管理员！");
        }
        this.handleQuery();
      });
    },
    //同步用量和状态
    ConsumptionAndState() {
      this.PublicMethod();
      let _this = this;
      let title = '';
      if (_this.selIccid.length > 0) {
        _this.$confirm('批量【同步用量和状态】-->卡信息 [' + _this.Operation.iccids.length + '] 张, ' + title + '是否继续?', '提示', {
          type: 'warning'
        }).then(() => {
          //console.log("upd");
          console.log(_this.Operation);
          _this.ConsumptionAndStateArrs();
        })
      } else {
        tools.open(_this, "请勾选需要同步用量和状态的卡号！");
      }
    },
    ConsumptionAndStateArrs() {
      let Pwd_Str = tools.encrypt(JSON.stringify(this.Operation));
      ConsumptionAndStateArr(Pwd_Str).then(response => {
        let jsonobj = JSON.parse(tools.Decrypt(response));
        if (jsonobj.code == 200) {
          //this.cardList = jsonobj.Data.Data;
          let msg = jsonobj.Data != null && jsonobj.Data != '' ? jsonobj.Data : jsonobj.msg;
          this.msgSuccess(msg);
        } else {
          this.msgError("获取数据异常，请联系管理员！");
        }
        this.handleQuery();
      });
    },
    /** 下载模板操作 */
    importTemplate() {
      importTemplate().then(response => {
        this.download(response.msg);
      });
    },
    importCardSel() {
      let map = {};
      map.path = "/getcsv/ImportCardSel.cvs";
      map.token = getToken();
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
      //console.log(map);
      window.open(process.env.VUE_APP_BASE_API + "/yunze/ExecutionTask/downloadConversion?Pstr=" + Pwd_Str, '_blank');
    },
    // 提交上传文件
    submitFileForm() {
      this.submitFileFormBtn = false;
      this.$refs.upload.submit();
    }
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

</style>

