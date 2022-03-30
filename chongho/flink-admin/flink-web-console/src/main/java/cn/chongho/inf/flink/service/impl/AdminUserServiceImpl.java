package cn.chongho.inf.flink.service.impl;

import cn.chongho.inf.flink.mapper.AdminUserMapper;
import cn.chongho.inf.flink.mapper.UserRoleMapper;
import cn.chongho.inf.flink.model.*;
import cn.chongho.inf.flink.service.AdminUserService;
import cn.chongho.inf.flink.utils.MD5Util;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;
import tk.mybatis.mapper.entity.Example;

import java.util.*;
import java.util.stream.Collectors;

/**
 * 系统用户
 * @author feihu.wang
 * @since 2022-02-14
 */
@Service
public class AdminUserServiceImpl implements AdminUserService {

    @Autowired
    private AdminUserMapper mapper;
    @Autowired
    private UserRoleMapper userRoleMapper;

    @Override
    public List<AdminUser> select(int page, int pageSize, String query) {
        return mapper.selectByQuery((page - 1) * pageSize,pageSize,"%"+query+"%");
    }

    @Override
    public int selectCount(String query) {
        return mapper.selectCountByName("%"+query+"%");
    }

    @Override
    public AdminUser login(String name, String pwd) {
        AdminUser temp = new AdminUser();
        temp.setName(name);
        temp.setPsw(MD5Util.MD5(pwd));
        return mapper.selectOne(temp);
    }

    @Override
    public AdminUser selectById(int id) {
        return mapper.selectByPrimaryKey(id);
    }

    @Override
    public boolean insert(AdminUser user) {
        user.setPsw(MD5Util.MD5(user.getPsw()));
        return mapper.insertSelective(user) > 0;
    }

    @Override
    public boolean update(AdminUser user) {
        user.setPsw(null);
        return mapper.updateByPrimaryKeySelective(user) > 0;
    }

    @Transactional
    @Override
    public boolean delete(int id) {
        mapper.deleteByPrimaryKey(id);
        userRoleMapper.deleteByUserId(id);
        return true;
    }

    @Override
    public List<Role> selectUserRole(int id) {
        List<Role> roles = userRoleMapper.selectByUserId(id);
        return roles;
    }

    @Transactional(rollbackFor = Exception.class)
    @Override
    public boolean updateRoleMenu(String ids, int userid, int creater) {
        if(ids.length() > 0){
            ids = ids.substring(0,ids.length()-1);
        }

        List<String> thisUserRoleIds = new ArrayList<>();
        Collections.addAll(thisUserRoleIds, ids.split(","));

        List<UserRole> userRole = getUserRole(userid);
        List<Integer> userHadRoleId = userRole.stream().map(UserRole::getRoleid).collect(Collectors.toList());

        thisUserRoleIds.forEach(addRuleId->{
            if(!userHadRoleId.contains(new Integer(addRuleId))){
                userRoleMapper.insert(new UserRole(userid, new Integer(addRuleId), creater));
            }
        });
        deleteUserRole(userid, thisUserRoleIds);
        return true;
    }

    private List<UserRole> getUserRole(Integer userid){
        Example userRoleExample = new Example(UserRole.class);
        Example.Criteria userRoleExampleCriteria = userRoleExample.createCriteria();
        userRoleExampleCriteria.andEqualTo("userid", userid);
        return userRoleMapper.selectByExample(userRoleExample);
    }

    private void deleteUserRole(int userid, List<String> thisUserRoleIds){
        Example userRoleExample = new Example(UserRole.class);
        Example.Criteria userRoleExampleCriteria = userRoleExample.createCriteria();
        userRoleExampleCriteria.andEqualTo("userid", userid);
        userRoleExampleCriteria.andNotIn("roleid", thisUserRoleIds);
        userRoleMapper.deleteByExample(userRoleExample);
    }

    @Override
    public WebResult updatePass(int userId, String psw, String oldPsw) {
        AdminUser user = mapper.selectByPrimaryKey(userId);
        if(user.getPsw().equalsIgnoreCase(MD5Util.MD5(oldPsw))){
            user.setUpdateuser(userId);
            user.setUpdatetime(new Date());
            user.setPsw(MD5Util.MD5(psw));
            if(mapper.updateByPrimaryKeySelective(user) > 0){
                return WebResult.success();
            }else{
                return WebResult.unKnown();
            }
        }else{
            return WebResult.error("旧密码错误");
        }
    }
}
