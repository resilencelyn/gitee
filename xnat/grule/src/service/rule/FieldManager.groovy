package service.rule

import cn.xnatural.app.CacheSrv
import cn.xnatural.app.ServerTpl
import cn.xnatural.app.Utils
import cn.xnatural.enet.event.EC
import cn.xnatural.enet.event.EL
import cn.xnatural.jpa.Repo
import cn.xnatural.remoter.Remoter
import com.alibaba.fastjson.JSON
import com.alibaba.fastjson.JSONObject
import entity.Collector
import entity.Field
import entity.FieldType
import org.codehaus.groovy.control.CompilerConfiguration
import org.codehaus.groovy.control.customizers.ImportCustomizer
import org.slf4j.LoggerFactory
import service.rule.spec.BaseSpec

import java.time.Duration

/**
 * 属性管理
 * 属性别名
 * 属性值函数
 */
class FieldManager extends ServerTpl {
    protected static final String PREFIX = "fieldHolder_"
    @Lazy protected repo = bean(Repo, 'jpa_rule_repo')
    @Lazy protected collectorManager = bean(CollectorManager)
    @Lazy protected cache = bean(CacheSrv)


    /**
     * 查找字段
     * @param keyOrName {@link Field#key} or {@link Field#name}
     * @return {@link FieldHolder}
     */
    FieldHolder findField(String keyOrName) {
        FieldHolder holder = cache?.get(PREFIX + keyOrName)
        if (holder == null) {
            holder = initField(repo.row(Field) {root, query, cb ->
                cb.or(cb.equal(root.get("key"), keyOrName), cb.equal(root.get("name"), keyOrName))
            })
            if (holder) {
                final expire = Duration.ofMinutes(getLong("fieldHolderCacheMaxKeep", 12 * 60 * 60 * 1000))
                cache?.set(PREFIX + holder.field.key, holder, expire)
                cache?.set(PREFIX + holder.field.name, holder, expire)
            }
        }
        holder
    }


    /**
     * 执行数据收集, 获取属性值
     * @param keyOrName 属性名
     * @param ctx 决策执行上下文
     * @return 当前属性的值
     */
    def dataCollect(String keyOrName, DecisionContext ctx) {
        final fieldHolder = findField(keyOrName)
        if (fieldHolder == null) {
            log.warn(ctx.logPrefix() + "未找到属性'$keyOrName'对应的配置".toString())
            return null
        }
        String collectorId = fieldHolder.choose(ctx) // 属性对应的 值 收集器名
        if (!collectorId) {
            log.warn(ctx.logPrefix() + "属性'" + keyOrName + "'没有对应的取值配置")
            return null
        }

        // 得到收集器
        final collectorHolder = collectorManager.findCollector(collectorId)
        // 未找到收集器
        if (collectorHolder == null) {
            log.warn(ctx.logPrefix() + "Not fund '${keyOrName}' mapped getter function '($collectorId)'".toString())
            return null
        }

        collectorHolder.populate(keyOrName, ctx)
    }


    /**
     * 得到属性对应的别名
     * @param keyOrName {@link Field#key} or {@link Field#name}
     * @return null: 没有别名
     */
    String alias(String keyOrName) {
        final field = findField(keyOrName)?.field
        if (field == null) return null
        else if (field.name == keyOrName) return field.key
        else if (field.key == keyOrName) return field.name
        null
    }


    /**
     * 属性值类型转换
     * @param keyOrName {@link Field#key} or {@link Field#name}
     * @param aValue 属性值
     * @return 转换后的值
     */
    def convert(String keyOrName, Object aValue) {
        if (aValue == null) return aValue
        final field = findField(keyOrName)?.field
        if (field == null) return aValue
        Utils.to(aValue, field.type.clzType)
    }


    // ======================= 监听变化, 并同步到集群其它节点 ==========================
    @EL(name = ['field.created', 'field.update', 'field.delete', 'field.dataVersion'], async = true)
    protected void listenFieldChange(EC ec, String key) {
        FieldHolder holder = cache?.remove(PREFIX + key)
        if (holder) cache?.remove(PREFIX + holder.field.name)
        final remoter = bean(Remoter)
        if (remoter && ec?.source() != remoter) { // 不是远程触发的事件
            remoter.dataVersion('field').update(key,  (findField(key)?.field?.updateTime?.time)?:System.currentTimeMillis(), null)
        }
    }


