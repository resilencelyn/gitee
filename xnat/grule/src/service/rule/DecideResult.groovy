package service.rule

/**
 * 决策结果
 */
enum DecideResult {
    Accept("通过", false), Reject("拒绝", true), Review("人工审核", false)
    String cnName
    /**
     * 是否阻断往下执行
     */
    boolean block

    DecideResult(String cnName, boolean block) {
        this.cnName = cnName
        this.block = block
    }
}