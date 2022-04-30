<template>
<div style="display: flex; flex-direction: column">
  <el-space>
    <el-select v-model="query.type" clearable placeholder="类型" style="width: 90px">
      <el-option v-for="opt in $cfg.opHistoryType" :key="opt.key" :value="opt.key" :label="opt.name"/>
    </el-select>
    <user-autocomplete v-model="query.operator" placeholder="选择操作员"/>
    <el-input v-model="query.dataId" placeholder="数据id" @keyup.enter="load(1)" style="width: 280px" />
    <el-input v-model="query.kw" placeholder="内容关键词" @keyup.enter="load(1)" >
      <template #append>
        <el-button @click="load(1)"><el-icon class="el-icon--right"><svg-search/></el-icon></el-button>
      </template>
    </el-input>
  </el-space>

  <div ref="table" style="flex: 1; margin-top: 12px">
    <el-table :data="opHistoryPage.list" stripe empty-text="无数据">
      <!-- 详情数据 -->
      <el-table-column type="expand">
        <template #default="{row}">
          <div style="width: 99%">
            <ace-json v-model="row._formatContent" readonly style="height: 320px;" />
          </div>
        </template>
      </el-table-column>
      <el-table-column label="类型" show-overflow-tooltip >
        <template #default="{row}">
          {{$cfg.opHistoryType.find(o => o.key === row.type).name}}
        </template>
      </el-table-column>
      <el-table-column label="数据id" prop="dataId"></el-table-column>
      <el-table-column label="操作员" prop="creatorName"></el-table-column>
    </el-table>
    <div style="display: flex; justify-content: flex-end; margin-top: 8px">
      <el-pagination background layout="prev, pager, next, jumper" :total="opHistoryPage.totalRow" :page-count="opHistoryPage.totalPage" @current-change="load($event)"/>
    </div>
  </div>
</div>
</template>

<script>
import AceJson from "../../../component/AceJson.vue";
import SvgSearch from "../../../component/svg/SvgSearch.vue";
import UserAutocomplete from "../../component/UserAutocomplete.vue";
export default {
  name: "OpHistory",
  components: {UserAutocomplete, SvgSearch, AceJson},
  data() {
    return {
      query: {},
      opHistoryPage: {
        page: 1, pageSize: 10, totalPage: 0, totalRow: 0, list: []
      },
    }
  },
  mounted() {
    this.load(1)
  },
  watch: {
    'query.type'() {
      this.load(1)
    },
    'query.operator'() {
      this.load(1)
    }
  },
  methods: {
    load(page = 1, pageSize = 10) {
      const loading = this.$loading({target: this.$refs.table})
      axios.get(`opHistoryPaging/${page}?limit=${pageSize}&${toQuery(this.query)}`).then(resp => {
        loading.close()
        if (resp.code === '00') {
          resp.data.list.forEach(o => o._formatContent = JSON.stringify(o.content, null, 4).trim())
          this.opHistoryPage = resp.data
        }
      }).catch(e => loading.close())
    }
  }
}
</script>

<style scoped>

</style>