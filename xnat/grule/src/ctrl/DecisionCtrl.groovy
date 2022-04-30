package ctrl

import cn.xnatural.app.Devourer
import cn.xnatural.app.Inject
import cn.xnatural.app.ServerTpl
import cn.xnatural.app.Utils
import cn.xnatural.http.ApiResp
import cn.xnatural.http.Ctrl
import cn.xnatural.http.HttpContext
import cn.xnatural.http.Path
import cn.xnatural.jpa.Page
import cn.xnatural.jpa.Repo
import com.alibaba.fastjson.JSON
import com.alibaba.fastjson.JSONObject
import com.alibaba.fastjson.parser.Feature
import entity.DecideRecord
import entity.Decision
import entity.UserPrivilege
import org.hibernate.query.internal.NativeQueryImpl
import org.hibernate.transform.Transformers
import service.rule.*
import service.rule.spec.DecisionSpec

import javax.persistence.criteria.CriteriaBuilder
import javax.persistence.criteria.CriteriaQuery
import javax.persistence.criteria.Predicate
import javax.persistence.criteria.Root
import java.util.concurrent.atomic.AtomicInteger
import java.util.function.Supplier

import static entity.Privilege.*

/**
 * 决策相关接口
 */
@Ctrl(prefix = 'decision')
class DecisionCtrl extends ServerTpl {

    @Lazy protected repo = bean(Repo, 'jpa_rule_repo')
    @Lazy protected collectorManager = bean(CollectorManager)
    @Lazy protected userSrv = bean(UserSrv)
    @Inject protected DecisionSrv decisionSrv
    @Inject protected DecisionManager decisionManager
    @Inject protected FieldManager fieldManager


    /**
     * 执行一条决策
     * @param id {@link Decision#id}
     */
    @Path(path = '{id}/decide')
    ApiResp<Map> decide(String id, Boolean async, HttpContext ctx) {
        final dCtx = decisionSrv.decide(id, ctx.request.id, async == null ? false : async, ctx.params())
        return ApiResp.ok(dCtx.result())
    }


    /**
     * 查询决策结果
     * @param decideId {@link DecideRecord#id}
     */
    @Path(path = 'decideResult/{decideId}', method = 'get')
    ApiResp<Map> decisionResult(String decideId) {
        if (!decideId) return ApiResp.fail("decideId must required")
        final dr = repo.byId(DecideRecord, decideId)
        if (!dr) return ApiResp.fail("Not found decide record: " + decideId)
        final decisionHolder = decisionManager.findDecision(dr.decision)
        final attrs = dr.data ? JSON.parseObject(dr.data) : [:]
        ApiResp.ok(
                [
                        decideId  : decideId,
                        decisionId: dr.decision,
                        status    : dr.status,
                        result    : dr.result,
                        desc      : dr.exception,
                        data      : decisionHolder.spec.returnAttrs.collectEntries { name ->
                            final field = fieldManager.findField(name)?.field
                            def v = attrs.containsKey(name) ? attrs.get(name) : attrs.get(fieldManager.alias(name))
                            if (v instanceof Optional) {v = v.orElse( null)}
                            //如果key是中文, 则翻译成对应的英文名
                            if (field && field.name == name) return [field.key, v]
                            else return [name, v]
                        }
                ]
        )
    }


