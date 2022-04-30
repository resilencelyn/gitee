<template>
  <div style="display: flex; flex-flow: column;">
    <div>
      <!-- 不能用 gutter 会导致 下边有横向流动条 -->
      <el-row justify="space-between">
        <el-col :span="2">
          <el-select v-model="query.result" clearable clearable placeholder="决策结果" @change="load(1)" style="width: 100%">
            <el-option v-for="opt in $cfg.decideResultType" :key="opt.key" :value="opt.key" :label="opt.name"/>
          </el-select>
        </el-col>
        <el-col :span="4">
          <decision-autocomplete v-model="query.decisionId" />
        </el-col>
        <el-col :span="5">
          <el-input v-model="query.id"  placeholder="流水id(精确)" @keyup.enter="load(1)"/>
        </el-col>
        <el-col :span="3">
          <el-input-number v-model="query.spend" placeholder="耗时(>=ms)" :min="0" @keyup.enter="load(1)" controls-position="right"  style="width: 100%"/>
        </el-col>
        <el-col :span="8">
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
        <el-col :span="1">
          <el-button @click="load(1)" type="primary" circle ><el-icon><svg-search/></el-icon></el-button>
        </el-col>
      </el-row>
      <!-- TODO 后端返回数据库是否支持属性查询 -->
      <div style="margin-top: 6px">
        <el-row justify="space-between" v-for="(item, index) in query.attrFilters">
          <el-col :span="7"><field-autocomplete v-model="item.fieldKey"/></el-col>
          <el-col :span="6">
            <el-select v-model="item.op"  @change="((item.op === 'desc' || item.op === 'asc') && item.fieldKey) ? load(1) : null" style="width: 100%">
              <el-option v-for="op in ops" :label="op.name" :value="op.key" />
            </el-select>
          </el-col>
          <el-col :span="8">
            <el-input v-if="item.op !== 'desc' && item.op !== 'asc'" v-model="item.value" placeholder="属性值" @keyup.enter="load" />
          </el-col>
          <el-col :span="2">
            <el-space>
              <el-button type="danger" @click="delAttrFilter(item)" size="mini" circle><el-icon> <svg-delete /> </el-icon></el-button>
              <el-button v-if="query.attrFilters.length === (index + 1)" type="success" @click="addAttrFilter" size="mini" circle><el-icon> <svg-plus /> </el-icon></el-button>
            </el-space>
          </el-col>
        </el-row>
      </div>
    </div>
    <div ref="table" style="flex: 1; margin-top: 12px">
      <el-table :data="decisionResultPage.list" stripe empty-text="无数据" row-key="id">
        <!-- 详情数据 -->
        <el-table-column type="expand">
          <template #default="{row}">
            <detail :item="row" style="width: calc(100% - 80px); margin: 0 auto" />
          </template>
        </el-table-column>
        <el-table-column label="决策" show-overflow-tooltip >
          <template #default="{row}">
            <el-icon v-if="row.exception" ><svg-warning /></el-icon>
            <router-link v-if="row.decisionName" :to="'/policy/decision?id=' + row.decision" class="el-link el-link--primary is-underline">{{row.decisionName}}</router-link>
            <span v-else>{{row.decisionId}}</span>
          </template>
        </el-table-column>
        <el-table-column label="流水id" width="294">
          <template #default="{row}">
            <router-link :to="'/data/collect?decideId=' + row.id" class="el-link el-link--primary is-underline">{{row.id}}</router-link>
          </template>
        </el-table-column>
        <el-table-column label="结果" width="70">
          <template #default="{row}">
            {{$cfg.decideResultType.find(o => o.key === row.result)?.name}}
          </template>
        </el-table-column>
        <el-table-column label="决策时间" width="168">
          <template #default="{row}">
            <date-item :time="row.occurTime"/>
          </template>
        </el-table-column>
        <el-table-column label="耗时(ms)" prop="spend" width="88" />
        <el-table-column label="异常信息" prop="exception" />
      </el-table>
      <div style="display: flex; justify-content: flex-end; margin-top: 8px">
        <el-pagination background layout="prev, pager, next, jumper" :total="decisionResultPage.totalRow" :page-count="decisionResultPage.totalPage" @current-change="load($event)"/>
      </div>
    </div>
  </div>
</template>

<script>
import DecisionAutocomplete from "../../component/DecisionAutocomplete.vue";
import FieldAutocomplete from "../../component/FieldAutocomplete.vue";
import SvgDelete from "../../../component/svg/SvgDelete.vue";
import SvgPlus from "../../../component/svg/SvgPlus.vue";
import SvgSearch from "../../../component/svg/SvgSearch.vue";
import DateItem from "../../../component/DateItem.vue";
import Detail from "./Detail.vue";
import SvgWarning from "../../../component/svg/SvgWarning.vue";

export default {
  name: "DecideResult",
  components: {SvgWarning, Detail, DateItem, SvgSearch, SvgPlus, SvgDelete, FieldAutocomplete, DecisionAutocomplete},
  data() {
    const d = new Date()
    return {
      query: {
        result: null, decisionId: null,
        startTime: new Date(d.getFullYear(), d.getMonth(), d.getDate()),
        endTime: null,
        attrFilters: [{fieldKey: null, op: '=', value: null}]
      },
      decisionResultPage: {
        page: 1, pageSize: 10, totalPage: 0, totalRow: 0, list: []
      },
      ops: [
        { name: '等于', key: '='},
        { name: '大于', key: '>'},
        { name: '大于等于', key: '>='},
        { name: '小于', key: '<'},
        { name: '小于等于', key: '<='},
        { name: '包含', key: 'contains'},
        { name: '降序', key: 'desc'},
        { name: '升序', key: 'asc'},
      ],
    }
  },
  mounted() {
    this.load(1)
  },
  watch: {
    'query.decisionId'() { this.load(1) },
  },
  methods: {
    load(page = 1, pageSize = 10) {
      const loading = this.$loading({target: this.$refs.table})
      const data = clone(this.query)
      data.startTime = data.startTime ? data.startTime.getTime() : null
      data.endTime = data.endTime ? data.endTime.getTime() : null
      data.attrConditions = JSON.stringify(data.attrFilters.filter(o => o.fieldKey != null));
      delete data.attrFilters;
      axios.get(`decision/decideResultPaging/${page}?limit=${pageSize}&${toQuery(data)}`).then(resp => {
        loading.close()
        if (resp.code === '00') {
          this.decisionResultPage = resp.data
        }
      }).catch(e => loading.close())
    },
    addAttrFilter() {
      this.query.attrFilters.push({fieldKey: null, op: '=', value: null})
    },
    delAttrFilter(item) {
      this.query.attrFilters.splice(this.query.attrFilters.indexOf(item), 1);
    }
  }
}
</script>

<style scoped>

</style>