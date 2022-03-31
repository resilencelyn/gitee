<template>
  <el-container class="home">
    <el-main  >

      <el-row :gutter="10">
        <el-col :xs="24" :sm="24" :md="24" :lg="24">
          <el-card class="update-log">
            <div slot="header" class="clearfix">
              <span>业务告警 </span>
              <span style="float: right;margin-right: 10px;">
                统计截止时间 {{indexpage.update_date}}
                <el-tooltip class="My_tooltip_46a6ff" placement="top">
                    <div slot="content">
                      数据统计时间会更具您的登录时间间隔生成最新数据，时间差默认为15分钟。
                      （PS:15分钟内点击刷新获取数据不变！）
                    </div>
                    <i class="el-icon-question"></i>
                  </el-tooltip>
                <el-button type="primary" style="margin-left: 10px;" size="mini" @click="loadIndex">刷 新</el-button>
              </span>
            </div>
            <div class="body">
              <el-row :gutter="10" class="my_panel-group" >
                <el-col :xs="24" :sm="8" :md="6" :lg="3" class="card-panel-col"  v-hasPermi="['yunze:index:downCount']">
                    <div class="card-panel">




                      <router-link v-if="status_ShowId_show" :to="'/card/card/'+tools.encrypt(JSON.stringify(indexpage.requestMap.Pmap_downCount))"  >
                          <div class="card-panel">
                            <div class="card-panel-icon-wrapper icon-simCard">
                              <svg-icon icon-class="close" class-name="card-panel-icon"/>
                            </div>
                            <div class="card-panel-description">
                              <div class="card-panel-text">
                                已停机
                              </div>
                              <div class="card-panel-text">
                                <span class="card-panel-text-font">
                                  <count-to :start-val="0" :end-val="indexpage.downCount" :duration="1500" class="card-panel-num"/>
                                </span> 张
                              </div>
                            </div>
                          </div>
                        </router-link>
                    </div>
                </el-col>
                <el-col :xs="24" :sm="8" :md="6" :lg="3" class="card-panel-col" v-hasPermi="['yunze:index:overdoseCount']">
                  <div class="card-panel">
                    <router-link v-if="status_ShowId_show"  :to="'/card/card/'+ tools.encrypt(JSON.stringify(indexpage.requestMap.Pmap_overdoseCount))">
                      <div class="card-panel-icon-wrapper icon-upwardTrend">
                        <svg-icon icon-class="exceeded" class-name="card-panel-icon"/>
                      </div>
                      <div class="card-panel-description">
                        <div class="card-panel-text">
                          超用量
                        </div>
                        <div class="card-panel-text">
                            <span class="card-panel-text-font">
                              <count-to :start-val="0" :end-val="indexpage.overdoseCount" :duration="1500" class="card-panel-num"/>
                            </span> 张
                        </div>
                      </div>
                    </router-link>
                  </div>
                </el-col>
                <el-col :xs="24" :sm="8" :md="6" :lg="3" class="card-panel-col" v-hasPermi="['yunze:index:expiringSoonCount']">
                  <div class="card-panel">
                    <router-link v-if="status_ShowId_show" :to="'/card/card/'+ tools.encrypt(JSON.stringify(indexpage.requestMap.Pmap_expiringSoonCount))">
                      <div class="card-panel-icon-wrapper icon-used">
                        <svg-icon icon-class="cardMaturity" class-name="card-panel-icon"/>
                      </div>
                      <div class="card-panel-description">
                        <div class="card-panel-text">
                          快到期
                        </div>
                        <div class="card-panel-text">
                            <span class="card-panel-text-font">
                              <count-to :start-val="0" :end-val="indexpage.expiringSoonCount" :duration="1500" class="card-panel-num"/>
                            </span> 张
                        </div>
                      </div>
                    </router-link>
                  </div>
                </el-col>
                <el-col :xs="24" :sm="8" :md="6" :lg="3" class="card-panel-col" v-hasPermi="['yunze:index:thresholdReachedCount']">
                  <div class="card-panel">
                    <router-link v-if="status_ShowId_show"  :to="'/card/card/'+ tools.encrypt(JSON.stringify(indexpage.requestMap.Pmap_thresholdReachedCount))">
                        <div class="card-panel-icon-wrapper icon-Proportion">
                          <svg-icon icon-class="threshold" class-name="card-panel-icon"/>
                        </div>
                        <div class="card-panel-description">
                          <div class="card-panel-text">
                            已达停机阈值
                          </div>
                          <div class="card-panel-text">
                              <span class="card-panel-text-font">
                                <count-to :start-val="0" :end-val="indexpage.thresholdReachedCount" :duration="1500" class="card-panel-num"/>
                              </span> 张
                          </div>
                        </div>
                      </router-link>
                  </div>
                </el-col>
              </el-row>
            </div>
          </el-card>
        </el-col>
      </el-row>

      <el-divider/>
      <!--数据面板-->
      <el-row :gutter="40" class="panel-group" style="margin-left: 0px;margin-right: 0px;">
        <el-col :xs="24" :sm="12" :md="8" :lg="6" class="card-panel-col"  v-hasPermi="['yunze:index:simCardCount']">
          <div class="card-panel" @click="handleSetLineChartData('newVisitis')">
            <router-link v-if="status_ShowId_show"  :to="'/card/card/'+ tools.encrypt(JSON.stringify(indexpage.requestMap.Pmap_simCardCount))">
              <div class="card-panel-icon-wrapper icon-simCard">
                <svg-icon icon-class="simCard" class-name="card-panel-icon"/>
              </div>
              <div class="card-panel-description">
                <div class="card-panel-text">
                  SIM卡总数
                </div>
                <div class="card-panel-text">
                  <span class="card-panel-text-font">
                    <count-to :start-val="0" :end-val="indexpage.simCardCount" :duration="1500" class="card-panel-num"/>
                  </span> 张
                </div>
              </div>
            </router-link>
          </div>
        </el-col>
        <el-col :xs="24" :sm="12" :md="8" :lg="6" class="card-panel-col"  v-hasPermi="['yunze:index:simCardNewCount']">
          <div class="card-panel" @click="handleSetLineChartData('messages')">
            <router-link v-if="status_ShowId_show"  :to="'/card/card/'+ tools.encrypt(JSON.stringify(indexpage.requestMap.Pmap_simCardNewCount))">
              <div class="card-panel-icon-wrapper icon-upwardTrend">
                <svg-icon icon-class="upwardTrend" class-name="card-panel-icon"/>
              </div>
              <div class="card-panel-description">
                <div class="card-panel-text">
                  SIM卡本月新增
                </div>
                <div class="card-panel-text">
                  <span class="card-panel-text-font">
                    <count-to :start-val="0" :end-val="indexpage.simCardNewCount" :duration="1500" class="card-panel-num"/>
                  </span> 张
                </div>
              </div>
            </router-link>
          </div>
        </el-col>
        <el-col :xs="24" :sm="12" :md="8" :lg="6" class="card-panel-col"  v-hasPermi="['yunze:index:currentMonth']">
          <div class="card-panel" v-if="status_ShowId_show" @click="handleSetLineChartData('purchases')">
              <div class="card-panel-icon-wrapper icon-used">
                <svg-icon icon-class="used" class-name="card-panel-icon"/>
              </div>
              <div class="card-panel-description">
                <div class="card-panel-text">
                  本月数据用量
                  <el-tooltip class="My_tooltip" placement="top">
                    <div slot="content">
                      数据统计截止日期 {{indexpage.record_date}}
                    </div>
                    <i class="el-icon-question"></i>
                  </el-tooltip>
                </div>
                <div class="card-panel-text">
                  <span class="card-panel-text-font">
                    <count-to :start-val="0" :decimals="2" :end-val="indexpage.currentMonth" :duration="1500" class="card-panel-num"/>
                  </span> GB
                </div>
              </div>
          </div>
        </el-col>
        <el-col :xs="24" :sm="12" :md="8" :lg="6" class="card-panel-col"   v-hasPermi="['yunze:index:simActivity']">
          <div class="card-panel" @click="handleSetLineChartData('Proportions')">
            <router-link v-if="status_ShowId_show"  :to="'/card/card/'+ tools.encrypt(JSON.stringify(indexpage.requestMap.Pmap_simActivity))">
              <div class="card-panel-icon-wrapper icon-Proportion">
                <svg-icon icon-class="Proportion" class-name="card-panel-icon"/>
              </div>
              <div class="card-panel-description">
                <div class="card-panel-text">
                  SIM卡月活跃度
                  <el-tooltip class="My_tooltip" placement="top">
                    <div slot="content">
                      数据统计截止日期 {{indexpage.record_date}}
                    </div>
                    <i class="el-icon-question"></i>
                  </el-tooltip>
                </div>
                <div class="card-panel-text">
                  <span class="card-panel-text-font">
                    <count-to :start-val="0" :decimals="2" :end-val="indexpage.simActivity" :duration="1500" class="card-panel-num"/>
                  </span> %
                </div>
              </div>
            </router-link>
          </div>
        </el-col>
      </el-row>
      <!--数据面板 end-->
      <el-divider/>
      <!--生命周期 业务使用情况-->
      <el-row :gutter="10">
        <el-col :xs="24" :sm="24" :md="8" :lg="8"  v-hasPermi="['yunze:index:lifeCycleDistribution']">
          <el-card class="update-log">
            <div slot="header" class="clearfix">
              <span>生命周期分布</span>
            </div>
            <div class="body" >
              <pie-chart className="status_ShowId" width="100%" height="350px" v-if="status_ShowId_show"
                         :P_legend="status_ShowId_P_legend" :P_series="status_ShowId_P_series"
                         :P_color="status_ShowId_P_color"
              />
            </div>
          </el-card>
        </el-col>
        <el-col :xs="24" :sm="24" :md="16" :lg="16">
          <el-card class="update-log">
            <div slot="header" class="clearfix">
              <span>业务量使用情况</span>
            </div>
            <div class="body">
              <el-tabs tab-position="right" v-model="activeName" @tab-click="handleClick">
                <el-tab-pane label="数据" name="flow"  v-hasPermi="['yunze:index:businessVolumeFlow']" v-if="status_ShowId_show" >
                  <el-row :gutter="5">
                    <el-col :xs="24" :sm="24" :md="16" :lg="16">
                      <line-chart className="CardFlow" width="92%" height="350px" v-if="CardFlow_show"
                                  :chart-data="CardFlowArr" :P_xAxis="CardFlow_xAxis" :P_ShowLable="CardFlow_ShowLable"
                                  :P_colorArr="CardFlow_colorArr"
                                  :CardFlow_config="CardFlow_config"/>
                    </el-col>
                    <el-col :xs="24" :sm="24" :md="8" :lg="8">
                      <el-row :gutter="5" style="padding: 10px;">
                        <el-col :xs="12" :sm="12" :md="12" :lg="12">
                          <i class="el-icon-top"></i>
                          TOP排行：
                          <el-tooltip class="My_tooltip" placement="top">
                            <div slot="content">
                              数据统计截止日期 {{indexpage.record_date}};
                              按月查询时显示当月用量排行榜[MB]，折线图用量规格为【GB】;
                              按日查询时显示昨日用量排行榜[MB]，折线图用量规格为[MB]。
                            </div>
                            <i class="el-icon-question"></i>
                          </el-tooltip>
                        </el-col>
                        <el-col :xs="12" :sm="12" :md="12" :lg="12">
                          <el-switch
                            style="display: block;text-align: right;"
                            v-model="flowType"
                            active-color="#1682e6"
                            @change="changeflowType"
                            inactive-color="#13ce66"
                            active-text="按月"
                            inactive-text="按日">
                          </el-switch>
                        </el-col>
                      </el-row>
                      <el-table :data="cardFlowList">
                        <el-table-column label="虚拟编号" align="left" width="125">
                          <template slot-scope="scope">
                            <router-link v-if="status_ShowId_show" :to="'/card/card/'+ tools.encrypt(JSON.stringify({'type':'1','value':scope.row.iccid}))" class="link-type">
                              <span>{{ scope.row.vid }}</span>
                            </router-link>
                          </template>
                        </el-table-column>
                        <el-table-column label="用量（MB）" align="right" prop="used"/>
                      </el-table>
                    </el-col>
                  </el-row>
                </el-tab-pane>
                <!--<el-tab-pane label="语音" name="voice"  v-hasPermi="['yunze:index:businessVolumeVoice']">语音</el-tab-pane>
                <el-tab-pane label="短信" name="ShortMessage"  v-hasPermi="['yunze:index:businessVolumeVoice']">短信</el-tab-pane>-->
              </el-tabs>
            </div>
          </el-card>
        </el-col>
      </el-row>
      <!--生命周期 业务使用情况 END-->
      <el-divider/>

      <!--业务面板2-->
      <el-row :gutter="40" class="panel-group" style="margin-left: 0px;margin-right: 0px;">
        <el-col :xs="24" :sm="12" :md="8" :lg="6" class="card-panel-col"  v-hasPermi="['yunze:index:weChatOrderCollection']">
          <div class="card-panel" @click="handleSetLineChartData('newVisitis')">
            <router-link v-if="status_ShowId_show"  :to="'/order/ordeAll/'+ tools.encrypt(JSON.stringify(indexpage.requestMap.Pmap_getOrderSum))">
              <div class="card-panel-icon-wrapper icon-simCard">
                <svg-icon icon-class="wechat" class-name="card-panel-icon"/>
              </div>
              <div class="card-panel-description">
                <div class="card-panel-text">
                  本月微信订单收款
                </div>
                <div class="card-panel-text">
                     <span class="card-panel-text-font">
                          <count-to :start-val="0" :decimals="2" :end-val="indexpage.weChatOrderCollection" :duration="1500"
                                    class="card-panel-num"/>
                      </span> 元
                </div>
              </div>
            </router-link>
          </div>
        </el-col>
        <el-col :xs="24" :sm="12" :md="8" :lg="6" class="card-panel-col"  v-hasPermi="['yunze:index:orderAmount']">
          <div class="card-panel" @click="handleSetLineChartData('messages')">
            <router-link v-if="status_ShowId_show"  :to="'/order/ordeAll/'+ tools.encrypt(JSON.stringify(indexpage.requestMap.Pmap_getOrderAmount))">
              <div class="card-panel-icon-wrapper icon-upwardTrend">
                <svg-icon icon-class="Collection" class-name="card-panel-icon"/>
              </div>
                <div class="card-panel-description">
                  <div class="card-panel-text">
                    本月订单总金额
                  </div>
                  <div class="card-panel-text">
                       <span class="card-panel-text-font">
                              <count-to :start-val="0" :decimals="2" :end-val="indexpage.orderAmount" :duration="1500"
                                        class="card-panel-num"/>
                        </span> 元
                  </div>
                </div>
            </router-link>
          </div>
        </el-col>
        <el-col :xs="24" :sm="12" :md="8" :lg="6" class="card-panel-col"  v-hasPermi="['yunze:index:systemCliqueCount']">
          <div class="card-panel" @click="handleSetLineChartData('purchases')">
            <router-link v-if="status_ShowId_show"  :to="'/agent/Dept/'">
              <div class="card-panel-icon-wrapper icon-used">
                <svg-icon icon-class="group" class-name="card-panel-icon"/>
              </div>
              <div class="card-panel-description">
                <div class="card-panel-text">
                  系统企业总数
                </div>
                <div class="card-panel-text">
                      <span class="card-panel-text-font">
                            <count-to :start-val="0" :end-val="indexpage.systemCliqueCount" :duration="1500" class="card-panel-num"/>
                      </span> 个
                </div>
              </div>
            </router-link>
          </div>
        </el-col>
        <el-col :xs="24" :sm="12" :md="8" :lg="6" class="card-panel-col"  v-hasPermi="['yunze:index:systemUserCount']">
          <div class="card-panel" @click="handleSetLineChartData('Proportions')">
            <router-link v-if="status_ShowId_show"  :to="'/agent/agentUser/'">
              <div class="card-panel-icon-wrapper icon-Proportion">
                <svg-icon icon-class="loggedUser" class-name="card-panel-icon"/>
              </div>
              <div class="card-panel-description">
                <div class="card-panel-text">
                  系统用户总数
                </div>
                <div class="card-panel-text">
                      <span class="card-panel-text-font">
                        <count-to :start-val="0" :end-val="indexpage.systemUserCount" :duration="1500" class="card-panel-num"/>
                      </span> 个
                </div>
              </div>
            </router-link>
          </div>
        </el-col>
      </el-row>
      <!--业务面板2 END-->
      <el-divider/>

      <!--业务面板 3-->
      <el-row :gutter="40" class="panel-group" style="margin-left: 0px;margin-right: 0px;">
        <el-col :xs="24" :sm="12" :md="8" :lg="6" class="card-panel-col"  v-hasPermi="['yunze:index:activationCardCount']">
          <div class="card-panel" @click="handleSetLineChartData('newVisitis')">
            <router-link v-if="status_ShowId_show"  :to="'/card/card/'+ tools.encrypt(JSON.stringify(indexpage.requestMap.Pmap_activationCardCount))">
              <div class="card-panel-icon-wrapper icon-simCard">
                <svg-icon icon-class="cardActivation" class-name="card-panel-icon"/>
              </div>
              <div class="card-panel-description">
                <div class="card-panel-text">
                  本月激活卡数
                </div>
                <div class="card-panel-text">
                     <span class="card-panel-text-font">
                          <count-to :start-val="0"  :end-val="indexpage.activationCardCount" :duration="1500"
                                    class="card-panel-num"/>
                      </span> 张
                </div>
              </div>
            </router-link>
          </div>
        </el-col>
        <el-col :xs="24" :sm="12" :md="8" :lg="6" class="card-panel-col"  v-hasPermi="['yunze:index:shipCard']">
          <div class="card-panel" @click="handleSetLineChartData('messages')">
            <router-link v-if="status_ShowId_show"  :to="'/card/card/'+ tools.encrypt(JSON.stringify(indexpage.requestMap.Pmap_shipCard))">
              <div class="card-panel-icon-wrapper icon-upwardTrend">
                <svg-icon icon-class="cardShip" class-name="card-panel-icon"/>
              </div>
              <div class="card-panel-description">
                <div class="card-panel-text">
                  本月发货卡数
                </div>
                <div class="card-panel-text">
                   <span class="card-panel-text-font">
                          <count-to :start-val="0" :end-val="indexpage.shipCard" :duration="1500"
                                    class="card-panel-num"/>
                    </span> 张
                </div>
              </div>
            </router-link>
          </div>
        </el-col>
        <el-col :xs="24" :sm="12" :md="8" :lg="6" class="card-panel-col"  v-hasPermi="['yunze:index:logInIp']">
          <div class="card-panel" @click="handleSetLineChartData('purchases')">
            <router-link v-if="status_ShowId_show"  :to="'/log/logininfor/'">
              <div class="card-panel-icon-wrapper icon-used">
                <svg-icon icon-class="Ip" class-name="card-panel-icon"/>
              </div>
              <div class="card-panel-description">
                <div class="card-panel-text">
                  本月登录IP总数
                </div>
                <div class="card-panel-text">
                      <span class="card-panel-text-font">
                            <count-to :start-val="0" :end-val="indexpage.logInIp" :duration="1500" class="card-panel-num"/>
                      </span> 个
                </div>
              </div>
            </router-link>
          </div>
        </el-col>
        <el-col :xs="24" :sm="12" :md="8" :lg="6" class="card-panel-col"  v-hasPermi="['yunze:index:onlineCount']">
          <div class="card-panel" @click="handleSetLineChartData('Proportions')">
            <router-link v-if="status_ShowId_show" :to="'/monitor/online/'">
              <div class="card-panel-icon-wrapper icon-Proportion">
                <svg-icon icon-class="onlineUser" class-name="card-panel-icon"/>
              </div>
              <div class="card-panel-description">
                <div class="card-panel-text">
                  当前在线用户
                  <el-tooltip style="color: red" placement="top">
                    <div slot="content">
                      数据统计截止日期 {{this.getOnelinCountTime}},默认 120S 刷新一次
                    </div>
                    <i class="el-icon-question"></i>
                  </el-tooltip>
                </div>
                <div class="card-panel-text">
                      <span class="card-panel-text-font">
                        <count-to :start-val="0" :end-val="indexpage.onlineCount" :duration="1500" class="card-panel-num"/>
                      </span> 个
                </div>
              </div>
            </router-link>



          </div>
        </el-col>
      </el-row>
      <!--业务面板 3 END-->
      <el-divider/>



    </el-main>
    <el-aside width="200px" style=" background-color: #ffffff;">
      <el-card class="update-log" style="height: 100%;">
        <div slot="header" class="clearfix" style="height: 18px;    margin-top: -8px;">
          <span style="font-size: 14px;">快捷入口</span>
        </div>
        <div class="body">
          <ul class="el-menu">
            <router-link  :to="'/card/ExecutionTask/'" class="link-type">

              <li role="menuitem" tabindex="1">
                <span>执行日志</span>
              </li>
            </router-link>
            <router-link  v-hasPermi="['yunze:card:list']" :to="'/card/card/'"
                         class="link-type">
              <li role="menuitem" tabindex="1">
                <span>物联卡管理</span>
              </li>
            </router-link>
            <router-link v-hasPermi="['yunze:card:Setting']" :to="'/card/cardSetting/'"
                         class="link-type">
              <li role="menuitem" tabindex="2">
                <span>物联卡设置</span>
              </li>
            </router-link>
            <router-link  v-hasPermi="['yunze:card:diagnosis']" :to="'/card/diagnosis/'"
                          class="link-type">
              <li role="menuitem" tabindex="1">
                <span>智能诊断</span>
              </li>
            </router-link>



            <router-link  v-hasPermi="['yunze:order:addPackage']" :to="'/card/rechargeTariff'"
                          class="link-type">
              <li role="menuitem" tabindex="1">
                <span>资费订购</span>
              </li>
            </router-link>
            <router-link  v-hasPermi="['yunze:tariffGroup:list']" :to="'/card/tariffGroupT/'"
                          class="link-type">
              <li role="menuitem" tabindex="1">
                <span>平台资费</span>
              </li>
            </router-link>
            <router-link  v-hasPermi="['yunze:agentTariffGroup:list']" :to="'/card/agentTariffGroup/'"
                          class="link-type">
              <li role="menuitem" tabindex="1">
                <span>客户资费</span>
              </li>
            </router-link>


            <!--<router-link v-hasPermi="['tool:apifile:list']" :to="'/system/apifile'"
                         class="link-type">
              <li role="menuitem" tabindex="2">
                <span>API使用</span>
              </li>
            </router-link>-->

            <router-link v-hasPermi="['system:dept:list']" :to="'/agent/Dept/'"
                         class="link-type">
              <li role="menuitem" tabindex="2">
                <span>企业管理</span>
              </li>
            </router-link>
            <router-link v-hasPermi="['yunze:agent:list']" :to="'/agent/agentUser/'"
                         class="link-type">
              <li role="menuitem" tabindex="2">
                <span>用户信息</span>
              </li>
            </router-link>

            <router-link v-hasPermi="['yunze:order:list']" :to="'/order/ordeAll/'"
                         class="link-type">
              <li role="menuitem" tabindex="2">
                <span>全部订单</span>
              </li>
            </router-link>

          </ul>

        </div>
      </el-card>
    </el-aside>

    <div  v-hasPermi="['yunze:index:onlineCount']">
      <div v-if="loadingOnelinCount==false" >
        {{this.getOnelinCount()}}
      </div>
    </div>
  </el-container>
