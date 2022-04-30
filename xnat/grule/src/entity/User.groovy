package entity


import cn.xnatural.jpa.UUIDEntity
import org.hibernate.annotations.DynamicUpdate
import org.hibernate.annotations.Type

import javax.persistence.*

@Entity
@DynamicUpdate
@Table(indexes = [
        @Index(name = "idx_name", columnList = "name", unique = true),
])
class User extends UUIDEntity {
    /**
     * 用户登录名
     */
    @Column(nullable = false)
    String name
    /**
     * 创建者 {@link User#id}
     */
    String creator
    /**
     * 登录的密码
     */
    String password
    /**
     * 上次登录时间
     */
    Date login
}
