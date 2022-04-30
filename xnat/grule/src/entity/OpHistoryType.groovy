package entity

/**
 * 操作历史类型
 */
enum OpHistoryType {
    Decision("决策"), Field("字段"), Collector("收集器")
    String cnName

    OpHistoryType(String cnName) {
        this.cnName = cnName
    }
}
