import Vue from 'vue'
import Router from 'vue-router'

Vue.use(Router)

const originalPush = Router.prototype.push
Router.prototype.push = function push(location) {
    return originalPush.call(this, location).catch(err => err)
}

export default new Router(
    {
        mode: 'history',
        base: process.env.BASE_URL,
        routes: [
            { path: '/Shopping',
                name: "Shopping",
                component: () => import("./views/Shopping"),
                meta: {
                    loginRequire: false,
                },
                children: [
                    {
                        path: "byCard",
                        name: "byCard",
                        component: () => import("./views/wechat/byCard"),
                    },
                ],
            },

            {
                path: '/wechat',
                name: "wechat",
                component: () => import("./views/wechat"),
                meta: {
                    loginRequire: true,
                },
                children: [
                    {
                        path: "bind",
                        name: "bind",
                        component: () => import("./views/wechat/bind"),
                    },
                    {
                        path: "index",
                        name: "index",
                        component: () => import("./views/wechat/index"),
                    },
                    {
                        path: "packet",
                        name: "packet",
                        component: () => import("./views/wechat/cardindex/packet"),
                    },
                    {
                        path: "pay_balance",
                        name: "pay_balance",
                        component: () => import("./views/wechat/cardindex/pay_balance"),
                    },
                    {
                        path: "order_info",
                        name: "order_info",
                        component: () => import("./views/wechat/cardindex/order_info"),
                    },
                    {
                        path: "card_auth",
                        name: "card_auth",
                        component: () => import("./views/wechat/cardindex/card_auth"),
                    },
                    {
                        path: "card_index",
                        name: "card_index",
                        component: () => import("./views/wechat/cardindex/card_index"),
                    },
                    {
                        path: "problem",
                        name: "problem",
                        component: () => import("./views/wechat/cardindex/problem"),
                    },
                    {
                        path: "cardReplacement",
                        name: "cardReplacement",
                        component: () => import("./views/wechat/cardReplacement"),
                    },
                    {
                        path: "updpwd",
                        name: "updpwd",
                        component: () => import("./views/wechat/cardindex/updpwd"),
                    },
                ]
            }]
    }
)
