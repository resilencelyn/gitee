package entity

import cn.xnatural.jpa.LongIdEntity
import org.hibernate.annotations.DynamicUpdate
import org.hibernate.annotations.Type

import javax.persistence.*

/**
 * 字段
 */
@Table(indexes = [
    @Index(name = "idx_key", columnList = "key", unique = true),
    @Index(name = "idx_name", columnList = "name", unique = true),
])
@Entity
@DynamicUpdate
class Field extends LongIdEntity {
    /**
     * 字段标识 唯一
     */
    @Column(nullable = false, name = '`key`')
    String key
    /**
     * 字段名 唯一
     */
    @Column(nullable = false)
    String name
    /**
     * 备注说明
     */
    String    comment
    @Column(nullable = false)
    @Enumerated(EnumType.STRING)
    FieldType type
    /**
     * 收集器选项
     * [{collectorId: '收集器id', chooseFn: '选择函数,返回true则使用此收集器'}]
     */
    @Lob
    @Basic
    @Type(type = "text")
    String    collectorOptions
    /**
     * 创建者 {@link User#id}
     */
    String    creator
    /**
     * 更新人 {@link User#id}
     */
    String    updater
}
