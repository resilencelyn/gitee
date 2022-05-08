package service.rule

import cn.xnatural.app.Inject
import cn.xnatural.app.ServerTpl
import cn.xnatural.enet.event.EL
import cn.xnatural.jpa.Repo
import cn.xnatural.sched.Sched
import com.alibaba.fastjson.JSON
import com.alibaba.fastjson.serializer.SerializerFeature
import core.OkHttpSrv
import entity.CollectRecord
import entity.DecideRecord
import entity.Lock

import java.text.SimpleDateFormat
import java.time.Duration

/**
 * 决策Service
 */
class DecisionSrv extends ServerTpl {
    protected static final String SAVE_RESULT = 'save_result'
    protected static final String DECIDE = 'decide'

    @Inject
    protected OkHttpSrv http
    @Inject(name = 'jpa_rule_repo')
    protected Repo repo
    @Inject
    protected DecisionManager decisionManager
    // 是否异步保存决策结果
    @Lazy
    protected asyncSave = getBoolean("asyncSave", false)


    @EL(name = 'sys.starting', async = true)
    protected void init() {
        queue(DECIDE).parallel(getInteger('decideQueue.parallel', 20))
        // 配置 异步保存决定结果 对列执行器
        if (asyncSave) {
            Long lastWarn // 上次告警时间
            queue(SAVE_RESULT)
                    .failMaxKeep(getInteger(SAVE_RESULT + ".failMaxKeep", 10000))
                    .parallel(getInteger("saveResult.parallel", 5))
                    .errorHandle {ex, me ->
                        if (lastWarn == null || (System.currentTimeMillis() - lastWarn >= Duration.ofSeconds(getLong(SAVE_RESULT + ".warnInterval", 60 * 3L)).toMillis())) {
                            lastWarn = System.currentTimeMillis()
                            log.error("保存决策结果到数据库错误", ex)
                            ep.fire("globalMsg", "保存决策结果到数据库错误: " + (ex.message?:ex.class.simpleName))
                        }
                        // 暂停一会
                        me.suspend(Duration.ofMillis(500 + new Random().nextInt(1000)))
                    }
        }
        bean(Sched)?.cron(getStr("cleanDecideRecordCron", "0 0 1 * * ? ")) {cleanDecideRecord()}
    }


    @EL(name = 'sys.stopping', async = true)
    protected void stop() {
        // 尽量等到 对列中的 数据都持久化完成
        long start = System.currentTimeMillis()
        if (asyncSave) {
            if (queue(SAVE_RESULT).waitingCount > 0) log.warn("等待决策结果数据保存完...")
            while (queue(SAVE_RESULT).waitingCount > 0 && System.currentTimeMillis() - start < 1000 * 60 * 2) {
                Thread.sleep(500)
            }
        }
    }


    /**
     * 执行一条决策
     * @param decisionId 决策id {@link entity.Decision#id}
     * @param decideId 决策执行id, 用于记录是哪次执行
     * @param isAsync 是否异步执行, 不等执行结果
     * @param input 输入
     * @return {@link DecisionContext}
     */
    DecisionContext decide(String decisionId, String decideId, boolean isAsync, Map<String, Object> input) {
        if (!decisionId) {
            throw new IllegalArgumentException('Param decisionId required')
        }
        final holder = decisionManager.findDecision(decisionId)
        if (holder == null) {
            throw new IllegalArgumentException("Not found decision: $decisionId")
        }
        holder.paramValidator?.apply(input) // 参数验证
        isAsync = input.getOrDefault("async", isAsync)
        decideId = decideId?:UUID.randomUUID().toString().replace("-", "")

        log.info("Run decision[${decisionId}(${holder.decision.name})]. id: $decideId, input: $input".toString())
        final dCtx = new DecisionContext(decideId, holder, input, ep)
        // 初始化status: 0001, 结束status: 0000, 错误status: EEEE
        repo.saveOrUpdate(new DecideRecord(
                id: dCtx.id, decision: holder.decision.id, occurTime: dCtx.startup, status: '0001',
                input: JSON.toJSONString(dCtx.input, SerializerFeature.WriteMapNullValue)
        ))

        if (isAsync) queue(DECIDE) { dCtx.start() }
        else dCtx.start()
        return dCtx
    }



    /**
     * 系统全局消息
     * @param msg
     */
    @EL(name = 'globalMsg', async = true)
    void globalMsg(String msg) {
        log.info("系统消息: " + msg)
        ep.fire("wsMsg", 'notify' , msg)
        String url = getStr('ddMsgNotifyUrl', null)
        if (url) {
            http.post(url).jsonBody(JSON.toJSONString([
                    msgtype: "text",
                    text: ["content": "GRULE(${app().profile}): $msg".toString()],
                    at: ["isAtAll": false]
            ])).debug().execute()
        }
    }


