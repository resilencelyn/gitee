package ctrl

import cn.xnatural.app.Inject
import cn.xnatural.app.ServerTpl
import cn.xnatural.app.Utils
import cn.xnatural.http.ApiResp
import cn.xnatural.http.Ctrl
import cn.xnatural.http.HttpContext
import cn.xnatural.http.Path
import cn.xnatural.jpa.Page
import cn.xnatural.jpa.Repo
import entity.CollectRecord
import entity.Collector
import entity.CollectorType
import entity.UserPrivilege
import service.rule.CollectorManager
import service.rule.DecisionManager
import service.rule.UserSrv

import javax.persistence.criteria.CriteriaBuilder
import javax.persistence.criteria.CriteriaQuery
import javax.persistence.criteria.Predicate
import javax.persistence.criteria.Root
import java.util.function.Supplier

import static entity.Privilege.*

@Ctrl(prefix = 'collector')
class CollectorCtrl extends ServerTpl {

    @Inject(name = 'jpa_rule_repo') protected Repo repo
    @Inject protected CollectorManager collectorManager
    @Inject protected DecisionManager decisionManager
    @Lazy protected userSrv = bean(UserSrv)


    /**
     * 分页查询收集器
     * @param page 第几页. >=1
     * @param limit 最多返回条数
     * @param nameLike 查询名字关键词
     * @param id 指定收集器id
     * @param type {@link CollectorType}
     */
    @Path(path = 'paging/{page}', method = 'get')
    ApiResp<Page<Collector>> paging(HttpContext hCtx, Integer page, Integer limit, String nameLike, String id, CollectorType type) {
        hCtx.auth(Collector_Read.name())
        if (limit && limit > 50) return ApiResp.fail("Param limit <=50")
        ApiResp.ok(
            repo.paging(Collector, page, limit?:10) { root, query, cb ->
                query.orderBy(cb.desc(root.get('updateTime')))
                final ps = []
                if (id) ps << cb.equal(root.get("id"), id)
                if (type) ps << cb.equal(root.get('type'), type)
                if (nameLike) {
                    final arr = nameLike.split(" ").findAll {k -> k.trim()}.findAll {it}
                    if (arr) {
                        ps << cb.and(arr.findResults {k -> cb.like(root.get('name'), '%' + k + '%')}.toArray(new Predicate[arr.size()]))
                    }
                }
                ps ? cb.and(ps.toArray(new Predicate[ps.size()])) : null
            }
        )
    }