    /**
     * 查询决策结果详情
     * @param decideId {@link DecideRecord#id}
     */
    @Path(path = 'decideResultDetail/{decideId}', method = 'get')
    ApiResp<Map> decideResultDetail(HttpContext hCtx, String decideId) {
        if (!decideId) return ApiResp.fail("decideId must required")
        final record = repo.byId(DecideRecord, decideId)
        if (!record) return ApiResp.fail("Not found decide record: " + decideId)
        if (userSrv.findPrivilege(hCtx.getSessionAttr("uId", String), Decision_Read_.name() + record.decision) == null) {
            return ApiResp.of("403", "No Permission")
        }
        ApiResp.ok(
                Utils.copier(record, [:]).ignore("metaClass")
                        .addConverter('data', {String jsonStr ->
                            jsonStr == null ? null : JSON.parseObject(jsonStr, Feature.OrderedField).collect { e ->
                                [key: e.key, name: fieldManager.findField(e.key)?.field?.name, value: e.value]
                            }
                        })
                        .addConverter('input', {String jsonStr ->
                            jsonStr == null ? null : JSON.parseObject(jsonStr)
                        })
                        .addConverter('dataCollectResult', {String jsonStr ->
                            jsonStr == null ? null : JSON.parseObject(jsonStr, Feature.OrderedField).collectEntries { e ->
                                // 格式为: collectorId[_数据key], 把collectorId替换为收集器名字
                                String collectorId
                                final arr = e.key.split("_")
                                if (arr.length == 1) collectorId = e.key
                                else collectorId = arr[0]
                                final collectorName = collectorManager.findCollector(collectorId)?.collector?.name
                                [
                                        (collectorName ? collectorName + (arr.length > 1 ? '_' + arr.drop(1).join('_') : '') : e.key): e.value
                                ]
                            }
                        })
                        .addConverter('detail', {String detailJsonStr ->
                            if (!detailJsonStr) return null
                            final detailJo = JSON.parseObject(detailJsonStr, Feature.OrderedField)
                            // 数据转换
                            detailJo.put('data', detailJo.getJSONObject('data')?.collect { Map.Entry<String, Object> e ->
                                [key: e.key, name: fieldManager.findField(e.key)?.field?.name, value: e.value]
                            }?:null)
                            detailJo.getJSONArray('policies')?.each {JSONObject pJo ->
                                pJo.put('data', pJo.getJSONObject('data')?.collect { Map.Entry<String, Object> e ->
                                    [key: e.key, name: fieldManager.findField(e.key)?.field?.name, value: e.value]
                                }?:null)
                                pJo.getJSONArray('items')?.each {JSONObject rJo ->
                                    rJo.put('data', rJo.getJSONObject('data')?.collect { Map.Entry<String, Object> e ->
                                        [key: e.key, name: fieldManager.findField(e.key)?.field?.name, value: e.value]
                                    }?:null)
                                }
                            }
                            detailJo
                        })
                        .build()
        )
    }


    /**
     * 分页查询决策
     * @param page 第几页. >=1
     * @param limit 最多返回条数
     * @param id 指定查询某个决策 {@link Decision#id}
     * @param dslLike 在决策正文DSL中搜索. 支持空格分割多关键词匹配
     * @param nameLike 按决策名字搜索. 支持空格分割多关键词匹配
     */
    @Path(path = 'paging/{page}')
    ApiResp<Page> paging(HttpContext hCtx, Integer page, Integer limit, String id, String dslLike, String nameLike) {
        if (limit && limit > 20) return ApiResp.fail("Param limit <= 20")
        ApiResp.ok(
                repo.paging(Decision, page, limit?:10) {root, query, cb ->
                    query.orderBy(cb.desc(root.get('updateTime')))
                    final ps = []

                    // 权限过虑: 只能查看自己能查看的决策
                    final subquery = query.subquery(UserPrivilege)
                    final root2 = subquery.from(UserPrivilege)
                    subquery.select(root2.get("resourceId"))
                    subquery.where(cb.and(cb.equal(root2.get("userId"), hCtx.getSessionAttr("uId")), cb.like(root2.get("privilegeId"), Decision_Read_.name() + (id?:'%'))))
                    ps << cb.in(root.get("id")).value(subquery)

                    if (nameLike) {
                        final arr = nameLike.split(" ").findAll {k -> k.trim()}.findAll {it}
                        if (arr) {
                            ps << cb.and(arr.findResults {k -> cb.like(root.get('name'), '%' + k + '%')}.toArray(new Predicate[arr.size()]))
                        }
                    }
                    if (dslLike) {
                        final arr = dslLike.split(" ").findAll {k -> k.trim()}.findAll {it}
                        if (arr) {
                            ps << cb.and(arr.findResults {k -> cb.like(root.get('dsl'), '%' + k + '%')}.toArray(new Predicate[arr.size()]))
                        }
                    }
                    ps ? cb.and(ps.toArray(new Predicate[ps.size()])) : null
                }.to{decision ->
                    Utils.copier(decision, [:]).ignore("metaClass")
                            .add("creatorName", { decision.creator ? userSrv.getUser(decision.creator)?.name : null} as Supplier)
                            .add("updaterName", { decision.updater ? userSrv.getUser(decision.updater)?.name : null} as Supplier)
                            .addConverter("apiConfig", {String s -> s ? JSON.parseArray(s) : null})
                            .build()
                }
        )
    }


