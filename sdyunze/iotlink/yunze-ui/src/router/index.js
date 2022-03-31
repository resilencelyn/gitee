import Vue from 'vue'
import Router from 'vue-router'

Vue.use(Router)

/* Layout */
import Layout from '@/layout'
import ParentView from '@/components/ParentView';

/**
 * Note: 路由配置项
 *
 * hidden: true                   // 当设置 true 的时候该路由不会再侧边栏出现 如401，login等页面，或者如一些编辑页面/edit/1
 * alwaysShow: true               // 当你一个路由下面的 children 声明的路由大于1个时，自动会变成嵌套的模式--如组件页面
 *                                // 只有一个时，会将那个子路由当做根路由显示在侧边栏--如引导页面
 *                                // 若你想不管路由下面的 children 声明的个数都显示你的根路由
 *                                // 你可以设置 alwaysShow: true，这样它就会忽略之前定义的规则，一直显示根路由
 * redirect: noRedirect           // 当设置 noRedirect 的时候该路由在面包屑导航中不可被点击
 * name:'router-name'             // 设定路由的名字，一定要填写不然使用<keep-alive>时会出现各种问题
 * meta : {
    noCache: true                // 如果设置为true，则不会被 <keep-alive> 缓存(默认 false)
    title: 'title'               // 设置该路由在侧边栏和面包屑中展示的名字
    icon: 'svg-name'             // 设置该路由的图标，对应路径src/assets/icons/svg
    breadcrumb: false            // 如果设置为false，则不会在breadcrumb面包屑中显示
  }
 */

