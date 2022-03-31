<template>
  <div class="app-container">
    <el-row :gutter="20" v-for="items in itemsArr">
      <el-col :span="mainwidth" :xs="24">
        <el-form :model="queryParams" ref="queryForm" :inline="true" v-show="showSearch" label-width="68px">
          <a href="https://www.5iot.com/" target="view_frame"><h1 style="text-align:center;">山东云则信息技术有限公司</h1></a>
          <a href="http://i.5iot.cn/" target="view_frame"><h1 style="text-align:center;color:blue;">
            <u>
              i.5iot.cn
            </u>
          </h1>
          </a>
        </el-form>

        <!--接口说明-->
        <el-collapse v-model="AccountActiveNames" style="margin-left: 10px; margin-top: 10px;">
          <el-collapse-item name="0">
            <template slot="title">
              <div style="line-height :10px">
                <h1 style=" padding-left: 15px; font-size:28px;">一、接口说明</h1>
              </div>
            </template>
            <p data-v-md-line="2" style="text-align:center;font-size:30px;line-height :8px;">版本说明</p>
            <el-table :data="items.editionList" border>
              <el-table-column label="版本号" align="center" prop="a"/>
              <el-table-column label="更新时间" align="center" prop="b"/>
              <el-table-column label="更新人" align="center" prop="c"/>
              <el-table-column label="主要内容或重大修改" align="center" prop="d"/>
            </el-table>
            <br>
            <h3 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;text-align:center;">
              <li>目录</li>
            </h3>
            <p data-v-md-line="2" style=" padding-left: 50px; font-size:20px;text-align:center;">
              1.1： 接口命名规则..........................................................................................3<br>
              1.2：
              请求方式.................................................................................................3<br>
              1.3：
              参数规则.................................................................................................3<br>
              1.4：
              API接口支撑...........................................................................................3<br>
            </p>
            <hr>
            <p data-v-md-line="2" style=" padding-left: 50px; font-size:20px;">1.1 接口命名规则</p>
            <ul data-v-md-line="5" style=" padding-left: 110px; line-height :20px; font-size:20px;">
              <li style="line-height :10px;">服务器地址：<code style="color:blue;">{{ items.ServerUrl }}</code></li>
              <div style="width: 300px;line-height :50px">
                <li>域名：<code style="color:blue;">{{ items.domainName }}</code></li>
              </div>
              <li>接口名：<code style="color:blue;">{{ items.InterfaceName }}</code></li>
            </ul>
            <div>
              <h1 style=" padding-left: 15px; font-size:28px;">通用参数</h1>
            </div>


            <el-table :data="items.cardList" border>
              <el-table-column label="名称" align="center" prop="a"/>
              <el-table-column label="类型" align="center" prop="b"/>
              <el-table-column label="必填" align="center" prop="c"/>
              <el-table-column label="说明" align="center" prop="d"/>
              <el-table-column label="备注" align="center" prop="e"/>
            </el-table>

            <p data-v-md-line="2" style=" padding-left: 50px; font-size:20px;">1.2 请求方式</p>
            <h1 style=" padding-left: 90px; font-size:20px;">请求方式: post</h1>
            <p data-v-md-line="2" style=" padding-left: 50px; font-size:20px;">1.3 参数规则</p>
            <ol type="1" style=" padding-left: 110px; line-height :20px; font-size:20px;">
              <li style="line-height :30px;"> 采用HTTP传输协议。</li>
              <li style="line-height :30px;"> 通过POST请求把提交的数据则放置在HTTP包的包体中。</li>
              <li style="line-height :35px;"> 所有参数值默认需要转换为UTF-8编码。</li>
              <li style="line-height :35px;"> 支持POST请求方式的API接口。</li>
            </ol>
            <p data-v-md-line="2" style=" padding-left: 50px; font-size:20px;">1.4 API接入支撑</p>
            <ul data-v-md-line="5" style=" padding-left: 110px; line-height :20px; font-size:20px;">
              <li style="line-height :10px;">技术支持人员联系邮箱：<code style="color:blue;">{{ items.mailbox }}</code></li>
            </ul>
            <p data-v-md-line="2" style=" padding-left: 50px; font-size:20px;">1.5免责声明</p>
            <p data-v-md-line="2" style=" padding-left: 90px; font-size:18px;">
              伴随着物联网业务的发展，在后续版本迭代过程中可能存在特定接口的参数出现变更的情况，请时刻关注文档更新，实际接入以最新版本文档为准。
            </p>
          </el-collapse-item>
        </el-collapse>
      </el-col>
    </el-row>

    <!--二、接口列表-->
    <el-row :gutter="20" v-for="items in ListArr">
      <el-col :span="mainwidthArr" :xs="24">
        <el-collapse v-model="InterfaceList" style="margin-left: 10px; margin-top: 10px;">
          <el-collapse-item name="0">
            <template slot="title">
              <div style="line-height :10px">
                <h1 style=" padding-left: 15px; font-size:28px;">二、接口列表</h1>
              </div>
            </template>
            <p data-v-md-line="2" style=" padding-left: 50px; font-size:23px;">2.1 查询类</p>
            <p data-v-md-line="2" style=" padding-left: 50px; line-height :7px; font-size:20px;">2.1.1 SIM卡信息详情</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>简要描述</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 107px; font-size:20px; line-height :7px;">查询物联卡码号信息基础信息。</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求URL</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 107px; font-size:20px; line-height :7px;color:blue;">
              http://api.5iot.cn/route/open/cardInfo</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求方式</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 107px; font-size:20px; line-height :7px;">POST</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求参数说明</li>
            </h1>
            <el-table :data="items.ArrList" border>
              <el-table-column label="参数名" align="center" prop="a"/>
              <el-table-column label="必选" align="center" prop="b"/>
              <el-table-column label="类型" align="center" prop="c"/>
              <el-table-column label="说明" align="center" prop="d">

              </el-table-column>
            </el-table>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求示例</li>
            </h1>
            <el-input type="textarea" :rows="2" style="width: 100%;border:0.5px solid #007d65;" autosize  placeholder="请输入内容"
                      v-model="textarea"/>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>返回参数说明</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 150px; font-size:20px; line-height :7px;">应答公共信息：</p>
            <el-table :data="items.AnswerList" border>
              <el-table-column label="参数名" align="center" prop="a"/>
              <el-table-column label="必选" align="center" prop="b"/>
              <el-table-column label="类型" align="center" prop="c"/>
              <el-table-column label="说明" align="center" prop="d"/>
            </el-table>
            <p data-v-md-line="2" style=" padding-left: 150px; font-size:20px; line-height :7px;">应答数据信息：</p>
            <el-table :data="items.DataList" border>
              <el-table-column label="参数名" align="center" prop="a"/>
              <el-table-column label="必选" align="center" prop="b"/>
              <el-table-column label="类型" align="center" prop="c"/>
              <el-table-column label="说明" align="center" prop="d"/>
            </el-table>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>返回报文举例</li>
            </h1>
            <el-input type="textarea" :rows="2" style="width: 100%;border:0.5px solid #007d65;" autosize  placeholder="请输入内容"
                      v-model="textarea1"/>
            <el-input type="textarea" :rows="2" style="width: 100%;border:0.5px solid #007d65;" autosize  placeholder="请输入内容"
                      v-model="textarea3"/>
            <p data-v-md-line="2" style=" padding-left: 50px; line-height :7px; font-size:20px;">2.1.2 SIM卡资费计划详情</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>简要描述</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 107px; font-size:20px; line-height :7px;">查询物联卡码号
              有效时间周期内资费计划详情。</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求URL</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 107px; font-size:20px; line-height :7px;color: blue">
              http://api.5iot.cn/route/open/cardPacketInfo</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求方式</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 107px; font-size:20px; line-height :7px;">POST</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求参数说明</li>
            </h1>
            <el-table :data="items.ExplainList" border>
              <el-table-column label="参数名" align="center" prop="a"/>
              <el-table-column label="必选" align="center" prop="b"/>
              <el-table-column label="类型" align="center" prop="c"/>
              <el-table-column label="说明" align="center" prop="d"/>
            </el-table>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求示例</li>
            </h1>
            <el-input type="textarea" :rows="2" style="width: 100%;border:0.5px solid #007d65;" autosize  placeholder="请输入内容"
                      v-model="textarea2"/>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>返回参数说明</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 150px; font-size:20px; line-height :7px;">应答公共信息：</p>
            <el-table :data="items.FromList" border>
              <el-table-column label="参数名" align="center" prop="a"/>
              <el-table-column label="必选" align="center" prop="b"/>
              <el-table-column label="类型" align="center" prop="c"/>
              <el-table-column label="说明" align="center" prop="d"/>
            </el-table>
            <p data-v-md-line="2" style=" padding-left: 150px; font-size:20px; line-height :7px;">应答数据信息：</p>
            <el-table :data="items.PublieList" border>
              <el-table-column label="参数名" align="center" prop="a"/>
              <el-table-column label="必选" align="center" prop="b"/>
              <el-table-column label="类型" align="center" prop="c"/>
              <el-table-column label="说明" align="center" prop="d"/>
            </el-table>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>返回报文举例</li>
            </h1>
            <el-input type="textarea" :rows="2" style="width: 100%;border:0.5px solid #007d65;" autosize  placeholder="请输入内容"
                      v-model="textarea4"/>
            <el-input type="textarea" :rows="2" style="width: 100%;border:0.5px solid #007d65;" autosize  placeholder="请输入内容"
                      v-model="textarea5"/>
            <p data-v-md-line="2" style=" padding-left: 50px; line-height :7px; font-size:20px;">2.1.3 SIM卡账期用量查询</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>简要描述</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 107px; font-size:20px; line-height :7px;">查询物联卡码号 账期用量。</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求URL</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 107px; font-size:20px; line-height :7px;color: blue">
              http://api.5iot.cn/route/open/simDataUsage</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求方式</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 107px; font-size:20px; line-height :7px;">POST</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求参数说明</li>
            </h1>
            <el-table :data="items.getList" border>
              <el-table-column label="参数名" align="center" prop="a"/>
              <el-table-column label="必选" align="center" prop="b"/>
              <el-table-column label="类型" align="center" prop="c"/>
              <el-table-column label="说明" align="center" prop="d"/>
            </el-table>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求示例</li>
            </h1>
            <el-input type="textarea" :rows="2" style="width: 100%;border:0.5px solid #007d65;" autosize  placeholder="请输入内容"
                      v-model="textarea6"/>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>返回参数说明</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 150px; font-size:20px; line-height :7px;">应答公共信息：</p>
            <el-table :data="items.getList1" border>
              <el-table-column label="参数名" align="center" prop="a"/>
              <el-table-column label="必选" align="center" prop="b"/>
              <el-table-column label="类型" align="center" prop="c"/>
              <el-table-column label="说明" align="center" prop="d"/>
            </el-table>
            <p data-v-md-line="2" style=" padding-left: 150px; font-size:20px; line-height :7px;">应答数据信息:</p>
            <el-table :data="items.getList2" border>
              <el-table-column label="参数名" align="center" prop="a"/>
              <el-table-column label="必选" align="center" prop="b"/>
              <el-table-column label="类型" align="center" prop="c"/>
              <el-table-column label="说明" align="center" prop="d"/>
            </el-table>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>返回报文举例</li>
            </h1>
            <el-input type="textarea" :rows="2" style="width: 100%;border:0.5px solid #007d65;" autosize  placeholder="请输入内容"
                      v-model="textarea7"/>
            <el-input type="textarea" :rows="2" style="width: 100%;border:0.5px solid #007d65;" autosize  placeholder="请输入内容"
                      v-model="textarea8"/>
            <p data-v-md-line="2" style=" padding-left: 50px; line-height :7px; font-size:20px;">2.1.4 SIM卡历史用量查询</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>简要描述</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 107px; font-size:20px; line-height :7px;">查询物联卡码号 历史用量。</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求URL</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 107px; font-size:20px; line-height :7px;color: blue">
              http://api.5iot.cn/route/open/simDataUsageMonthly</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求方式</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 107px; font-size:20px; line-height :7px;">POST</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求参数说明</li>
            </h1>
            <el-table :data="items.getList3" border>
              <el-table-column label="参数名" align="center" prop="a"/>
              <el-table-column label="必选" align="center" prop="b"/>
              <el-table-column label="类型" align="center" prop="c"/>
              <el-table-column label="说明" align="center" prop="d"/>
            </el-table>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求示例</li>
            </h1>
            <el-input type="textarea" :rows="2" style="width: 100%;border:0.5px solid #007d65;" autosize  placeholder="请输入内容"
                      v-model="textarea9"/>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>返回参数说明</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 150px; font-size:20px; line-height :7px;">应答公共信息:</p>
            <el-table :data="items.getList4" border>
              <el-table-column label="参数名" align="center" prop="a"/>
              <el-table-column label="必选" align="center" prop="b"/>
              <el-table-column label="类型" align="center" prop="c"/>
              <el-table-column label="说明" align="center" prop="d"/>
            </el-table>
            <p data-v-md-line="2" style=" padding-left: 150px; font-size:20px; line-height :7px;">应答数据信息:</p>
            <el-table :data="items.getList5" border>
              <el-table-column label="参数名" align="center" prop="a"/>
              <el-table-column label="必选" align="center" prop="b"/>
              <el-table-column label="类型" align="center" prop="c"/>
              <el-table-column label="说明" align="center" prop="d"/>
            </el-table>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>返回报文举例</li>
            </h1>
            <el-input type="textarea" :rows="2" style="width: 100%;border:0.5px solid #007d65;" autosize  placeholder="请输入内容"
                      v-model="textarea10"/>
            <el-input type="textarea" :rows="2" style="width: 100%;border:0.5px solid #007d65;" autosize  placeholder="请输入内容"
                      v-model="textarea11"/>
            <p data-v-md-line="2" style=" padding-left: 50px; line-height :7px; font-size:20px;">2.1.5 SIM卡生命周期查询</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>简要描述</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 107px; font-size:20px; line-height :7px;">查询物联卡码号 生命周期。</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求URL</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 107px; font-size:20px; line-height :7px;color: blue">
              http://api.5iot.cn/route/open/simStatus</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求方式</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 107px; font-size:20px; line-height :7px;">POST</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求参数说明</li>
            </h1>
            <el-table :data="items.getList6" border>
              <el-table-column label="参数名" align="center" prop="a"/>
              <el-table-column label="必选" align="center" prop="b"/>
              <el-table-column label="类型" align="center" prop="c"/>
              <el-table-column label="说明" align="center" prop="d"/>
            </el-table>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求示例</li>
            </h1>
            <el-input type="textarea" :rows="2" style="width: 100%;border:0.5px solid #007d65;" autosize  placeholder="请输入内容"
                      v-model="textarea12"/>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>返回参数说明</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 150px; font-size:20px; line-height :7px;">应答公共信息:</p>
            <el-table :data="items.getList7" border>
              <el-table-column label="参数名" align="center" prop="a"/>
              <el-table-column label="必选" align="center" prop="b"/>
              <el-table-column label="类型" align="center" prop="c"/>
              <el-table-column label="说明" align="center" prop="d"/>
            </el-table>
            <p data-v-md-line="2" style=" padding-left: 150px; font-size:20px; line-height :7px;">应答数据信息:</p>
            <el-table :data="items.getList8" border>
              <el-table-column label="参数名" align="center" prop="a"/>
              <el-table-column label="必选" align="center" prop="b"/>
              <el-table-column label="类型" align="center" prop="c"/>
              <el-table-column label="说明" align="center" prop="d"/>
            </el-table>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>返回报文举例</li>
            </h1>
            <el-input type="textarea" :rows="2" style="width: 100%;border:0.5px solid #007d65;" autosize  placeholder="请输入内容"
                      v-model="textarea13"/>
            <el-input type="textarea" :rows="2" style="width: 100%;border:0.5px solid #007d65;" autosize  placeholder="请输入内容"
                      v-model="textarea14"/>
            <p data-v-md-line="2" style=" padding-left: 50px; line-height :7px; font-size:20px;">2.1.5 SIM卡在网状态查询</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>简要描述</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 107px; font-size:20px; line-height :7px;">查询物联卡码号 在网状态信息等。</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求URL</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 107px; font-size:20px; line-height :7px;color: blue">
              http://api.5iot.cn/route/open/simSession</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求方式</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 107px; font-size:20px; line-height :7px;">POST</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求参数说明</li>
            </h1>
            <el-table :data="items.getList9" border>
              <el-table-column label="参数名" align="center" prop="a"/>
              <el-table-column label="必选" align="center" prop="b"/>
              <el-table-column label="类型" align="center" prop="c"/>
              <el-table-column label="说明" align="center" prop="d"/>
            </el-table>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求示例</li>
            </h1>
            <el-input type="textarea" :rows="2" style="width: 100%;border:0.5px solid #007d65;" autosize  placeholder="请输入内容"
                      v-model="textarea15"/>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>返回参数说明</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 150px; font-size:20px; line-height :7px;">应答公共信息:</p>
            <el-table :data="items.getList10" border>
              <el-table-column label="参数名" align="center" prop="a"/>
              <el-table-column label="必选" align="center" prop="b"/>
              <el-table-column label="类型" align="center" prop="c"/>
              <el-table-column label="说明" align="center" prop="d"/>
            </el-table>
            <p data-v-md-line="2" style=" padding-left: 150px; font-size:20px; line-height :7px;">应答数据信息:</p>
            <el-table :data="items.getList11" border>
              <el-table-column label="参数名" align="center" prop="a"/>
              <el-table-column label="必选" align="center" prop="b"/>
              <el-table-column label="类型" align="center" prop="c"/>
              <el-table-column label="说明" align="center" prop="d"/>
            </el-table>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>返回报文举例</li>
            </h1>
            <el-input type="textarea" :rows="2" style="width: 100%;border:0.5px solid #007d65;" autosize  placeholder="请输入内容"
                      v-model="textarea16"/>
            <el-input type="textarea" :rows="2" style="width: 100%;border:0.5px solid #007d65;" autosize  placeholder="请输入内容"
                      v-model="textarea17"/>
            <p data-v-md-line="2" style=" padding-left: 50px; line-height :7px; font-size:20px;">2.1.6 资费计划 可用查询</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>简要描述</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 107px; font-size:20px; line-height :7px;">查询 集团下可用资费计划。</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求URL</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 107px; font-size:20px; line-height :7px;color: blue">
              http://api.5iot.cn/route/open/tariffplan</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求方式</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 107px; font-size:20px; line-height :7px;">POST</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求参数说明</li>
            </h1>
            <el-table :data="items.getList12" border>
              <el-table-column label="参数名" align="center" prop="a"/>
              <el-table-column label="必选" align="center" prop="b"/>
              <el-table-column label="类型" align="center" prop="c"/>
              <el-table-column label="说明" align="center" prop="d"/>
            </el-table>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求示例</li>
            </h1>
            <el-input type="textarea" :rows="2" style="width: 100%;border:0.5px solid #007d65;" autosize  placeholder="请输入内容"
                      v-model="textarea18"/>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>返回参数说明</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 150px; font-size:20px; line-height :7px;">应答公共信息:</p>
            <el-table :data="items.getList13" border>
              <el-table-column label="参数名" align="center" prop="a"/>
              <el-table-column label="必选" align="center" prop="b"/>
              <el-table-column label="类型" align="center" prop="c"/>
              <el-table-column label="说明" align="center" prop="d"/>
            </el-table>
            <p data-v-md-line="2" style=" padding-left: 150px; font-size:20px; line-height :7px;">应答数据信息:</p>
            <el-table :data="items.getList14" border>
              <el-table-column label="参数名" align="center" prop="a"/>
              <el-table-column label="必选" align="center" prop="b"/>
              <el-table-column label="类型" align="center" prop="c"/>
              <el-table-column label="说明" align="center" prop="d"/>
            </el-table>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>返回报文举例</li>
            </h1>
            <el-input type="textarea" :rows="2" style="width: 100%;border:0.5px solid #007d65;" autosize  placeholder="请输入内容"
                      v-model="textarea19"/>
            <el-input type="textarea" :rows="2" style="width: 100%;border:0.5px solid #007d65;" autosize  placeholder="请输入内容"
                      v-model="textarea20"/>
            <p data-v-md-line="2" style=" padding-left: 50px; line-height :7px; font-size:20px;">2.1.7 集团 预存余额查询</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>简要描述</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 107px; font-size:20px; line-height :7px;">查询集团预存可用余额。</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求URL</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 107px; font-size:20px; line-height :7px;color: blue">
              http://api.5iot.cn/route/open/balance</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求方式</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 107px; font-size:20px; line-height :7px;">POST</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求参数说明</li>
            </h1>
            <el-table :data="items.getList15" border>
              <el-table-column label="参数名" align="center" prop="a"/>
              <el-table-column label="必选" align="center" prop="b"/>
              <el-table-column label="类型" align="center" prop="c"/>
              <el-table-column label="说明" align="center" prop="d"/>
            </el-table>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求示例</li>
            </h1>
            <el-input type="textarea" :rows="2" style="width: 100%;border:0.5px solid #007d65;" autosize  placeholder="请输入内容"
                      v-model="textarea21"/>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>返回参数说明</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 150px; font-size:20px; line-height :7px;">应答公共信息:</p>
            <el-table :data="items.getList16" border>
              <el-table-column label="参数名" align="center" prop="a"/>
              <el-table-column label="必选" align="center" prop="b"/>
              <el-table-column label="类型" align="center" prop="c"/>
              <el-table-column label="说明" align="center" prop="d"/>
            </el-table>
            <p data-v-md-line="2" style=" padding-left: 150px; font-size:20px; line-height :7px;">应答数据信息:</p>
            <el-table :data="items.getList17" border>
              <el-table-column label="参数名" align="center" prop="a"/>
              <el-table-column label="必选" align="center" prop="b"/>
              <el-table-column label="类型" align="center" prop="c"/>
              <el-table-column label="说明" align="center" prop="d"/>
            </el-table>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>返回报文举例</li>
            </h1>
            <el-input type="textarea" :rows="2" style="width: 100%;border:0.5px solid #007d65;" autosize  placeholder="请输入内容"
                      v-model="textarea22"/>
            <el-input type="textarea" :rows="2" style="width: 100%;border:0.5px solid #007d65;" autosize  placeholder="请输入内容"
                      v-model="textarea23"/>

            <p data-v-md-line="2" style=" padding-left: 50px; font-size:23px;">2.2业务类</p>
            <p data-v-md-line="2" style=" padding-left: 50px; line-height :7px; font-size:20px;">2.2.1 SIM卡状态变更</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>简要描述</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 107px; font-size:20px; line-height :7px;">变更生命周期 停复机。</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求URL</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 107px; font-size:20px; line-height :7px;color: blue">
              http://api.5iot.cn/route/open/changeSimStatus</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求方式</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 107px; font-size:20px; line-height :7px;">POST</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求参数说明</li>
            </h1>
            <el-table :data="items.getList18" border>
              <el-table-column label="参数名" align="center" prop="a"/>
              <el-table-column label="必选" align="center" prop="b"/>
              <el-table-column label="类型" align="center" prop="c"/>
              <el-table-column label="说明" align="center" prop="d"/>
            </el-table>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求示例</li>
            </h1>
            <el-input type="textarea" :rows="2" style="width: 100%;border:0.5px solid #007d65;" autosize  placeholder="请输入内容"
                      v-model="textarea24"/>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>返回参数说明</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 150px; font-size:20px; line-height :7px;">应答公共信息:</p>
            <el-table :data="items.getList19" border>
              <el-table-column label="参数名" align="center" prop="a"/>
              <el-table-column label="必选" align="center" prop="b"/>
              <el-table-column label="类型" align="center" prop="c"/>
              <el-table-column label="说明" align="center" prop="d"/>
            </el-table>
            <p data-v-md-line="2" style=" padding-left: 150px; font-size:20px; line-height :7px;">应答数据信息:</p>
            <el-table :data="items.getList20" border>
              <el-table-column label="参数名" align="center" prop="a"/>
              <el-table-column label="必选" align="center" prop="b"/>
              <el-table-column label="类型" align="center" prop="c"/>
              <el-table-column label="说明" align="center" prop="d"/>
            </el-table>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>返回报文举例</li>
            </h1>
            <el-input type="textarea" :rows="2" style="width: 100%;border:0.5px solid #007d65;" autosize  placeholder="请输入内容"
                      v-model="textarea25"/>
            <el-input type="textarea" :rows="2" style="width: 100%;border:0.5px solid #007d65;" autosize  placeholder="请输入内容"
                      v-model="textarea26"/>
            <p data-v-md-line="2" style=" padding-left: 50px; line-height :7px; font-size:20px;">2.2.2 SIM卡数据功能开关</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>简要描述</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 107px; font-size:20px; line-height :7px;">单卡断开网 操作。</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求URL</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 107px; font-size:20px; line-height :7px;color: blue">
              http://api.5iot.cn/route/open/simApnFunction</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求方式</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 107px; font-size:20px; line-height :7px;">POST</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求参数说明</li>
            </h1>
            <el-table :data="items.getList21" border>
              <el-table-column label="参数名" align="center" prop="a"/>
              <el-table-column label="必选" align="center" prop="b"/>
              <el-table-column label="类型" align="center" prop="c"/>
              <el-table-column label="说明" align="center" prop="d"/>
            </el-table>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求示例</li>
            </h1>
            <el-input type="textarea" :rows="2" style="width: 100%;border:0.5px solid #007d65;" autosize  placeholder="请输入内容"
                      v-model="textarea27"/>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>返回参数说明</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 150px; font-size:20px; line-height :7px;">应答公共信息:</p>
            <el-table :data="items.getList22" border>
              <el-table-column label="参数名" align="center" prop="a"/>
              <el-table-column label="必选" align="center" prop="b"/>
              <el-table-column label="类型" align="center" prop="c"/>
              <el-table-column label="说明" align="center" prop="d"/>
            </el-table>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>返回报文举例</li>
            </h1>
            <el-input type="textarea" :rows="2" style="width: 100%;border:0.5px solid #007d65;" autosize  placeholder="请输入内容"
                      v-model="textarea28"/>
            <el-input type="textarea" :rows="2" style="width: 100%;border:0.5px solid #007d65;" autosize  placeholder="请输入内容"
                      v-model="textarea29"/>
            <p data-v-md-line="2" style=" padding-left: 50px; line-height :7px; font-size:20px;">2.2.3 资费计划订购</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>简要描述</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 107px; font-size:20px; line-height :7px;">单卡订购 资费计划。</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求URL</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 107px; font-size:20px; line-height :7px;color: blue">
              http://api.5iot.cn/route/open/subscribe</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求方式</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 107px; font-size:20px; line-height :7px;">POST</p>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>返回参数说明</li>
            </h1>
            <el-table :data="items.getList23" border>
              <el-table-column label="参数名" align="center" prop="a"/>
              <el-table-column label="必选" align="center" prop="b"/>
              <el-table-column label="类型" align="center" prop="c"/>
              <el-table-column label="说明" align="center" prop="d"/>
            </el-table>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>请求示例</li>
            </h1>
            <el-input type="textarea" :rows="2" style="width: 100%;border:0.5px solid #007d65;" autosize  placeholder="请输入内容"
                      v-model="textarea30"/>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>返回参数说明</li>
            </h1>
            <p data-v-md-line="2" style=" padding-left: 150px; font-size:20px; line-height :7px;">应答公共信息:</p>
            <el-table :data="items.getList24" border>
              <el-table-column label="参数名" align="center" prop="a"/>
              <el-table-column label="必选" align="center" prop="b"/>
              <el-table-column label="类型" align="center" prop="c"/>
              <el-table-column label="说明" align="center" prop="d"/>
            </el-table>
            <h1 data-v-md-line="2" style=" padding-left: 75px; font-size:25px;">
              <li>返回报文举例</li>
            </h1>
            <el-input type="textarea" :rows="2" style="width: 100%;border:0.5px solid #007d65;" autosize  placeholder="请输入内容"
                      v-model="textarea31"/>
            <el-input type="textarea" :rows="2" style="width: 100%;border:0.5px solid #007d65;" autosize  placeholder="请输入内容"
                      v-model="textarea32"/>
          </el-collapse-item>
        </el-collapse>
      </el-col>
    </el-row>

    <!--三、辅助说明-->
    <el-row :gutter="20" v-for="items in gitListArr">
      <el-col :span="mainwidthArrL" :xs="24">
        <el-collapse v-model="InterfaceList2" style="margin-left: 10px; margin-top: 10px;">
          <el-collapse-item name="0">
            <template slot="title">
              <div style="line-height :10px">
                <h1 style=" padding-left: 15px; font-size:28px;">三、辅助说明</h1>
              </div>
            </template>
            <h1 data-v-md-line="2" style="  font-size:25px;">
              3.1 卡状态说明
            </h1>
            <table  border="2px" width="1000px" height="30px" cellspacing="0" cellpadding="2">
              <tr>
                <td>字段说明</td>
                <td>对应状态</td>
              </tr>
              <tr>
                <td>库存</td>
                <td style="padding-left: 50px; font-size:10px;">1</td>
              </tr>
              <tr>
                <td>可测试</td>
                <td style="padding-left: 50px; font-size:10px;">2</td>
              </tr>
              <tr>
                <td>待激活</td>
                <td style="padding-left: 50px; font-size:10px;">3</td>
              </tr>
              <tr>
                <td>已激活</td>
                <td style="padding-left: 50px; font-size:10px;">4</td>
              </tr>
              <tr>
                <td>已停机</td>
                <td style="padding-left: 50px; font-size:10px;">5</td>
              </tr>
              <tr>
                <td>预销户</td>
                <td style="padding-left: 50px; font-size:10px;">6</td>
              </tr>
              <tr>
                <td>已销户</td>
                <td style="padding-left: 50px; font-size:10px;">7</td>
              </tr>
              <tr>
                <td>未知</td>
                <td style="padding-left: 50px; font-size:10px;">8</td>
              </tr>
            </table>
            <h1 data-v-md-line="2" style="  font-size:25px;">
              3.2 卡类型说明
            </h1>
            <table  border="2px" width="1000px" height="30px" cellspacing="0" cellpadding="2">
              <tr>
                <td>字段说明</td>
                <td>对应状态</td>
              </tr>
              <tr>
                <td>中国移动</td>
                <td style="padding-left: 50px; font-size:10px;">1</td>
              </tr>
              <tr>
                <td>中国联通</td>
                <td style="padding-left: 50px; font-size:10px;">2</td>
              </tr>
              <tr>
                <td>中国电信</td>
                <td style="padding-left: 50px; font-size:10px;">3</td>
              </tr>
              <tr>
                <td>其他</td>
                <td style="padding-left: 50px; font-size:10px;">4</td>
              </tr>
            </table>
            <h1 data-v-md-line="2" style="  font-size:25px;">
              3.3 生效类型说明
            </h1>
            <table  border="2px" width="1000px" height="30px" cellspacing="0" cellpadding="2">
              <tr>
                <td>字段说明</td>
                <td>对应状态</td>
              </tr>
              <tr>
                <td>已失效</td>
                <td style="padding-left: 50px; font-size:10px;">0</td>
              </tr>
              <tr>
                <td>生效中</td>
                <td style="padding-left: 50px; font-size:10px;">1</td>
              </tr>
            </table>
            <h1 data-v-md-line="2" style="  font-size:25px;">
              3.4 生效类别说明
            </h1>
            <table  border="2px" width="1000px" height="30px" cellspacing="0" cellpadding="2">
              <tr>
                <td>字段说明</td>
                <td >对应状态</td>
              </tr>
              <tr>
                <td>当月生效</td>
                <td style="padding-left: 50px; font-size:10px;">1</td>
              </tr>
              <tr>
                <td>次月生效</td>
                <td style="padding-left: 50px; font-size:10px;">2</td>
              </tr>
              <tr>
                <td>周期延顺</td>
                <td style="padding-left: 50px; font-size:10px;">3</td>
              </tr>
            </table>
            <h1 data-v-md-line="2" style="  font-size:25px;">
              3.5 返回错误编码说明
            </h1>
            <table  id="tab"  border="2px" width="1000px" height="30px" cellspacing="0" cellpadding="2">
              <tr>
                <td >字段说明</td>
                <td style="padding-left: 210px;">对应状态</td>
              </tr>
              <tr>
                <td>缺少路径或appId</td>
                <td style="padding-left: 50px;">-1</td>
              </tr>
              <tr>
                <td>账户查询失败</td>
                <td style="padding-left: 50px; ">1001</td>
              </tr>
              <tr>
                <td>访问频率超限，请稍后访问</td>
                <td style="padding-left: 50px; ">1002</td>
              </tr>
              <tr>
                <td>无访问权限</td>
                <td style="padding-left: 50px;">1003</td>
              </tr>
              <tr>
                <td>未找到集团对应号码信息请核对后重试！</td>
                <td style="padding-left: 50px; ">1004</td>
              </tr>
              <tr>
                <td>操作失败请稍后重试！（具体看返回信息）</td>
                <td style="padding-left: 50px; ">500</td>
              </tr>
              <tr>
                <td >参数错误</td>
                <td style="padding-left: 50px; ">400</td>
              </tr>
            </table>
          </el-collapse-item>
        </el-collapse>
      </el-col>
    </el-row>

    <!--四、请求案例-->
    <el-row :gutter="20" v-for="items in gitListArr4">
      <el-col :span="mainwidthArr4" :xs="24">
        <el-collapse v-model="InterfaceList4" style="margin-left: 10px; margin-top: 10px;">
          <el-collapse-item name="0">
            <template slot="title">
              <div style="line-height :10px">
                <h1 style=" padding-left: 15px; font-size:28px;">四、请求案例</h1>
              </div>
            </template>

            <el-input type="textarea" :rows="2" style="width: 100%;border:0.5px solid #007d65;" autosize  placeholder="请输入内容"
                      v-model="Requestcase"/>


          </el-collapse-item>
        </el-collapse>
      </el-col>
    </el-row>

  </div>
