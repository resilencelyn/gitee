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
import entity.Decision
import entity.FieldType
import org.codehaus.groovy.control.CompilerConfiguration
import org.codehaus.groovy.control.customizers.ImportCustomizer
import service.rule.spec.DecisionSpec

import java.text.SimpleDateFormat
import java.time.Duration
import java.util.function.Function
import java.util.regex.Pattern

/**
 * 决策管理器
 */
class DecisionManager extends ServerTpl {
    // 缓存key前缀
    protected static final String PREFIX = 'decisionHolder_'
    @Lazy protected repo = bean(Repo, 'jpa_rule_repo')
    @Lazy protected cache = bean(CacheSrv)


    /**
     * 查找 决策
     * @param decisionId {@link Decision#id}
     * @return {@link DecisionHolder}
     */
    DecisionHolder findDecision(String decisionId) {
        DecisionHolder holder = cache?.get(PREFIX + decisionId)
        if (holder == null) {
            holder = initDecisionHolder(repo.byId(Decision, decisionId))
            if (holder) {
                cache?.set(PREFIX + decisionId, holder, Duration.ofMinutes(getLong("decisionHolderCacheMaxKeep", 12 * 60 * 60 * 1000)))
            }
        }
        holder
    }


    @EL(name = ['decision.delete', 'decision.update', 'decision.created', 'decision.dataVersion'], async = true)
    protected void listenDecisionChange(EC ec, String id) {
        cache?.remove(PREFIX + id)
        final remoter = bean(Remoter)
        if (remoter && ec?.source() != remoter) { // 不是远程触发的事件
            remoter.dataVersion('decision').update(id, (findDecision(id)?.decision?.updateTime?.time)?:System.currentTimeMillis(), null)
        }
    }


