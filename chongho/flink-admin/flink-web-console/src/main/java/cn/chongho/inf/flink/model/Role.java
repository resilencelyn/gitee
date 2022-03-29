package cn.chongho.inf.flink.model;

import lombok.Data;

import javax.persistence.Id;
import javax.persistence.Transient;
import java.util.Date;

/**
 * 系统角色
 * @author feihu.wang
 * @since 2022-02-14
 */
@Data
public class Role {
    @Id
    private Integer id;

    private String name;

    private Date createtime;

    private Integer creator;

    private String description;

    private Integer updateuser;

    private Date updatetime;
    @Transient
    private int userid;
    @Transient
    private String createuser;
}
