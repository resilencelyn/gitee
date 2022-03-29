package cn.chongho.inf.flink;

import cn.chongho.inf.flink.constants.Constant;
import cn.chongho.inf.flink.model.DbSource;
import cn.chongho.inf.flink.service.DbSourceService;
import cn.chongho.inf.flink.service.impl.DbInfoServiceImpl;
import org.junit.Ignore;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.test.context.junit4.SpringRunner;

import java.util.List;

@Ignore
@SpringBootTest
@RunWith(SpringRunner.class)
public class DbTest {

    @Autowired
    private DbInfoServiceImpl dbInfoService;

    @Autowired
    private DbSourceService dbSourceService;

    @Test
    public void getDbInfo(){
        DbSource dbSource = new DbSource();
        dbSource.setEnableFlag(Constant.EnableFlag.ENABLE.ordinal());
        List<DbSource> allDbSource = dbSourceService.getAllDbSource(dbSource);
        allDbSource.forEach(db -> {
            dbInfoService.createTableByDbId(db.getId());
        });
    }
}