    /**
     * 添加一条决策
     * @param dsl 决策DSL
     * @param apiConfig api配置
     */
    @Path(path = '/', method = 'put')
    ApiResp<Map> add(HttpContext hCtx, String dsl, String apiConfig) {
        hCtx.auth(Decision_Add.name())
        if (!dsl) return ApiResp.fail("Param dsl required")
        DecisionSpec spec
        try {
            spec = DecisionSpec.of(dsl)
        } catch (ex) {
            log.error("语法错误", ex)
            return ApiResp.fail('语法错误: ' + ex.message)
        }
        //dsl 验证
        if (!spec.决策名) return ApiResp.fail("决策名 不能为空")
        // TODO 验证子决策是否存在

        final decision = new Decision(
                name: spec.决策名, comment: spec.决策描述, apiConfig: adjustApiConfig(apiConfig),
                dsl: dsl, creator: hCtx.getSessionAttr("uId")
        )

        repo.saveOrUpdate(decision)

        ep.fire("decision.created", decision.id)
        ep.fire('enHistory', decision, decision.creator)
        ApiResp.ok(
                Utils.copier(decision, [:]).ignore("metaClass")
                        .addConverter("apiConfig", {String s -> s ? JSON.parseArray(s) : null})
                        .build()
        ).desc(repo.count(Decision, "name", decision.name) > 1 ? "决策名(${decision.name})重复" : null)
    }


    /**
     * 更新决策DSL
     * @param id 决策id
     * @param dsl 决策DSL内容
     */
    @Path(path = "{id}/updateDsl", method = "post")
    ApiResp updateDsl(HttpContext hCtx, String id, String dsl) {
        hCtx.auth(Decision_Update_.name() + id)
        if (!id) return ApiResp.fail("Param id required")
        if (!dsl) return ApiResp.fail("Param dsl required")
        final decision = repo.byId(Decision, id)
        if (!decision) return ApiResp.fail("Param id not found")

        DecisionSpec spec
        try {
            spec = DecisionSpec.of(dsl)
        } catch (ex) {
            log.error("语法错误", ex)
            return ApiResp.fail('语法错误: ' + ex.message)
        }
        //dsl 验证
        if (!spec.决策名) return ApiResp.fail("决策名 不能为空")
        decision.dsl = dsl
        decision.name = spec.决策名
        decision.comment = spec.决策描述
        decision.updater = hCtx.getSessionAttr('uId')

        repo.saveOrUpdate(decision)

        ep.fire("decision.update", decision.id)
        ep.fire('enHistory', decision, decision.updater)
        ApiResp.ok().attr("name", decision.name).attr("comment", decision.comment)
                .desc(repo.count(Decision, "name", decision.name) > 1 ? "决策名(${decision.name})重复" : null)
    }


    /**
     * 更新决策访问api配置
     * @param id 决策id
     * @param apiConfig api 配置json
     */
    @Path(path = "{id}/updateApiConfig", method = "post")
    ApiResp<Decision> updateApiConfig(HttpContext hCtx, String id, String apiConfig) {
        hCtx.auth(Decision_Update_.name() + id)
        if (!id) return ApiResp.fail("Param id required")
        final decision = repo.byId(Decision, id)
        if (!decision) return ApiResp.fail("Param id not found")

        decision.apiConfig = adjustApiConfig(apiConfig)
        decision.updater = hCtx.getSessionAttr('uId')
        repo.saveOrUpdate(decision)

        ep.fire("decision.update", decision.id)
        ep.fire('enHistory', decision, decision.updater)
        ApiResp.ok()
    }


    /**
     * 删除决策
     * @param id {@link Decision#id}
     */
    @Path(path = '{id}', method = "delete")
    ApiResp del(HttpContext hCtx, String id) {
        if (!id) return ApiResp.fail("Param id required")
        hCtx.auth(Decision_Del_.name() + id)
        final decision = repo.byId(Decision, id)
        if (!decision) return ApiResp.fail("Param id not found")
        repo.delete(decision)

        ep.fire('decision.delete', decision.id)
        ApiResp.ok()
    }


