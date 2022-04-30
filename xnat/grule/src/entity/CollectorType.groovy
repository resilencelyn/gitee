package entity

/**
 * 收集器类型
 */
enum CollectorType {
    // 默认高精度计算
    Http("接口"), Script("脚本"), Sql("SQL")
    String cnName

    CollectorType(String cnName) {
        this.cnName = cnName
    }
}