    @Path(path = '/', method = 'put')
    ApiResp add(
        HttpContext hCtx, String name, CollectorType type, String url, String bodyStr, Boolean recordResult,
        String method, String parseScript, String contentType, String comment, String computeScript, String dataSuccessScript,
        String sqlScript, Integer minIdle, Integer maxActive, Integer timeout, Boolean enabled, String cacheKey, String cacheTimeoutFn
    ) {
        hCtx.auth(Collector_Add.name())
        Collector collector = new Collector(name: name, type: type, comment: comment, enabled: (enabled == null ? true : enabled), recordResult: recordResult == null ? true : recordResult)
        if (!collector.name) return ApiResp.fail('Param name required')
        if (!collector.type) return ApiResp.fail('Param type required')
        if (CollectorType.Http == collector.type) {
            if (!url) return ApiResp.fail('Param url required')
            if (!method) return ApiResp.fail('Param method required')
            if (!contentType && !'get'.equalsIgnoreCase(method)) return ApiResp.fail('Param contentType required')
            if (!url.startsWith("http") && !url.startsWith('${')) return ApiResp.fail('Param url incorrect')
            collector.parseScript = parseScript?.trim()
            if (collector.parseScript && !collector.parseScript.startsWith("{") && !collector.parseScript.endsWith("}")) {
                return ApiResp.fail('Param parseScript a function, must startWith {, endWith }')
            }
            collector.dataSuccessScript = dataSuccessScript?.trim()
            if (collector.dataSuccessScript && !collector.dataSuccessScript.startsWith("{") && !collector.dataSuccessScript.endsWith("}")) {
                return ApiResp.fail('Param dataSuccessScript a function, must startWith {, endWith }')
            }
            collector.url = url
            collector.method = method
            collector.bodyStr = bodyStr
            collector.contentType = contentType
            collector.timeout = timeout
        } else if (CollectorType.Script == collector.type) {
            if (!computeScript) return ApiResp.fail('Param computeScript required')
            collector.computeScript = computeScript
        } else if (CollectorType.Sql == collector.type) {
            if (!url) return ApiResp.fail('Param url required')
            if (!sqlScript) return ApiResp.fail('Param sqlScript required')
            if (!url.startsWith("jdbc")) return ApiResp.fail('url incorrect')
            if (minIdle < 0 || collector.minIdle > 20) return ApiResp.fail('Param minIdle >=0 and <= 20')
            if (maxActive < 1 || collector.maxActive > 50) ApiResp.fail('Param maxActive >=1 and <= 50')
            collector.url = url
            collector.sqlScript = sqlScript
            collector.minIdle = minIdle
            collector.maxActive = maxActive
        } else return ApiResp.fail('Not support type: ' + collector.type)
        collector.creator = hCtx.getSessionAttr("uId", String)
        collector.cacheKey = cacheKey
        collector.cacheTimeoutFn = cacheTimeoutFn
        try {
            repo.saveOrUpdate(collector)
        } catch (ex) {
            def cause = ex
            while (cause != null) {
                if (cause.message.contains("Duplicate entry")) {
                    return ApiResp.fail("$name aleady exist")
                }
                cause = cause.cause
            }
            throw ex
        }
        ep.fire('collector.created', collector.id)
        ep.fire('enHistory', collector, collector.creator)
        ApiResp.ok(collector)
    }


    @Path(path = '{id}', method = 'post')
    ApiResp update(
        HttpContext hCtx, String id, String name, String url, String bodyStr, Boolean recordResult,
        String method, String parseScript, String contentType, String comment, String computeScript, String dataSuccessScript,
        String sqlScript, Integer minIdle, Integer maxActive, Integer timeout, Boolean enabled, String cacheKey, String cacheTimeoutFn
    ) {
        hCtx.auth(Collector_Update.name())
        if (!id) return ApiResp.fail("Param id required")
        if (!name) return ApiResp.fail("Param name required")
        def collector = repo.byId(Collector, id)
        if (collector == null) return ApiResp.fail("Param id: $id not found")
        if (CollectorType.Http == collector.type) {
            if (!url) return ApiResp.fail('Param url required')
            if (!method) return ApiResp.fail('Param method required')
            if ('post'.equalsIgnoreCase(method) && !contentType) {
                return ApiResp.fail('Param contentType required')
            }
            if (!url.startsWith("http") && !url.startsWith('${')) return ApiResp.fail('Param url incorrect')
            collector.parseScript = parseScript?.trim()
            if (collector.parseScript && (!collector.parseScript.startsWith('{') || !collector.parseScript.endsWith('}'))) {
                return ApiResp.fail('Param parseScript is function, must startWith {, endWith }')
            }
            collector.dataSuccessScript = dataSuccessScript?.trim()
            if (collector.dataSuccessScript && !collector.dataSuccessScript.startsWith("{") && !collector.dataSuccessScript.endsWith("}")) {
                return ApiResp.fail('Param dataSuccessScript is function, must startWith {, endWith }')
            }
            collector.url = url
            collector.method = method
            collector.contentType = contentType
            collector.bodyStr = bodyStr
            collector.timeout = timeout
        } else if (CollectorType.Script == collector.type) {
            if (!computeScript) return ApiResp.fail('Param computeScript required')
            collector.computeScript = computeScript?.trim()
            if (collector.computeScript && (collector.computeScript.startsWith('{') || collector.computeScript.endsWith('}'))) {
                return ApiResp.fail('Param computeScript is pure script. cannot startWith { or endWith }')
            }
        } else if (CollectorType.Sql == collector.type) {
            if (!url) return ApiResp.fail('Param url required')
            if (!sqlScript) return ApiResp.fail('Param sqlScript required')
            if (!url.startsWith("jdbc")) return ApiResp.fail('Param url incorrect')
            if (minIdle < 0 || minIdle > 20) return ApiResp.fail('Param minIdle >=0 and <= 20')
            if (maxActive < 1 || maxActive > 50) return ApiResp.fail('Param maxActive >=1 and <= 50')
            collector.url = url
            collector.minIdle = minIdle
            collector.maxActive = maxActive
            collector.sqlScript = sqlScript
        }
        collector.name = name
        collector.comment = comment
        collector.enabled = enabled == null ? true : enabled
        collector.recordResult = recordResult == null ? true : recordResult
        collector.cacheKey = cacheKey
        collector.cacheTimeoutFn = cacheTimeoutFn
        collector.updater = hCtx.getSessionAttr("uId")

        try {
            repo.saveOrUpdate(collector)
        } catch (ex) {
            def cause = ex
            while (cause != null) {
                if (cause.message.contains("Duplicate entry")) {
                    return ApiResp.fail("$name aleady exist")
                }
                cause = cause.cause
            }
            throw ex
        }
        ep.fire('collector.update', collector.id)
        ep.fire('enHistory', collector, collector.updater)
        ApiResp.ok(collector)
    }