    /**
     * 决策结果详情查看
     * @param page 第几页. >=1
     * @param limit 最多返回条数
     * @param id {@link DecideRecord#id}
     * @param decisionId {@link DecideRecord#decision}
     */
    @Path(path = 'decideResultPaging/{page}')
    ApiResp<Page> decideResultPaging(
            HttpContext hCtx, Integer page, Integer limit, String id, String decisionId, DecideResult result,
            Long spend, String exception, String attrConditions, Long startTime, Long endTime
    ) {
        final myId = hCtx.getSessionAttr("uId", String)
        if (limit && limit > 20) return ApiResp.fail("Param limit <=20")
        Date start = startTime ? new Date(startTime) : null
        Date end = endTime ? new Date(endTime) : null
        final conditionFn = { Root<DecideRecord> root, CriteriaQuery<DecideRecord> query, CriteriaBuilder cb  ->
            final ps = []
            if (id) ps << cb.like(root.get('id'), id + "%")
            if (start) ps << cb.greaterThanOrEqualTo(root.get('occurTime'), start)
            if (end) ps << cb.lessThanOrEqualTo(root.get('occurTime'), end)

            // 权限过虑: 只能查看自己能查看的决策
            final subquery = query.subquery(UserPrivilege)
            final root2 = subquery.from(UserPrivilege)
            subquery.select(root2.get("resourceId"))
            subquery.where(cb.and(cb.equal(root2.get("userId"), myId), cb.like(root2.get("privilegeId"), Decision_Read_.name() + (decisionId?:'%'))))
            ps << cb.in(root.get("decision")).value(subquery)

            if (spend) ps << cb.ge(root.get('spend'), spend)
            if (result) ps << cb.equal(root.get('result'), result)
            if (exception) ps << cb.like(root.get('exception'), '%' + exception + '%')
            final orders = []
            if (attrConditions) { // json查询 暂时只支持mysql5.7+,MariaDB 10.2.3+
                JSON.parseArray(attrConditions).each {JSONObject jo ->
                    final fieldKey = jo.getString('fieldKey')
                    if (!fieldKey) return
                    final field = fieldManager.findField(fieldKey)?.field
                    if (field == null) return

                    final exp = cb.function("JSON_EXTRACT", String, root.get('data'), cb.literal('$.' + field.key))
                    final op = jo.getString('op')
                    if (op == "desc") { //降序
                        orders << cb.desc(exp.as(field.type.clzType))
                        return
                    } else if (op == 'asc') { //升序
                        orders << cb.asc(exp.as(field.type.clzType))
                        return
                    }
                    final value = jo.getString('value')
                    if (value == null || value.empty) return

                    if (op == '=') {
                        ps << cb.equal(exp, value)
                    } else if (op == '>') {
                        ps << cb.gt(exp.as(field.type.clzType), Utils.to(value, field.type.clzType))
                    } else if (op == '<') {
                        ps << cb.lt(exp.as(field.type.clzType), Utils.to(value, field.type.clzType))
                    } else if (op == '>=') {
                        ps << cb.ge(exp.as(field.type.clzType), Utils.to(value, field.type.clzType))
                    } else if (op == '<=') {
                        ps << cb.le(exp.as(field.type.clzType), Utils.to(value, field.type.clzType))
                    } else if (op == 'contains') {
                        ps << cb.like(exp, '%' + value + '%')
                    } else throw new IllegalArgumentException("Param attrCondition op('$op') unknown")
                }
            }
            if (orders) { // 按照data中的属性进行排序
                query.orderBy(orders)
            } else { // 默认时间降序
                query.orderBy(cb.desc(root.get('occurTime')))
            }
            ps ? cb.and(ps.toArray(new Predicate[ps.size()])) : null
        }
        ApiResp.ok(
                repo.paging(DecideRecord, page, limit?:10, { root, query, cb ->
                    query.multiselect(
                            root.get("id"), root.get("decision"), root.get("status"), root.get("result"),
                            root.get("occurTime"), root.get("spend"), root.get("exception")
                    )
                    conditionFn(root, query, cb)
                }, { root, query, cb ->
                    conditionFn(root, query, cb)
                }) .to{record ->
                    Utils.copier(record, [:]).ignore("metaClass")
                            .add('decisionName', {
                                decisionManager.findDecision(record.decision)?.decision?.name
                            } as Supplier)
                            .build()
                }
        )
    }


