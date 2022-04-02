package cn.chongho.inf.flink;

import cn.chongho.inf.flink.model.Jar;
import cn.chongho.inf.flink.restapi.JarApi;
import cn.chongho.inf.flink.service.JarService;
import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;
import org.junit.Ignore;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.test.context.junit4.SpringRunner;

import java.math.BigDecimal;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

@Ignore
@SpringBootTest
@RunWith(SpringRunner.class)
public class JarTest {


    @Autowired
    private JarApi jarApiRequest;

    @Autowired
    private JarService jarService;


    @Test
    public void getJars(){
        List<JSONObject> allJars = jarApiRequest.getAllJars();
        allJars.forEach(jsonObject -> {
            Jar jar = new Jar();
            jar.setFileName(jsonObject.getString("name"));
            jar.setJobKey(jsonObject.getString("id"));
            jar.setCreateTime(jsonObject.getDate("uploaded"));
            System.out.println(jar.toString());
        });

    }

    @Test
    public void runJars(){
        String args = "--bootstrap.servers 192.168.46.202:9092,192.168.46.203:9092,192.168.46.204:9092 --group.id sync-mall-info --hostname rm-2ze9jx1bi865q0189.mysql.rds.aliyuncs.com --db sailing --username sailing --password Zhnx#sailing@2021 --checkpoint.path hdfs://flink-node-01:9000/tmp/flink/checkpoints";
        List<String>  argsArray = Arrays.asList(args.split(" "));
        Map<String, Object> params= new HashMap<>();
        params.put("entryClass" ,"com.cfpamf.rtc.biz.mall.GoodsAndStoreAddJob");
        params.put("programArgsList" , argsArray);
    }


}
