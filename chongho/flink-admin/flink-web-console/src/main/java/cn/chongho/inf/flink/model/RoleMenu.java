package cn.chongho.inf.flink.model;

import lombok.Data;

import javax.persistence.Id;
import java.util.Date;

/**
 * 角色和菜单关联
 * @author feihu.wang
 * @since 2022-02-14
 */
@Data
public class RoleMenu {
    @Id
    private Integer menuid;
    @Id
    private Integer roleid;

    private Integer flag;

    private Integer creator;

    private Date createtime;


}
