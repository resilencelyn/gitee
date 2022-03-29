package cn.chongho.inf.flink.model;

import lombok.Data;

import javax.persistence.Id;
import javax.persistence.Table;
import java.util.Date;

/**
 * @author feihu.wang
 * @since 2022-02-14
 */
@Data
@Table(name = "users")
public class AdminUser {
    @Id
    private Integer id;

    private Integer tenantid;

    private String name;

    private String psw;

    private String email;

    private Integer creator;

    private Date createtime;

    private Integer flag;

    private Date logintime;

    private Integer updateuser;

    private Date updatetime;


}
