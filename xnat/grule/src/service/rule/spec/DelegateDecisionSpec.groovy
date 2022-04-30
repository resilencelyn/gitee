package service.rule.spec

import service.rule.DecideResult
import service.rule.DecisionContext
import service.rule.DecisionSrv

import java.util.concurrent.atomic.AtomicInteger

/**
 * 用于 策略中 {@link PolicySpec} 的子决策调用
 */
class DelegateDecisionSpec extends BaseSpec {
    /**
     * 要调用的 决策id
     * {@link entity.Decision#id}
     */
    String 决策id
    /**
     * 是否异步执行
     * 默认同步
     */
    boolean 异步


    /**
     * 要传给决策的参数
     * 决策 {
     *     决策id = 'test1'
     *
     *     // 左(子DecisionContext) = 右(父DecisionContext)
     *     传参 {
     *          p1 = 1
     *          p2 = p2
     *          p3 = 'aa'
     *      }
     * }
     * @param 参数函数
     */
    void 传参(Closure<Void> 参数函数) {
        nodes << Tuple.tuple('Input', { DecisionContext ctx ->
            def input = new LinkedHashMap<String, Object>() {
                @Override
                Object get(Object key) { ctx.data.get(key) }
            }
            def cl = 参数函数.rehydrate(input, 参数函数, this)
            cl.resolveStrategy = Closure.DELEGATE_FIRST
            cl()
            return input
        })
    }


    /**
     * 把参数输出到当前决策执行的上下文
     * 会覆盖子决策返回配置
     * 决策 {
     *     决策id = 'test1'
     *
     *     // 左(父DecisionContext) = 右(子DecisionContext)
     *     输出 {
     *          p1 = 1
     *          p2 = p2
     *          p3 = 'aa'
     *      }
     * }
     *
     * 1. 没有输出配置默认会把子决策的返回属性当输出
     * 2. 配置了就不会输出子决策中的返回属性了
     * @param 输出函数
     */
    void 输出(Closure<Void> 输出函数) {
        nodes << Tuple.tuple('Output', { DecisionContext ctx ->
            def output = new LinkedHashMap<String, Object>() {
                @Override
                Object get(Object key) { ctx.data.get(key) }
            }
            def cl = 输出函数.rehydrate(output, 输出函数, this)
            cl.resolveStrategy = Closure.DELEGATE_FIRST
            cl()
            return output
        })
    }


    /**
     * 计算子决策的结果
     * @param ctx {@link DecisionContext}
     * @return {@link DecideResult}
     */
    DecideResult compute(DecisionContext ctx) {
        Map<String, Object> input = [:]
        nodes.each { node ->
            if ("Input" == node.v1) {
                input.putAll(node.v2.call(ctx))
            } else if ("Output" == node.v1) {

            } else throw new IllegalArgumentException("Unknown type: " + node.v1)
        }
        input.put("decisionId", 决策id) // 在字段选择收集器 可能会用到
        final times = ctx.delegateDecisionCount.computeIfAbsent(决策id, { new AtomicInteger()})
        DecisionContext ctx2 = ctx.decisionSrv.decide(决策id, ctx.id + "_" + times.getAndIncrement(), 异步, input)
        if (异步) return null
        Map<String, Object> output
        nodes.each {node ->
            if ("Output" == node.v1) {
                if (output == null) output = [:]
                output.putAll(node.v2.call(ctx2))
            }
        }
        if (output == null) {
            ctx2.result()['data']?.each { e -> ctx.data.put(e.key, e.value)} //设置返回属性
        } else {
            output.each { e -> ctx.data.put(e.key, e.value)}
        }
        return ctx2.decideResult
    }


    @Override
    String name() { 决策id }
}
