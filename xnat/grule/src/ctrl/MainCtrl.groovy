package ctrl

import cn.xnatural.app.ServerTpl
import cn.xnatural.app.Utils
import cn.xnatural.enet.event.EL
import cn.xnatural.http.*
import cn.xnatural.jpa.Page
import cn.xnatural.jpa.Repo
import com.alibaba.fastjson.JSON
import com.alibaba.fastjson.JSONObject
import core.HttpSrv
import entity.*
import service.FileUploader
import service.rule.DecideResult
import service.rule.UserSrv

import javax.persistence.criteria.Predicate
import java.util.concurrent.ConcurrentHashMap
import java.util.function.Supplier

import static entity.Privilege.*

@Ctrl
class MainCtrl extends ServerTpl {

    @Lazy protected fileUploader = bean(FileUploader)
    @Lazy protected userSrv = bean(UserSrv)
    @Lazy protected repo = bean(Repo, 'jpa_rule_repo')
    // 静态文件目录
    @Lazy protected File staticDir = new File(bean(HttpSrv).getAttr("staticDir", String, Utils.baseDir("static").absolutePath))
    protected final Set<WebSocket> wss = ConcurrentHashMap.newKeySet()


    // 全局拦截
    @Filter
    void filter(HttpContext hCtx) {
        // 设置用户权限绑定到请求
        hCtx.setAttr(HttpServer.PRIVILEGES, {
            final uId = hCtx.getSessionAttr('uId', String)
            if (!uId) return null
            return new HashSet(7) {
                @Override
                boolean contains(Object o) { MainCtrl.this.userSrv.findPrivilege(uId, (String) o) }
            }
        } as Supplier)
    }


    /**
     * 登录
     * @param username 用户名
     * @param password 密码
     */
    @Path(path = 'login')
    ApiResp login(HttpContext hCtx, String username, String password) {
        if (!username) return ApiResp.fail('Param username not empty')
        if (!password) return ApiResp.fail('Param password not empty')

        final user = repo.row(User) { root, query, cb -> cb.equal(root.get('name'), username)}
        if (!user) return ApiResp.fail("用户不存在")
        hCtx.setSessionAttr('uId', user.id)
        hCtx.auth(Login.name()) // 验证是否允许登录
        if (password != user.password) return ApiResp.fail('密码错误')
        hCtx.setSessionAttr('uName', user.name)
        user.login = new Date()
        repo.saveOrUpdate(user)
        repo.saveOrUpdate(new UserSession(valid: true, sessionId: hCtx.sessionId, userId: user.id, data: JSON.toJSONString([uId: user.id, uName: user.name])))
        currentUser(hCtx)
    }


    /**
     * 获取当前 会话 中的用户信息
     */
    @Path(path = 'currentUser', method = 'get')
    ApiResp currentUser(HttpContext hCtx) {
        hCtx.auth(Login.name())
        return ApiResp.ok().attr('id', hCtx.getSessionAttr('uId'))
                .attr('name', hCtx.getSessionAttr('uName'))
    }


    /**
     * 退出会话
     */
    @Path(path = 'logout')
    ApiResp logout(HttpContext hCtx) {
        hCtx.removeSessionAttr('uId')
        hCtx.regFinishedFn {
            final session = repo.byId(UserSession, hCtx.sessionId)
            if (session && session.valid) {
                session.valid = false
                repo.saveOrUpdate(session)
            }
        }
        ApiResp.ok()
    }


    /**
     * websocket 向所有客户端发送消息
     */
    @EL(name = 'wsMsg', async = true)
    void wsMsgBroadcast(String type, String msg, Object data) {
        final msgJo = new JSONObject().fluentPut("type", type).fluentPut("msg", msg).fluentPut("data", data)
        wss.each {ws -> ws.send(msgJo.toString())}
    }

    /**
     * websocket 接收客户端的消息
     */
    @WS(path = 'ws')
    void receiveWs(WebSocket ws) {
        log.info('WS connect. {}', ws.session.getRemoteAddress())
        ws.listen(new WsListener() {

            @Override
            void onClose(WebSocket wst) { wss.remove(wst) }

            @Override
            void onText(String msg) {
                log.info('ws receive client msg: {}', msg)
            }
        })
        wss.add(ws)
    }


