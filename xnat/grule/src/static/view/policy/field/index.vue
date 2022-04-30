<template>
    <div style="display: flex; flex-flow: column;">
        <el-space>
            <el-button v-if="hasAddAuth" type="primary" @click="showAddDialog = true" circle>
                <el-icon><svg-plus/></el-icon>
            </el-button>
            <collector-autocomplete v-model="query.collectorId" />
            <el-input v-model="query.kw" placeholder="按字段名搜索" @keyup.enter="load(1)">
                <template #append>
                    <el-button @click="load(1)"><el-icon><svg-search/></el-icon></el-button>
                </template>
            </el-input>
        </el-space>
        <div ref="table" style="flex: 1; margin-top: 12px">
            <el-table :data="fieldPage.list" stripe border empty-text="无数据">
              <el-table-column v-if="!hasUpdateAuth" type="expand">
                <template #default="{row}">
                  <div v-if="row.collectorOptions && row.collectorOptions.length" style="width: calc(100% - 80px); margin: 0 auto; display: flex">
                    <el-form label-width="80px" disabled style="width: calc(100% - 10px)">
                      <el-form-item label="值函数">
                        <div v-for="(opt, index) in row.collectorOptions" :key="opt.collectorId" style="width: 100%; margin-top: 2px;">
                          <collector-autocomplete v-model="opt.collectorId" disabled />
                          <ace-groovy v-model="opt.chooseFn" style="height: 80px" :placeholder='"// Groovy脚本. 返回true代表选择当前收集器"' readonly />
                        </div>
                      </el-form-item>
                    </el-form>
                  </div>
                </template>
              </el-table-column>
                <el-table-column label="标识" prop="key" show-overflow-tooltip />
                <el-table-column label="名字" prop="name" show-overflow-tooltip />
                <el-table-column label="类型" :width="hasUpdateAuth ? 120 : 70">
                    <template #default="{row}">
                      <el-select v-if="hasUpdateAuth" v-model="row.type" @change="updateType(row)" :disabled="row._typeChanging" style="width: 100%">
                        <el-option v-for="t in $cfg.fieldType" :label="t.name" :value="t.key" />
                      </el-select>
                      <span v-else>{{$cfg.fieldType.find(o => o.key === row.type).name}}</span>
                    </template>
                </el-table-column>
                <el-table-column label="更新时间" width="166">
                    <template #default="{row}">
                        <date-item :time="row.updateTime"/>
                    </template>
                </el-table-column>
                <el-table-column label="收集器">
                    <template #default="{row}">
                        <div v-if="row.collectorOptions" v-for="opt in row.collectorOptions" :key="opt.collectorId">
                            <el-link v-if="opt.collectorName" type="primary" @click="jumpToCollector({id: opt.collectorId})">
                                {{opt.collectorName}}
                            </el-link>
                            <span v-else>{{opt.collectorId}}</span>
                        </div>
                    </template>
                </el-table-column>
                <el-table-column label="描述" show-overflow-tooltip>
                    <template #default="{row}">
                        <span>{{row.comment}}</span>
                    </template>
                </el-table-column>
                <el-table-column label="操作" v-if="hasUpdateAuth || hasDeleteAuth" :width="20 + (hasUpdateAuth ? 64 : 0) + (hasDeleteAuth ? 64 : 0)">
                    <template #default="{row}">
                        <el-space>
                            <el-link v-if="hasUpdateAuth" type="primary" @click="showEditDialog = true; editField = row">
                                <el-icon><svg-edit /></el-icon>编辑
                            </el-link>
                            <el-link v-if="hasDeleteAuth" type="danger" @click="del(row)">
                                <el-icon><svg-delete /></el-icon>删除
                            </el-link>
                        </el-space>
                    </template>
                </el-table-column>
            </el-table>
            <div style="display: flex; justify-content: flex-end; margin-top: 8px">
                <el-pagination background layout="prev, pager, next, jumper" :total="fieldPage.totalRow" :page-count="fieldPage.totalPage" @current-change="load($event)"/>
            </div>
        </div>
    </div>

    <el-dialog v-model="showAddDialog" destroy-on-close center width="680px">
        <template #title>添加字段</template>
        <add-or-edit @close="showAddDialog = false; load(1)"/>
    </el-dialog>
    <el-dialog v-model="showEditDialog" destroy-on-close center width="680px">
        <template #title=>编辑字段</template>
        <add-or-edit :field="editField" @close="showEditDialog = false; load(1)"/>
    </el-dialog>
</template>

<script>
import SvgPlus from "../../../component/svg/SvgPlus.vue";
import CollectorAutocomplete from "../../component/CollectorAutocomplete.vue";
import SvgSearch from "../../../component/svg/SvgSearch.vue";
import DateItem from "../../../component/DateItem.vue";
import SvgEdit from "../../../component/svg/SvgEdit.vue";
import SvgDelete from "../../../component/svg/SvgDelete.vue";
import AddOrEdit from "./AddOrEdit.vue";
import AceGroovy from "../../../component/AceGroovy.vue";
export default {
    name: "Field",
    components: {AceGroovy, AddOrEdit, SvgDelete, SvgEdit, DateItem, SvgSearch, CollectorAutocomplete, SvgPlus},
    data() {
        return {
            query: { kw: null, collector: null, },
            showAddDialog: false,
            showEditDialog: false, editField: null,
            fieldPage: {
                page: 1, pageSize: 10, totalPage: 0, totalRow: 0, list: []
            }
        }
    },
    created() {
    },
    mounted() {
        this.load(1)
    },
    watch: {
        'query.collectorId'() {
            this.load(1)
        }
    },
    computed: {
        hasAddAuth() {
            return this.$user.privileges.has('Field_Add')
        },
        hasUpdateAuth() {
            return this.$user.privileges.has('Field_Update')
        },
        hasDeleteAuth() {
            return this.$user.privileges.has('Field_Del')
        },
    },
    methods: {
        jumpToCollector(query) {
            this.$router.push({path: '/policy/collector', query: query})
        },
        load(page = 1, pageSize = 10) {
            const loading = this.$loading({target: this.$refs.table})
            axios.get(`field/paging/${page}?limit=${pageSize}&` + toQuery(this.query)).then(resp => {
                loading.close()
                if (resp.code === '00') {
                  resp.data.list.map(item => {
                    item._lastType = item.type // 记录更新前的类型
                    item._typeChanging = false // 更新类型中
                  })
                  this.fieldPage = resp.data
                }
            }).catch(e => loading.close())
        },
        updateType(field) {
          field._typeChanging = true
          axios.post(`field/${field.id}/updateType`, {type: field.type}).then(res => {
            field._typeChanging = false
            if (res.code === '00') {
              field._lastType = field.type
              this.$message.success(`更新字段类型(${field.name})成功`)
            } else {
              field.type = field._lastType
            }
          }).catch(e => field._typeChanging = false)
        },
        del(field) {
            this.$confirm(
                    `确认删除字段: ${field.name}?`, '警告',
                    {
                        confirmButtonText: '确认',
                        cancelButtonText: '取消',
                        type: 'warning',
                    }
            ).then(() => {
                axios.delete(`field/${field.id}`).then(resp => {
                    if (resp.code === '00') {
                      this.$message.success('删除字段: ' + field.name + ' 成功')
                      this.load(1)
                    }
                })
            }).catch(e => {})
        }
    }
}
</script>

<style scoped>
</style>