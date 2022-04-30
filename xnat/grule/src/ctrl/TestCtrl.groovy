package ctrl

import cn.xnatural.aio.AioClient
import cn.xnatural.aio.AioServer
import cn.xnatural.app.ServerTpl
import cn.xnatural.app.Utils
import cn.xnatural.enet.event.EL
import cn.xnatural.http.*
import cn.xnatural.jpa.Page
import cn.xnatural.jpa.Repo
import core.OkHttpSrv

import entity.Test
import entity.UploadFile
import entity.VersionFile
import service.FileUploader
import service.TestService

import java.text.SimpleDateFormat
import java.util.concurrent.ConcurrentHashMap

import static cn.xnatural.http.ApiResp.fail
import static cn.xnatural.http.ApiResp.ok

@Ctrl(prefix = 'test')
class TestCtrl extends ServerTpl {
    @Lazy def            ts   = bean(TestService)
    @Lazy def            repo = bean(Repo)
    @Lazy def            fu   = bean(FileUploader)
    @Lazy def            http = bean(OkHttpSrv)

    final Set<WebSocket> wss = ConcurrentHashMap.newKeySet()


    @Filter(order = 1)
    void filter1(HttpContext ctx) {
        log.info('test filter1 ============')
    }

    @Filter(order = 0)
    void filter2(HttpContext ctx) {
        // log.info('test filter2 ============')
    }


    @EL(name = 'testWsMsg')
    void wsMsgBroadcast(String msg) {
        wss.each {ws -> ws.send(msg)}
    }


    @WS(path = 'msg')
    void wsMsg(WebSocket ws) {
        log.info('WS connect. {}', ws.session.remoteAddress)
        ws.listen(new WsListener() {

            @Override
            void onClose(WebSocket wst) {
                wss.remove(wst)
            }

            @Override
            void onText(String msg) {
                log.info('test ws receive client msg: {}', msg)
            }
        })
        wsMsgBroadcast('上线: ' + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss").format(new Date()))
        wss.add(ws)
    }


    // 测试抛出错误
    @Path(path = 'error', produce = "application/json")
    def error() {
        throw new Exception('错误测试')
    }


    // dao 测试
    @Path(path = 'dao')
    ApiResp dao(String type) {
        if ('file' == type) {
            return ok(Page.of(
                repo.paging(VersionFile, 1, 10, { root, query, cb -> query.orderBy(cb.desc(root.get('id')))}),
                {UploadFile e ->
                    Utils.copier(e, [:]).ignore("metaClass")
                            .addConverter("createTime", {Date d -> d?.getTime()})
                            .build()
                }
            ))
        } else {
            return ok(ts?.findTestData())
        }
        return

        // 非数字比较大小(时间大小比较)
        repo.row(Test, { root, query, cb -> cb.greaterThanOrEqualTo(root.get("createTime"), new Date())})
    }


    // 接收form 表单提交
    @Path(path = 'form', consumer = 'application/x-www-form-urlencoded')
    ApiResp form(String param1, HttpContext ctx) {
        ok(ctx.request.formParams)
    }


    // json 参数
    @Path(path = 'json', consumer = 'application/json')
    ApiResp json(String param1, HttpContext ctx) {
        ok(ctx.request.jsonParams)
    }


    // 接收post string
    @Path(path = 'string')
    ApiResp string(HttpContext ctx) {
        ok(ctx.request.bodyStr)
    }


    // 文件上传
    @Path(path = 'upload')
    ApiResp upload(FileData file, String version) {
        if (file == null) return ApiResp.fail('文件未上传')
        fu.save(file)
        repo.saveOrUpdate(new VersionFile(version: version, finalName: file.finalName, originName: file.originName, size: file.size))
        ok(fu.toFullUrl(file.finalName))
    }


    // 异步响应(手动ctx.render)
    @Path(path = 'async')
    void async(String p1, HttpContext ctx) {
        ctx.render(
            ok('p1: ' + p1 + ", " + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss").format(new Date()))
        )
    }


    // 测试登录
    @Path(path = 'login')
    ApiResp login(String username, HttpContext hCtx) {
        ok(hCtx.getSessionAttr("uId"))
    }


    // 权限测试
    @Path(path = 'auth')
    ApiResp auth(String auth, HttpContext ctx) {
        ok(ctx.auth(auth?:'auth1'))
    }


    // 远程事件调用
    @Path(path = 'remote')
    void remote(HttpContext ctx, String app, String event, String p1) {
        ts.remote(app?:"gy", event?:'eName1', p1?:'p1',{
            if (it instanceof Exception) ctx.render(fail(it.message))
            else ctx.render ok(it)
        })
    }


    // 远程http调用
    @Path(path = 'http')
    ApiResp http(String url) {
        ok(http.get(url?:'http://gy/test/cus').debug().execute())
    }


    // aio 测试发送
    @Path(path = 'aio')
    ApiResp aio(String host, Integer port, String msg) {
        bean(AioClient).send(
            host?:'localhost', port?:bean(AioServer).port,
            msg?:("send " + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss").format(new Date()))
        )
        ok()
    }


    //故意超时接口
    @Path(path = 'timeout')
    ApiResp timeout(Long wait) {
        Thread.sleep(wait?:1)
        ok().desc("超时: ${wait}ms" )
    }


    // 下载excel文件
    void downXlsx(HttpContext ctx) {
        ctx.response.contentType('application/vnd.ms-excel;charset=utf-8')
        ctx.response.header('Content-Disposition', "attachment;filename=${new SimpleDateFormat("yyyyMMddHHmmss").format(new Date())}.xlsx")
        def wb = org.apache.poi.ss.usermodel.WorkbookFactory.create(true)
        def bos = new ByteArrayOutputStream(); wb.write(bos)
        ctx.render(bos.toByteArray())
    }


    @Path(path = 'cus')
    ApiResp cus(String p1) {
        log.info("here ================")
        return ok('p1: ' + p1 + ", " + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss").format(new Date()))
    }
}
