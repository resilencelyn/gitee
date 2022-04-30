<template>
  <div style="display: flex; flex-flow: column;">
    <el-row justify="space-between">
      <el-col :span="6">
        <el-space>
          <el-select v-model="query.type" clearable  @change="load(1)" placeholder="类型" style="width: 100%">
            <el-option v-for="opt in $cfg.collectorType" :key="opt.key" :value="opt.key" :label="opt.name"/>
          </el-select>
          <el-select v-model="query.success" clearable  @change="load(1)" placeholder="是否成功" style="width: 100%">
            <el-option v-for="opt in successTypes" :key="opt.key" :value="opt.key" :label="opt.name"/>
          </el-select>
          <el-select v-model="query.dataSuccess" clearable  @change="load(1)" placeholder="是否查得" style="width: 100%">
            <el-option v-for="opt in boolTypes" :key="opt.key" :value="opt.key" :label="opt.name"/>
          </el-select>
          <el-select v-model="query.cache" clearable  @change="load(1)" placeholder="是否缓存" style="width: 100%">
            <el-option v-for="opt in boolTypes" :key="opt.key" :value="opt.key" :label="opt.name"/>
          </el-select>
        </el-space>
      </el-col>
      <el-col :span="3"><decision-autocomplete v-model="query.decisionId" /></el-col>
      <el-col :span="3"><collector-autocomplete v-model="query.collector" /></el-col>
      <el-col :span="2">
        <el-input-number v-model="query.spend" placeholder="耗时(>=ms)" :min="0" @keyup.enter="load(1)" controls-position="right"  style="width: 100%"/>
      </el-col>
      <el-col :span="3"><el-input v-model="query.decideId"  placeholder="决策流水id(精确)" @keyup.enter="load(1)"/></el-col>
      <el-col :span="6">
        <el-space>
          <el-date-picker v-model="query.startTime" type="datetime" placeholder="开始时间" 
                          format="YYYY-MM-DD HH:mm:ss"
                          :editable="false" :clearable="false" :disabledDate="(d) => d.getTime() > Date.now()"
                          @change="load(1)" style="width: 100%;"/>
          -
          <el-date-picker v-model="query.endTime" type="datetime" placeholder="结束时间" 
                          format="YYYY-MM-DD HH:mm:ss"
                          :editable="false" :disabledDate="(d) => d.getTime() < query.startTime"
                          @change="load(1)" style="width: 100%;"/>
        </el-space>
      </el-col>
      <el-col :span="0.5">
        <el-button @click="load(1)" type="primary" circle ><el-icon><svg-search/></el-icon></el-button>
      </el-col>
    </el-row>
    <div ref="table" style="flex: 1; margin-top: 12px">
      <el-table :data="collectResultPage.list" stripe empty-text="无数据" @expand-change="loadDetail" :row-key="(row) => row.id">
        <!-- 详情数据 -->
        <el-table-column type="expand">
          <template #default="{row}">
            <div style="width: calc(100% - 40px); margin: 0 auto" v-loading="row._loadingDetail">
              <el-form label-width="100px" label-position="right">
                <el-form-item v-if="row.collectorType === 'Http'" label="接口地址">{{row.url}}</el-form-item>
                <el-form-item v-if="row.collectorType === 'Http' && row.body" label="接口Body">{{row.body}}</el-form-item>
                <el-form-item label="收集结果">{{row.result}}</el-form-item>
                <el-form-item label="执行异常">{{row.exception}}</el-form-item>
                <el-form-item v-if="row.collectorType === 'Http'" label="解析结果">{{row.resolveResult}}</el-form-item>
                <el-form-item v-if="row.collectorType === 'Http'" label="解析异常">{{row.resolveException}}</el-form-item>
              </el-form>
            </div>
          </template>
        </el-table-column>
        <el-table-column label="收集器" show-overflow-tooltip >
          <template #default="{row}">
            <router-link v-if="row.collectorName" :to="'/policy/collector?id=' + row.collector" class="el-link el-link--primary is-underline">{{row.collectorName}}</router-link>
            <span v-else>{{row.collector}}</span>
          </template>
        </el-table-column>
        <el-table-column label="类型" width="100" >
          <template #default="{row}">
            {{$cfg.collectorType.find(o => o.key === row.collectorType).name}}
          </template>
        </el-table-column>
        <el-table-column label="收集时间" width="168" >
          <template #default="{row}">
            <date-item :time="row.collectDate" />
          </template>
        </el-table-column>
        <el-table-column label="决策" show-overflow-tooltip >
          <template #default="{row}">
            <router-link v-if="row.decisionName" :to="'/policy/decision?id=' + row.decisionId" class="el-link el-link--primary is-underline">{{row.decisionName}}</router-link>
            <span v-else>{{row.decisionId}}</span>
          </template>
        </el-table-column>
        <el-table-column label="决策流水" >
          <template #default="{row}">
            <el-link type="primary" @click="query.decideId = row.decideId; load(1)">{{row.decideId}}</el-link>
          </template>
        </el-table-column>
        <el-table-column label="耗时(ms)" prop="spend" width="100" />
        <el-table-column label="状态" width="70" >
          <template #default="{row}">
            {{row.status === '0000' ? '成功' : '失败'}}
          </template>
        </el-table-column>
        <el-table-column label="查得" width="60" >
          <template #default="{row}">
            {{row.dataStatus === '0000' ? '是' : '否'}}
          </template>
        </el-table-column>
        <el-table-column label="缓存" width="60" >
          <template #default="{row}">
            {{row.cache ? '是' : '否'}}
          </template>
        </el-table-column>
      </el-table>
      <div style="display: flex; justify-content: flex-end; margin-top: 8px">
        <el-pagination background layout="prev, pager, next, jumper" :total="collectResultPage.totalRow" :page-count="collectResultPage.totalPage" @current-change="load($event)"/>
      </div>
    </div>
  </div>
