package com.orion.ops.service.impl;

import com.alibaba.fastjson.JSON;
import com.baomidou.mybatisplus.core.conditions.query.LambdaQueryWrapper;
import com.orion.lang.wrapper.HttpWrapper;
import com.orion.ops.consts.*;
import com.orion.ops.consts.event.EventKeys;
import com.orion.ops.consts.event.EventParamsHolder;
import com.orion.ops.consts.system.SystemEnvAttr;
import com.orion.ops.dao.UserInfoDAO;
import com.orion.ops.entity.domain.UserInfoDO;
import com.orion.ops.entity.dto.LoginBindDTO;
import com.orion.ops.entity.dto.UserDTO;
import com.orion.ops.entity.request.UserLoginRequest;
import com.orion.ops.entity.request.UserResetRequest;
import com.orion.ops.entity.vo.UserLoginVO;
import com.orion.ops.interceptor.UserActiveInterceptor;
import com.orion.ops.service.api.PassportService;
import com.orion.ops.utils.*;
import com.orion.utils.Exceptions;
import com.orion.utils.Strings;
import org.springframework.data.redis.core.RedisTemplate;
import org.springframework.stereotype.Service;

import javax.annotation.Resource;
import java.util.Date;
import java.util.concurrent.TimeUnit;

/**
 * 认证服务
 *
 * @author Jiahang Li
 * @version 1.0.0
 * @since 2021/4/16 23:34
 */
@Service("passportService")
public class PassportServiceImpl implements PassportService {

    @Resource
    private UserInfoDAO userInfoDAO;

    @Resource
    private RedisTemplate<String, String> redisTemplate;

    @Resource
    private UserActiveInterceptor userActiveInterceptor;

    @Override
    public UserLoginVO login(UserLoginRequest request) {
        // 查询用户
        LambdaQueryWrapper<UserInfoDO> query = new LambdaQueryWrapper<UserInfoDO>()
                .eq(UserInfoDO::getUsername, request.getUsername())
                .last(Const.LIMIT_1);
        UserInfoDO userInfo = userInfoDAO.selectOne(query);
        Valid.notNull(userInfo, MessageConst.USERNAME_PASSWORD_ERROR);
        Valid.isTrue(Const.ENABLE.equals(userInfo.getLockStatus()), MessageConst.USER_LOCKED);
        // 检查密码
        boolean validPassword = ValueMix.validPassword(request.getPassword(), userInfo.getSalt(), userInfo.getPassword());
        if (validPassword) {
            Valid.isTrue(Const.ENABLE.equals(userInfo.getUserStatus()), MessageConst.USER_DISABLED);
        } else {
            // 开启登陆失败锁定
            if (EnableType.of(SystemEnvAttr.LOGIN_FAILURE_LOCK.getValue()).getValue()) {
                // 修改登录失败次数
                UserInfoDO updateUser = new UserInfoDO();
                updateUser.setId(userInfo.getId());
                updateUser.setFailedLoginCount(userInfo.getFailedLoginCount() + 1);
                updateUser.setUpdateTime(new Date());
                if (updateUser.getFailedLoginCount() >= Integer.parseInt(SystemEnvAttr.LOGIN_FAILURE_LOCK_THRESHOLD.getValue())) {
                    // 锁定用户
                    updateUser.setLockStatus(Const.DISABLE);
                }
                userInfoDAO.updateById(updateUser);
            }
            throw Exceptions.invalidArgument(MessageConst.USERNAME_PASSWORD_ERROR);
        }
        // 更新用户信息
        Long userId = userInfo.getId();
        String username = userInfo.getUsername();
        String ip = request.getIp();
        UserInfoDO updateUser = new UserInfoDO();
        updateUser.setId(userId);
        // 检查头像
        if (!AvatarPicHolder.isExist(userInfo.getAvatarPic())) {
            String url = AvatarPicHolder.generatorUserAvatar(userId, userInfo.getNickname());
            userInfo.setAvatarPic(url);
            updateUser.setAvatarPic(url);
        }
        updateUser.setFailedLoginCount(0);
        updateUser.setUpdateTime(new Date());
        updateUser.setLastLoginTime(new Date());
        userInfoDAO.updateById(updateUser);
        // 设置登陆信息
        long timestamp = System.currentTimeMillis();
        String loginToken = ValueMix.createLoginToken(userId, timestamp);
        UserDTO userCache = new UserDTO();
        userCache.setId(userId);
        userCache.setUsername(username);
        userCache.setNickname(userInfo.getNickname());
        userCache.setRoleType(userInfo.getRoleType());
        userCache.setUserStatus(userInfo.getUserStatus());
        userCache.setTimestamp(timestamp);
        // 设置绑定信息
        LoginBindDTO bind = new LoginBindDTO();
        bind.setTimestamp(timestamp);
        bind.setLoginIp(ip);
        // 设置登陆缓存
        long expire = Long.parseLong(SystemEnvAttr.LOGIN_TOKEN_EXPIRE.getValue());
        String userInfoKey = Strings.format(KeyConst.LOGIN_TOKEN_KEY, userId);
        redisTemplate.opsForValue().set(userInfoKey, JSON.toJSONString(userCache), expire, TimeUnit.HOURS);
        // 设置绑定缓存
        String loginBindKey = Strings.format(KeyConst.LOGIN_TOKEN_BIND_KEY, userId, timestamp);
        redisTemplate.opsForValue().set(loginBindKey, JSON.toJSONString(bind), expire, TimeUnit.HOURS);
        // 设置活跃时间
        userActiveInterceptor.setActiveTime(userId, timestamp);
        // 返回
        UserLoginVO loginInfo = new UserLoginVO();
        loginInfo.setToken(loginToken);
        loginInfo.setUserId(userId);
        loginInfo.setUsername(username);
        loginInfo.setNickname(userInfo.getNickname());
        loginInfo.setRoleType(userInfo.getRoleType());
        // 设置操作日志参数
        EventParamsHolder.addParam(EventKeys.INNER_USER_ID, userId);
        EventParamsHolder.addParam(EventKeys.INNER_USER_NAME, username);
        return loginInfo;
    }

