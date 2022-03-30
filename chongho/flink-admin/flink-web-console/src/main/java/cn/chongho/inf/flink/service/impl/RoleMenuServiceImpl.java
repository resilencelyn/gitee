package cn.chongho.inf.flink.service.impl;

import cn.chongho.inf.flink.mapper.MenuMapper;
import cn.chongho.inf.flink.mapper.RoleMenuMapper;
import cn.chongho.inf.flink.model.Menu;
import cn.chongho.inf.flink.model.RoleMenu;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import cn.chongho.inf.flink.service.RoleMenuService;
import org.springframework.transaction.annotation.Transactional;
import tk.mybatis.mapper.entity.Example;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.stream.Collectors;

/**
 * 角色相关权限业务业务接口实现类
 *
 * @author Raye
 * @date 2017/3/23
 */
@Service
public class RoleMenuServiceImpl implements RoleMenuService {

    @Autowired
    private MenuMapper mapper;

    @Autowired
    private RoleMenuMapper roleMenuMapper;

    @Override
    public List<Menu> selectByRoleId(int id) {
        HashMap<Integer,ArrayList<Menu>> map = new HashMap<Integer, ArrayList<Menu>>();
        List<Menu> menus = mapper.selectByRoleId(id);
        for(Menu menu : menus){
            int parentid = menu.getParentid();
            if(menu.getRoleid() != 0){
                menu.setAdditionalParameters(new HashMap<String, Object>());
                menu.getAdditionalParameters().put("item-selected",true);
            }
            if(map.containsKey(parentid)){
                map.get(parentid).add(menu);
            }else{
                ArrayList<Menu> temp = new ArrayList<Menu>();
                temp.add(menu);
                map.put(parentid,temp);
            }
        }
        for(Menu menu : menus){
            int mid = menu.getId();
            if(map.containsKey(mid)){
                menu.setType("folder");
                menu.setChildren(map.get(mid));
            }else{
                menu.setType("item");
            }
        }
        return map.get(0);
    }

    @Transactional(rollbackFor = Exception.class)
    @Override
    public boolean updateRoleMenu(String ids,int roleid,int userid) {
        if(ids.length() > 0){
            ids = ids.substring(0,ids.length()-1);
        }
        List<String> thisRoleMenuIds = new ArrayList<>();
        Collections.addAll(thisRoleMenuIds, ids.split(","));

        List<RoleMenu> roleMenuList = getRoleMenu(roleid);
        List<Integer> haveMenuId = roleMenuList.stream().map(RoleMenu::getMenuid).collect(Collectors.toList());

        thisRoleMenuIds.forEach(addMenuId->{
            if(!haveMenuId.contains(new Integer(addMenuId))){
                roleMenuMapper.insert(new RoleMenu(new Integer(addMenuId), roleid, userid));
            }
        });
        deleteRoleMenu(roleid, thisRoleMenuIds);
        return true;
    }

    private List<RoleMenu> getRoleMenu(Integer roleid){
        Example roleMenuExample = new Example(RoleMenu.class);
        Example.Criteria roleCriteria = roleMenuExample.createCriteria();
        roleCriteria.andEqualTo("roleid", roleid);
        roleCriteria.andEqualTo("flag", 1);
        return roleMenuMapper.selectByExample(roleMenuExample);
    }

    private void deleteRoleMenu(Integer roleid, List<String> thisRoleMenuIds){
        Example roleMenuExample = new Example(RoleMenu.class);
        Example.Criteria roleCriteria = roleMenuExample.createCriteria();
        roleCriteria.andEqualTo("roleid", roleid);
        roleCriteria.andNotIn("menuid", thisRoleMenuIds);
        roleMenuMapper.deleteByExample(roleMenuExample);
    }
}
