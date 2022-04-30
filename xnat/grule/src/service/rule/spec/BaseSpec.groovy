package service.rule.spec


abstract class BaseSpec {
    /**
     * 自定义规则属性. 例: 自定义id, 描述
     */
    @Lazy Map<String, Object> attrs = new HashMap<>()
    /**
     * 顺序节点: 拒绝, 通过, 人工, 操作, 清除
     * 依次遍历节点执行
     */
    @Lazy List<Tuple2<String, Closure>> nodes = new LinkedList<>()

    /**
     * 名字
     */
    abstract String name()


    /**
     * 创建 关联执行闭包(即: 闭包的执行过程中数据存放使用传进来的执行上下文ctx)
     * 读: 当前闭包, 当前执行上下文, 决策自定义函数
     * 写: 当前执行上下文
     * @param src 原闭包
     * @param ctx 当前执行上下文
     * @return 临时执行闭包
     */
    static Closure rehydrate1(Closure src, Map ctx, Object _this) {
        final cl = src.rehydrate(new HashMap(1) {
            @Override
            Object put(Object key, Object value) { ctx.put(key, value); return value }

            @Override
            Object get(Object key) {
                try {
                    return src.getProperty(key)
                } catch (MissingPropertyException | MissingFieldException ex) {
                    return ctx.get(key)
                }
            }
        }, src, _this)
        // 不能用 DELEGATE_ONLY 不然 调不了决策中自定义函数
        cl.setResolveStrategy(Closure.DELEGATE_FIRST)
        return cl
    }


    /**
     * 创建 独立执行闭包(即: 会为闭包单独创建一个数据存放空间)
     * 读: 当前闭包, 独立执行空间(map), 当前执行上下文
     * 写: 独立执行空间(map)
     * @param src 原闭包
     * @param ctx 当前执行上下文
     * @return 临时执行闭包
     */
    static <T> Closure<T> rehydrate2(Closure<T> src, Map ctx, Object _this) {
        final cl = src.rehydrate(new LinkedHashMap(7) {
            @Override
            Object get(Object key) {
                try {
                    return src.getProperty(key)
                } catch (MissingPropertyException | MissingFieldException ex) {
                    return super.containsKey(key) ? super.get(key) : ctx.get(key)
                }
            }
        }, src, _this)
        cl.setResolveStrategy(Closure.DELEGATE_ONLY)
        return cl
    }
}
