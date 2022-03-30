package cn.chongho.inf.flink.restapi;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONArray;
import com.alibaba.fastjson.JSONObject;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.http.HttpEntity;
import org.springframework.http.HttpHeaders;
import org.springframework.http.MediaType;
import org.springframework.stereotype.Service;
import org.springframework.web.client.RestTemplate;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * @author ming
 */
@Service
@Slf4j
public class JobApi {

    @Value("${sys.savepoint-path}")
    private String savepointPath;

    @Autowired
    private RestTemplate restTemplate;

    @Autowired
    private RestTemplate restTemplateByRequestFactory;


    private static final String  JOBS_RUL_PREFIX = "/jobs/";

    /**
     * 返回所有提交的job
     * @return
     */
    public List<JSONObject> getAllJobs(String flinkUrl){
        String url = flinkUrl + JOBS_RUL_PREFIX + "overview";
        JSONObject jarsJSONObject = restTemplate.getForObject(url , JSONObject.class);
        JSONArray files = jarsJSONObject.getJSONArray("jobs");
        if(files.isEmpty()){
            return null;
        }
        return files.toJavaList(JSONObject.class);
    }

    /**
     * trigger a savepoint.
     * @param jobId  {"request-id":"4ff40b1af9aad8e18222a442b705b472"}
     * @return
     */
    public String savepoint(String flinkUrl, String jobId) {
        String url = flinkUrl + JOBS_RUL_PREFIX + jobId + "/savepoints";
        Map<String, Object> params = new HashMap();
        params.put("target-directory", savepointPath);
        log.info("rest req:{}", url);
        HttpHeaders headers = new HttpHeaders();
        headers.setContentType(MediaType.APPLICATION_JSON_UTF8);
        HttpEntity<String> entity = new HttpEntity<>(JSON.toJSONString(params), headers);
        JSONObject triggerObj = restTemplate.postForObject(url, entity, JSONObject.class);
        return triggerObj.getString("request-id");
    }


    /**
     *  终止任务
     * @param flinkUrl
     * @param jobId
     */
    public void cancelJob(String flinkUrl, String jobId) {
        String url = flinkUrl + JOBS_RUL_PREFIX + jobId;
        restTemplateByRequestFactory.patchForObject(url, "",String.class);
    }

    /**
     * 带保存点停止任务
     * @param flinkUrl
     * @param jobId
     * @return
     */
    public String stopJob(String flinkUrl, String jobId) {
        String url = flinkUrl + JOBS_RUL_PREFIX + jobId + "/stop";
        Map<String, Object> params = new HashMap();
        params.put("targetDirectory", savepointPath);
        HttpHeaders headers = new HttpHeaders();
        headers.setContentType(MediaType.APPLICATION_JSON_UTF8);
        HttpEntity<String> entity = new HttpEntity<>(JSON.toJSONString(params), headers);
        JSONObject triggerObj = restTemplate.postForObject(url, entity, JSONObject.class);
        return triggerObj.getString("request-id");
    }

    /**
     * 查询savepoint的地址
     * @param jobId
     * @param triggerId
     * @return
     */
    public String getSavepointLocation(String flinkUrl, String jobId,String triggerId){
        String url = flinkUrl + JOBS_RUL_PREFIX + jobId + "/savepoints/" + triggerId;
        JSONObject forObject = restTemplate.getForObject(url, JSONObject.class);
        JSONObject operation = forObject.getObject("operation", JSONObject.class);
        if(operation == null){
            return "";
        }
        return operation.getString("location");
    }


    public List<JSONObject> getCheckpoint(String flinkUrl, String jobId){
        String url = flinkUrl + JOBS_RUL_PREFIX + jobId + "/checkpoints";
        JSONObject forObject = restTemplate.getForObject(url, JSONObject.class);
        JSONArray historyPoint = forObject.getJSONArray("history");
        return historyPoint.toJavaList(JSONObject.class);
    }

    /**
     * 查询集群和任务数量信息
     * @return
     */
    public JSONObject getOverviewInfo(String flinkUrl){
        String url = flinkUrl + "overview";
        return restTemplate.getForObject(url, JSONObject.class);
    }
}
