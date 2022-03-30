package cn.chongho.inf.flink.model;

import lombok.Data;

import javax.persistence.Id;
import java.util.Date;

/**
 * 用户角色
 */
@Data
public class UserRole {
    @Id
    private Integer userid;
    @Id
    private Integer roleid;

    private Integer creator;

    private Date createtime;

    public UserRole() {
    }

    public UserRole(Integer userid, Integer roleid, Integer creator) {
        this.userid = userid;
        this.roleid = roleid;
        this.creator = creator;
        this.createtime = new Date();
    }
}