    /**
     * 统计一段时间的决策结果
     * @param startTime 毫秒时间缀 默认只查当天的
     * @param endTime 毫秒时间缀
     */
    @Path(path = 'countDecideResult', method = 'get')
    ApiResp countDecide(HttpContext hCtx, Long startTime, Long endTime) {
        hCtx.auth(Login.name())
        Date start = startTime ? new Date(startTime) : {
            final cal = Calendar.getInstance()
            cal.set(Calendar.SECOND, 0)
            cal.set(Calendar.MINUTE, 0)
            cal.set(Calendar.HOUR_OF_DAY, 0)
            cal.time
        }()
        Date end = endTime ? new Date(endTime) : new Date()
        hCtx.response.cacheControl(5) // 缓存5秒
        ApiResp.ok(
                repo.trans { ses ->
                    ses.createQuery("""
                        select new map(t1.decision as decisionId, t2.name as decisionName, t1.result as result, count(t1.decision) as total) from DecideRecord t1
                        left join Decision t2 on t2.id=t1.decision
                        where 
                            t1.occurTime>:start and t1.occurTime<=:end and t1.result is not null
                            and t1.decision in (
                                select resourceId from UserPrivilege where userId=:uId and privilegeId like :pIdLike
                            )
                        group by t1.decision, t1.result
                    """.trim())
                            .setParameter("start", start).setParameter("end", end)
                            .setParameter("uId", hCtx.getSessionAttr("uId", String))
                            .setParameter("pIdLike", Decision_Read_.name() + '%').list()
                }
        )
    }


