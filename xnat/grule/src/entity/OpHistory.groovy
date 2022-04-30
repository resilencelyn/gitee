package entity

import cn.xnatural.jpa.LongIdEntity
import org.hibernate.annotations.Type

import javax.persistence.*

/**
 * 操作历史记录
 */
@Entity
@Table(indexes = [@Index(name = "idx_createTime", columnList = "createTime")])
class OpHistory extends LongIdEntity {
    /**
     * 数据类型: Decision, Collector, Field
     */
    @Column(nullable = false)
    @Enumerated(EnumType.STRING)
    OpHistoryType type
    /**
     * 数据id
     */
    @Column(nullable = false)
    String dataId
    /**
     * 操作员 {@link User#id}
     */
    String operator
    /**
     * 保存各个字段的 json 字符串
     */
    @Lob
    @Basic
    @Type(type = "text")
    String content
}