</template>

<script>
export default {
  name: "apifile",
  data() {
    return {
      AccountActiveNames: ['1'],//面板默认 展开 【接口说明】
      InterfaceList: ['1'],//面板默认 展开 【接口列表】
      InterfaceList2: ['1'],//面板默认 展开 【辅助说明】
      InterfaceList4: ['1'],//面板默认 展开 【请求案例】
      textarea: '{\n' +
        '\t"Param": {\n' +
        '\t\t"type": "iccid",\n' +
        '\t\t"cardno": "89860423123123132135"\n' +
        '\t},\n' +
        '\t"verify": {\n' +
        '\t\t"timeStamp": "1625120582169",\n' +
        '\t\t"password": "uhbhkh123123",\n' +
        '\t\t"appId": "test",\n' +
        '\t\t"sign": "C0EBF5F9FCBFFE72CAA5AAF753EACCA5"\n' +
        '\t}\n' +
        '}',
      textarea1: '成功：\n' +
        '{\n' +
        '\t"status": "0"，\n' +
        '     "message": "success",\n' +
        '     "result": [\n' +
        '{\n' +
        '      \n' +
        '}\n' +
        '           ]\n' +
        '}',
      textarea3: '失败：\n' +
        '{\n' +
        '    "message": "账户查询失败",\n' +
        '    "result": null,\n' +
        '    "status": "1001"\n' +
        '}',
      textarea2: '{\n' +
        '\t"Param": {\n' +
        '\t\t"type": "iccid",\n' +
        '\t\t"cardno": "89860423123123132135"\n' +
        '\t},\n' +
        '\t"verify": {\n' +
        '\t\t"timeStamp": "1625120582169",\n' +
        '\t\t"password": "uhbhkh123123",\n' +
        '\t\t"appId": "test",\n' +
        '\t\t"sign": "C0EBF5F9FCBFFE72CAA5AAF753EACCA5"\n' +
        '\t}\n' +
        '}',
      textarea4: '成功：\n' +
        '{\n' +
        '\t"status": "0"，\n' +
        '     "message": "success",\n' +
        '     "result": [\n' +
        '{\n' +
        '      \n' +
        '}\n' +
        '           ]\n' +
        '}',
      textarea5: '失败：\n' +
        '{\n' +
        '    "message": "账户查询失败",\n' +
        '    "result": null,\n' +
        '    "status": "1001"\n' +
        '}',
      textarea6: '{\n' +
        '\t"Param": {\n' +
        '\t\t"iccid": "89860423123123132135"\n' +
        '\t},\n' +
        '\t"verify": {\n' +
        '\t\t"timeStamp": "1625120582169",\n' +
        '\t\t"password": "uhbhkh123123",\n' +
        '\t\t"appId": "test",\n' +
        '\t\t"sign": "C0EBF5F9FCBFFE72CAA5AAF753EACCA5"\n' +
        '\t}\n' +
        '}',
      textarea7: '成功：\n' +
        '{\n' +
        '\t"status": "0"，\n' +
        '     "message": "success",\n' +
        '     "result": {\n' +
        '        "useUnit": "MB",\n' +
        '        "useAmount": 2503\n' +
        '}\n' +
        '}',
      textarea8: '失败：\n' +
        '{\n' +
        '    "message": "无访问权限",\n' +
        '    "result": null,\n' +
        '    "status": "1003"\n' +
        '}',
      textarea9: '{\n' +
        '\t"Param": {\n' +
        '\t\t"iccid": "89860423123123132135",\n' +
        '\t\t"dateTime": "202101"\n' +
        '\t},\n' +
        '\t"verify": {\n' +
        '\t\t"timeStamp": "1625120582169",\n' +
        '\t\t"password": "uhbhkh123123",\n' +
        '\t\t"appId": "test",\n' +
        '\t\t"sign": "C0EBF5F9FCBFFE72CAA5AAF753EACCA5"\n' +
        '\t}\n' +
        '}',
      textarea10: '成功：\n' +
        '{\n' +
        '\t"status": "0"，\n' +
        '     "message": "success",\n' +
        '     "result": {\n' +
        '        "useUnit": "MB",\n' +
        '        "useAmount": 29.63\n' +
        '}\n' +
        '}',
      textarea11: '失败：\n' +
        '{\n' +
        '    "message": "无访问权限",\n' +
        '    "result": null,\n' +
        '    "status": "1003"\n' +
        '}',
      textarea12: '{\n' +
        '\t"Param": {\n' +
        '\t\t"iccid": "89860423123123132135"\n' +
        '\t},\n' +
        '\t"verify": {\n' +
        '\t\t"timeStamp": "1625120582169",\n' +
        '\t\t"password": "uhbhkh123123",\n' +
        '\t\t"appId": "test",\n' +
        '\t\t"sign": "C0EBF5F9FCBFFE72CAA5AAF753EACCA5"\n' +
        '\t}\n' +
        '}   ',
      textarea13: '成功：\n' +
        '{\n' +
        '\t"status": "0"，\n' +
        '     "message": "success",\n' +
        '     "result": {\n' +
        '        "statusMessage": "正常",\n' +
        '        "statusCode": 1\n' +
        '}\n' +
        '}',
      textarea14: '失败：\n' +
        '{\n' +
        '    "message": "无访问权限",\n' +
        '    "result": null,\n' +
        '    "status": "1003"\n' +
        '}',
      textarea15: '{\n' +
        '\t"Param": {\n' +
        '\t\t"iccid": "89860423123123132135"\n' +
        '\t},\n' +
        '\t"verify": {\n' +
        '\t\t"timeStamp": "1625120582169",\n' +
        '\t\t"password": "uhbhkh123123",\n' +
        '\t\t"appId": "test",\n' +
        '\t\t"sign": "C0EBF5F9FCBFFE72CAA5AAF753EACCA5"\n' +
        '\t}\n' +
        '}',
      textarea16: '成功：\n' +
        '{\n' +
        '\t"status": "0"，\n' +
        '     "message": "success",\n' +
        '     "result": {\n' +
        '        "typeName": "移动EC",\n' +
        '        "imei": null,\n' +
        '        "Data": {\n' +
        '            "apnId": "CMIOT",\n' +
        '            "rat": "6",\n' +
        '            "ipv6": "",\n' +
        '            "ip": "192.168.0.1",\n' +
        '            "ipv6Prefix": "",\n' +
        '            "status": "01",\n' +
        '            "createDate": "2021-01-02 13:07:39"\n' +
        '        },\n' +
        '        "type": 1,\n' +
        '        "statusMessage": null,\n' +
        '        "statusCode": -1}\n' +
        '}',
      textarea17: '失败：\n' +
        '{\n' +
        '    "message": "无访问权限",\n' +
        '    "result": null,\n' +
        '    "status": "1003"\n' +
        '}',
      textarea18: '{\n' +
        '\t"Param": {\n' +
        '\t\t"companyId": "3365"\n' +
        '\t},\n' +
        '\t"verify": {\n' +
        '\t\t"timeStamp": "1625120582169",\n' +
        '\t\t"password": "uhbhkh123123",\n' +
        '\t\t"appId": "test",\n' +
        '\t\t"sign": "C0EBF5F9FCBFFE72CAA5AAF753EACCA5"\n' +
        '\t}\n' +
        '}',
      textarea19: '成功：\n' +
        '{\n' +
        '\t"status": "0"，\n' +
        '     "message": "success",\n' +
        '     "result": [\n' +
        ' \t\t{\n' +
        '\t\t  "blocPlantGroupId": "YZ202107020435363732",\n' +
        '"blocPlantId": "YZ3742202107020448446521",\n' +
        '"blocPlanName": "NB1年50MB",\n' +
        '"blocPlanPrice": "10.00",\n' +
        '"blocPlanCapacity": "50.00",\n' +
        '"blocPlanCost": "8.00",\n' +
        '"blocPlanTimeValue": "1",\n' +
        '"blocPlanTimeUnit": "年",\n' +
        '}\n' +
        ']\t       \n' +
        '}',
      textarea20: '失败：\n' +
        '{\n' +
        '    "message": "无访问权限",\n' +
        '    "result": null,\n' +
        '    "status": "1003"\n' +
        '}',
      textarea21: '{\n' +
        '\t"Param": {\n' +
        '\t\t"companyId": "3365"\n' +
        '\t},\n' +
        '\t"verify": {\n' +
        '\t\t"timeStamp": "1625120582169",\n' +
        '\t\t"password": "uhbhkh123123",\n' +
        '\t\t"appId": "test",\n' +
        '\t\t"sign": "C0EBF5F9FCBFFE72CAA5AAF753EACCA5"\n' +
        '\t}\n' +
        '}',
      textarea22: '成功：\n' +
        '{\n' +
        '\t"status": "0"，\n' +
        '     "message": "success",\n' +
        '     "result": [\n' +
        ' \t\t{\n' +
        '\t\t  "blocPlantGroupId": "YZ202107020435363732",\n' +
        '"blocPlantId": "YZ3742202107020448446521",\n' +
        '"blocPlanName": "NB1年50MB",\n' +
        '"blocPlanPrice": "10",\n' +
        '"blocPlanCapacity": "50",\n' +
        '"blocPlanCost": "8",\n' +
        '"blocPlanTimeValue": "1",\n' +
        '"blocPlanTimeUnit": "年",\n' +
        '}\n' +
        ']\t       \n' +
        '}',
      textarea23: '失败：\n' +
        '{\n' +
        '    "message": "无访问权限",\n' +
        '    "result": null,\n' +
        '    "status": "1003"\n' +
        '}',
      textarea24: '{\n' +
        '\t"Param": {\n' +
        '\t\t"iccid": "89860423123123132135",\n' +
        '\t\t"Status": "0"\n' +
        '\t},\n' +
        '\t"verify": {\n' +
        '\t\t"timeStamp": "1625120582169",\n' +
        '\t\t"password": "uhbhkh123123",\n' +
        '\t\t"appId": "test",\n' +
        '\t\t"sign": "C0EBF5F9FCBFFE72CAA5AAF753EACCA5"\n' +
        '\t}\n' +
        '}',
      textarea25: '成功：\n' +
        '{\n' +
        '\t"status": "0"，\n' +
        '     "message": "success",\n' +
        '     "result": {}\n' +
        '}',
      textarea26: '失败：\n' +
        '{\n' +
        '    "message": "无访问权限",\n' +
        '    "result": null,\n' +
        '    "status": "1003"\n' +
        '}',
      textarea27: '{\n' +
        '\t"Param": {\n' +
        '\t\t"iccid": "89860423123123132135",\n' +
        '\t\t"StatusBreak": "0"\n' +
        '\t},\n' +
        '\t"verify": {\n' +
        '\t\t"timeStamp": "1625120582169",\n' +
        '\t\t"password": "uhbhkh123123",\n' +
        '\t\t"appId": "test",\n' +
        '\t\t"sign": "C0EBF5F9FCBFFE72CAA5AAF753EACCA5"\n' +
        '\t}\n' +
        '}',
      textarea28: '成功：\n' +
        '{\n' +
        '\t"status": "0"，\n' +
        '     "message": "success",\n' +
        '     "result": {}\n' +
        '}',
      textarea29: '失败：\n' +
        '{\n' +
        '    "message": "无访问权限",\n' +
        '    "result": null,\n' +
        '    "status": "1003"\n' +
        '}',
      textarea30: '{\n' +
        '\t"Param": {\n' +
        '\t\t"iccid": "89860423123123132135",\n' +
        '\t\t"companyId": "3365",\n' +
        '"effectiveType": "1",\n' +
        '"blocPlantId": "YZ3742202107020448446521"\n' +
        '\t},\n' +
        '\t"verify": {\n' +
        '\t\t"timeStamp": "1625120582169",\n' +
        '\t\t"password": "uhbhkh123123",\n' +
        '\t\t"appId": "test",\n' +
        '\t\t"sign": "C0EBF5F9FCBFFE72CAA5AAF753EACCA5"\n' +
        '\t}\n' +
        '}',
      textarea31: '成功：\n' +
        '{\n' +
        '\t"status": "0"，\n' +
        '     "message": "success",\n' +
        '     "result": {}\n' +
        '}',
      textarea32: '失败：\n' +
        '{\n' +
        '    "message": "无访问权限",\n' +
        '    "result": null,\n' +
        '    "status": "1003"\n' +
        '}',


      //参数
      queryParams: {},
      mainwidth: 24,//宽度
      mainwidthArr: 24,
      mainwidthArrL: 24,
      mainwidthArr4: 24,
      // 显示搜索条件
      showSearch: true,
      itemsArr: [//接口说明
        {
          ServerUrl: 'http://5iot.com/api',
          domainName: 'api.5iot.cn',
          InterfaceName: 'API 接口名称',
          mailbox: 'feizhenlong@139.com',
          editionList: [//版本说明
            {a: 'V1.0.0', b: '2021-1-11', c: '', d: '新增接口：'},
            {a: 'V1.0.1', b: '2021-7-15', c: '', d: '新增接口： tariffplan| balance| subscribe：'},
            {
              a: 'V1.0.2', b: '2022-1-20', c: '', d: 'cardPacketInfo接口新增参数 all 可查询所有 【生效中】、【未生效】资费计划详情\n' +
                '卡信息接口新增 备注、分组、发货日期、激活日期、到期时间\n' +
                '卡状态说明更新\n' +
                'V1.0.2因变更【卡状态说明】所以独立出新的调用接口地址 临时地址（api1.0.1.5iot.cn）[后续还是以api.5iot.cn]\n' +
                'V1.0.1版本接口保留至 2022-03-15 23:59:59 后自动更新为V1.0.2 版本：'
            },
          ],
          cardList: [//接口命名规则
            {a: 'appId', b: 'String', c: '是', d: 'Verify Map 内', e: 'appId用于唯一标识请求'},
            {a: 'password', b: 'String', c: '是', d: 'Verify Map 内', e: '密码'},
            {a: 'timeStamp', b: 'String', c: '是', d: 'Verify Map 内', e: '时间戳 “”+System.currentTimeMillis()'},
            {a: 'sign', b: 'String', c: '是', d: 'Verify Map 内', e: '签名，根据算法获取 详情参考 案例'},
          ],
        },
      ],
      ListArr: [//  二、接口列表
        {

          ArrList: [// 请求参数说明
            {
              a: 'type', b: '是', c: 'String', d: 'msisdn：所查询的物联卡号码，最长13位数字，举例：14765004176。\n' +
                'iccid:集成电路卡识别码，IC卡的唯一识别号码，共有20位字符组成，举例：89860423123123132135。\n' +
                'imsi:国际移动用户识别码，其总长度不超过15位，使用0~9的数字，举例：460079650004176。 \n' +
                'vid：物联网卡虚拟编号'
            },
            {a: 'cardno', b: '是', c: 'String', d: ''},

          ],
          AnswerList: [//  应答公共信息
            {a: 'status', b: '是', c: 'String', d: '错误码。0-成功   非0-失败'},
            {a: 'message', b: '是', c: 'String', d: '错误信息。错误码对应的错误描述，参考错误码列表'},
            {
              a: 'result',
              b: '是',
              c: 'Object',
              d: '返回结果集（status为“0”时，result包含正确的结果数据；status为“非0”时， result可能为空，也可能包含其他提示数据）'
            },
          ],
          DataList: [//   应答数据信息
            {a: 'vid', b: '是', c: 'String', d: '物联网卡虚拟编号'},
            {a: 'msisdn', b: '是', c: 'String', d: '所查询的物联卡号码，最长13位数字，举例：14765004176。'},
            {a: 'iccid', b: '是', c: 'String', d: '集成电路卡识别码，IC卡的唯一识别号码，共有20位字符组成，举例：89860423123123132135。'},
            {a: 'imsi', b: '是', c: 'String', d: '国际移动用户识别码，其总长度不超过15位，使用0~9的数字，举例：460079650004176。 '},
            {a: 'imei', b: '是', c: 'String', d: '国际移动设备识别码，共有15~17位数字，前8位（TAC）是型号核准号码（早期为6位），是区分品牌和型号的编码。'},
            {a: 'deptName', b: '是', c: 'String', d: '所属集团名称'},
            {a: 'statusId', b: '是', c: 'int', d: '卡状态  详细请参考 3.1 卡状态说明'},
            {a: 'packageId', b: '是', c: 'String', d: '套餐编号'},
            {a: 'packageName', b: '是', c: 'String', d: '套餐名称'},
            {a: 'used', b: '是', c: 'double', d: '周期用量'},
            {a: 'remaining', b: '是', c: 'double', d: '周期剩余'},
            {a: 'type', b: '是', c: 'String', d: '卡类型  详细请参考 3.2 卡类型说明'},
            {a: 'packetArr', b: '是', c: 'list', d: 'List<Map<String,Object>>资费计划仅展示最后生效1条'},
            {a: 'validName', b: '是', c: 'String', d: '计费单位 （月/年） packetArr{}里数据'},
            {a: 'validTime', b: '是', c: 'int', d: '计费时间 int packetArr{}里数据'},
            {a: 'startTime', b: '否', c: 'datetime', d: '计费开始时间 packetArr{}里数据'},
            {a: 'endTime', b: '否', c: 'datetime', d: '计费结束时间 packetArr{}里数据'},
            {a: 'packetName', b: '是', c: 'String', d: '资费计划名称 packetArr{}里数据'},
            {a: 'status', b: '是', c: 'int', d: '生效状态详细请参3.3生效类型说明packetArr{}里数据'},
            {a: 'remark', b: '否', c: 'String', d: '备注'},
            {a: 'grouping', b: '否', c: 'String', d: '分组'},
            {a: 'deliverDate', b: '否', c: 'String', d: '发货日期 yyyy-MM-dd'},
            {a: 'activateDate', b: '否', c: 'String', d: '激活日期 yyyy-MM-dd'},
            {a: 'expireTime', b: '否', c: 'datetime', d: '到期时间 yyyy-MM-dd HH:mm:ss'},
          ],
          ExplainList: [ // 请求参数说明
            {
              a: 'type', b: '是', c: 'String', d: 'msisdn：所查询的物联卡号码，最长13位数字，举例：14765004176。\n' +
                'iccid:集成电路卡识别码，IC卡的唯一识别号码，共有20位字符组成，举例：89860423123123132135。\n' +
                'imsi:国际移动用户识别码，其总长度不超过15位，使用0~9的数字，举例：460079650004176。 \n' +
                'vid：物联网卡虚拟编号'
            },
            {a: 'cardno', b: '是', c: 'String', d: ''},
            {a: 'all', b: '否', c: 'String', d: '默认不填写时为 ‘0’填写为‘1’时查询 【生效中】、【未生效】所有 资费计划详情'},
          ],
          FromList: [
            {a: 'status', b: '是', c: 'String', d: '错误码。0-成功   非0-失败'},
            {a: 'message', b: '是', c: 'String', d: '错误信息。错误码对应的错误描述，参考错误码列表'},
            {
              a: 'result',
              b: '是',
              c: 'Object',
              d: '返回结果集（status为“0”时，result包含正确的结果数据；status为“非0”时， result可能为空，也可能包含其他提示数据）'
            },
          ],
          PublieList: [
            {a: 'packageName', b: '是', c: 'String', d: '资费组名称'},
            {a: 'createTime', b: '否', c: 'datetime', d: '创建时间'},
            {a: 'startTime', b: '否', c: 'datetime', d: '计费开始时间 '},
            {a: 'endTime', b: '否', c: 'datetime', d: '计费结束时间 '},
            {a: 'packetName', b: '是', c: 'String', d: '资费计划名称 '},
            {a: 'status', b: '是', c: 'int', d: '生效状态 详细请参3.3生效类型说明 '},
            {a: 'packageId', b: '是', c: 'String', d: '资费组编号'},
            {a: 'packetId', b: '是', c: 'String', d: '资费计划编号'},
            {a: 'ordType', b: '是', c: 'String', d: '生效类别 详细请参3.4生效类别说明 '},
          ],
          getList: [
            {a: 'iccid', b: '是', c: 'String', d: 'iccid:集成电路卡识别码，IC卡的唯一识别号码，共有20位字符组成，举例：89860423123123132135。'},
          ],
          getList1: [
            {a: 'status', b: '是', c: 'String', d: '错误码。0-成功   非0-失败'},
            {a: 'message', b: '是', c: 'String', d: '错误信息。错误码对应的错误描述，参考错误码列表'},
            {
              a: 'result',
              b: '是',
              c: 'Object',
              d: '返回结果集（status为“0”时，result包含正确的结果数据；status为“非0”时， result可能为空，也可能包含其他提示数据）'
            },
          ],
          getList2: [
            {a: 'useUnit', b: '是', c: 'String', d: '用量单位'},
            {a: 'useAmount', b: '是', c: 'double', d: '用量'},
          ],
          getList3: [
            {a: 'iccid', b: '是', c: 'String', d: 'iccid:集成电路卡识别码，IC卡的唯一识别号码，共有20位字符组成，举例：89860423123123132135。'},
            {a: 'dateTime', b: '是', c: 'String', d: '时间 yyyyMM 如 202101 注意：当月的去账期用量查询！'},
          ],
          getList4: [
            {a: 'status', b: '是', c: 'String', d: '错误码。0-成功   非0-失败'},
            {a: 'message', b: '是', c: 'String', d: '错误信息。错误码对应的错误描述，参考错误码列表'},
            {a: 'result', b: '是', c: 'Object', d: '返回结果集（status为“0”时，result包含正确的结果数据；status'},
            {a: '', b: '', c: '', d: '为“非0”时， result可能为空，也可能包含其他提示数据）'},
          ],
          getList5: [
            {a: 'useUnit', b: '是', c: 'String', d: '用量单位'},
            {a: 'useAmount', b: '是', c: 'double', d: '用量'},
          ],
          getList6: [
            {a: 'iccid', b: '是', c: 'String', d: 'iccid:集成电路卡识别码，IC卡的唯一识别号码，共有20位字符组成，举例：89860423123123132135。'},
          ],
          getList7: [
            {a: 'status', b: '是', c: 'String', d: '错误码。0-成功   非0-失败'},
            {a: 'message', b: '是', c: 'String', d: '错误信息。错误码对应的错误描述，参考错误码列表'},
            {
              a: 'result',
              b: '是',
              c: 'Object',
              d: '返回结果集（status为“0”时，result包含正确的结果数据；status为“非0”时， result可能为空，也可能包含其他提示数据）'
            },
          ],
          getList8: [
            {a: 'statusMessage', b: '是', c: 'String', d: '状态描述'},
            {a: 'statusCode', b: '是', c: 'double', d: '生命周期状态 详细参考 3.1 卡状态说明'},
          ],
          getList9: [
            {a: 'iccid', b: '是', c: 'String', d: 'iccid:集成电路卡识别码，IC卡的唯一识别号码，共有20位字符组成，举例：89860423123123132135。'},
          ],
          getList10: [
            {a: 'status', b: '是', c: 'String', d: '错误码。0-成功   非0-失败'},
            {a: 'message', b: '是', c: 'String', d: '错误信息。错误码对应的错误描述，参考错误码列表'},
            {
              a: 'result',
              b: '是',
              c: 'Object',
              d: '返回结果集（status为“0”时，result包含正确的结果数据；status为“非0”时， result可能为空，也可能包含其他提示数据）'
            },
          ],
          getList11: [
            {a: 'typeName', b: '是', c: 'String', d: '运营商系统类型'},
            {a: 'imei', b: '否', c: 'String', d: '获取当前绑定IMEI (typeName为‘电信CMP’取值 )'},
            {a: 'statusMessage', b: '否', c: 'String', d: '断开网状态说明(typeName为‘电信CMP’取值 )'},
            {
              a: 'statusCode', b: '否', c: 'String', d: '断开网编码 1 开网状态 2 断网状态\n' +
                '(typeName为‘电信CMP’取值 )'
            },
            {a: 'Data', b: '否', c: 'Map', d: '(typeName为‘移动EC’取值 )'},
            {a: 'apnId', b: '是', c: 'String', d: 'APN ID [Data 内部数据]'},
            {
              a: 'rat', b: '是', c: 'String', d: '接入方式：[Data 内部数据]\n' +
                '1: 3G\n' +
                '2: 2G\n' +
                '6: 4G\n' +
                '8: NB'
            },
            {a: 'ipv6', b: '否', c: 'String', d: 'Ipv6 地址[Data 内部数据]'},
            {a: 'ip', b: '是', c: '无', d: 'IP 地址[Data 内部数据]'},
            {a: 'ipv6Prefix', b: '是', c: '无', d: 'Ip6前缀[Data 内部数据]'},
            {
              a: 'status', b: '是', c: '无', d: '在线状态[Data 内部数据]\n' +
                '00: 离线\n' +
                '01：在线'
            },
            {a: 'createDate', b: '是', c: '无', d: '会话创建时间[Data 内部数据]'},
          ],
          getList12: [
            {a: 'companyId', b: '是', c: 'String', d: '集团编号 '},
          ],
          getList13: [
            {a: 'status', b: '是', c: 'String', d: '错误码。0-成功   非0-失败 '},
            {a: 'message', b: '是', c: 'String', d: '错误信息。错误码对应的错误描述，参考错误码列表'},
            {
              a: 'result',
              b: '是',
              c: 'Object',
              d: '返回结果集（status为“0”时，result包含正确的结果数据；status为“非0”时， result可能为空，也可能包含其他提示数据）'
            },
          ],
          getList14: [
            {a: 'blocPlantGroupId', b: '是', c: 'String', d: '资费组编号'},
            {a: 'blocPlantId', b: '否', c: 'String', d: '资费计划编号'},
            {a: 'blocPlanName', b: '否', c: 'String', d: '资费计划名称'},
            {a: 'blocPlanPrice', b: '否', c: 'String', d: '资费计划售价'},
            {a: 'blocPlanCapacity', b: '否', c: 'Map', d: '资费计划规格(单位为 MB)'},
            {a: 'blocPlanCost', b: '否', c: 'String', d: '资费计划成本'},
            {a: 'blocPlanTimeValue', b: '是', c: 'String', d: '资费计划 时间值 如： 1'},
            {a: 'blocPlanTimeUnit', b: '是', c: '无', d: '资费计划时间单位 如 ：月/年'},
          ],
          getList15: [
            {a: 'companyId', b: '是', c: 'String', d: '集团编号 '},
          ],
          getList16: [
            {a: 'status', b: '是', c: 'String', d: '错误码。0-成功   非0-失败'},
            {a: 'message', b: '是', c: 'String', d: '错误信息。错误码对应的错误描述，参考错误码列表'},
            {
              a: 'result',
              b: '是',
              c: 'Object',
              d: '返回结果集（status为“0”时，result包含正确的结果数据；status为“非0”时， result可能为空，也可能包含其他提示数据）'
            },
          ],
          getList17: [
            {a: 'companyName', b: '否', c: 'String', d: '集团名称'},
            {a: 'companyBalance', b: '否', c: 'String', d: '集团可用余额'},
          ],
          getList18: [
            {a: 'iccid', b: '是', c: 'String', d: 'iccid:集成电路卡识别码，IC卡的唯一识别号码，共有20位字符组成，举例：89860423123123132135。'},
            {a: 'Status', b: '是', c: 'String', d: '0 开机 1 停机'},
          ],
          getList19: [
            {a: 'status', b: '是', c: 'String', d: '错误码。0-成功   非0-失败'},
            {a: 'message', b: '是', c: 'String', d: '错误信息。错误码对应的错误描述，参考错误码列表'},
            {
              a: 'result',
              b: '是',
              c: 'Object',
              d: '返回结果集（status为“0”时，result包含正确的结果数据；status为“非0”时， result可能为空，也可能包含其他提示数据）'
            },
          ],
          getList20: [],
          getList21: [
            {a: 'iccid', b: '是', c: 'String', d: 'iccid:集成电路卡识别码，IC卡的唯一识别号码，共有20位字符组成，举例：89860423123123132135。'},
            {a: 'StatusBreak', b: '是', c: 'String', d: '0 开网 1 断网'},
          ],
          getList22: [
            {a: 'status', b: '是', c: '无', d: '错误码。0-成功   非0-失败'},
            {a: 'message', b: '是', c: '无', d: '错误信息。错误码对应的错误描述，参考错误码列表'},
            {a: 'result', b: '是', c: '无', d: '返回结果集（status为“0”时，result包含正确的结果数据；status为“非0”时， result可能为空，也可能包含其他提示数据）'},
          ],
          getList23: [
            {a: 'iccid', b: '是', c: 'String', d: 'iccid:集成电路卡识别码，IC卡的唯一识别号码，共有20位字符组成，举例：89860423123123132135。'},
            {a: 'companyId', b: '是', c: 'String', d: '集团编号 '},
            {
              a: 'effectiveType', b: '是', c: 'String', d: '生效类型详细请参3.4生效类别说明\n' +
                '当月生效\t1\n' +
                '次月生效\t2\n' +
                '周期延顺\t3 '
            },
            {a: 'blocPlantId', b: '是', c: 'String', d: '资费计划编号 (来源 2.1.6 资费计划 可用查询 ) '},
          ],
          getList24: [
            {a: 'status', b: '是', c: '无', d: '错误码。0-成功   非0-失败'},
            {a: 'message', b: '是', c: '无', d: '错误信息。错误码对应的错误描述，参考错误码列表'},
            {a: 'result', b: '是', c: '无', d: '返回结果集（status为“0”时，result包含正确的结果数据；status为“非0”时， result可能为空，也可能包含其他提示数据）'},
          ],
        },
      ],
      //三、 辅助说明
      gitListArr: [
        {

        }
      ],
      //
      gitListArr4: [
        {

        }
      ],




      //请求案例

      Requestcase:"// 服务器请求地址     http://api.5iot.com/route/open/cardInfo\n" +
        "\tprotected  static String server_Ip = \"http://api.5iot.cn/route/open\";\n" +
        "\t// appId\n" +
        "\tpublic static String appId = null;\n" +
        "\t//秘钥密码\n" +
        "\tpublic static String password= null;\n" +
        "\n" +
        "\n" +
        "\t/**\n" +
        "\t * Sign 加密类\n" +
        "\t * @param map\n" +
        "\t * @param key\n" +
        "\t * @return\n" +
        "\t */\n" +
        "\tpublic static String getSign(Map<String,Object> map,String key){\n" +
        "\t\tList<String> list=new ArrayList<>(map.keySet());\n" +
        "\t\tCollections.sort(list);\n" +
        "\t\tStringBuffer sb=new StringBuffer();\n" +
        "\t\tfor(int i=0;i<list.size();i++){\n" +
        "\t\t\tString k =list.get(i);\n" +
        "\t\t\tString v=(String )map.get(k);\n" +
        "\t\t\tsb.append(k).append(\"=\").append(v).append(\"&\");\n" +
        "\t\t}\n" +
        "\t\tString signstr=sb.append(\"key=\").append(key).toString();\n" +
        "\t\tString sign = MD5Util.MD5Encode(signstr).toUpperCase();\n" +
        "\t\treturn sign;\n" +
        "\t}\n" +
        "\n" +
        "\n" +
        "\tpublic static void main(String[] args) {\n" +
        "\n" +
        "\t\tMap<String, Object> map = new HashMap<String, Object>();\n" +
        "\t\tMap<String, Object> Parmap = new HashMap<String, Object>();\n" +
        "\t\tMap<String, Object> verify = new HashMap<String, Object>();\n" +
        "\n" +
        "\t\tString appId = \"test\";\n" +
        "\t\tString password = \"uhbhkh123123\";\n" +
        "\t\tString access_key = \"1234567896132\";\n" +
        "\t\tString timeStamp = \"\"+System.currentTimeMillis() ;\n" +
        "\n" +
        "\t\tverify.put(\"password\", password);\n" +
        "\t\tverify.put(\"appId\", appId);\n" +
        "\t\tverify.put(\"timeStamp\", timeStamp);\n" +
        "\t\tverify.put(\"sign\", getSign(verify, access_key));\n" +
        "\n" +
        "\t\tParmap.put(\"type\", \"iccid\");\n" +
        "\t\tParmap.put(\"cardno\", \"89860423123123132135\");\n" +
        "\n" +
        "\n" +
        "\t\tmap.put(\"verify\", verify);\n" +
        "\t\tmap.put(\"Param\", Parmap);\n" +
        "\n" +
        "\t\tserver_Ip +=\"/cardInfo\";\n" +
        "\t\tString result = null;\n" +
        "\n" +
        "\t\tString data = JSON.toJSONString(map);\n" +
        "\t\tresult = HttpUtil.post(server_Ip, data);// 返回结果字符串\n" +
        "\t\tSystem.out.println(result);\n" +
        "\t}",


    }
  },
  created() {


  },

  methods: {},
}
</script>

<style scoped>

</style>
