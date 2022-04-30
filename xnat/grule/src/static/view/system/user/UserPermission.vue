<template>
    <div style="display: flex;">
      <!-- 还可以为用户分配的权限区域 -->
      <div v-if="hasUpdatePrivilege" style="display: flex; flex-direction: column; flex: 10">
        <el-input v-model="pNameLike1" placeholder="可添加权限: 按权限名过虑" style="margin-right: 8px" />
        <el-scrollbar height="400px" style="margin-top: 8px">
          <div v-loading="canGrantPrivilegePage.loading && canGrantPrivilegePage.page === 1">
            <div v-for="item in canGrantPrivilegePage.list" :key="item.privilegeId" style="display: flex">
              <div :class="item._class" style="flex: 10"> {{ item.privilegeName }} </div>
              <div :class="item._class" style="flex: 1; cursor: pointer" title="新增权限" @click="add(item)">
                <el-icon><svg-plus /></el-icon>
              </div>
            </div>
            <div v-if="!canGrantPrivilegePage.noMore && canGrantPrivilegePage.list.length" v-loading="canGrantPrivilegePage.loading && canGrantPrivilegePage.page > 1" style="display: flex; justify-content: center; align-items: center; cursor: pointer; margin-top: 6px;">
              <el-alert @click="loadCanGrantPrivilege(++canGrantPrivilegePage.page)" title="点击加载更多" type="info" center :closable="false"/>
            </div>
            <div v-if="canGrantPrivilegePage.list.length < 1 && !canGrantPrivilegePage.loading" style="display: flex; justify-content: center; align-items: center; height: 398px">
              <el-alert v-if="hasGrantAuth" title="系统管理员可搜索所有可分配的权限" type="warning" center :closable="false"/>
              <el-alert v-else title="无可分配的权限" type="warning" center :closable="false"/>
            </div>
          </div>
        </el-scrollbar>
      </div>

      <div v-if="hasUpdatePrivilege" style="display: flex; justify-content: center; align-items: center; flex: 1; font-size: 1.6em">
        <el-icon><svg-right /></el-icon>
      </div>

      <!-- 用户已有权限区域 -->
      <div style="display: flex; flex-direction: column; flex: 10">
        <div style="display: flex">
          <el-input v-model="pNameLike2" :placeholder="'用户(' + user.name + ')已有权限: 按权限名过虑'" style="margin-right: 8px"></el-input>
<!--          <el-button type="danger">批量删除</el-button>-->
        </div>
        <el-scrollbar height="400px" style="margin-top: 8px">
          <div v-loading="existPrivilegePage.loading && existPrivilegePage.page === 1">
            <div v-for="item in existPrivilegePage.list" :key="item.privilegeId" style="display: flex">
              <div :class="item._class" style="flex: 10">{{ item.privilegeName }}</div>
              <div v-if="hasUpdatePrivilege" :class="item._class" @click="del(item)" style="flex: 1; margin-right: 8px; cursor: pointer; " title="删除权限">
                <el-icon><svg-minus /></el-icon>
              </div>
            </div>
            <div v-if="!existPrivilegePage.noMore && existPrivilegePage.list.length" v-loading="existPrivilegePage.loading && existPrivilegePage.page > 1" style="display: flex; justify-content: center; align-items: center; cursor: pointer; margin-top: 6px;">
              <el-alert @click="loadExistPrivilege(++existPrivilegePage.page)" title="点击加载更多" type="info" center :closable="false"/>
            </div>
            <div v-if="existPrivilegePage.list.length < 1 && !existPrivilegePage.loading" style="display: flex; justify-content: center; align-items: center; height: 398px">
              <el-alert title="无权限" type="warning" center :closable="false"/>
            </div>
          </div>
        </el-scrollbar>
      </div>
    </div>