// 公共路由
export const constantRoutes = [
  {
    path: '/redirect',
    component: Layout,
    hidden: true,
    children: [
      {
        path: '/redirect/:path(.*)',
        component: (resolve) => require(['@/views/redirect'], resolve)
      }
    ]
  },
  {
    path: '/login',
    component: (resolve) => require(['@/views/login'], resolve),
    hidden: true
  },
  {
    path: '/404',
    component: (resolve) => require(['@/views/error/404'], resolve),
    hidden: true
  },
  {
    path: '/401',
    component: (resolve) => require(['@/views/error/401'], resolve),
    hidden: true
  },
  {
    path: '',
    component: Layout,
    redirect: 'index',
    children: [
      {
        path: 'index',
        component: (resolve) => require(['@/views/index'], resolve),
        name: '首页',
        meta: { title: '首页', icon: 'dashboard', noCache: true, affix: true }
      }
    ]
  },
  {
    path: '/user',
    component: Layout,
    hidden: true,
    redirect: 'noredirect',
    children: [
      {
        path: 'profile',
        component: (resolve) => require(['@/views/system/user/profile/index'], resolve),
        name: 'Profile',
        meta: { title: '个人中心', icon: 'user' }
      }
    ]
  },
  {
    path: '/dict',
    component: Layout,
    hidden: true,
    children: [
      {
        path: 'type/data/:dictId(\\d+)',
        component: (resolve) => require(['@/views/system/dict/data'], resolve),
        name: 'Data',
        meta: { title: '字典数据', icon: '' }
      }
    ]
  },
  {
    path: '/job',
    component: Layout,
    hidden: true,
    children: [
      {
        path: 'log',
        component: (resolve) => require(['@/views/monitor/job/log'], resolve),
        name: 'JobLog',
        meta: { title: '调度日志' }
      }
    ]
  },
  /**
   * polling*/
  {
    path: '/polling',
    component: Layout,
    hidden: true,
    children: [
      {
        path: 'type/index/:cd_id(\\d+)',
        component: (resolve) => require(['@/views/yunze/API/polling/index'], resolve),
        name: 'polling',
        meta: { title: '轮询进度查询', icon: '' }
      }
    ]
  },






  {
    path: '/card',
    component: Layout,
    hidden: true,
    children: [
      {
        path: 'card/:Pstr(.*)',
        component: (resolve) => require(['@/views/yunze/flowcard/card/index'], resolve),
        name: 'Card',
        meta: { title: '物联卡管理', icon: '' }
      },
      {
        path: 'type/cardFlow/:iccid(.*)',
        component: (resolve) => require(['@/views/yunze/flowcard/card/cardFlow'], resolve),
        name: 'CardFlow',
        meta: { title: '订购资费查询', icon: '' }
      },
      {
        path: 'type/cardFlowHis/:iccid(.*)',
        component: (resolve) => require(['@/views/yunze/flowcard/card/cardFlowHis'], resolve),
        name: 'CardFlowHis',
        meta: { title: '用量详情', icon: '' }
      },
      {
        path: 'type/cardChange/:iccid(.*)',
        component: (resolve) => require(['@/views/yunze/flowcard/card/cardChange'], resolve),
        name: 'CardChange',
        meta: { title: '卡信息变更查询', icon: '' }
      },
      {
        path: 'ExecutionTask/:TaskId(.*)',
        component: (resolve) => require(['@/views/yunze/flowcard/executionTask/index'], resolve),
        name: 'executionTask',
        meta: { title: '执行日志', icon: '' }
      },
      {
        path: 'cardSetting',
        component: (resolve) => require(['@/views/yunze/flowcard/cardSetting/index'], resolve),
        name: 'cardSetting',
        meta: { title: '物联卡设置', icon: '' }
      },{
        path: 'diagnosis/:Pstr(.*)',
        component: (resolve) => require(['@/views/yunze/flowcard/diagnosis/index'], resolve),
        name: 'diagnosis',
        meta: { title: '智能诊断', icon: '' }
      },{
        path: 'agentTariffGroup/:Pstr(.*)',
        component: (resolve) => require(['@/views/yunze/flowcard/tariffGroup/agent'], resolve),
        name: 'agentTariffGroup',
        meta: { title: '客户资费', icon: '' }
      },
      {
        path: 'tariffGroupT/:Pstr(.*)',
        component: (resolve) => require(['@/views/yunze/flowcard/tariffGroup/index'], resolve),
        name: 'tariffGroupT',
        meta: { title: '平台资费', icon: '' }
      },
      {
        path: 'rechargeTariff',
        component: (resolve) => require(['@/views/yunze/flowcard/rechargeTariff/index'], resolve),
        name: 'rechargeTariff',
        meta: { title: '资费订购', icon: '' }
      },
    ]
  },




  /**
   * OrderShopping*/
  {
    path: '/order',
    component: Layout,
    hidden: true,
    children: [
      {
        path: 'OrderShopping/:ordNo(.*)',
        component: (resolve) => require(['@/views/yunze/order/OrderShopping/index'], resolve),
        name: 'OrderShopping',
        meta: { title: '购物订单'}
      },
      {
        path: 'ordeAll/:Pstr(.*)',
        component: (resolve) => require(['@/views/yunze/order/ordeAll/index'], resolve),
        name: 'OrderAll',
        meta: { title: '全部订单', icon: '' }
      },
    ]
  },



  {
    path: '/agent',
    component: Layout,
    hidden: true,
    children: [
      {
        path: 'agentUser',
        component: (resolve) => require(['@/views/yunze/user/agentUser/index'], resolve),
        name: 'agentUser',
        meta: { title: '用户信息'}
      },
      {
        path: 'Dept',
        component: (resolve) => require(['@/views/system/dept/index'], resolve),
        name: 'agentUser',
        meta: { title: '企业管理'}
      }
    ]
  },



  {
    path: '/specialOperation',
    component: Layout,
    hidden: true,
    children: [
      {/*** 修改分组及备注 操作 */
        path: 'type/cardInfoEdit',
        component: (resolve) => require(['@/views/yunze/specialOperation/cardInfoEdit'], resolve),
        name: 'cardInfoEdit',
        meta: { title: '变更卡分组备注', icon: '' }
      },
      {/*** 批量停复机、断开网 操作 */
        path: 'type/Selbatch',
        component: (resolve) => require(['@/views/yunze/specialOperation/Selbatch'], resolve),
        name: 'Selbatch',
        meta: { title: '批量停复机、断开网' }
      },
      {/*** 查询IMEI 操作 */
        path: 'type/selImei',
        component: (resolve) => require(['@/views/yunze/specialOperation/selImei'], resolve),
        name: 'selImei',
        meta: { title: '查询IMEI' }

      },
      {/*** 批量取消实名 */
        path: 'type/realname',
        component: (resolve) => require(['@/views/yunze/specialOperation/realname'], resolve),
        name: 'realname',
        meta: { title: '批量取消实名' }
      },
      {/*** 批量更新卡信息 */
        path: 'type/cardInfoReplace',
        component: (resolve) => require(['@/views/yunze/specialOperation/cardInfoReplace'], resolve),
        name: 'cardInfoReplace',
        meta: { title: '批量更新卡信息' }
      }



    ]
  },







  {
    path: '/monitor',
    component: Layout,
    hidden: true,
    children: [
      {
        path: 'online/',
        component: (resolve) => require(['@/views/monitor/online/index'], resolve),
        name: 'Online',
        meta: { title: '在线用户', icon: '' }
      },

    ]
  },



  {
    path: '/log',
    component: Layout,
    hidden: true,
    children: [
      {
        path: 'logininfor/',
        component: (resolve) => require(['@/views/monitor/logininfor/index'], resolve),
        name: 'Logininfor',
        meta: { title: '登录日志', icon: '' }
      },
    ]
  },





/**
 * 自动化配置
 * */
  {
    path: '/ccurl',
    component: Layout,
    hidden: true,
    children: [
      {
        path: 'type/index/:id(.*)',
        component: (resolve) => require(['@/views/yunze/automation/ccurl/index'], resolve),
        name: 'ccurl',
        meta: { title: '自动化配置', icon: ''}
      }
    ]
  },
  /**
   * 推送记录
   * */
  {
    path: '/cchis',
    component: Layout,
    hidden: true,
    children: [
      {
        path: 'type/index/:id(.*)',
        component: (resolve) => require(['@/views/yunze/automation/cchis/index'], resolve),
        name: 'cchis',
        meta: { title: '推送记录', icon: ''}
      }
    ]
  },







  {
    path: '/system',
    component: Layout,
    hidden: true,
    children: [
      {
        path: 'deptPrestore/:Pstr(.*)',
        component: (resolve) => require(['@/views/yunze/system/deptPrestore/index'], resolve),
        name: 'deptPrestore',
        meta: { title: '企业预存', icon: '' }
      },
      {
        path: 'apifile',
        component: (resolve) => require(['@/views/tool/apifile/index'], resolve),
        name: 'apifile',
        meta: { title: 'API使用', icon: ''}
      }
    ]
  },




  {
    path: '/gen',
    component: Layout,
    hidden: true,
    children: [
      {
        path: 'edit/:tableId(\\d+)',
        component: (resolve) => require(['@/views/tool/gen/editTable'], resolve),
        name: 'GenEdit',
        meta: { title: '修改生成配置' }
      }
    ]
  }
]

export default new Router({
  mode: 'history', // 去掉url中的#
  scrollBehavior: () => ({ y: 0 }),
  routes: constantRoutes
})
