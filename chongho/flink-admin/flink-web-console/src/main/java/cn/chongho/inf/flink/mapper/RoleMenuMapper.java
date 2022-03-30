package cn.chongho.inf.flink.mapper;

import org.apache.ibatis.annotations.Delete;
import tk.mybatis.mapper.common.Mapper;
import cn.chongho.inf.flink.model.RoleMenu;

public interface RoleMenuMapper extends Mapper<RoleMenu> {

    @Delete({
            "delete from role_menu",
            "where roleid = #{roleid}"
    })
    int deleteByRoleid(String roleid);
}