</template>
<script>
import SvgSearch from "../../../component/svg/SvgSearch.vue";
import SvgDelete from "../../../component/svg/SvgDelete.vue";
import PermissionAutocomplete from "../../component/PermissionAutocomplete.vue";
import SvgRight from "../../../component/svg/SvgRight.vue";
import SvgPlus from "../../../component/svg/SvgPlus.vue";
import SvgMinus from "../../../component/svg/SvgMinus.vue";
export default {
    name: "UserPermission",
    components: {SvgMinus, SvgPlus, SvgRight, PermissionAutocomplete, SvgDelete, SvgSearch},
    props: {
        user: {type: Object, required: true},
    },
    data() {
        return {
          pNameLike1: null,
          pNameLike2: null,
          existPrivilegePage: {
            page: 1, pageSize: 10, totalPage: 0, totalRow: 0, list: [], noMore: false, loading: false,
          },
          canGrantPrivilegePage: {
            page: 1, pageSize: 10, totalPage: 0, totalRow: 0, list: [], noMore: false, loading: false,
          },
        }
    },
    mounted() {
      this.loadExistPrivilege()
      this.loadCanGrantPrivilege()
    },
    computed: {
      hasGrantAuth() {
        return this.$user.privileges.has('Grant')
      },
      hasUpdatePrivilege() {
        return this.$user.privileges.has('Grant') || this.$user.privileges.has( 'User_Grant_' + this.user.id)
      },
    },
    watch: {
      pNameLike1() {
        this.loadCanGrantPrivilege(1)
      },
      // 多个空格组成关键词搜索
      pNameLike2() {
        this.loadExistPrivilege(1)
      }
    },
    methods: {
        add(privilege) {
            axios.put(`user/${this.user.id}/addPrivilege/${privilege.privilegeId}`).then(resp => {
                if (resp.code === '00') {
                  this.$message.success(`给用户(${this.user.name})添加权限(${privilege.privilegeName})成功`)
                  if (this.$user.id === this.user.id && privilege.privilegeId.startsWith('User_Read_')) this.$emit('reload')
                  this.loadCanGrantPrivilege(1)
                  this.loadExistPrivilege(1)
                  this.user.privileges.add(privilege.privilegeId)
                }
            })
        },
        // 加载为用户还可分配的权限
        loadCanGrantPrivilege(page = 1, pageSize = 10) {
          if (this.canGrantPrivilegePage.noMore && page > 1) return
          this.canGrantPrivilegePage.loading = true
          axios.get(`user/${this.$user.id}/privilegePaging/${page}?limit=${pageSize}&${toQuery({nameLike: this.pNameLike1, againstUid: this.user.id})}`).then(resp => {
            this.canGrantPrivilegePage.loading = false
            if (resp.code === '00') {
              // 不同类别的权限区别显示
              resp.data.list.forEach(o => {
                if (o.type === 'Static') o._class = 'static-privilege-item'
                else if (o.type === 'User') o._class = 'dyn-user-privilege-item'
                else if (o.type === 'Decision') o._class = 'dyn-decision-privilege-item'
              })
              if (page > 1) {
                this.canGrantPrivilegePage.page = resp.data.page
                resp.data.list.forEach(item => this.canGrantPrivilegePage.list.push(item))
              } else {
                this.canGrantPrivilegePage = resp.data
              }
              if (resp.data.page === resp.data.totalPage) this.canGrantPrivilegePage.noMore = true
            }
          }).catch(e => this.canGrantPrivilegePage.loading = false)
        },
        // 加载用户已有的权限
        loadExistPrivilege(page = 1, pageSize = 15) {
          if (this.existPrivilegePage.noMore && page > 1) return
          this.existPrivilegePage.loading = true
          axios.get(`user/${this.user.id}/privilegePaging/${page}?limit=${pageSize}&${toQuery({nameLike: this.pNameLike2})}`).then(resp => {
            this.existPrivilegePage.loading = false
            if (resp.code === '00') {
              // 不同类别的权限区别显示
              resp.data.list.forEach(o => {
                if (o.type === 'Static') o._class = 'static-privilege-item'
                else if (o.type === 'User') o._class = 'dyn-user-privilege-item'
                else if (o.type === 'Decision') o._class = 'dyn-decision-privilege-item'
              })
              if (page > 1) {
                this.existPrivilegePage.page = resp.data.page
                resp.data.list.forEach(item => this.existPrivilegePage.list.push(item))
              } else {
                this.existPrivilegePage = resp.data
              }
              if (resp.data.page === resp.data.totalPage) this.existPrivilegePage.noMore = true
            }
          }).catch(() => this.existPrivilegePage.loading = false)
        },
        del(privilege) {
            this.$confirm(
                    `确认删除用户(${this.user.name})的权限(${privilege.privilegeName})?`, '警告',
                    {
                        confirmButtonText: '确认',
                        cancelButtonText: '取消',
                        type: 'warning',
                    }
            ).then(() => {
                axios.delete(`user/${privilege.userId}/delPrivilege/${privilege.privilegeId}`).then(resp => {
                    if (resp.code === '00') {
                      this.$message.success(`删除用户(${this.user.name})权限(${privilege.privilegeName})成功`)
                      if (this.$user.id === this.user.id && privilege.privilegeId.startsWith('User_Read_')) this.$emit('reload')
                      this.loadExistPrivilege(1)
                      this.loadCanGrantPrivilege(1)
                      this.user.privileges.delete(privilege.privilegeId)
                    }
                })
            }).catch(e => {})
        }
    }
}
</script>

<style scoped>
.static-privilege-item {
  display: flex;
  align-items: center;
  justify-content: center;
  height: 40px;
  background: var(--el-color-primary-light-9);
  margin-top: 6px;
  color: var(--el-color-primary);
}
.dyn-user-privilege-item {
  display: flex;
  align-items: center;
  justify-content: center;
  height: 40px;
  background: var(--el-color-primary-light-9);
  margin-top: 6px;
  color: var(--el-color-warning);
}
.dyn-decision-privilege-item {
  display: flex;
  align-items: center;
  justify-content: center;
  height: 40px;
  background: var(--el-color-primary-light-9);
  margin-top: 6px;
  color: var(--el-color-success);
}
</style>