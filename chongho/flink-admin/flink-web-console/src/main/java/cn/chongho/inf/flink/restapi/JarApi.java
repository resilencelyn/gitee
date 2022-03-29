package cn.chongho.inf.flink.restapi;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONArray;
import com.alibaba.fastjson.JSONObject;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.core.io.ByteArrayResource;
import org.springframework.http.HttpEntity;
import org.springframework.http.HttpHeaders;
import org.springframework.http.MediaType;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Service;
import org.springframework.util.LinkedMultiValueMap;
import org.springframework.util.MultiValueMap;
import org.springframework.web.client.RestTemplate;
import org.springframework.web.multipart.MultipartFile;

import java.io.IOException;
import java.util.List;
import java.util.Map;

/**
 * @author ming
 */
@Service
@Slf4j
public class JarApi {

    @Value("${sys.url}")
    private String flinkApiUrl;

    @Autowired
    private RestTemplate restTemplate;


    private static final String  JARS_RUL_PREFIX = "/jars/";

    /**
     * 返回所有提交的jars包
     * @return
     */
    public List<JSONObject> getAllJars(){
        String url = flinkApiUrl + JARS_RUL_PREFIX;
        JSONObject jarsJSONObject = restTemplate.getForObject(url , JSONObject.class);
        JSONArray files = jarsJSONObject.getJSONArray("files");
        if(files.isEmpty()){
            return null;
        }
        return files.toJavaList(JSONObject.class);
    }

    /**
     * 运行任务
     * @param runParameter
     * @param jarid
     */
    public String runJars(String flinkUrl,Map<String, Object> runParameter , String jarid){
        String url = flinkUrl + JARS_RUL_PREFIX + jarid + "/run";
        HttpHeaders headers = new HttpHeaders();
        //定义请求参数类型，这里用json所以是MediaType.APPLICATION_JSON
        headers.setContentType(MediaType.APPLICATION_JSON);
        HttpEntity<String> formEntity = new HttpEntity<>(JSON.toJSONString(runParameter), headers);
        JSONObject jsonObject = restTemplate.postForObject(url, formEntity, JSONObject.class);
        return jsonObject.getString("jobid");
    }

    /**
     * 上传jar包
     * @param file
     */
    public JSONObject uploadJar(MultipartFile file) throws IOException {
        String url = flinkApiUrl + "/jars/upload";
        HttpHeaders header = new HttpHeaders();
        header.setContentType(MediaType.MULTIPART_FORM_DATA);

        HttpHeaders fileHeader = new HttpHeaders();
        fileHeader.setContentType(MediaType.parseMediaType(file.getContentType()));
        fileHeader.setContentDispositionFormData("uploadFile", file.getOriginalFilename());

        MultiValueMap<String, Object> multiValueMap = new LinkedMultiValueMap<>();
        HttpEntity<ByteArrayResource> fileEntity = new HttpEntity<>(new ByteArrayResource(file.getBytes()), fileHeader);
        multiValueMap.add("uploadFile", fileEntity);
        HttpEntity<MultiValueMap<String, Object>> httpEntity = new HttpEntity<>(multiValueMap, header);
        ResponseEntity<JSONObject> postForEntity = restTemplate.postForEntity(url, httpEntity, JSONObject.class);
        return postForEntity.getBody();
    }

    /**
     * 删除指定包
     * @param jarid
     * @return
     */
    public boolean deleteJar(String jarid){
        String url = flinkApiUrl + "/jars/" + jarid;
        try {
            restTemplate.delete(url);
            return true;
        }catch (Exception e){
            log.error("删除jar失败",e);
        }
        return false;
    }
}
