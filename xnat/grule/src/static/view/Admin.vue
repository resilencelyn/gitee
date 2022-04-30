<template>
    <el-container ref="container" style="width: 100%; height: 100vh">
        <transition name="fade">
            <el-aside class="aside" ref="aside" style="display: flex; flex-flow: column" :width="collapsed ? '64px': '170px'">
                <el-image class="user-avatar" :src="require('@/img/logo-cat.png')" fit="cover"></el-image>
                <!-- 左侧1级菜单 -->
                <el-scrollbar style="flex: 1;">
                    <el-menu class="sidebar" :default-active="current1Menu" :collapse="collapsed" @select="active1Menu">
                        <el-menu-item v-for="(item, index) in routes.filter(o => o._privilegeId ? $user.privileges?.has(o._privilegeId) : o._menu)" :key="item.path" :index="item.path">
                            <el-icon v-if="item.meta.svg"><component :is="item.meta.svg"></component></el-icon>
                            <template #title>{{item.meta.title}}</template>
                        </el-menu-item>
                    </el-menu>
                </el-scrollbar>
            </el-aside>
        </transition>

        <el-container style="height: 100%">
            <!-- 头部 -->
            <el-header ref="header" style="--el-header-padding: 0; --el-header-height: auto">
                <div style="display: flex; justify-content: space-between; align-items: center;">
                    <!-- 折叠左侧菜单图标 -->
                    <el-icon style="font-size: 1.8em">
                        <svg-expand v-if="collapsed" @click="() => (collapsed = !collapsed)"/>
                        <svg-fold v-else @click="() => (collapsed = !collapsed)"/>
                    </el-icon>
                    <div style="display: flex; justify-content: center; align-items: center; margin-right: 20px; cursor: pointer;">
                      <el-dropdown :append-to-body="false">
                        <el-avatar :size="40" :src="require('@/img/avatar.jpg')" />
                        <template #dropdown>
                          <el-dropdown-menu>
                            <el-dropdown-item disabled>
                              <span style="font-size: 20px; text-align: center">{{ $user.name }}</span>
                            </el-dropdown-item>
                            <el-dropdown-item divided>
                              <span @click="showChangePwdDialog = true">修改密码</span>
                            </el-dropdown-item>
                            <el-dropdown-item divided>
                              <span @click="logout">退出</span>
                            </el-dropdown-item>
                          </el-dropdown-menu>
                        </template>
                      </el-dropdown>
                    </div>
                </div>

                <el-divider style="margin: .2em 0 0 0"/>
                <!-- 上头2级菜单 -->
                <el-menu v-if="tabs && tabs.length > 0" :default-active="current2Menu" mode="horizontal" @select="active2Menu">
                    <el-menu-item v-for="item in tabs?.filter(o => o._privilegeId ? $user.privileges?.has(o._privilegeId) : o._menu)" :key="item.path" :index="item._parent.path +'/'+ item.path" style="height: 2.8em">
                        <el-icon v-if="item.meta.svg"><component :is="item.meta.svg"></component></el-icon>
                        <template #title>{{item.meta.title}}</template>
                    </el-menu-item>
                </el-menu>
            </el-header>

            <!-- 内容块 -->
            <el-main id="content" style="width: 100%; --el-main-padding: 12px 12px 2px 12px;">
                <el-scrollbar :height="contentHeight">
                    <router-view v-if="$user.id" style="flex: 1"></router-view>
                </el-scrollbar>
            </el-main>
        </el-container>
    </el-container>
    <el-dialog v-model="showChangePwdDialog" destroy-on-close center width="520px" :close-on-click-modal="false">
      <template #title>修改密码</template>
      <change-pass @close="showChangePwdDialog = false;"/>
    </el-dialog>
</template>
<script>
    import SvgHome from "../component/svg/SvgHome.vue";
    import SvgSetting from "../component/svg/SvgSetting.vue";
    import SvgSearch from "../component/svg/SvgSearch.vue";
    import SvgFold from "../component/svg/SvgFold.vue";
    import SvgExpand from "../component/svg/SvgExpand.vue";
    import SvgTrendCharts from "../component/svg/SvgTrendCharts.vue";
    import SvgEdit from "../component/svg/SvgEdit.vue";
    import ChangePass from "./ChangePass.vue";

    export default {
        name: 'Admin',
        components: {ChangePass, SvgEdit, SvgTrendCharts, SvgExpand, SvgFold, SvgSearch, SvgSetting, SvgHome},
        data() {
            return {
                collapsed: false,
                tabs: [],
                contentHeight: '100%',
                current2Menu: null,
                showChangePwdDialog: false,
            };
        },
        created() {
            this.current2Menu = this.getCurrent2Menu()
            this.tabs = this.routes.find(r => r.path === this.current1Menu).children
            if (this.tabs == null || this.tabs.length < 1) this.collapsed = true
        },
        mounted() {
            window.addEventListener('resize',()=>{
              this.contentHeight = (this.$refs.container.$el.clientHeight - this.$refs.header.$el.clientHeight - 12)
            });
        },
        computed: {
            // 需要显示的路由菜单
            routes() {
                return this.$router.options.routes.filter(item => {
                    if (item.children) {
                        item.children = item?.children.map(o => {
                          o._parent = item
                          return o
                        })
                    }
                    return item._menu
                })
            },
            current1Menu() {
                const matched = this.$router.currentRoute.value.matched
                if (matched && matched.length > 0) return matched[0].path
                return this.routes[0].path
            },
        },
        watch: {
            $route(to, from) {
              this.$nextTick(() => this.current2Menu = this.getCurrent2Menu())
              this.tabs = this.routes.find(r => r.path === to.matched[0].path)?.children
            },
        },
        methods: {
            active1Menu(path) {
                this.$router.push(path)
            },
            active2Menu(path) {
                this.$router.push(path)
            },
            getCurrent2Menu() {
                const matched = this.$router.currentRoute.value.matched
                if (matched && matched.length > 1) return matched[1].path
                return this.routes[0].children.length > 0 ? this.routes[0].children[0]?.path : null
            },
            logout() {
              const loading = this.$loading({target: document.body})
              axios.get('logout').then((res) => {
                loading.close()
                if (res.code === '00') {
                  this.$user.id = null
                  this.$router.push(`/login`);
                }
              }).catch(() => loading.close())
            }
        },
    };
</script>
<style>
#content .el-scrollbar__view{
    display: flex; flex-direction: column;
    min-height: 100%;
}
</style>
<style scoped>
.user-avatar{
    border: .2rem solid var(--el-color-warning-lighter);
    box-sizing: border-box;
    margin: .5rem auto;
    border-radius: 50%;
    width: 90%;
}
.sidebar{
    border: none;
    width: 100%;
}
.aside{
    border-right: 1px solid #eee;
    margin-right: 1rem;
    transition: width 300ms;
}
</style>