</template>

<script>
import DecisionAutocomplete from "../../component/DecisionAutocomplete.vue";
import CollectorAutocomplete from "../../component/CollectorAutocomplete.vue";
import DateItem from "../../../component/DateItem.vue";
import SvgSearch from "../../../component/svg/SvgSearch.vue";

export default {
  name: "CollectResult",
  components: {SvgSearch, DateItem, CollectorAutocomplete, DecisionAutocomplete},
  data() {
    const d = new Date()
    return {
      successTypes: [
        { name: '成功', key: 'true'},
        { name: '失败', key: 'false'},
      ],
      boolTypes: [
        { name: '是', key: 'true'},
        { name: '否', key: 'false'},
      ],
      query: {startTime: new Date(d.getFullYear(), d.getMonth(), d.getDate()),},
      collectResultPage: {
        page: 1, pageSize: 10, totalPage: 0, totalRow: 0, list: []
      },
    }
  },
  mounted() {
    clone(this.$router.currentRoute.value.query, this.query)
    this.load(1)
  },
  watch: {
    'query.decisionId'() { this.load(1) },
    'query.collector'() { this.load(1) },
  },
  methods: {
    load(page = 1, pageSize = 10) {
      const data = clone(this.query)
      data.startTime = data.startTime ? data.startTime.getTime() : null
      data.endTime = data.endTime ? data.endTime.getTime() : null
      const loading = this.$loading({target: this.$refs.table})
      axios.get(`collector/collectResultPaging/${page}?limit=${pageSize}&${toQuery(data)}`).then(resp => {
        loading.close()
        if (resp.code === '00') {
          resp.data.list.forEach(o => o._loadingDetail = false)
          this.collectResultPage = resp.data
        }
      }).catch(e => loading.close())
    },
    loadDetail(item) {
      if (item.result) return
      item._loadingDetail = true
      axios.get(`collector/collectResult/${item.id}`).then(resp => {
        item._loadingDetail = false
        if (resp.code === '00') clone(resp.data, item)
      }).catch(e => item._loadingDetail = false)
    }
  }
}
</script>

<style scoped>

</style>