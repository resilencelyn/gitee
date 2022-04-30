package entity


import cn.xnatural.jpa.IEntity
import org.hibernate.annotations.DynamicUpdate
import org.hibernate.annotations.Type
import service.rule.DecideResult

import javax.persistence.*

/**
 * 决策记录
 */
@Entity
@DynamicUpdate
@Table(indexes = [@Index(name = "idx_occurTime", columnList = "occurTime")])
class DecideRecord implements IEntity {
    DecideRecord() {}

    DecideRecord(String id, String decision, String status, DecideResult result, Date occurTime, Integer spend, String exception) {
        this.id = id
        this.decision = decision
        this.status = status
        this.result = result
        this.occurTime = occurTime
        this.spend = spend
        this.exception = exception
    }


    /**
     * 决策流水id
     */
    @Id
    @Column(length = 50)
    String       id
    /**
     * 决策id
     * 关联 {@link Decision#id}
     */
    @Column(length = 36, nullable = false)
//    @ManyToOne
//    @JoinColumn(foreignKey = @ForeignKey(ConstraintMode.NO_CONSTRAINT), nullable = false)
    String decision
    /**
     * 执行结果状态
     */
    String       status
    /**
     * 决策结果
     */
    @Column(length = 30)
    @Enumerated(EnumType.STRING)
    DecideResult result
    /**
     * 决策时间
     */
    @Column(nullable = false)
    Date         occurTime
    /**
     * 执行花费时间
     */
    Integer      spend
    /**
     * 异常信息
     */
    @Column(length = 1000)
    String       exception
    /**
     * 输入参数 json 字符串
     */
    @Lob
    @Basic
    @Type(type = "text")
    String       input
    /**
     * 最终数据集
     */
    @Lob
    @Basic
    @Type(type = "text")
    String       data
    /**
     * 执行过程描述详情
     */
    @Lob
    @Basic
    @Type(type = "text")
    String       detail
    /**
     * 数据收集结果集
     */
    @Lob
    @Basic
    @Type(type = "text")
    String dataCollectResult
}