    /**
     * 操作记录
     * @param page 第几页. >=1
     * @param limit 最多返回条数
     * @param dataId {@link OpHistory#dataId}
     * @param operator {@link OpHistory#operator}
     * @param type 数据类型: {@link OpHistoryType}
     * @param kw 内容关键词, 以空格分割
     */
    @Path(path = 'opHistoryPaging/{page}')
    ApiResp<Page> opHistoryPaging(HttpContext hCtx, Integer page, Integer limit, String dataId, String operator, String kw, OpHistoryType type, Long startTime, Long endTime) {
        hCtx.auth(OpHistory_Read.name())
        if (OpHistoryType.Field == type) hCtx.auth(Field_Read.name())
        if (OpHistoryType.Collector == type) hCtx.auth(Collector_Read.name())
        if (limit && limit > 20) return ApiResp.fail("Param limit <=20")
        Date start = startTime ? new Date(startTime) : null
        Date end = endTime ? new Date(endTime) : null
        final kws = kw ? kw.split(" ").findAll {k -> k.trim()}.findAll {it} : null
        ApiResp.ok(
                repo.paging(OpHistory, page, limit?:10) { root, query, cb ->
                    query.orderBy(cb.desc(root.get('createTime')))
                    final ps = []
                    if (start) ps << cb.greaterThanOrEqualTo(root.get('createTime'), start)
                    if (end) ps << cb.lessThanOrEqualTo(root.get('createTime'), end)
                    if (dataId) ps << cb.equal(root.get('dataId'), dataId)
                    if (operator) ps << cb.equal(root.get('operator'), operator)
                    if (kws) {
                        ps << cb.and(kws.findResults {k -> cb.like(root.get('content'), '%' + k + '%')}.toArray(new Predicate[kws.size()]))
                    }

                    // 权限过虑: 只能查看自己能查看的决策
                    final subquery = query.subquery(UserPrivilege)
                    final root2 = subquery.from(UserPrivilege)
                    subquery.select(root2.get("resourceId"))
                    subquery.where(cb.and(cb.equal(root2.get("userId"), hCtx.getSessionAttr("uId")), cb.like(root2.get("privilegeId"), Decision_Read_.name() + '%')))
                    if (type) {
                        ps << cb.equal(root.get('type'), type)
                        if (OpHistoryType.Decision == type) ps << cb.in(root.get("dataId")).value(subquery)
                    } else {
                        ps << cb.or(cb.equal(root.get("type"), OpHistoryType.Field), cb.equal(root.get("type"), OpHistoryType.Collector), cb.in(root.get("dataId")).value(subquery))
                    }

                    ps ? cb.and(ps.toArray(new Predicate[ps.size()])) : null
                }.to {op ->
                    Utils.copier(op, [:]).ignore("metaClass")
                            .addConverter("content", {String content -> content ? JSON.parseObject(content) : null})
                            .add("creatorName", {userSrv.getUser(op.operator)?.name} as Supplier)
                            .build()
                }
        )
    }


    @Path(path = 'webConfig', produce = "application/json")
    ApiResp<Map<String, Object>> webConfig() {
        ApiResp.ok(
                JSON.parseObject(new File(staticDir, "config.json").getText('utf-8'))
                        .fluentPut("env", app().profile)
                        .fluentPut("decideResultType", DecideResult.values().findResults {[name: it.cnName, key: it.name()]})
                        .fluentPut("fieldType", FieldType.values().findResults {[name: it.cnName, key: it.name()]})
                        .fluentPut("collectorType", CollectorType.values().findResults {[name: it.cnName, key: it.name()]})
                        .fluentPut("opHistoryType", OpHistoryType.values().findResults {[name: it.cnName, key: it.name()]})
        )
    }


    @Path(path = 'health')
    ApiResp health() {
        ApiResp.ok(
                ['status': 'GREEN', 'detail':
                        [
                                'db': ['status': 'UP'],
                        ],
                ]
        )
    }