</template>

<script>
  import  { findToDay } from "@/api/yunze/system/index";
  import  { list } from "@/api/monitor/online";

  import tools from "@/utils/yunze/tools";

  import LineChart from './dashboard/LineChart'
  import PieChart from './dashboard/PieChart'
  import CountTo from 'vue-count-to'

  export default {
    name: "index",
    components: {
      PieChart,
      LineChart,
      CountTo,
    },
    created() {

      //不点击刷新按钮时获取的第一次缓存的数据
      if(window['IoTLink-indexData']!=undefined && window['IoTLink-indexData']!=null){
       this.indexpage =  window['IoTLink-indexData'];
        this.status_ShowId_show = false;
        this.Myload();
      }else{
        this.loadIndex();//加载首页
      }



     /* this.cardFlowList = [{
        'vid': '1410158305863',
        'used': '100000000.01',
        'iccid': '8986112124301965946'
      }, {'vid': '1410158305863', 'used': '100000000.01', 'iccid': '8986112124301965946'},
        {'vid': '1410158305863', 'used': '100000000.01', 'iccid': '8986112124301965946'}, {
          'vid': '1410158305863',
          'used': '100000000.01',
          'iccid': '8986112124301965946'
        },
        {'vid': '1410158305863', 'used': '100000000.01', 'iccid': '8986112124301965946'},];
*/

      this.CardFlow_ShowLable = {Day_CardFlowArr: "日用量MB", Month_CardFlowArr: "月用量GB"};
      this.CardFlow_config = {"smooth": false, 'areaStyle_show': "true"};
    },
    data() {
      return {
        loading: true,
        activeName: 'flow',
        allowloading:true,//是否允许加载
        loadingOnelinCount:true,//是否 已经加载在线人数
        getOnelinCountTime:"",//获取在线 人数 时间
        indexpage:{
          id: null,
          downCount: 0,
          overdoseCount: 0,
          expiringSoonCount: 0,
          thresholdReachedCount: 0,
          simCardCount: 0,
          simCardNewCount: 0,
          currentMonth: 0,
          simActivity: 0,
          lifeCycleDistribution: {
            data:[],
          },
          businessVolumeFlow: {
            Day_CardFlowMap:{
              Day_CardFlowArr:[],
            },
            Day_CardFlow_xAxis:[],
            Day_cardFlowList:[
              {
                iccid:'',
                used:'',
                vid:'',
              }
            ],
            Month_CardFlowMap:{
              Month_CardFlowArr:[],
            },
            Month_CardFlow_xAxis:[],
            Month_cardFlowList:[
              {
                iccid:'',
                used:'',
                vid:'',
              }
            ],
            requestMap:{
              Pmap_simCardCount:{},
              Pmap_downCount:{},
              Pmap_overdoseCount:{},
              Pmap_expiringSoonCount:{},
              Pmap_thresholdReachedCount:{},
              Pmap_simCardNewCount:{},
              Pmap_currentMonth:{},
              Pmap_simActivity:{},
              Pmap_status_ShowIdGroup:{},
              Pmap_activationCardCount:{},
              Pmap_shipCard:{},
              Pmap_customerNewCount:{},
              Pmap_salesContractCount:{},
              Pmap_getOrderSum:{},
              Pmap_getOrderAmount:{},
              Pmap_logInIp:{},
            },
          },
          businessVolumeVoice: {},
          businessVolumeMessage: {},
          customerCount: 0,
          customerNewCount: 0,
          salesContractCount: 0,
          salesContractDepositCount: 0,
          weChatOrderCollection: 0,
          orderAmount: 0,
          systemCliqueCount: 0,
          systemUserCount: 0,
          activationCardCount: 0,
          shipCard: 0,
          logInIp: 0,
          create_date: '',
          update_date: '',
          record_date: '',
          dept_id: '',
          onlineCount: 0,
        },

        // 版本号
        version: "0.5.0",
        title: '12312312',
        linkArr: [],
        tools: tools,
        status_ShowId_P_legend: {},
        status_ShowId_show:false,
        status_ShowId_P_series: {
          data:[],
          name: "生命周期占比",
          radius: [15, 95],
        },
        status_ShowId_P_color: ['#b180f5', '#0186ff', '#61c5f9', '#38cb8e', '#fa535f', '#fe9e74', '#899caf', '#303133'],

        flowType: false,//false = 日查询 月查询 [用量]
        cardFlowList: [],//数据排行
        CardFlowArr: {},//折线图 【用量】
        CardFlow_show:false,
        CardFlow_ShowLable: {},//折线图 key 对应显示 lable
        CardFlow_xAxis: [],//折线图 X 轴显示
        CardFlow_colorArr: ['#3c97fd', '#67c23a'],//折线图 颜色
        CardFlow_config: {},//折线图 基础配置
      };




    },
    mounted() {

    },
    methods: {
      goTarget(href) {
        window.open(href, "_blank");
      },
      handleClick(tab, event) {
        console.log(tab, event);
      },

      handleSetLineChartData(type) {

      },

      //按日按月查询 [数据]
      changeflowType(){

        if(this.indexpage.businessVolumeFlow.Day_CardFlow_xAxis!=null &&  this.indexpage.businessVolumeFlow.Day_CardFlow_xAxis.length>0){
          //this.CardFlow_xAxis = this.indexpage.lifeCycleDistribution.Day_CardFlow_xAxis;
          if(this.flowType==true){
            this.CardFlowArr = this.indexpage.businessVolumeFlow.Month_CardFlowMap;
            this.CardFlow_xAxis = this.indexpage.businessVolumeFlow.Month_CardFlow_xAxis;
            this.cardFlowList = this.indexpage.businessVolumeFlow.Month_cardFlowList;
            this.CardFlow_colorArr = ['#67c23a'];
          }else{
            this.CardFlowArr = this.indexpage.businessVolumeFlow.Day_CardFlowMap;
            this.CardFlow_xAxis = this.indexpage.businessVolumeFlow.Day_CardFlow_xAxis;
            this.cardFlowList = this.indexpage.businessVolumeFlow.Day_cardFlowList;
            this.CardFlow_colorArr = ['#3c97fd'];
          }
          this.CardFlow_show = true;
        }else{

        }
      },

      //获取在线用户总数
      getOnelinCount(){
        console.log("getOnelinCount")
        if(this.loadingOnelinCount==false){
          this.loadingOnelinCount = true;
          list(this.queryParams).then(response => {
            //console.log(response)
            this.getOnelinCountTime =  tools.getdatetime();
            this.indexpage.onlineCount = response.total;
            let _this = this;
            setTimeout(function(){
              _this.loadingOnelinCount = false;
              //console.log("setloadingOnelinCount")
            },120*1000);
          });
        }
      },


      loadIndex(){
        //console.log(this.allowloading)
        if(this.allowloading) {

          let Pwd_Str = tools.encrypt(JSON.stringify({}));
          findToDay(Pwd_Str).then(response => {
            this.allowloading = false;//禁止重复多次加载
            let jsonobj = JSON.parse(tools.Decrypt(response));
            // console.log(jsonobj);
            if (jsonobj.code == 200) {
              let data = jsonobj.Data.data;
              if (data != null && data != undefined) {
                this.indexpage = data;
                this.indexpage.onlineCount = 1;
                if (data.businessVolumeFlow != null) {
                  let businessVolumeFlow = JSON.parse(data.businessVolumeFlow);
                  this.indexpage.businessVolumeFlow = businessVolumeFlow;
                }
                if (data.businessVolumeMessage != null) {
                  let businessVolumeMessage = JSON.parse(data.businessVolumeMessage);
                  this.indexpage.businessVolumeMessage = businessVolumeMessage;
                }
                if (data.businessVolumeVoice != null) {
                  let businessVolumeVoice = JSON.parse(data.businessVolumeVoice);
                  this.indexpage.businessVolumeVoice = businessVolumeVoice;
                }
                if (data.lifeCycleDistribution != null) {
                  let lifeCycleDistribution = JSON.parse(data.lifeCycleDistribution);
                  this.indexpage.lifeCycleDistribution = lifeCycleDistribution;
                }
                if (data.requestMap != null) {
                  let requestMap = JSON.parse(data.requestMap);
                  this.indexpage.requestMap = requestMap;
                }
                // console.log( this.indexpage.requestMap)
                window['IoTLink-indexData'] = this.indexpage;
                this.Myload();
              } else {
                this.status_ShowId_P_series.data = [
                  {value: 0, name: '库存'},
                  {value: 0, name: '可测试'},
                  {value: 0, name: '待激活'},
                  {value: 0, name: '已激活'},
                  {value: 0, name: '已停机'},
                  {value: 0, name: '预销户'},
                  {value: 0, name: '已销户'},
                  {value: 0, name: '未知'},
                ];
                this.status_ShowId_show = true;

                this.CardFlowArr = {
                  expectedData: [0, 0, 0, 0, 0, 0, 0],
                  /*actualData: [120, 82, 91, 154, 162, 140, 145]*/
                };
                this.CardFlow_xAxis = ['01-12', '01-13', '01-14', '01-15', '01-16', '01-17', '01-18'];
                this.CardFlow_show = true;
              }


              this.msgSuccess(jsonobj.Data.Message);
            } else {
              let msg = jsonobj.Data != null && jsonobj.Data != '' && jsonobj.Data.Message != null && jsonobj.Data.Message != '' ? jsonobj.Data.Message : jsonobj.msg;
              this.msgError(msg);
            }
            if (jsonobj.deptId == '100'){
              this.loadingOnelinCount = false;//加载 在线用户
            }
            let _this = this;
            setTimeout(function () {
              _this.allowloading = true;
              //console.log("setAllowloading")
            }, 30000);
          })

        }else{
          this.msgError("请30S后再请求刷新！");
        }
    },


      Myload(){

        //生命周期分布
        if(this.indexpage.lifeCycleDistribution.data!=null &&  this.indexpage.lifeCycleDistribution.data.length>0){
          this.status_ShowId_P_series.data = this.indexpage.lifeCycleDistribution.data;
        }else{
          this.status_ShowId_P_series.data = [
            {value: 0, name: '库存'},
            {value: 0, name: '可测试'},
            {value: 0, name: '待激活'},
            {value: 0, name: '已激活'},
            {value: 0, name: '已停机'},
            {value: 0, name: '预销户'},
            {value: 0, name: '已销户'},
            {value: 0, name: '未知'},
          ];
        }

        //业务使用量
        // 数据
        if(this.indexpage.businessVolumeFlow.Day_CardFlow_xAxis!=null &&  this.indexpage.businessVolumeFlow.Day_CardFlow_xAxis.length>0){
          //this.CardFlow_xAxis = this.indexpage.lifeCycleDistribution.Day_CardFlow_xAxis;
          this.CardFlowArr = this.indexpage.businessVolumeFlow.Day_CardFlowMap;
          this.CardFlow_xAxis = this.indexpage.businessVolumeFlow.Day_CardFlow_xAxis;
          this.cardFlowList = this.indexpage.businessVolumeFlow.Day_cardFlowList;
        }else{
          this.CardFlowArr = {
            expectedData: [100, 120, 161, 134, 105, 160, 165],
            /*actualData: [120, 82, 91, 154, 162, 140, 145]*/
          };
          this.CardFlow_xAxis = ['01-12', '01-13', '01-14', '01-15', '01-16', '01-17', '01-18'];
        }
        this.CardFlow_show = true;
        this.status_ShowId_show = true;
        //console.log( this.indexpage.requestMap.Pmap_activationCardCount)
      },

    },








  };
