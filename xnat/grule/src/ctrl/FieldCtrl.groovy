package ctrl

import cn.xnatural.app.Inject
import cn.xnatural.app.ServerTpl
import cn.xnatural.app.Utils
import cn.xnatural.http.ApiResp
import cn.xnatural.http.Ctrl
import cn.xnatural.http.HttpContext
import cn.xnatural.http.Path
import cn.xnatural.jpa.Repo
import com.alibaba.fastjson.JSON
import com.alibaba.fastjson.JSONObject
import entity.Collector
import entity.Field
import entity.FieldType
import service.rule.CollectorManager

import javax.persistence.criteria.Predicate

import static entity.Privilege.*

@Ctrl(prefix = 'field')
class FieldCtrl extends ServerTpl {

    @Inject(name = "jpa_rule_repo") protected Repo repo
    @Inject protected CollectorManager collectorManager
    // 指标字符限制: 字母,中文开头,可包含字数,字母,下划线,中文
    @Lazy protected fieldNamePattern = /^[a-zA-Z\u4E00-\u9FA5]+[0-9a-zA-Z\u4E00-\u9FA5_]*$/


    /**
     * 字段分页查询
     * @param page 第几页. >=1
     * @param limit 最多返回条数
     * @param collectorId 收集器id {@link Collector#id}
     * @param kw 关键词
     */
    @Path(path = 'paging/{page}', method = 'get')
    ApiResp paging(HttpContext hCtx, Integer page, Integer limit, String collectorId, String kw) {
        hCtx.auth(Field_Read.name())
        if (limit && limit > 50) return ApiResp.fail("Param limit <=50")
        ApiResp.ok(repo.paging(Field, page, (limit?:10)) { root, query, cb ->
            query.orderBy(cb.desc(root.get('updateTime')))
            final ps = []
            if (kw) {
                ps << cb.or(
                        cb.like(root.get('key'), '%' + kw + '%'),
                        cb.like(root.get('name'), '%' + kw + '%'),
                        cb.like(root.get('comment'), '%' + kw + '%')
                )
            }
            if (collectorId) {
                ps << cb.function("JSON_SEARCH", String, root.get('collectorOptions'), cb.literal('one'), cb.literal(collectorId)).isNotNull()
            }
            ps ? cb.and(ps.toArray(new Predicate[ps.size()])) : null
        }.to{field -> Utils.copier(field, [:]).ignore("metaClass")
            .addConverter("collectorOptions") {String opts ->
                opts == null ? null : JSON.parseArray(opts).collect {JSONObject jo ->
                    jo.fluentPut(
                            "collectorName",
                            collectorManager.findCollector(jo.getString('collectorId'))?.collector?.name
                    )
                }
            }
            .build()
        })
    }


    /**
     * 添加字段
     * @param key 字段标识
     * @param name 字段名
     * @param type 字段类型 {@link FieldType}
     * @param comment 字段说明
     * @param collectorOptions {@link Field#collectorOptions}
     */
    @Path(path = '/', method = 'put')
    ApiResp<Field> add(HttpContext hCtx, String key, String name, FieldType type, String comment, String collectorOptions) {
        hCtx.auth(Field_Add.name())
        if (!key) return ApiResp.fail("Param key required")
        if (!name) return ApiResp.fail("Param name required")
        if (!type) return ApiResp.fail("Param type required")
        if (!(key ==~ fieldNamePattern)) {
            return ApiResp.fail("Param key illegal: 字母或中文开头,可包含字数,字母,下划线,中文")
        }
        if (!(name ==~ fieldNamePattern)) {
            return ApiResp.fail("Param name illegal: 字母或中文开头,可包含字数,字母,下划线,中文")
        }
        // 验证 collectorOptions
        if (collectorOptions) {
            JSON.parseArray(collectorOptions).each {JSONObject jo ->
                if (!jo['collectorId']) throw new IllegalArgumentException("Param collectorOptions.collectorId required")
                if (!repo.exist(Collector, "id", jo['collectorId'])) {
                    throw new IllegalArgumentException("Param collectorOptions.collectorId not found")
                }
            }
        }
        final field = new Field(
                key: key, name: name, type: type, comment: comment,
                collectorOptions: collectorOptions, creator: hCtx.getSessionAttr('uId')
        )
        try {
            repo.saveOrUpdate(field)
        } catch (ex) {
            def cause = ex
            while (cause != null) {
                if (cause.message.contains("Duplicate entry")) {
                    if (cause.message.contains("name")) {
                        return ApiResp.fail("$name 已存在")
                    }
                    if (cause.message.contains("key")) {
                        return ApiResp.fail("$key 已存在")
                    }
                }
                cause = cause.cause
            }
            throw ex
        }
        ep.fire('field.created', field.key)
        ep.fire('enHistory', field, field.creator)
        ApiResp.ok(field)
    }


