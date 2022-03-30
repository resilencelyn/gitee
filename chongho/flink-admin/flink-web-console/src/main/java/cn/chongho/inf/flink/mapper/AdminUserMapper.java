package cn.chongho.inf.flink.mapper;

import org.apache.ibatis.annotations.*;
import org.apache.ibatis.type.JdbcType;
import tk.mybatis.mapper.common.Mapper;
import cn.chongho.inf.flink.model.AdminUser;

import java.util.List;

public interface AdminUserMapper extends Mapper<AdminUser> {

    @Select({
            "select",
            "u.id, u.name, u.email, u.creator, u.flag, u.logintime,c.name createuser",
            "from users u LEFT JOIN users c ON u.creator=c.id",
            "where u.name LIKE #{query} LIMIT #{begin},#{pagesize}"
    })
    @Results({
            @Result(column="id", property="id", jdbcType= JdbcType.INTEGER, id=true),
            @Result(column="name", property="name", jdbcType= JdbcType.VARCHAR),
            @Result(column="psw", property="psw", jdbcType= JdbcType.VARCHAR),
            @Result(column="email", property="email", jdbcType= JdbcType.VARCHAR),
            @Result(column="creator", property="creator", jdbcType= JdbcType.INTEGER),
            @Result(column="createuser", property="createUser", jdbcType= JdbcType.VARCHAR),
            @Result(column="flag", property="flag", jdbcType= JdbcType.INTEGER)
    })
    List<AdminUser> selectByQuery(@Param("begin") int begin, @Param("pagesize") int pageSize, @Param("query") String query);


    @Select({"SELECT COUNT(id) FROM users WHERE name LIKE #{name}"})
    int selectCountByName(String name);

    /**
     * 登录后更新用户最后登录时间
     * @param user
     * @return
     */
    @Update({"UPDATE users SET logintime=CURRENT_TIMESTAMP WHERE name=#{name} AND psw=#{psw}"})
    int updateLoginTime(AdminUser user);

}