</script>

<style scoped lang="scss">
  .home {
    blockquote {
      /*padding: 10px 20px;*/
      margin: 0 0 20px;
      font-size: 17.5px;
      border-left: 5px solid #eee;
    }

    hr {
      margin-top: 20px;
      margin-bottom: 20px;
      border: 0;
      border-top: 1px solid #eee;
    }

    .col-item {
      margin-bottom: 20px;
    }

    ul {
      padding: 0;
      margin: 0;
    }

    font-family: "open sans", "Helvetica Neue", Helvetica, Arial, sans-serif;
    font-size: 13px;
    color: #676a6c;
    overflow-x: hidden;

    ul {
      list-style-type: none;
    }

    h4 {
      margin-top: 0px;
    }

    h2 {
      margin-top: 10px;
      font-size: 26px;
      font-weight: 100;
    }

    p {
      margin-top: 10px;

      b {
        font-weight: 700;
      }
    }

    .update-log {
      ol {
        display: block;
        list-style-type: decimal;
        margin-block-start: 1em;
        margin-block-end: 1em;
        margin-inline-start: 0;
        margin-inline-end: 0;
        padding-inline-start: 40px;
      }
    }

    .app-container {
      padding: 8px !important;
    }

    .my-link-type {
      text-align: center;
    }

    .el-main {
      padding: 8px 10px !important;
    }

    aside {
      padding: 8px 10px !important;
    }

    .el-menu-item {
      padding: 0px !important;
      font-size: 16px;
    }

    .el-menu {
      border-right: solid 0px #e6e6e6;
    }

    .right_5 {
      margin-right: 5px;
    }

    .el-divider--horizontal {
      margin: 8px 0 !important;
    }


  }


  .el-card__body {
    padding: 0px 10px 0px 10px!important;
  }

  .My_tooltip {
    color: #fd883c;
  }

  .My_tooltip_46a6ff {
    color: #46a6ff;
  }

  .el-card__header {
    padding: 10px 19px !important;
  }
