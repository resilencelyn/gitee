<template>
    <div style="display: flex; flex-flow: column;">
        <div style="display: flex">
            <el-button v-if="hasAddAuth" type="primary" @click="create" circle>
                <el-icon><svg-plus/></el-icon>
            </el-button>
            <el-input v-model="query.nameLike" placeholder="决策名" @keyup.enter="load(1)" style="width: 280px; margin-left: 10px">
                <template #append>
                    <el-button @click="load(1)"><el-icon class="el-icon--right"><svg-search/></el-icon></el-button>
                </template>
            </el-input>
        </div>
        <div ref="table" style="flex: 1; margin-top: 20px">
            <div v-if="decisionPage && decisionPage.list && decisionPage.list.length > 0">
                <el-collapse v-model="activeId" accordion>
                    <el-collapse-item v-for="(item, index) in decisionPage.list" :key="item._id" :name="item._id" >
                        <template #title>
                            <div style="display: flex; justify-content: space-between; width: 100%">
                                <div>
                                    <span v-if="!item.id" style="color: red">*</span>
                                    <el-space>
                                        <span>{{item.name}} </span>
                                        <date-item :time="item.updateTime" title="更新时间"></date-item>
                                        <span title="创建人">{{item.creatorName}}</span>
                                        <span title="更新人">{{item.updaterName}}</span>
                                    </el-space>
                                </div>
                                <el-space>
                                    <el-button v-if="item.id ? $user.privileges.has('Decision_Update_' + item.id) : true" type="primary" round size="mini" @click.stop="showApiConfigDialog = true; apiConfigDecision = item">API配置</el-button>
                                    <el-button type="warning" round size="mini" @click.stop="showTestDialog = true; testDecision = item" :disabled="item.id == null" >API测试</el-button>
                                    <el-button v-if="item.id ? $user.privileges.has('Decision_Del_' + item.id) : true" type="danger" round size="mini" @click.stop="del(item)">删除</el-button>
                                </el-space>
                            </div>
                        </template>
                        <ace-groovy v-if="activeId === item._id" v-model="item.dsl" :readonly.sync="item.id ? !($user.privileges.has('Decision_Update_' + item.id)) : false" style="height: 680px" @save="save(item)"/>
                    </el-collapse-item>
                </el-collapse>
                <div style="display: flex; justify-content: flex-end; margin-top: 8px">
                    <el-pagination background layout="prev, pager, next, jumper" :total="decisionPage.totalRow" :page-count="decisionPage.totalPage" @current-change="load($event)"/>
                </div>
            </div>
            <el-empty v-else description="无数据"></el-empty>
        </div>
    </div>
    <el-dialog v-model="showTestDialog" destroy-on-close center width="760px">
        <template #title>API测试: {{testDecision?.name}}</template>
        <test :decision="testDecision"/>
    </el-dialog>
    <el-dialog v-model="showApiConfigDialog" destroy-on-close center width="800px" top="8vh">
        <template #title>API配置: {{apiConfigDecision?.name}}</template>
        <api-config :decision="apiConfigDecision"/>
    </el-dialog>
</template>

<script>
import DateItem from "../../../component/DateItem.vue";
import AceGroovy from "../../../component/AceGroovy.vue";
import SvgSearch from "../../../component/svg/SvgSearch.vue";
import dayjs from '../../../js/lib/dayjs.min.js'
import Test from "./Test.vue";
import ApiConfig from "./ApiConfig.vue";
import SvgPlus from "../../../component/svg/SvgPlus.vue";
export default {
    name: "Decision",
    components: {SvgPlus, ApiConfig, Test, SvgSearch, AceGroovy, DateItem},
    data() {
        return {
            activeId: null,
            query: {nameLike: null, id: null},
            decisionPage: {
                page: 1, pageSize: 10, totalPage: 0, totalRow: 0, list: []
            },
            showTestDialog: false, testDecision: null,
            showApiConfigDialog: false, apiConfigDecision: null,
        }
    },
    mounted() {
        clone(this.$router.currentRoute.value.query, this.query)
        this.load(1)
    },
    watch: {
        '$route.query'(v) {
            this.query = {}
            clone(v, this.query)
            this.load(1)
        }
    },
    computed: {
      hasAddAuth() {
        return this.$user.privileges.has('Decision_Add')
      },
    },
    methods: {
        save(decision) {
            if (decision.id) this.updateDsl(decision)
            else { // add new
                this.$message.info(`添加新决策: ${decision.name}`)
                axios.put('decision/', decision).then(resp => {
                    if (resp.code === '00') {
                        clone(resp.data, decision)
                        this.activeId = [decision._id, decision.id]
                        decision._id = decision.id
                        this.activeId = decision.id
                        this.$message.success(`添加新决策: ${decision.name} 成功`)
                    }
                })
            }
        },
        updateDsl(decision) {
            this.$message.info(`更新决策: ${decision.name}`)
            axios.post(`decision/${decision.id}/updateDsl`, {dsl: decision.dsl}).then(resp => {
                if (resp.code === '00') {
                  clone(resp.data, decision)
                  decision.updaterName = this.$user.name
                  this.$message.success(`更新决策: ${decision.name} 成功`)
                }
            })
        },
        del(decision) {
            if (decision.id) {
                this.$confirm(
                    `确认删除决策: ${decision.name}?`, '警告',
                    {
                        confirmButtonText: '确认',
                        cancelButtonText: '取消',
                        type: 'warning',
                    }
                ).then(() => {
                    this.$message.info('开始删除决策: ' + decision.name)
                    axios.delete(`decision/${decision.id}`).then(resp => {
                        if (resp.code === '00') {
                          this.$message.success('删除决策: ' + decision.name + ' 成功')
                          this.load(1)
                          //this.decisionPage.list.splice(this.decisionPage.list.indexOf(decision), 1);
                          localStorage.removeItem('rule.test.' + decision.id)
                        }
                    })
                }).catch(e => {})
            } else {
                this.decisionPage.list.splice(this.decisionPage.list.indexOf(decision), 1);
            }
        },
        load(page = 1, pageSize = 10) {
            const loading = this.$loading({target: this.$refs.table})
            axios.get(`decision/paging/${page}?limit=${pageSize}&${toQuery(this.query)}`).then(resp => {
                loading.close()
                if (resp.code === '00') {
                  resp.data.list.map(o => o._id = o.id)
                  this.decisionPage = resp.data
                }
            }).catch(e => loading.close())
        },
        create() {
            const _id = '决策_' + dayjs().format('YYYYMMDD-HHmmssSSS');
            this.decisionPage.list.unshift({
                _id: _id,
                name: _id,
                apiConfig: [
                    {
                        "code": "async", "name": "是否异步", "type": "Bool", "require": false, "defaultValue": 'false'
                    },
                    {
                        "code": "callback", "name": "回调地址", "type": "Str", "require": false,
                    },
                ],
                dsl:
`// 保存: Ctrl+s, 查找: Ctrl+f
决策名 = '${_id}'
决策描述 = ''

// 返回的调用决策方的结果属性值
// 返回属性 '身份证号码'

策略 {
    策略名 = '策略1'

    // 执行此策略的条件, false: 不执行, true 或者 不配置默认 执行此策略
    // 条件 { true }
    // 操作 { 策略预操作 }

    规则 {
        规则名 = '测试'

        拒绝 {
            年龄 > 50
        }
    }
}`
            })
            this.activeId = _id
        }
    }
}
</script>

<style scoped>

</style>