    /**
     * 初始化默认属性集
     * order: 1f 目的是为了 后于 {@link CollectorManager#initDefaultCollector()}
     */
    @EL(name = 'jpa_rule.started', async = true, order = 1f)
    protected void initDefaultField() {
        if (!repo.exist(Field)) {
            log.info("初始化默认属性集")
            repo.saveOrUpdate(new Field(key: 'idNumber', name: '身份证号码', type: FieldType.Str))
            repo.saveOrUpdate(new Field(key: 'name', name: '姓名', type: FieldType.Str))
            repo.saveOrUpdate(new Field(key: 'mobileNo', name: '手机号码', type: FieldType.Str))
            repo.saveOrUpdate(new Field(key: 'age', name: '年龄', type: FieldType.Int, comment: '根据身份整计算',
                collectorOptions: JSON.toJSONString([[
                        collectorId: repo.byAttr(Collector, "name", "年龄")?.id,
                        chooseFn: 'true'
                ]])))
            repo.saveOrUpdate(new Field(key: 'gender', name: '性别', type: FieldType.Str, comment: '值: F(女),M(男)',
                collectorOptions: JSON.toJSONString([[
                        collectorId: repo.byAttr(Collector, "name", "性别")?.id,
                        chooseFn: 'true'
                ]])))
            repo.saveOrUpdate(new Field(key: 'week', name: '星期几', type: FieldType.Int, comment: '值: 1,2,3,4,5,6,7',
                collectorOptions: JSON.toJSONString([[
                        collectorId: repo.byAttr(Collector, "name", "星期几")?.id,
                        chooseFn: 'true'
                ]])))
            repo.saveOrUpdate(new Field(key: 'currentDateTime', name: '当前日期时间', type: FieldType.Str, comment: '值: yyyy-MM-dd HH:mm:ss',
                collectorOptions: JSON.toJSONString([[
                        collectorId: repo.byAttr(Collector, "name", "当前日期时间")?.id,
                        chooseFn: 'true'
                ]])))
            repo.saveOrUpdate(new Field(key: 'currentDate', name: '当前日期', type: FieldType.Str, comment: '值: yyyy-MM-dd',
                collectorOptions: JSON.toJSONString([[
                        collectorId: repo.byAttr(Collector, "name", "当前日期")?.id,
                        chooseFn: 'true'
                ]])))
            repo.saveOrUpdate(new Field(key: 'currentTime', name: '当前时间', type: FieldType.Str, comment: '值: HH:mm:ss',
                collectorOptions: JSON.toJSONString([[
                        collectorId: repo.byAttr(Collector, "name", "当前时间")?.id,
                        chooseFn: 'true'
                ]])))
        }
    }


    /**
     * 初始化
     * @param {@link Field}
     */
    protected FieldHolder initField(Field field) {
        if (field == null) return
        Binding binding = new Binding()
        final config = new CompilerConfiguration()
        binding.setProperty('LOG', LoggerFactory.getLogger("ROOT"))
        final icz = new ImportCustomizer()
        config.addCompilationCustomizers(icz)
        icz.addImports(JSON.class.name, JSONObject.class.name, Utils.class.name)

        new FieldHolder(field: field, collectorChooseMap: field.collectorOptions ? JSON.parseArray(field.collectorOptions).collectEntries {JSONObject jo ->
            [jo['collectorId'], new GroovyShell(Thread.currentThread().contextClassLoader, binding, config).evaluate("{ -> $jo.chooseFn }")]
        } : null)
    }


    /**
     * 字段/属性/指标 Holder
     */
    class FieldHolder {
        Field field
        LinkedHashMap<String, Closure> collectorChooseMap

        /**
         * 根据当前执行上下文选择出 属性的收集器
         * @param ctx 当前执行上下文
         * @return 收集器id
         */
        String choose(DecisionContext ctx) {
            if (collectorChooseMap == null) return null
            collectorChooseMap.find {e -> BaseSpec.rehydrate2(e.value, ctx.data, this)()}?.key
        }
    }
}