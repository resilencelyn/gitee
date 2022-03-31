<template>
  <div class="app-container home">

    <el-row :gutter="20">

      <el-col :sm="24" :lg="12" style="padding-left: 20px">
        <h2>  IoTLink</h2>
        <h3>  IoTLink 快速开发框架无需授权即可商业使用，代码全部开源免费且无任何加密。</h3>
        <h3>  IoTLink基于 SpringBoot、Vue、Mybatis、RabbitMq、Mysql、Redis 等开发,支持物联网卡、物联网模组、卡+模组融合管理。</h3>
        <h4>  提供状态、资费、客户、进销存、合同、订单、续费、充值、诊断、账单等功能。</h4>
        <h4>  平台可同时接入中国移动、中国电信、中国联通、第三方物联网卡进行统一管理。逐步完善平台，助您快速接入物联网，让万物互联更简单。</h4>
        <h4>  通信管理:主要用于所有物联卡的综合信息查询，同时提供不涉及资费产品订购的终端和物联卡的功能配置管理。</h4>
        <h4>  资费管理:资费决定了用户使用通信服务功能，如数据、短信和语言服务的权限和计费方式。</h4>
        <h4>  生命周期:重新定义了物联卡在使用过程中各个特征时期，目前分为七大生命周期，包括：可测试、库存、待激活、已激活、已停机、预销户、已销户(参考移动EC)。</h4>
        <p>
          <b>当前版本:</b> <span>v{{ version }}</span>
        </p>
        <p>
          <el-button type="danger">&yen;免费开源</el-button>
          <el-button
            type="danger"
            size="mini"
            icon="el-icon-cloudy"
            plain
            @click="goTarget('https://www.aliyun.com/minisite/goods?userCode=3mwvbeng')"
          >领取阿里云通用云产品1888优惠券</el-button>
        </p>
        <p>
          <el-button
            type="primary"
            size="mini"
            icon="el-icon-cloudy"
            plain
            @click="goTarget('https://gitee.com/sdyunze/iotlink?_from=gitee_search')"
          >访问码云</el-button
          >
          <el-button
            size="mini"
            icon="el-icon-s-home"
            plain
            @click="goTarget('http://demo.5iot.com')"
            >访问主页</el-button
          >
        </p>
      </el-col>
      <el-col :sm="24" :lg="12" style="padding-left: 50px">
        <el-row>
          <el-col :span="12">
            <h2>技术选型</h2>
          </el-col>
        </el-row>
        <el-row>
          <el-col :span="6">
            <h4>后端技术</h4>
            <ul>
              <li>SpringBoot</li>
              <li>Spring Security</li>
              <li>JWT</li>
              <li>MyBatis</li>
              <li>Druid</li>
              <li>Fastjson</li>
              <li>RabbtMQ</li>
              <li>...</li>
            </ul>
          </el-col>
          <el-col :span="6">
            <h4>前端技术</h4>
            <ul>
              <li>Vue</li>
              <li>Vuex</li>
              <li>Element-ui</li>
              <li>Axios</li>
              <li>Sass</li>
              <li>Quill</li>
              <li>...</li>
            </ul>
          </el-col>
        </el-row>
      </el-col>
    </el-row>

    <el-divider />

    <el-row :gutter="20">
      <el-col :xs="24" :sm="24" :md="12" :lg="8">
        <el-card class="update-log">
          <div slot="header" class="clearfix">
            <span>联系信息</span>
          </div>
          <div class="body">
            <p>
              <i class="el-icon-s-promotion"></i> 官网：<el-link
                href="http://www.5iot.com"
                target="_blank"
                >http://www.5iot.com</el-link
              >
            </p>
            <p>
              <i class="el-icon-money"></i> 文档：
              <el-link
                href="http://www.5iot.com/doc"
                target="_blank"
              >http://www.5iot.com/doc</el-link
              >
            </p>
          </div>
        </el-card>
      </el-col>
      <el-col :xs="24" :sm="24" :md="12" :lg="8">
        <el-card class="update-log">
          <div slot="header" class="clearfix">
            <span>更新日志</span>
          </div>

          <el-collapse accordion v-model="activeName">
            <el-collapse-item title="v0.7.0 - 2022-03-31"  name="3">
              <ol>
                <li>新增 智能诊断、卡号匹对</li>
                <li>新增 通道-轮询算法类型 高频轮询、智能轮询</li>
                <li>新增 批量取消实名 批量更新卡信息</li>
                <li>新增 定时任务[未划分通道-邮件抄送通知]</li>
                <li>新增 定时任务[未划分资费组-邮件抄送通知]</li>
                <li>新增 定时任务[归属为总平台-有用量-邮件抄送通知]</li>
                <li>新增 定时任务[清除大于X天用量记录(默认为开启状态清除100之前用量记录)]</li>
                <li>优化 界面操作 高级查询默认隐藏</li>
                <li>优化 登录界面样式</li>
                <li>优化 执行日志导出csv文件转换换EXCLE</li>
                <li>优化 轮询同步消费者、轮询算法类型实现</li>
                <li>整合 用户信息、企业列表、内部管理</li>
                <li>开放 微信Web端用量查询 充值续费 商品购买</li>
              </ol>
            </el-collapse-item>
            <el-collapse-item title="v0.6.0 - 2022-02-14"  name="2">
              <ol>
                <li>新增首页看板、看板选项点击快速跳转查询</li>
                <li>新增首页数据生成任务</li>
                <li>优化说明文档架构图</li>
              </ol>
            </el-collapse-item>
            <el-collapse-item title="v0.5.0 - 2022-01-14"  name="1">
              <ol>
                <li>IoTLink系统开发版发布</li>
              </ol>
            </el-collapse-item>
          </el-collapse>
        </el-card>
      </el-col>
      <el-col :xs="24" :sm="24" :md="12" :lg="8">
        <el-card class="update-log">
          <div slot="header" class="clearfix">
            <span>关注公众号获取一手行业信息</span>
          </div>
          <div class="body">
            <img
              src="../../public/wxgzh.jpg"
              alt="donate"
            />

          </div>
        </el-card>
      </el-col>
    </el-row>
  </div>
</template>

<script>
export default {
  name: "iotlinkindex",
  data() {
    return {
      // 版本号
      version: "0.7.0",
      activeName:'3',
    };
  },
  methods: {
    goTarget(href) {
      window.open(href, "_blank");
    },
  },
};
</script>

<style scoped lang="scss">
.home {
  blockquote {
    padding: 10px 20px;
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
}
</style>

