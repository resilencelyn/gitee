package entity

import cn.xnatural.jpa.LongIdEntity
import org.hibernate.annotations.DynamicUpdate

import javax.persistence.Column
import javax.persistence.Entity
import javax.persistence.EnumType
import javax.persistence.Enumerated
import javax.persistence.Index
import javax.persistence.Table

/**
 * 用户权限关系对应
 * 所有的权限都应在这张表中
 */
@Entity
@Table(indexes = [
        @Index(name = "idx_u_p", columnList = "userId,privilegeId", unique = true),
])
@DynamicUpdate
class UserPrivilege extends LongIdEntity {
    /**
     * {@link User#id}
     * 与用户关联
     */
    String userId
    /**
     * 权限类型
     */
    @Column(nullable = false)
    @Enumerated(EnumType.STRING)
    Privilege.Type type
    /**
     * 权限id表示
     */
    @Column(nullable = false)
    String privilegeId
    /**
     * 权限名称
     */
    @Column(nullable = false, length = 500)
    String privilegeName
    /**
     * 资源id
     */
    String resourceId
}