    /**
     * 统计一段时间的规则结果
     * @param startTime 毫秒时间缀 默认只查当天的
     * @param endTime 毫秒时间缀
     * @param limit 最多查询多少记录 > 0 , <= 50
     */
    @Path(path = 'countRuleResult', method = 'get')
    void countRuleResult(HttpContext hCtx, String decisionId, Long startTime, Long endTime, Integer limit) {
        hCtx.auth(Login.name())
        Date start = startTime ? new Date(startTime) : {
            final cal = Calendar.getInstance()
            cal.set(Calendar.SECOND, 0)
            cal.set(Calendar.MINUTE, 0)
            cal.set(Calendar.HOUR_OF_DAY, 0)
            cal.time
        }()
        Date end = endTime ? new Date(endTime) : new Date()
        limit = (!limit || limit > 50) ? 20 : limit
        hCtx.response.cacheControl(5) // 缓存5秒

        // mysql 8.0.4+ or mariaDb 10.6+ 有json_table函数
        final hasJsonTable = {
            final verArr = repo.DBVersion.split("\\.")
            final dialect = repo.dialect
            dialect.containsIgnoreCase("mysql") && verArr[0].toInteger() >= 8 && verArr[2].toInteger() >= 4
        }

        if (hasJsonTable()) {
            final sql = """
                SELECT
                    t1.decision as decisionId, t3.name as decisionName, t2.policyName, t2.ruleName, if (isnull(t2.result), 'Accept', t2.result) as result, count(1) as total
                FROM decide_record t1
                join json_table(JSON_SET(t1.detail, '\$.id', t1.id),
                    '\$' COLUMNS (
                        id varchar(50) path '\$.id',
                        NESTED PATH '\$.policies[*]' COLUMNS (
                            policyName varchar(200) path '\$.attrs."策略名"',
                                NESTED PATH '\$.items[*]' COLUMNS (
                                    ruleName varchar(200) path '\$.attrs."规则名"',
                                    result varchar(20) path '\$.result'
                        )))
                ) t2 on t2.id=t1.id
                left join decision t3 on t1.decision = t3.id
                where
                    t1.occur_time>=:start and t1.occur_time<=:end and t1.detail is not null and t1.result is not null
                    and t1.decision in (
                        select resource_id from user_privilege where user_id=:uId and privilege_id like :pIdLike
                    )
                group by decisionId, policyName, ruleName, result
                order by case t2.result when 'Reject' then 3 when 'Review' then 2 when 'Accept' then 1 when isnull(t2.result) then 1 end desc, total desc, t1.occur_time desc
            """.trim()

            hCtx.render(
                    ApiResp.ok(repo.trans {ses ->
                        ses.createNativeQuery(sql).unwrap(NativeQueryImpl.class).setResultTransformer(Transformers.ALIAS_TO_ENTITY_MAP)
                                .setParameter("start", start).setParameter("end", end)
                                .setParameter("uId", hCtx.getSessionAttr("uId", String))
                                .setParameter("pIdLike", Decision_Read_.name() + (decisionId?:'%'))
                                .setMaxResults(limit).list()
                    })
            )
        }
        else {
            final hql = """
                select new map(t1.decision as decisionId, t2.name as decisionName, t1.detail as detail) from DecideRecord t1
                left join Decision t2 on t2.id=t1.decision
                where 
                    t1.occurTime>=:start and t1.occurTime<=:end and t1.detail is not null and t1.result is not null
                    and t1.decision in (
                        select resourceId from UserPrivilege where userId=:uId and privilegeId like :pIdLike
                    )
                order by t1.occurTime desc
            """.trim()

            final Map<String, AtomicInteger> records = [:]
            final Long totalRow = repo.trans { ses ->
                ses.createQuery("""
                    select count(t1.id) from DecideRecord t1
                    where 
                        t1.occurTime>=:start and t1.occurTime<=:end and t1.detail is not null and t1.result is not null
                        and t1.decision in (
                            select resourceId from UserPrivilege where userId=:uId and privilegeId like :pIdLike
                        )
                """.trim()).setParameter("start", start).setParameter("end", end)
                        .setParameter("uId", hCtx.getSessionAttr("uId"))
                        .setParameter("pIdLike", Decision_Read_.name() + (decisionId?:'%'))
                        .setMaxResults(1).getSingleResult()
            }
            if (!totalRow) {
                hCtx.render(ApiResp.ok())
                return
            }
            // 异步并发查询
            final devourer = new Devourer("countRuleResult", exec()).parallel(getInteger("countRuleResult.parallel", 6))
            final pageSize = 20
            final totalPage = (int) (Math.ceil(totalRow / Double.valueOf(pageSize)))
            final limiter = new AtomicInteger(totalPage)
            // 汇总所有查询并响应数据给接口函数
            final tryEnd = {
                if (limiter.decrementAndGet() <= 0) {
                    // decisionId, decisionName, policyName, ruleName, result, total
                    final Set<String> uniqueKey = new HashSet<>(limit)
                    hCtx.render(
                            ApiResp.ok(
                                    records.findResults {e ->
                                        final arr = e.key.split("\\|\\|")
                                        return [decisionId: arr[0], decisionName: arr[1], policyName: arr[2], ruleName: arr[3], result: arr[4], total: e.value.get()]
                                    }.sort {o1, o2 ->
                                        // 把拒绝多的排前面
                                        if (o1['result'] == "Reject" && o2['result'] == "Reject") return o2['total'] - o1['total']
                                        else if (o1['result'] == "Reject") return -1
                                        else if (o2['result'] == "Reject") return 1
                                        else return 0
                                    }.takeWhile {e ->
                                        uniqueKey.add(e['decisionId'] + e['policyName'] + e['ruleName'])
                                        uniqueKey.size() <= limit
                                    }
                            )
                    )
                }
            }
            // 遍历提交异步查询任务
            for (int page = 0; page < totalPage ; page++) {
                final startPos = page * pageSize
                devourer.offer {
                    repo.trans { ses ->
                        ses.createQuery(hql).setParameter("start", start).setParameter("end", end)
                                .setParameter("uId", hCtx.getSessionAttr("uId"))
                                .setParameter("pIdLike", Decision_Read_.name() + (decisionId?:'%'))
                                .setFirstResult(startPos).setMaxResults(pageSize).list()
                    }.each {Map<String, String> record ->
                        JSON.parseObject(record["detail"])?.getJSONArray("policies")?.each {JSONObject pJo ->
                            pJo.getJSONArray("items")?.findAll {JSONObject rJo -> rJo['attrs']['规则名']}?.findResults { JSONObject rJo ->
                                record['decisionId'] + '||' + record['decisionName'] + '||' + pJo['attrs']['策略名'] +'||' + rJo['attrs']['规则名'] + '||' + (rJo['result']?:DecideResult.Accept)
                            }?.each {k ->
                                def counter = records.get(k)
                                if (counter == null) {
                                    synchronized (records) {
                                        counter = records.get(k)
                                        if (counter == null) {
                                            counter = new AtomicInteger()
                                            records.put(k, counter)
                                        }
                                    }
                                }
                                counter.incrementAndGet()
                            }
                        }
                    }
                    tryEnd()
                }
            }
        }
    }


    // 更新 适配 api配置 参数
    protected String adjustApiConfig(String apiConfig) {
        if (apiConfig) {
            final params = JSON.parseArray(apiConfig)
            for (final itt = params.iterator(); itt.hasNext(); ) {
                JSONObject jo = itt.next()
                if (!jo.getString("code") || !jo.getString("name")) itt.remove()
                for (final ittt = jo.iterator(); ittt.hasNext(); ) {
                    if (ittt.next().key.startsWith('_')) ittt.remove()
                }
            }
            return params ? params.toString() : null
        }
        null
    }
}
