<template>
    <el-space>
        <el-button v-if="hasAddAuth"  type="primary" @click="showAddDialog = true" circle>
            <el-icon><svg-plus/></el-icon>
        </el-button>
        <el-input v-model="query.nameLike"  placeholder="按用户名搜索" @keyup.enter="load(1)">
            <template #append>
                <el-button @click="load(1)" ><el-icon><svg-search/></el-icon></el-button>
            </template>
        </el-input>
    </el-space>
    <div ref="table" style="flex: 1; margin-top: 12px">
        <div style="padding: 5px 0; margin: 12px 10px;" v-for="item in userPage.list" :key="item.id">
            <el-space style="font-size: 16px; font-weight: bold;">
                <span :title="item.id === $user.id ? '当前用户' : ''">{{item.name}}</span>
                <span v-if="item.privileges?.has('Grant')">(系统管理员)</span>
                <span v-if="item.privileges?.has('User_Add')">(用户管理员)</span>
                <span v-if="item.login">上次登录时间: <date-item :time="item.login" /></span>
                <el-icon v-if="$user.privileges?.has('User_ResetPassword_' + item.id)" @click="showResetPassDialog = true; resetPassUser = item" title="重置密码" style="cursor: pointer"><svg-lock /></el-icon>
                <el-icon v-if="$user.privileges?.has('User_Del_' + item.id)" @click="del(item)" title="删除" style="cursor: pointer"><svg-delete /></el-icon>
            </el-space>

            <pre>{{item.comment}}</pre>
            <user-permission :user="item" @reload="load(1)"/>
        </div>
        <div style="display: flex; justify-content: flex-end; margin-top: 8px">
            <el-pagination background layout="prev, pager, next, jumper" :total="userPage.totalRow" :page-count="userPage.totalPage" @current-change="load($event)"/>
        </div>
    </div>
    <el-dialog v-model="showAddDialog" destroy-on-close center width="520px" :close-on-click-modal="false">
        <template #title>添加用户</template>
        <add-or-edit @close="showAddDialog = false; load(1)"/>
    </el-dialog>

    <el-dialog v-model="showResetPassDialog" destroy-on-close center width="520px" :close-on-click-modal="false">
      <template #title>重置密码</template>
      <reset-pass :user="resetPassUser" @close="showResetPassDialog = false; resetPassUser = null;"/>
    </el-dialog>
</template>

<script>
import SvgPlus from "../../../component/svg/SvgPlus.vue";
import SvgSearch from "../../../component/svg/SvgSearch.vue";
import SvgEdit from "../../../component/svg/SvgEdit.vue";
import SvgDelete from "../../../component/svg/SvgDelete.vue";
import SvgLock from "../../../component/svg/SvgLock.vue";
import DateItem from "../../../component/DateItem.vue";
import UserPermission from "./UserPermission.vue";
import AddOrEdit from "./AddOrEdit.vue";
import ResetPass from "./ResetPass.vue";
export default {
    name: "User",
    components: {ResetPass, AddOrEdit, UserPermission, DateItem, SvgLock, SvgDelete, SvgEdit, SvgSearch, SvgPlus},
    data() {
        return {
            showAddDialog: false,
            showUpdateDialog: false, updateUser: null,
            showResetPassDialog: false, resetPassUser: null,
            query: {},
            userPage: {
                page: 1, pageSize: 10, totalPage: 0, totalRow: 0, list: []
            }
        }
    },
    mounted() {
        this.load(1)
    },
    computed: {
        hasAddAuth() {
          return this.$user.privileges.has('User_Add')
        }
    },
    methods: {
        load(page = 1, pageSize = 2) {
            const loading = this.$loading({target: this.$refs.table})
            axios.get(`user/paging/${page}?limit=${pageSize}&${toQuery(this.query)}`).then(resp => {
                loading.close()
                if (resp.code === '00') {
                  resp.data.list.forEach(u => {
                    if (u.id === this.$user.id) u.privileges = this.$user.privileges
                    else this.$initUserPrivilege(u)
                  })
                  this.userPage = resp.data
                }
            }).catch(e => loading.close())
        },
        del(user) {
            this.$confirm(
                    `确认删除用户: ${user.name}?`, '警告',
                    {
                        confirmButtonText: '确认',
                        cancelButtonText: '取消',
                        type: 'warning',
                    }
            ).then(() => {
                axios.delete(`user/${user.id}`).then(resp => {
                    if (resp.code === '00') {
                        this.$message.success('删除用户: ' + user.name + ' 成功')
                        this.load(1)
                    }
                })
            }).catch(e => {})
        },
    }
}
</script>

<style scoped>

</style>