    @Path(path = '{id}/enable/{enabled}', method = 'post')
    ApiResp enable(HttpContext hCtx, String id, Boolean enabled) {
        hCtx.auth(Collector_Update.name())
        if (!id) return ApiResp.fail("Param id required")
        if (enabled == null) return ApiResp.fail("Param enabled required")
        final collector = repo.byId(Collector, id)
        if (collector == null) return ApiResp.fail("Param id: $id not found")
        collector.enabled = enabled
        collector.updater = hCtx.getSessionAttr('uId')
        repo.saveOrUpdate(collector)
        ep.fire('collector.update', collector.id)
        ep.fire('enHistory', collector, collector.updater)
        ApiResp.ok(collector)
    }


    @Path(path = '{id}', method = 'delete')
    ApiResp del(HttpContext hCtx, String id) {
        hCtx.auth(Collector_Del.name())
        if (!id) return ApiResp.fail("Param id required")
        final collector = repo.byId(Collector, id)
        if (collector == null) return ApiResp.fail("Param id: $id not found")
        repo.delete(collector)
        ep.fire('collector.delete', id)
        ApiResp.ok()
    }


    @Path(path = '{id}/test')
    ApiResp testCollector(HttpContext hCtx, String id) {
        hCtx.auth(Collector_Read.name())
        if (!id) return ApiResp.fail("Param id required")
        ApiResp.ok(collectorManager.testCollector(id, hCtx.params()))
    }


    /**
     * 获取收集结果详情
     * @param id 收集结果id
     */
    @Path(path = "collectResult/{id}", method = "get")
    ApiResp collectResult(HttpContext hCtx, Long id) {
        if (id == null) return ApiResp.fail("Param id required")
        final record = repo.byId(CollectRecord, id)
        if (record == null) return ApiResp.fail("Not Found")
        if (userSrv.findPrivilege(hCtx.getSessionAttr("uId", String), Decision_Read_.name() + record.decision) == null) {
            return ApiResp.of("403", "No Permission")
        }
        ApiResp.ok(Utils.copier(record, [:]).ignore("metaClass")
                .add("decisionName", {
                    decisionManager.findDecision(record.decision)?.decision?.name
                } as Supplier)
                .add("collectorName", {
                    collectorManager.findCollector(record.collector)?.collector?.name
                } as Supplier)
                .add("collectorType", {
                    collectorManager.findCollector(record.collector)?.collector?.type
                } as Supplier)
                .build())
    }


