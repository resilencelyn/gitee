<template>
  <div v-loading="item._loadingDetail">
    <div style="display: flex; min-height: 260px; max-height: 600px">
      <div style="flex: 1; display: flex; align-items: center">
        <span style="font-size: 14px; width: 20px">入参</span>
        <ace-json v-model="inputStr" readonly style="flex: 1" />
      </div>
      <div style="flex: 1; display: flex; margin-left: 8px; align-items: center;">
        <span style="font-size: 14px; width: 20px;">数据收集</span>
        <ace-json v-model="dataCollectResultStr" style="flex: 1;" readonly />
      </div>
    </div>
    <div style="display: flex; margin-top: 10px">
      <!-- 左侧策略/规则集表格 -->
      <div style="width: 55%; display: flex; flex-flow: column;">
        <el-row justify="space-between">
          <el-col :span="12">执行策略/规则集</el-col>
          <el-col :span="12">
            <el-input v-model="ruleKw" placeholder="策略名,规则名过虑" />
          </el-col>
        </el-row>
        <div style="margin-top: 2px">
          <el-table :data="policies" border row-key="_id" default-expand-all style="width: 100%" max-height="800" empty-text="无数据">
            <el-table-column label="属性">
              <template #default="{row}">
                <attr-table :attrs="row.attrs"/>
              </template>
            </el-table-column>
            <el-table-column label="结果" width="70" align="center">
              <template #default="{row}">
                {{$cfg.decideResultType.find(o => o.key === row.result)?.name}}
              </template>
            </el-table-column>
            <el-table-column label="数据" align="center">
              <template #default="{row}">
                <attr-table v-if="row.data && row.data.length" :attrs="row.data.reduce((obj, item) => {obj[(item.name ? item.name : item.key)] = item.value; return obj;}, {})"/>
              </template>
            </el-table-column>
          </el-table>
        </div>
      </div>

      <!-- 右侧属性集表格 -->
      <div style="flex: 1; margin-left: 6px">
        <el-row justify="space-between">
          <el-col :span="12">属性结果集</el-col>
          <el-col :span="12">
            <el-input v-model="attrKw"  placeholder="属性名过虑" />
          </el-col>
        </el-row>
        <div style="margin-top: 2px">
          <el-table :data="attrs" border stripe max-height="800" empty-text="无数据">
            <el-table-column label="属性名" min-width="40%" align="right">
              <template #default="{row}">
                {{row.name ? row.name : row.key}}
              </template>
            </el-table-column>
            <el-table-column label="属性值" prop="value" min-width="60%" align="left"></el-table-column>
          </el-table>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
import AceJson from "../../../component/AceJson.vue";
import SvgSearch from "../../../component/svg/SvgSearch.vue";
import AttrTable from "../../../component/AttrTable.vue";
export default {
  name: "Detail",
  components: {AttrTable, SvgSearch, AceJson},
  props: {item: {type: Object, required: true}},
  data() {
    return {
      ruleKw: null,
      attrKw: null,
    }
  },
  computed: {
    title() {
      return this.item.decisionName + ' ' + this.item.id + ' ' + this.$cfg.decideResultType.find(o => o.key === this.item.result).name;
    },
    inputStr() {
      // TODO 顺序
      return this.item.input ? JSON.stringify(this.item.input, null, 4) : ''
    },
    dataCollectResultStr() {
      // TODO 顺序
      return this.item.dataCollectResult ? JSON.stringify(this.item.dataCollectResult, null, 4) : ''
    },
    policies() {
      if (this.item.detail == null || this.item.detail.policies == null || this.item.detail.policies.length < 1) return [];
      if (this.ruleKw != null && this.ruleKw !== '') {
        return clone(this.item.detail.policies).filter((p, index, arr) => {
          if (p.items && p.items.length > 0) {
            p.children = p.items.filter(o => {
              if (o.attrs['规则名'] && o.attrs['规则名'].indexOf(this.ruleKw) >= 0) return true
              if (o.attrs['评分卡名'] && o.attrs['评分卡名'].indexOf(this.ruleKw) >= 0) return true
              if (o.attrs['决策名'] && o.attrs['决策名'].indexOf(this.ruleKw) >= 0) return true
              return false
            })
          }

          if (p.attrs['策略名'] && p.attrs['策略名'].indexOf(this.ruleKw) === -1 && p.children.length === 0) return false;
          return true;
        });
      }
      return this.item.detail.policies
    },
    attrs() {
      if (this.attrKw && this.item.data) {
        return this.item.data.filter((item, index, arr) => {
          if (item.key && item.key.toLowerCase().indexOf(this.attrKw.toLowerCase()) >= 0) return true
          if (item.name && item.name.indexOf(this.attrKw) >= 0) return true
          return false
        })
      }
      return this.item.data;
    }
  },
  mounted() {
    this.load()
  },
  methods: {
    load() {
      if (this.item.detail) return
      this.item._loadingDetail = true
      axios.get(`decision/decideResultDetail/${this.item.id}`).then(resp => {
        this.item._loadingDetail = false
        if (resp.code === '00') {
          let id = 0; // el-table 需要一个 row-key
          resp.data.detail.policies.forEach(p => {
            if (!p['_id']) p['_id'] = (++id) + '';
            if (p.items && p.items.length > 0) {
              let idd = 0;
              p.items.forEach(o => {
                if (!o._id) o._id = p._id + "_" + (++idd);
              })
              p.children = p.items
            }
          });
          clone(resp.data, this.item)
        }
      }).catch(e => this.item._loadingDetail = false)
    }
  }
}
</script>
<style scoped>

</style>