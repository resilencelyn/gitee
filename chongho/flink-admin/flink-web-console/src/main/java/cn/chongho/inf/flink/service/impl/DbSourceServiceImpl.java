package cn.chongho.inf.flink.service.impl;

import cn.chongho.inf.flink.constants.Constant;
import cn.chongho.inf.flink.mapper.DbSourceMapper;
import cn.chongho.inf.flink.model.DbSource;
import cn.chongho.inf.flink.service.DbSourceService;
import cn.chongho.inf.flink.utils.DesUtils;
import cn.chongho.inf.flink.utils.StringUtils;
import org.springframework.beans.BeanUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;

import javax.annotation.Resource;
import java.util.Date;
import java.util.List;

/**
 * @author ming
 */
@Service
public class DbSourceServiceImpl implements DbSourceService {

    @Value("${sys.des.secretkey}")
    private String desSecretkey;

    @Value("${sys.des.offset}")
    private String desOffset;

    @Resource
    private DbSourceMapper dbSourceMapper;

    @Autowired
    private DbInfoServiceImpl dbInfoService;

    @Override
    public List<DbSource> getDbSourceByPage(int pageNum, int pageSize, DbSource dbSource) {
        int offset = (pageNum-1) * pageSize;
        return dbSourceMapper.selectByPage(offset, pageSize, dbSource);
    }

    @Override
    public int selectCount(DbSource dbSource) {
        return dbSourceMapper.countByPage(dbSource);
    }

    @Override
    public List<DbSource> getAllDbSource(DbSource dbSource) {
        return dbSourceMapper.select(dbSource);
    }

    @Override
    public DbSource getDbSourceById(Integer dbSourceId) {
        return dbSourceMapper.selectOne(new DbSource(dbSourceId));
    }

    @Override
    public boolean updateDbSource(DbSource dbSource) {
        if(StringUtils.isEmpty(dbSource.getUsername())){
            dbSource.setUsername(null);
        }else{
            dbSource.setUsername(DesUtils.encrypt(desSecretkey, desOffset, dbSource.getUsername()));
        }
        if(StringUtils.isEmpty(dbSource.getPassword())){
            dbSource.setPassword(null);
        }else{
            dbSource.setPassword(DesUtils.encrypt(desSecretkey, desOffset, dbSource.getPassword()));
        }
        return dbSourceMapper.updateByPrimaryKeySelective(dbSource) > 0;
    }

    @Override
    public boolean addDbSource(DbSource dbSource) {
        DbSource newDbSource = new DbSource();
        BeanUtils.copyProperties(dbSource ,newDbSource);
        newDbSource.setCreateTime(new Date());
        newDbSource.setEnableFlag(Constant.EnableFlag.ENABLE.ordinal());
        if(!StringUtils.isEmpty(newDbSource.getUsername())){
            newDbSource.setUsername(DesUtils.encrypt(desSecretkey, desOffset, newDbSource.getUsername()));
        }
        if(!StringUtils.isEmpty(newDbSource.getPassword())){
            newDbSource.setPassword(DesUtils.encrypt(desSecretkey, desOffset, newDbSource.getPassword()));
        }
        return dbSourceMapper.insertSelective(newDbSource) > 0;
    }

    @Override
    public boolean syncDbInfo(Integer dbSourceId) {
        try {
            dbInfoService.createTableByDbId(dbSourceId);
            return true;
        }catch (Exception e){
            return false;
        }
    }
}