    @Path(path = ['index.html', '/'])
    File index(HttpContext hCtx) {
        if (app().profile == 'pro') hCtx.response.cacheControl(1800)
        new File(staticDir, "index.html")
    }

    @Path(path = 'test.html')
    File testHtml(HttpContext hCtx) {
        hCtx.response.cacheControl(3)
        new File(staticDir, "test.html")
    }

    @Path(path = 'favicon.ico')
    File favicon(HttpContext hCtx) {
        hCtx.response.cacheControl(60 * 60 * 2)
        hCtx.response.contentType("image/x-icon")
        new File(staticDir, "favicon.ico")
    }


    @Path(path = 'file/{fName}')
    File file(String fName, HttpContext hCtx) {
        if (app().profile == 'pro') hCtx.response.cacheControl(1800)
        fileUploader.findFile(fName)
    }


    // ====================== api-doc =========================

    @Path(path = 'api-doc/{fName}.json', produce = 'application/json')
    String swagger_data(String fName, HttpContext hCtx) {
        final f = new File(Utils.baseDir("../conf"), "${fName}.json")
        f.exists() ? f.getText('utf-8') : null
    }
    @Path(path = 'api-doc/{fName}')
    File swagger_ui(String fName, HttpContext hCtx) {
        hCtx.response.cacheControl(1800)
        new File(staticDir,"swagger-ui/${fName == '/' ? 'index.html' : fName}")
    }

    // ==========================js =====================

    @Path(path = 'js/{fName}')
    File js(String fName, HttpContext hCtx) {
        if (app().profile == 'pro') hCtx.response.cacheControl(1800)
        new File(staticDir,"js/$fName")
    }

    @Path(path = 'js/lib/{fName}')
    File js_lib(String fName, HttpContext hCtx) {
        hCtx.response.cacheControl(86400) // 一天
        new File(staticDir,"js/lib/$fName")
    }


    @Path(path = 'component/{fName}')
    File component(String fName, HttpContext hCtx) {
        if (app().profile == 'pro') hCtx.response.cacheControl(1800)
        new File(staticDir,"component/$fName")
    }

    @Path(path = 'view/{fName}')
    File view(String fName, HttpContext hCtx) {
        if (app().profile == 'pro') hCtx.response.cacheControl(1800)
        new File(staticDir,"view/$fName")
    }
    @Path(path = 'view/data/{fName}')
    File view_data(String fName, HttpContext hCtx) {
        if (app().profile == 'pro') hCtx.response.cacheControl(1800)
        new File(staticDir,"view/data/$fName")
    }
    @Path(path = 'view/policy/collector/{fName}')
    File view_policy_collector(String fName, HttpContext hCtx) {
        hCtx.auth(Collector_Read.name())
        if (app().profile == 'pro') hCtx.response.cacheControl(1800)
        new File(staticDir,"view/policy/collector/$fName")
    }
    @Path(path = 'view/policy/field/{fName}')
    File view_policy_field(String fName, HttpContext hCtx) {
        hCtx.auth(Field_Read.name())
        if (app().profile == 'pro') hCtx.response.cacheControl(1800)
        new File(staticDir,"view/policy/field/$fName")
    }


    // =======================css ========================

    @Path(path = 'css/{fName}')
    File css(String fName, HttpContext hCtx) {
        if (app().profile == 'pro') hCtx.response.cacheControl(1800)
        new File(staticDir,"css/$fName")
    }

    @Path(path = 'css/fonts/{fName}')
    File css_fonts(String fName, HttpContext hCtx) {
        hCtx.response.cacheControl(86400) // 一天
        new File(staticDir,"css/fonts/$fName")
    }

    @Path(path = 'css/lib/{fName}')
    File css_lib(String fName, HttpContext hCtx) {
        hCtx.response.cacheControl(86400) // 一天
        new File(staticDir,"css/lib/$fName")
    }

    // ================= 图片 ======================
    @Path(path = 'img/{fName}')
    File img(String fName, HttpContext hCtx) {
        hCtx.response.cacheControl(172800)
        new File(staticDir,"img/$fName")
    }
}