</style>


<style lang="scss" scoped>
  .panel-group {
    /*margin-top: 18px;*/

    .card-panel-col {
      /* margin-bottom: 32px;*/
    }

    .card-panel {
      height: 70px;
      cursor: pointer;
      font-size: 12px;
      position: relative;
      overflow: hidden;
      color: #666;
      background: #fff;
      box-shadow: 4px 4px 40px rgba(0, 0, 0, .05);
      border-color: rgba(0, 0, 0, .05);

      &:hover {
        .card-panel-icon-wrapper {
          color: #fff;
        }

        .icon-simCard {
          background: #40c9c6;
        }

        .icon-upwardTrend {
          background: #36a3f7;
        }

        .icon-used {
          background: #f4516c;
        }

        .icon-Proportion {
          background: #1682e6
        }
      }

      .icon-simCard {
        color: #40c9c6;
      }

      .icon-upwardTrend {
        color: #36a3f7;
      }

      .icon-used {
        color: #f4516c;
      }

      .icon-Proportion {
        color: #1682e6
      }

      .card-panel-icon-wrapper {
        float: left;
        /* margin: 14px 0 0 14px;*/
        padding: 10px;
        transition: all 0.38s ease-out;
        border-radius: 6px;
        margin-right: 5px;
      }

      .card-panel-icon {
        float: left;
        font-size: 48px;
      }

      .card-panel-description {
        /* float: right;*/
        font-weight: bold;
        margin: 10px;
        margin-left: 5px;

        .card-panel-text {
          line-height: 18px;
          color: rgba(0, 0, 0, 0.45);
          font-size: 16px;
          margin-bottom: 12px;
        }

        .card-panel-text-font {
          font-weight: 700;
          font-size: 1.24rem;
          color: #3b4e66;
          margin-right: 0.1rem;
        }

        .card-panel-num {
          font-size: 20px;
        }
      }
    }
  }

  @media (max-width: 550px) {
    .card-panel-description {
      display: none;
    }

    .card-panel-icon-wrapper {
      float: none !important;
      width: 100%;
      height: 100%;
      margin: 0 !important;

      .svg-icon {
        display: block;
        margin: 14px auto !important;
        float: none !important;
      }
    }
  }


