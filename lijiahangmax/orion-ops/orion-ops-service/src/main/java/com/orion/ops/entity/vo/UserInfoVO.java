package com.orion.ops.entity.vo;

import com.orion.ops.entity.domain.UserInfoDO;
import com.orion.ops.entity.dto.UserDTO;
import com.orion.utils.convert.TypeStore;
import com.orion.utils.time.Dates;
import lombok.Data;

import java.util.Date;
import java.util.Optional;

/**
 * 用户列表
 *
 * @author Jiahang Li
 * @version 1.0.0
 * @since 2021/5/28 17:03
 */
@Data
public class UserInfoVO {

    /**
     * id
     */
    private Long id;

    /**
     * 用户名
     */
    private String username;

    /**
     * 名称
     */
    private String nickname;

    /**
     * 角色类型 10管理员 20开发 30运维
     *
     * @see com.orion.ops.consts.user.RoleType
     */
    private Integer role;

    /**
     * 用户状态 1启用 2停用
     *
     * @see com.orion.ops.consts.Const#ENABLE
     * @see com.orion.ops.consts.Const#DISABLE
     */
    private Integer status;

    /**
     * 锁定状态 1正常 2锁定
     *
     * @see com.orion.ops.consts.Const#ENABLE
     * @see com.orion.ops.consts.Const#DISABLE
     */
    private Integer locked;

    /**
     * 手机号
     */
    private String phone;

    /**
     * 邮箱
     */
    private String email;

    /**
     * 最后一次登陆时间
     */
    private Date lastLoginTime;

    /**
     * 最后登录时间
     */
    private String lastLoginAgo;

    /**
     * 头像base64
     */
    private String avatar;

    static {
        TypeStore.STORE.register(UserInfoDO.class, UserInfoVO.class, d -> {
            UserInfoVO vo = new UserInfoVO();
            vo.setId(d.getId());
            vo.setUsername(d.getUsername());
            vo.setNickname(d.getNickname());
            vo.setRole(d.getRoleType());
            vo.setStatus(d.getUserStatus());
            vo.setLocked(d.getLockStatus());
            vo.setPhone(d.getContactPhone());
            vo.setEmail(d.getContactEmail());
            Date lastLoginTime = d.getLastLoginTime();
            vo.setLastLoginTime(lastLoginTime);
            Optional.ofNullable(lastLoginTime).map(Dates::ago).ifPresent(vo::setLastLoginAgo);
            return vo;
        });
    }

    static {
        TypeStore.STORE.register(UserDTO.class, UserInfoVO.class, d -> {
            UserInfoVO vo = new UserInfoVO();
            vo.setId(d.getId());
            vo.setUsername(d.getUsername());
            vo.setNickname(d.getNickname());
            vo.setRole(d.getRoleType());
            vo.setStatus(d.getUserStatus());
            return vo;
        });
    }

}
