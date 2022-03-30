package cn.chongho.inf.flink.mapper;

import org.apache.ibatis.annotations.Delete;
import org.apache.ibatis.annotations.Result;
import org.apache.ibatis.annotations.Results;
import org.apache.ibatis.annotations.Select;
import org.apache.ibatis.type.JdbcType;
import tk.mybatis.mapper.common.Mapper;
import cn.chongho.inf.flink.model.Role;
import cn.chongho.inf.flink.model.UserRole;

import java.util.HashMap;
import java.util.List;

public interface UserRoleMapper extends Mapper<UserRole> {

    @Delete({
            "delete from user_role",
            "where roleid = #{roleid}"
    })
    int deleteByRoleid(String roleid);

    @Select({"SELECT r.id, r.name, r.createtime, r.creator, r.description,u.name createuser,u1.userid ",
            "FROM role r LEFT JOIN users u ON r.creator=u.id",
            " LEFT JOIN (SELECT roleid,userid FROM user_role WHERE userid=#{userId}) u1 ON r.id=u1.roleid  "})
    @Results({
            @Result(column="id", property="id", jdbcType= JdbcType.INTEGER, id=true),
            @Result(column="name", property="name", jdbcType= JdbcType.VARCHAR),
            @Result(column="creator", property="creator", jdbcType= JdbcType.INTEGER),
            @Result(column="createuser", property="createuser", jdbcType= JdbcType.VARCHAR),
            @Result(column="description", property="description", jdbcType= JdbcType.VARCHAR),
            @Result(column = "userid",property = "userid",jdbcType = JdbcType.INTEGER)
    })
    List<Role> selectByUserId(int userId);


    @Delete({"DELETE FROM user_role WHERE userid=#{id}"})
    int deleteByUserId(int id);
}