</style>


<style lang="scss" scoped>
  .my_panel-group {
    /*margin-top: 18px;*/

    .card-panel-col {
      /* margin-bottom: 32px;*/
    }

    .card-panel {
      /*height: 70px;*/
      cursor: pointer;
      font-size: 12px;
      position: relative;
      overflow: hidden;
      color: #666;
      background: #fff;
      box-shadow: 4px 4px 40px rgba(0, 0, 0, .05);
      border-color: rgba(0, 0, 0, .05);

      &:hover {
        .card-panel-icon-wrapper {
          color: #fff;
        }

        .icon-simCard {
          background: #40c9c6;
        }

        .icon-upwardTrend {
          background: #36a3f7;
        }

        .icon-used {
          background: #f4516c;
        }

        .icon-Proportion {
          background: #1682e6
        }
      }

      .icon-simCard {
        color: #40c9c6;
      }

      .icon-upwardTrend {
        color: #36a3f7;
      }

      .icon-used {
        color: #f4516c;
      }

      .icon-Proportion {
        color: #1682e6
      }

      .card-panel-icon-wrapper {
        float: left;
        /* margin: 14px 0 0 14px;*/
        padding: 10px;
        transition: all 0.38s ease-out;
        border-radius: 6px;
        margin-right: 5px;
      }

      .card-panel-icon {
        float: left;
        font-size: 40px;
      }

      .card-panel-description {
        /* float: right;*/
        font-weight: bold;
        margin: 5px;
        margin-left: 2.5px;

        .card-panel-text {
          line-height: 24px;
          color: rgba(0, 0, 0, 0.45);
          font-size: 15px;
          /*margin-bottom: 12px;*/
        }

        .card-panel-text-font {
          font-weight: 700;
          font-size: 1.24rem;
          color: #3b4e66;
          margin-right: 0.1rem;
        }

        .card-panel-num {
          font-size: 20px;
        }
      }
    }
  }

  @media (max-width: 550px) {
    .card-panel-description {
      display: none;
    }

    .card-panel-icon-wrapper {
      float: none !important;
      width: 100%;
      height: 100%;
      margin: 0 !important;

      .svg-icon {
        display: block;
        margin: 14px auto !important;
        float: none !important;
      }
    }
  }


</style>
<style>
  body{
    background-color:  rgb(255 255 255 / 50%);
  }
</style>
