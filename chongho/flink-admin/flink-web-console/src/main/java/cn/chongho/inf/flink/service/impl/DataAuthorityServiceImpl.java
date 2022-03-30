package cn.chongho.inf.flink.service.impl;

import cn.chongho.inf.flink.constants.Constant;
import cn.chongho.inf.flink.mapper.DataAuthorityMapper;
import cn.chongho.inf.flink.model.BaseJob;
import cn.chongho.inf.flink.model.DataAuthority;
import cn.chongho.inf.flink.model.UserDataAuthority;
import cn.chongho.inf.flink.service.DataAuthorityService;
import cn.chongho.inf.flink.utils.StringUtils;
import lombok.extern.slf4j.Slf4j;
import org.apache.ibatis.session.RowBounds;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import tk.mybatis.mapper.entity.Example;

import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

@Service
@Slf4j
public class DataAuthorityServiceImpl implements DataAuthorityService {

    @Autowired
    private DataAuthorityMapper dataAuthorityMapper;

    @Override
    public int selectCount(DataAuthority dataAuthority) {
        return dataAuthorityMapper.selectCount(dataAuthority);
    }

    @Override
    public List<DataAuthority> selectByPage(int page, int pageSize, DataAuthority dataAuthority) {
        int offset = (page-1) * pageSize;
        return dataAuthorityMapper.selectByRowBounds(dataAuthority, new RowBounds(offset, pageSize));
    }

    @Override
    public boolean delete(Integer id, Integer loginUserId) {
        DataAuthority dataAuthority = new DataAuthority();
        dataAuthority.setId(id);
        dataAuthority.setEnableFlag(0);
        return dataAuthorityMapper.updateByPrimaryKeySelective(dataAuthority) > 0;
    }

    @Override
    public boolean update(Integer dataType, Integer dataId, String userIdsStr) {
        if(StringUtils.isEmpty(userIdsStr)){
            DataAuthority dataAuthority = new DataAuthority();
            dataAuthority.setEnableFlag(0);
            dataAuthority.setUpdateTime(LocalDateTime.now());

            Example example = new Example(DataAuthority.class);
            Example.Criteria criteria = example.createCriteria();
            criteria.andEqualTo("enableFlag", 1);
            criteria.andEqualTo("dataType", dataType);
            criteria.andEqualTo("dataId", dataId);
            dataAuthorityMapper.updateByExampleSelective(dataAuthority, example);
            return true;
        }
        userIdsStr = userIdsStr.substring(0, userIdsStr.length()-1);
        List<String> userIds = new ArrayList<>();
        Collections.addAll(userIds, userIdsStr.split(","));
        Map<Integer, Integer> dataAuthoritiesMap = selectDataAuthorities(dataType, dataId).stream().collect(Collectors.toMap(DataAuthority::getUserId, DataAuthority::getEnableFlag));
        for(String addUserId : userIds){
            if(!dataAuthoritiesMap.containsKey(Integer.parseInt(addUserId))){
                dataAuthorityMapper.insert(new DataAuthority(dataId, dataType, Integer.parseInt(addUserId)));
            }else if(dataAuthoritiesMap.get(Integer.parseInt(addUserId)) == 0){
                DataAuthority dataAuthority = new DataAuthority();
                dataAuthority.setEnableFlag(1);
                dataAuthority.setUpdateTime(LocalDateTime.now());

                Example example = new Example(DataAuthority.class);
                Example.Criteria criteria = example.createCriteria();
                criteria.andEqualTo("userId", Integer.parseInt(addUserId));
                criteria.andEqualTo("dataType", dataType);
                criteria.andEqualTo("dataId", dataId);
                dataAuthorityMapper.updateByExampleSelective(dataAuthority, example);
            }
        }
        deleteDataAuthorities(dataType, dataId, userIds);
        return true;
    }


    private List<DataAuthority> selectDataAuthorities(Integer dataType, Integer dataId){
        Example example = new Example(DataAuthority.class);
        Example.Criteria criteria = example.createCriteria();
        criteria.andEqualTo("dataType", dataType);
        criteria.andEqualTo("dataId", dataId);
        return dataAuthorityMapper.selectByExample(example);
    }

    private void deleteDataAuthorities(Integer dataType, Integer dataId, List<String> addUserIds){
        Example example = new Example(DataAuthority.class);
        Example.Criteria criteria = example.createCriteria();
        criteria.andEqualTo("dataType", dataType);
        criteria.andEqualTo("dataId", dataId);
        criteria.andNotIn("userId", addUserIds);
        criteria.andEqualTo("enableFlag", 1);

        DataAuthority dataAuthority = new DataAuthority();
        dataAuthority.setEnableFlag(0);
        dataAuthority.setUpdateTime(LocalDateTime.now());
        dataAuthorityMapper.updateByExampleSelective(dataAuthority, example);
    }

    @Override
    public List<UserDataAuthority> selectAllUser(Integer dataId,Integer dataType) {
        return dataAuthorityMapper.selectUserAuthority(dataId, dataType);
    }


    @Override
    public void checkDataAuthority(BaseJob baseJob, Constant.DataType dataType, Integer loginUserId){
        if(!baseJob.getCreateUserId().equals(loginUserId)){
            Example example = new Example(DataAuthority.class);
            Example.Criteria criteria = example.createCriteria();
            criteria.andEqualTo("dataType", dataType.getValue());
            criteria.andEqualTo("dataId", baseJob.getId());
            criteria.andEqualTo("userId", loginUserId);
            criteria.andEqualTo("enableFlag", 1);
            if(dataAuthorityMapper.selectCountByExample(example) == 0){
                throw new RuntimeException("对应数据没有权限");
            }
        }
    }
}