    /**
     * 收集结果分页查询
     * @param page 第几页. >=1
     * @param limit 最多返回条数
     * @param decideId {@link entity.CollectRecord#decideId}
     * @param decisionId {@link entity.CollectRecord#decision}
     */
    @Path(path = 'collectResultPaging/{page}')
    ApiResp<Page> collectResultPaging(
            HttpContext hCtx, Integer page, Integer limit, String decideId, CollectorType collectorType, String collector, String decisionId,
            Long spend, Boolean success, Boolean dataSuccess, Boolean cache, Long startTime, Long endTime
    ) {
        Date start = startTime ? new Date(startTime) : null
        Date end = endTime ? new Date(endTime) : null
        if (limit && limit > 50) return ApiResp.fail("Param limit <=50")
        final conditionFn = { Root<CollectRecord> root, CriteriaQuery<?> query, CriteriaBuilder cb ->
            final ps = []
            if (start) ps << cb.greaterThanOrEqualTo(root.get('collectDate'), start)
            if (end) ps << cb.lessThanOrEqualTo(root.get('collectDate'), end)
            if (decideId) ps << cb.like(root.get('decideId'), decideId + "%")

            if (decisionId) ps << cb.equal(root.get('decision'), decisionId)

            // 权限过虑: 只能查看自己能查看的决策
            final subquery = query.subquery(UserPrivilege)
            final root2 = subquery.from(UserPrivilege)
            subquery.select(root2.get("resourceId"))
            subquery.where(cb.and(cb.equal(root2.get("userId"), hCtx.getSessionAttr("uId")), cb.like(root2.get("privilegeId"), Decision_Read_.name() + '%')))
            ps << cb.in(root.get("decision")).value(subquery)

            if (collector) ps << cb.equal(root.get('collector'), collector)
            if (collectorType) {
                final subquery2 = query.subquery(String)
                final root22 = subquery2.from(Collector)
                subquery.select(root22.get("id"))
                subquery.where(cb.equal(root22.get("type"), collectorType))
                ps << cb.in(root.get("collector")).value(subquery2)
            }
            if (spend) ps << cb.ge(root.get('spend'), spend)
            if (success != null) {
                if (success) {
                    ps << cb.equal(root.get('status'), '0000')
                } else {
                    ps << cb.notEqual(root.get('status'), '0000')
                }
            }
            if (dataSuccess != null) {
                if (dataSuccess) {
                    ps << cb.equal(root.get('dataStatus'), '0000')
                } else {
                    ps << cb.notEqual(root.get('dataStatus'), '0000')
                }
            }
            if (cache != null) {
                if (cache) {
                    ps << cb.equal(root.get('cache'), true)
                } else {
                    ps << cb.notEqual(root.get('cache'), true)
                }
            }
            ps ? cb.and(ps.toArray(new Predicate[ps.size()])) : null
        }
        ApiResp.ok(
                repo.paging(CollectRecord, page, limit?:10, { root, query, cb ->
                    query.multiselect(
                            root.get("id"), root.get("decideId"), root.get("decision"), root.get("collector"), root.get("status"),
                            root.get("dataStatus"), root.get("collectDate"), root.get("spend"), root.get("cache")
                    )
                    query.orderBy(cb.desc(root.get('collectDate')))
                    conditionFn(root, query, cb)
                }, {root, query, cb ->
                    conditionFn(root, query, cb)}
                ).to{record -> Utils.copier(record, [:]).ignore("metaClass")
                        .add("decisionName", {
                            decisionManager.findDecision(record.decision)?.decision?.name
                        } as Supplier)
                        .add("collectorName", {
                            collectorManager.findCollector(record.collector)?.collector?.name
                        } as Supplier)
                        .add("collectorType", {
                            collectorManager.findCollector(record.collector)?.collector?.type
                        } as Supplier)
                        .build()
                }
        )
    }
}
