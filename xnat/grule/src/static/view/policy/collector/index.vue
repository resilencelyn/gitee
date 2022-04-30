<template>
    <div style="display: flex; flex-flow: column;">
        <el-space>
            <el-button v-if="hasAddAuth" type="primary" @click="showAddDialog = true" circle>
                <el-icon><svg-plus/></el-icon>
            </el-button>
            <el-select v-model="query.type" clearable placeholder="类型" style="width: 90px">
                <el-option v-for="opt in $cfg.collectorType" :key="opt.key" :value="opt.key" :label="opt.name"/>
            </el-select>
            <el-input v-model="query.nameLike" placeholder="按收集器名搜索" @keyup.enter="load(1)">
                <template #append>
                    <el-button @click="load(1)"><el-icon><svg-search/></el-icon></el-button>
                </template>
            </el-input>
        </el-space>
        <div ref="table" style="flex: 1; margin-top: 12px">
            <el-table :data="collectorPage.list" stripe border empty-text="无数据">
                <el-table-column v-if="!hasUpdateAuth" type="expand">
                  <template #default="{row}">
                    <div style="width: calc(100% - 40px); margin: 0 auto">
                      <add-or-edit :collector="row"/>
                    </div>
                  </template>
                </el-table-column>
                <el-table-column label="收集器名" prop="name" show-overflow-tooltip />
                <el-table-column label="类型" width="64">
                    <template #default="{row}">
                        {{$cfg.collectorType.find(o => o.key === row.type).name}}
                    </template>
                </el-table-column>
                <el-table-column label="状态" width="68">
                    <template #default="scope">
                        <el-switch v-model="scope.row.enabled" :disabled="!hasUpdateAuth" :loading="scope.row._statusSwitching" @change="switchStatus(scope.row)" />
                    </template>
                </el-table-column>
                <el-table-column label="更新时间"  width="166">
                    <template #default="scope">
                        <date-item :time="scope.row.updateTime"/>
                    </template>
                </el-table-column>
                <el-table-column label="描述" show-overflow-tooltip>
                    <template #default="scope">
                        <span>{{scope.row.comment}}</span>
                    </template>
                </el-table-column>
                <el-table-column label="操作" :width="68 + (hasUpdateAuth ? 55 : 0) + (hasDeleteAuth ? 55 : 0)">
                    <template #default="{row}">
                        <el-space>
                            <el-link v-if="hasUpdateAuth" type="primary" @click="showEditDialog = true; editCollector = row">
                                <el-icon><svg-edit /></el-icon>编辑
                            </el-link>
                            <el-link v-if="hasDeleteAuth" type="danger" @click="del(row)">
                                <el-icon><svg-delete /></el-icon>删除
                            </el-link>
                            <el-link type="warning" @click="showTestDialog = true; testCollector = row">
                                测试
                            </el-link>
                        </el-space>
                    </template>
                </el-table-column>
            </el-table>
            <div style="display: flex; justify-content: flex-end; margin-top: 8px">
                <el-pagination background layout="prev, pager, next, jumper" :total="collectorPage.totalRow" :page-count="collectorPage.totalPage" @current-change="load($event)"/>
            </div>
        </div>
    </div>
    <el-dialog v-model="showAddDialog" destroy-on-close center width="820px" top="5vh" :close-on-click-modal="false">
        <template #title>添加收集器</template>
        <add-or-edit @close="showAddDialog = false; load(1)"/>
    </el-dialog>
    <el-dialog v-model="showEditDialog" destroy-on-close center width="820px" top="5vh">
        <template #title=>编辑收集器</template>
        <add-or-edit :collector="editCollector" @close="showEditDialog = false; load(1)"/>
    </el-dialog>
    <el-dialog v-model="showTestDialog" destroy-on-close center width="720px" top="10vh">
        <template #title=>测试收集器: {{testCollector?.name}}</template>
        <test :collector="testCollector" @close="showTestDialog = false"/>
    </el-dialog>
</template>

<script>
import SvgEdit from "../../../component/svg/SvgEdit.vue";
import SvgDelete from "../../../component/svg/SvgDelete.vue";
import SvgSearch from "../../../component/svg/SvgSearch.vue";
import SvgPlus from "../../../component/svg/SvgPlus.vue";
import DateItem from "../../../component/DateItem.vue";
import AddOrEdit from "./AddOrEdit.vue";
import Test from "./Test.vue";
export default {
    name: "Collector",
    components: {Test, AddOrEdit, DateItem, SvgPlus, SvgSearch, SvgDelete, SvgEdit},
    data() {
        return {
            showAddDialog: false,
            query: { nameLike: null, type: null, },
            showEditDialog: false, editCollector: null,
            showTestDialog: false, testCollector: null,
            collectorPage: {
                page: 1, pageSize: 10, totalPage: 0, totalRow: 0, list: []
            }
        }
    },
    mounted() {
        clone(this.$router.currentRoute.value.query, this.query)
        this.load(1)
    },
    computed: {
        hasAddAuth() {
            return this.$user.privileges.has('Collector_Add')
        },
        hasUpdateAuth() {
            return this.$user.privileges.has('Collector_Update')
        },
        hasDeleteAuth() {
            return this.$user.privileges.has('Collector_Del')
        },
    },
    watch: {
        'query.type'() {
            this.load(1)
        }
    },
    methods: {
        load(page = 1, pageSize = 10) {
            const loading = this.$loading({target: this.$refs.table})
            axios.get(`collector/paging/${page}?limit=${pageSize}&${toQuery(this.query)}`).then(resp => {
                loading.close()
                if (resp.code === '00') {
                    this.collectorPage = resp.data
                    this.collectorPage.list?.map(item => item._statusSwitching = false)
                }
            }).catch(e => loading.close())
        },
        del(collector) {
            this.$confirm(
                    `确认删除收集器: ${collector.name}?`, '警告',
                    {
                        confirmButtonText: '确认',
                        cancelButtonText: '取消',
                        type: 'warning',
                    }
            ).then(() => {
                axios.delete('collector/' + collector.id).then(resp => {
                    if (resp.code === '00') {
                        this.$message.success('删除收集器: ' + collector.name + ' 成功')
                        this.load(1)
                    }
                })
            }).catch(e => {})
        },
        switchStatus(item) {
            item._statusSwitching = true
            axios.post(`collector/${item.id}/enable/${item.enabled}`).then(resp => {
                item._statusSwitching = false
                if (resp.code === '00') {
                  this.$message.success(`${item.enabled ? '启用' : '禁用'}: ${item.name} 成功`);
                } else {
                  item.enabled = !item.enabled
                }
            }).catch(e => {
              item.enabled = !item.enabled
              item._statusSwitching = false
            })
        },
    }
}
</script>

<style scoped>
</style>