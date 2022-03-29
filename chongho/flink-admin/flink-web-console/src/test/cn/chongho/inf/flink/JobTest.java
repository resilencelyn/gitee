package cn.chongho.inf.flink;


import cn.chongho.inf.flink.restapi.JobApi;
import cn.chongho.inf.flink.service.tasks.SyncCheckPointTask;
import cn.chongho.inf.flink.service.tasks.SyncJobStatusTask;
import org.junit.Ignore;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.test.context.junit4.SpringRunner;

import javax.annotation.Resource;


@Ignore
@SpringBootTest
@RunWith(SpringRunner.class)
public class JobTest {


    @Autowired
    private SyncJobStatusTask syncJobStatusTask;

    @Resource
    private JobApi jobApiRequest;

    @Autowired
    private SyncCheckPointTask syncCheckPointTask;


    @Test
    public void syncJobStatus(){
        syncJobStatusTask.doSync();
    }

    @Test
    public void testSavepointLocation(){

    }

    @Test
    public void testCheckpoint(){
    }

    @Test
    public void testSyncCheckPoint(){
        syncCheckPointTask.doSync();
    }
}