    // 决策执行结果监听
    @EL(name = 'decision.end')
    void endDecision(DecisionContext ctx) {
        log.info("end decision: " + JSON.toJSONString(ctx.summary(), SerializerFeature.WriteMapNullValue))

        // 保存决策结果到数据库
        final Runnable doSave = () -> {
            repo.saveOrUpdate(
                    repo.byId(DecideRecord, ctx.id).tap {
                        status = ctx.status
                        exception = ctx.summary()['exception']
                        result = ctx.decideResult
                        spend = ctx.summary()['spend']
                        data = JSON.toJSONString(ctx.summary()['data'], SerializerFeature.WriteMapNullValue)
                        detail = JSON.toJSONString(ctx.summary()['detail'], SerializerFeature.WriteMapNullValue)
                        final dr = ctx.summary()['dataCollectResult']
                        if (dr) dataCollectResult = JSON.toJSONString(dr, SerializerFeature.WriteMapNullValue)
                    }
            )
        }
        if (asyncSave) queue(SAVE_RESULT, doSave)
        else doSave.run()

        // 调用如果传了 callback(回调Url), 则会在决策执行完成后把结果回给此url
        String cbUrl = ctx.input['callback']
        if (cbUrl?.startsWith('http')) {
            async {
                final result = JSON.toJSONString(ctx.result(), SerializerFeature.WriteMapNullValue)
                for (i in 0..< getInteger("callbackMaxTry", 2)) {
                    try {
                        http.post(cbUrl).jsonBody(result).debug().execute()
                        break
                    } catch (ex) {
                        log.error("call back result error", ex)
                    }
                }
            }
        }
    }


    /**
     * 计划清理过期DecideRecord数据
     */
    protected void cleanDecideRecord() {
        final lock = new Lock(name: 'cleanDecideRecord', comment: "清理过期数据")
        // 执行清理逻辑
        final doClean = {
            long cleanTotal = 0

            // DecideRecord 清理函数
            final clean = {DecideRecord dr ->
                int count = repo.trans { session ->
                    // 删除关联的 收集器记录
                    session.createQuery("delete from CollectRecord where decideId=:decideId")
                            .setParameter("decideId", dr.id).executeUpdate()
                    session.createQuery("delete from DecideRecord where id=:id")
                            .setParameter("id", dr.id).executeUpdate()
                }
                cleanTotal += count
                // 持续更新锁时间
                if (cleanTotal % 20 == 0) repo.saveOrUpdate(lock)
                log.info("Deleted expire decideRecord: {}", JSON.toJSONString(dr))
            }

            //保留多少条数据
            final keepCount = getLong("decideRecord.keepCount", 0)
            if (keepCount > 0) {
                for (long total = repo.count(DecideRecord); total > keepCount; total--) {
                    clean(repo.row(DecideRecord) { root, query, cb -> query.orderBy(cb.asc(root.get("occurTime")))})
                    // 为了防止有多个进程在删, 所以每隔10次 重新去统计总, 减少删除误差
                    if (cleanTotal % getInteger("deleteUnit", 10) == 0) {
                        total = repo.count(DecideRecord)
                    }
                }
            }

            //保留多天的数据,如果 配置了decideRecord.keepCount 则不执行此清理
            final keepDay = getInteger("decideRecord.keepDay", 0)
            if (keepDay > 0 && !keepCount) {
                final cal = Calendar.getInstance()
                cal.add(Calendar.DAY_OF_MONTH, -keepDay)
                do {
                    final dr = repo.row(DecideRecord) { root, query, cb -> query.orderBy(cb.asc(root.get("occurTime")))}
                    if (dr.occurTime == null || dr.occurTime < cal.time) clean(dr)
                    else break
                } while (true)
            }
            return cleanTotal
        }
        try {
            repo.saveOrUpdate(lock)
            async {
                try {
                    final total = doClean()
                    ep.fire("globalMsg", "清理过期决策数据结束. 共计: " + total)
                } finally {
                    repo.delete(lock)
                }
                if (getBoolean("optimizeTable", false)) {
                    repo.execute("optimize table " + repo.tbName(DecideRecord))
                    repo.execute("optimize table " + repo.tbName(CollectRecord))
                }
            }
        } catch (ex) {
            def cause = ex
            while (cause != null) {
                if (cause.message.contains("Duplicate entry")) {
                    final exist = repo.byAttr(Lock, "name", lock.name)
                    if (exist) {
                        // 当锁隔一段时间没有被更新 就会删除锁, 防止系统意外down,而锁未来的及删的情况发生
                        if (System.currentTimeMillis() - exist.updateTime.time > Duration.ofMinutes(getLong("lockTimeout", 30)).toMillis()) {
                            repo.delete(exist)
                            cleanDecideRecord() // 删除锁后重试
                            return
                        } else {
                            throw new RuntimeException("清理中... 开始时间: " + new SimpleDateFormat('yyyy-MM-dd HH:mm:ss').format(exist.createTime))
                        }
                    } else {
                        throw new RuntimeException("刚清理完")
                    }
                }
                cause = cause.cause
            }
            repo.delete(lock)
            throw ex
        }
    }
}