    /**
     * 字段更新
     * @param id 字段id {@link Field#id}
     * @param key 字段标识
     * @param name 字段名
     * @param type {@link FieldType}
     * @param comment 字段说明
     */
    @Path(path = '{id}', method = 'post')
    ApiResp<Field> update(HttpContext hCtx, Long id, String key, String name, FieldType type, String comment, String collectorOptions) {
        hCtx.auth(Field_Update.name())
        if (!id) return ApiResp.fail("Param id required")
        if (!key) return ApiResp.fail("Param key required")
        if (!name) return ApiResp.fail("Param name required")
        if (!type) return ApiResp.fail("Param type required")
        if (!(key ==~ fieldNamePattern)) {
            return ApiResp.fail("Param key illegal: 字母或中文开头,可包含字数,字母,下划线,中文")
        }
        if (!(name ==~ fieldNamePattern)) {
            return ApiResp.fail("Param key illegal: 字母或中文开头,可包含字数,字母,下划线,中文")
        }
        final field = repo.byId(Field, id)
        if (field == null) return ApiResp.fail("Param id: $id not found")

        // 验证 collectorOptions
        if (collectorOptions) {
            JSON.parseArray(collectorOptions).each {JSONObject jo ->
                if (!jo['collectorId']) throw new IllegalArgumentException("Param collectorOptions.collectorId required")
                if (!repo.count(Collector, "id", jo['collectorId'])) {
                    throw new IllegalArgumentException("Param collectorOptions.collectorId not found")
                }
            }
        }
        field.key = key
        field.name = name
        field.type = type
        field.comment = comment
        field.collectorOptions = collectorOptions
        field.updater = hCtx.getSessionAttr("uId")
        try {
            repo.saveOrUpdate(field)
        } catch (ex) {
            def cause = ex
            while (cause != null) {
                if (cause.message.contains("Duplicate entry")) {
                    if (cause.message.contains("name")) {
                        return ApiResp.fail("$name aleady exist")
                    }
                    if (cause.message.contains("key")) {
                        return ApiResp.fail("$key aleady exist")
                    }
                }
                cause = cause.cause
            }
            throw ex
        }
        ep.fire('field.update', field.key)
        ep.fire('enHistory', field, field.updater)
        ApiResp.ok(field)
    }


    /**
     * 更新类型
     * @param id 字段id
     * @param type {@link FieldType}
     */
    @Path(path = "{id}/updateType", method = 'post')
    ApiResp updateType(HttpContext hCtx, Long id, FieldType type) {
        hCtx.auth(Field_Update.name())
        if (!id) return ApiResp.fail("Param id required")
        if (!type) return ApiResp.fail("Param type required")
        final field = repo.byId(Field, id)
        if (field == null) return ApiResp.fail("Param id: $id not found")
        field.type = type
        field.updater = hCtx.getSessionAttr("uId")
        repo.saveOrUpdate(field)
        ep.fire('field.update', field.key)
        ep.fire('enHistory', field, field.updater)
        ApiResp.ok()
    }


    /**
     * 删除字段
     * @param id 字段id
     */
    @Path(path = '{id}', method = "delete")
    ApiResp del(HttpContext hCtx, Long id) {
        hCtx.auth(Field_Del.name())
        if (!id) return ApiResp.fail("Param id required")
        final field = repo.byId(Field, id)
        if (!field) return ApiResp.fail("Param id not found")
        repo.delete(field)
        ep.fire('field.delete', field.key)
        ApiResp.ok()
    }
}
