package service


import cn.xnatural.app.ServerTpl
import cn.xnatural.app.v.VChain
import cn.xnatural.app.v.VProcessor
import cn.xnatural.enet.event.EL
import cn.xnatural.jpa.Page
import cn.xnatural.jpa.Repo
import cn.xnatural.remoter.Remoter
import cn.xnatural.task.TaskContext
import cn.xnatural.task.TaskWrapper
import com.alibaba.fastjson.JSON
import core.OkHttpSrv
import core.mode.builder.ObjBuilder
import core.mode.pipeline.Pipeline
import entity.Decision
import entity.Test

import java.text.SimpleDateFormat
import java.util.function.Consumer

class TestService extends ServerTpl {
    @Lazy repo = bean(Repo)
    @Lazy http = bean(OkHttpSrv)


    @EL(name = 'sys.heartbeat', async = true)
    void timeNotify() {
        // 向测试 web socket 每分钟发送消息
        ep.fire("testWsMsg", '系统时间: ' + new SimpleDateFormat('yyyy-MM-dd HH:mm:ss').format(new Date()))
    }


    Page findTestData() {
        repo.trans{ s ->
            repo.saveOrUpdate(
                new Test(
                    name: new SimpleDateFormat('yyyy-MM-dd HH:mm:ss').format(new Date()),
                    age: Integer.valueOf(new SimpleDateFormat('ss').format(new Date()))
                )
            )
            repo.paging(Test, 1, 10, { root, query, cb -> query.orderBy(cb.desc(root.get('createTime')))})
        }
    }


    def hibernateTest() {
        repo?.trans{s ->
            println '============='
//            println s.createNativeQuery('select name, age from test where age>10')
//                .setResultTransformer(Transformers.ALIAS_TO_ENTITY_MAP)
//                .setMaxResults(1).uniqueResult()
//            println s.createQuery('select name,age from Test where age>10')
//                // .setResultTransformer(Transformers.ALIAS_TO_ENTITY_MAP)
//                .setMaxResults(1).singleResult['age']
            println s.createQuery('from Test where age>10').setMaxResults(1).list()
        }

        findTestData()
        println "total: " + repo.count(Test)
    }


    def wsClientTest() {
        def i = 1
        http.ws('ws://localhost:7100/test/ws', 90,{msg, ws ->
            log.info("消息" + (i++) + ": " + msg)
        })
    }


    def okHttpTest() {
        def hp = ep.fire('http.hp')
        // hp = '39.104.28.131/gy'
        if (hp) {
            // log.info '接口访问xxx: ' + okHttp.http().get("http://$hp/test/xxx").execute()
            log.info '接口访问dao: ' + http.get("http://$hp/test/dao").cookie('sId', '222').param('type', 'file').execute()
            log.info '接口访问form: ' + http.post("http://$hp/test/form?sss=22").param('p1', '中文').execute()
            log.info '接口访问json: ' + http.post("http://$hp/test/json").contentType('application/json').param('p1', '中文 123').execute()
            log.info '接口访问json: ' + http.post("http://$hp/test/json").jsonBody(JSON.toJSONString([a:'1', b:2])).execute()
            http.post("http://$hp/test/upload?cus=11111").param('p2', 'abc 怎么')
                .param('f1', new File('C:\\Users\\xiangxb\\Desktop\\新建文本文档.txt'))
                .execute({log.info '接口访问upload: ' + it})
        }
    }


    @EL(name = "eName11")
    void taskTest() {
        new TaskContext<>('test ctx', null, exec())
            .addTask(new TaskWrapper().step {param, me -> me.info("执行任务....")})
            .addTask(new TaskWrapper().step {param, me ->
                me.info("执行任务")
            }.step { param, me ->
                me.task().ctx().addTask(new TaskWrapper().step{param1, mee -> mee.info("执行衍生任务....")})
            })
            .start()
    }


    @EL(name = "eName10")
    def testObjBuilder() {
        println ObjBuilder.of(Map).add("a", {"b"}).build()
    }


    @EL(name = "eName9")
    def testPipe() {
        println new Pipeline(key: 'test pipe').add({ i -> i + "xxx"}).run("qqq")
    }


    @EL(name = "eName8")
    def testVChain() {
        new VChain().add(new VProcessor() {
            @Override
            def pre(Object param) {
                log.info('down1')
            }

            @Override
            def post(Object param) {
                log.info('up1')
            }
        }).add(new VProcessor() {
            @Override
            def pre(Object param) {
                log.info('down2')
            }

            @Override
            def post(Object param) {
                log.info('up2')
            }
        }).run()
    }


    def remote(String app, String eName, String param = 'xx', Consumer fn) {
        // 远程调用
        fn.accept(bean(Remoter).fire(app?:'gy', eName?:'eName1', ['p1']))
        // bean(Remoter).fireAsync(app?:'gy', eName?:'eName1', fn, [])
    }


    @EL(name = "testevent.cus")
    def cus(def p) { p }


    @EL(name = "testevent.jpa")
    def jpa() { repo.count(Decision) }


    @EL(name = "testevent.error")
    def error(String p) {
        throw new RuntimeException("抛个错 $p")
    }

    @EL(name = "testevent.timeout")
    void timeout(Long wait) {
        Thread.sleep(wait?:1)
    }
}
