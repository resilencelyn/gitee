package cn.chongho.inf.flink.service.impl;

import cn.chongho.inf.flink.mapper.JarMapper;
import cn.chongho.inf.flink.model.Jar;
import cn.chongho.inf.flink.service.JarService;
import org.springframework.beans.BeanUtils;
import org.springframework.stereotype.Service;

import javax.annotation.Resource;
import java.util.Date;
import java.util.List;

/**
 * @author ming
 */
@Service
public class JarServiceImpl implements JarService {

    @Resource
    private JarMapper jarMapper;

    @Override
    public List<Jar> getJarsByPage(int pageNum, int pageSize, Jar jar, Date startTime , Date endTime) {
        int offset = (pageNum-1) * pageSize;
        return jarMapper.selectByPage(offset ,pageSize,jar ,startTime, endTime);
    }

    @Override
    public int selectCount(Jar jar, Date startTime ,Date endTime) {
        return jarMapper.countByPage(jar ,startTime, endTime);
    }

    @Override
    public List<Jar> getAllJars(Jar jar) {
        return jarMapper.select(jar);
    }

    @Override
    public boolean updateJar(Jar jar) {
        return jarMapper.updateByPrimaryKeySelective(jar) > 0;
    }

    @Override
    public boolean addJar(Jar jar) {
        Jar newJar = new Jar();
        BeanUtils.copyProperties(jar ,newJar);
        newJar.setCreateTime(new Date());
        return jarMapper.insert(newJar) > 1;
    }

    @Override
    public Jar getJarOne(Jar jar) {
        return jarMapper.selectOne(jar);
    }
}