    /**
     * 初始化决策
     */
    protected DecisionHolder initDecisionHolder(Decision decision) {
        if (!decision || !decision.dsl) return
        final spec = DecisionSpec.of(decision.dsl)
        final apiConfig = decision.apiConfig ? JSON.parseArray(decision.apiConfig) : null
        Map<String, Function<Object, Boolean>> validFunMap = [:]
        Map<String, Pattern> patternMap = [:]
        final holder = new DecisionHolder(decision: decision, spec: spec, paramValidator: (apiConfig ? {Map<String, Object> params ->
            apiConfig.each { JSONObject paramCfg ->
                String code = paramCfg.getString('code') // 参数code
                def value = params.get(code)
                if (paramCfg.getBoolean('require') && value == null) { // 必传验证
                    throw new IllegalArgumentException("Param '$code' require")
                }
                def fixValue // 固定值
                def enumValues // 枚举值
                Pattern pattern // 正则验证
                Function<Object, Boolean> validFun // 函数验证
                def type = FieldType.valueOf(paramCfg.getString('type')) // 参数类型. FieldType
                if (type == null || !FieldType.enumConstants.find {it == type}) throw new Exception("参数验证类型配置错误. type: $type")

                if (FieldType.Bool == type) { // Boolean 值验证
                    if (value == null) value = paramCfg.getBoolean('defaultValue')
                    if (value != null) {
                        if (!'true'.equalsIgnoreCase(value.toString()) && !'false'.equalsIgnoreCase(value.toString())) {
                            throw new IllegalArgumentException("Param '$code' type is boolean. value is 'true' or 'false'")
                        }
                        value = value instanceof Boolean ? value : Boolean.valueOf(value.toString())
                    }
                    fixValue = paramCfg.getBoolean('fixValue')
                }
                if (FieldType.Int == type) { // 整数验证
                    if (value == null) value = paramCfg.getLong('defaultValue')
                    if (value != null) {
                        try {
                            value = value instanceof Long ? value : Long.valueOf(value)
                        } catch (ex) {
                            throw new IllegalArgumentException("Param '$code' is not a number. value: $value", ex)
                        }
                        def min = paramCfg.getLong('min')
                        if (min != null && value < min) {
                            throw new IllegalArgumentException("Param '$code' must >= $min")
                        }
                        def max = paramCfg.getLong('max')
                        if (max != null && value > max) {
                            throw new IllegalArgumentException("Param '$code' must <= $max")
                        }
                    }
                    fixValue = paramCfg.getLong('fixValue')
                    enumValues = paramCfg.getJSONArray('enumValues')?.findAll{it}
                }
                if (FieldType.Decimal == type) { // 小数验证
                    if (value == null) value = paramCfg.getBigDecimal('defaultValue')
                    if (value != null) {
                        try {
                            value = value instanceof BigDecimal ? value : new BigDecimal(value.toString())
                        } catch (ex) {
                            throw new IllegalArgumentException("Param '$code' is not a decimal. value: $value", ex)
                        }
                        def min = paramCfg.getBigDecimal('min')
                        if (min != null && value < min) {
                            throw new IllegalArgumentException("Param '$code' must >= $min")
                        }
                        def max = paramCfg.getBigDecimal('max')
                        if (max != null && value > max) {
                            throw new IllegalArgumentException("Param '$code' must <= $max")
                        }
                    }
                    fixValue = paramCfg.getBigDecimal('fixValue')
                    enumValues = paramCfg.getJSONArray('enumValues')?.findAll{it}
                }
                if (FieldType.Str == type) { // 字符串验证
                    if (value == null) value = paramCfg.getString('defaultValue')
                    if (value != null) {
                        def maxLength = paramCfg.getInteger('maxLength')
                        if (maxLength != null && value.toString().length() > maxLength) {
                            throw new IllegalArgumentException("Param '$code' length must < $maxLength")
                        }
                        def fixLength = paramCfg.getInteger('fixLength')
                        if (fixLength != null && value.toString().length() != fixLength) {
                            throw new IllegalArgumentException("Param '$code' length must equal $fixLength")
                        }
                    }
                    fixValue = paramCfg.getString('fixValue')
                    enumValues = paramCfg.getJSONArray('enumValues')?.findAll{it}
                    if (paramCfg.getString('regex')) {
                        pattern = patternMap.get(code)
                        if (pattern == null) {
                            pattern = Pattern.compile(paramCfg.getString('regex'))
                            patternMap.put(code, pattern)
                        }
                    }
                    if (paramCfg.getString('validFun')) {
                        validFun = validFunMap.get(code)
                        if (validFun == null) {
                            Binding binding = new Binding()
                            def config = new CompilerConfiguration()
                            def icz = new ImportCustomizer()
                            config.addCompilationCustomizers(icz)
                            icz.addImports(Utils.class.name)
                            validFun = new GroovyShell(Thread.currentThread().contextClassLoader, binding, config).evaluate("${paramCfg.getString('validFun')}") as Function
                            validFunMap.put(code, validFun)
                        }
                    }
                }
                if (FieldType.Time == type) { // 时间验证
                    if (value == null) value = paramCfg.getString('defaultValue')
                    if (value != null) {
                        String format = paramCfg.getString('format')
                        if (!format) throw new IllegalArgumentException("Param '$code' type is Time, format must config")
                        try {
                            // if (value.toString().length() != format.length()) throw new Exception()
                            new SimpleDateFormat(format).parse(value.toString())
                        } catch (ex) {
                            throw new IllegalArgumentException("Param '$code' format error. format: $format, value: $value")
                        }
                    }
//                    def min = paramCfg.getDate('min')
//                    if (min != null && value < min) {
//                        throw new IllegalArgumentException("Param '$code' must >= ${paramCfg.getString('min')}")
//                    }
//                    def max = paramCfg.getDate('max')
//                    if (max != null && value > max) {
//                        throw new IllegalArgumentException("Param '$code' must <= ${paramCfg.getString('max')}")
//                    }
//                    fixValue = paramCfg.getDate('fixValue')
//                    enumValues = paramCfg.getJSONArray('enumValues')?.collect {JSONObject jo -> jo?.getDate('value')}?.findAll{it}
                }

                if (value) {
                    if (fixValue != null) {  // 固定值验证
                        if (value != fixValue) {
                            throw new IllegalArgumentException("Param '$code' value fixed be '$fixValue'")
                        }
                    }
                    if (enumValues) { // 枚举值验证
                        if (!enumValues.find {it == value}) {
                            throw new IllegalArgumentException("Param '$code' enum values: '${enumValues.join(",")}'")
                        }
                    }
                    if (pattern) { // 正则验证
                        if (!pattern.matcher(value.toString()).find()) {
                            throw new IllegalArgumentException("Param '$code' regex not match: '$value'")
                        }
                    }
                    if (validFun) {
                        if (!validFun.apply(value)) {
                            throw new IllegalArgumentException("Param '$code' function valid fail: '$value'")
                        }
                    }
                }

                if (value != null || params.containsKey(code)) params.put(code, value) // 类型矫正
            }
            true
        } : null))
        log.info("初始化决策(${decision.id}): ${decision.name}".toString())
        holder
    }
}