    @Override
    public void logout() {
        UserDTO user = Currents.getUser();
        if (user == null) {
            return;
        }
        Long id = user.getId();
        Long timestamp = user.getCurrentBindTimestamp();
        // 删除token
        redisTemplate.delete(Strings.format(KeyConst.LOGIN_TOKEN_BIND_KEY, id, timestamp));
        // 删除活跃时间
        userActiveInterceptor.deleteActiveTime(id);
    }

    @Override
    public void resetPassword(UserResetRequest request) {
        UserDTO current = Currents.getUser();
        Long updateUserId = request.getUserId();
        final boolean isAdmin = Currents.isAdministrator();
        final boolean updateCurrent = current.getId().equals(updateUserId);
        // 检查权限
        if (!updateCurrent && !isAdmin) {
            throw Exceptions.httpWrapper(HttpWrapper.of(ResultCode.NO_PERMISSION));
        }
        // 查询更新用户
        UserInfoDO userInfo = userInfoDAO.selectById(updateUserId);
        Valid.notNull(userInfo, MessageConst.UNKNOWN_USER);
        // 检查原密码是否正确
        if (updateCurrent) {
            String beforePassword = Valid.notBlank(request.getBeforePassword(), MessageConst.BEFORE_PASSWORD_EMPTY);
            String validBeforePassword = ValueMix.encPassword(beforePassword, userInfo.getSalt());
            Valid.isTrue(validBeforePassword.equals(userInfo.getPassword()), MessageConst.BEFORE_PASSWORD_ERROR);
        }
        if (updateCurrent) {
            // 修改自己密码不记录
            EventParamsHolder.setSave(false);
        } else {
            EventParamsHolder.addParam(EventKeys.USERNAME, userInfo.getUsername());
        }
        // 修改密码
        String newPassword = ValueMix.encPassword(request.getPassword(), userInfo.getSalt());
        UserInfoDO updateUser = new UserInfoDO();
        Long userId = userInfo.getId();
        updateUser.setId(userId);
        updateUser.setPassword(newPassword);
        updateUser.setLockStatus(Const.ENABLE);
        updateUser.setFailedLoginCount(0);
        updateUser.setUpdateTime(new Date());
        userInfoDAO.updateById(updateUser);
        // 删除token
        RedisUtils.deleteLoginToken(redisTemplate, userId);
        // 删除活跃时间
        userActiveInterceptor.deleteActiveTime(userId);
    }

    @Override
    public UserDTO getUserByToken(String token, String checkIp) {
        if (Strings.isBlank(token)) {
            return null;
        }
        // 解析 token 信息
        Long[] info = ValueMix.getLoginTokenInfo(token);
        if (info == null) {
            return null;
        }
        Long userId = info[0];
        Long timestamp = info[1];
        // 获取用户绑定信息
        String bindCache = redisTemplate.opsForValue().get(Strings.format(KeyConst.LOGIN_TOKEN_BIND_KEY, userId, timestamp));
        if (Strings.isBlank(bindCache)) {
            return null;
        }
        // 获取用户登陆信息
        String userCache = redisTemplate.opsForValue().get(Strings.format(KeyConst.LOGIN_TOKEN_KEY, userId));
        if (Strings.isBlank(userCache)) {
            return null;
        }
        LoginBindDTO bind = JSON.parseObject(bindCache, LoginBindDTO.class);
        UserDTO user = JSON.parseObject(userCache, UserDTO.class);
        // 检查 ip 是否为登陆时的ip
        if (checkIp != null && !checkIp.equals(bind.getLoginIp())) {
            return null;
        }
        // 检查多端登陆
        if (!EnableType.of(SystemEnvAttr.ALLOW_MULTIPLE_LOGIN.getValue()).getValue()) {
            // 不是登陆的时间戳则证明后续有人登陆
            if (!timestamp.equals(user.getTimestamp())) {
                return null;
            }
        }
        user.setCurrentBindTimestamp(bind.getTimestamp());
        return user;
    }

}
