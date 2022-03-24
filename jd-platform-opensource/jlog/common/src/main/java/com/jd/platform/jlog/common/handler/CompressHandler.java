package com.jd.platform.jlog.common.handler;

import com.alibaba.fastjson.JSON;
import com.jd.platform.jlog.common.utils.CollectionUtil;
import com.jd.platform.jlog.common.utils.ZstdUtils;

import java.nio.charset.StandardCharsets;
import java.util.Map;

import static com.jd.platform.jlog.common.constant.Constant.MIN;
import static com.jd.platform.jlog.common.constant.Constant.THRESHOLD;
import static com.jd.platform.jlog.common.handler.CollectMode.*;

/**
 * @author tangbohu
 * @version 1.0.0
 * @ClassName CompressHandler.java
 * @createTime 2022年03月10日 20:52:00
 */
public class CompressHandler {

    /**
     * 压缩策略
     */
    private long compress;

    /**
     * 超过limit的才压缩
     */
    private long threshold;


    private CompressHandler(long compress, long threshold){
        this.compress = compress;
        this.threshold = threshold;
    }


    private static volatile CompressHandler instance = null;


    public static void buildCompressHandler(Long compress, Long threshold) {
        if(compress == null || compress < 1){
            compress = COMPRESS_LOG_RESP;
        }
        if( threshold == null || threshold < MIN){
            threshold = THRESHOLD;
        }
        instance = new CompressHandler(compress, threshold);
    }


    /**
     * req一般无需压缩 除非图片base64等触发压缩
     * @param map tagMap
     * @return Outcome
     */
    public static Outcome compressReq(Map<String, Object> map){
        if(instance == null || CollectionUtil.isEmpty(map) || !isMatched(instance.compress, C_REQ)){
            return new Outcome(map);
        }
        return new Outcome(map, ZstdUtils.compress(JSON.toJSONBytes(map)));
    }


    /**
     * 暂不开启普通log压缩
     */
    public static Outcome compressLog(String content){
        if(instance == null || !isMatched(instance.compress, C_LOG)){ return new Outcome(content); }
        if(content.length() < instance.threshold){
            new Outcome(content);
        }
        return new Outcome(ZstdUtils.compress(content.getBytes()));
    }


    /**
     * 压缩 resp
     * @param cntByte bt
     * @return Outcome
     */
    public static Outcome compressResp(byte[] cntByte){
        if(instance == null || !isMatched(instance.compress, C_RESP)){
            return new Outcome(cntByte);
        }
        return new Outcome(ZstdUtils.compress(cntByte));
    }



    public static synchronized void refresh(Long compress, Long threshold){
        instance = null;
        buildCompressHandler(compress, threshold);
    }

    public long getCompress() {
        return compress;
    }

    public void setCompress(long compress) {
        this.compress = compress;
    }


    public static class Outcome{

        Map<String, Object> tagMap;
        Object content;

        public Map<String, Object> getTagMap() {
            return tagMap;
        }

        public void setMap(Map<String, Object> tagMap) {
            this.tagMap = tagMap;
        }

        public Object getContent() {
            return content;
        }

        public void setContent(Object content) {
            this.content = content;
        }

        public Outcome(Map<String, Object> tagMap) {
            this.tagMap = tagMap;
        }

        public Outcome(Map<String, Object> tagMap, byte[] content) {
            this.tagMap = tagMap;
            this.content = content;
        }

        public Outcome(Object content) {
            this.content = content;
        }

    }


     /*
      public static Outcome compressReq(Map<String, Object> map){
        if(instance == null || CollectionUtil.isEmpty(map) || !isMatched(instance.compress, C_REQ)){
            return new Outcome(map);
        }

        for (Map.Entry<String, Object> entry : map.entrySet()) {
            String val = entry.getValue().toString();
            if(val.length() > instance.threshold){
                val = new String(ZstdUtils.compress(val.getBytes()), StandardCharsets.ISO_8859_1);
            }
            map.put(entry.getKey(), val);
        }
        return new Outcome(map);
      }

      public static Outcome compressResp(String content, byte[] cntByte){
        if(instance == null || !isMatched(instance.compress, C_RESP)){
            return new Outcome(content);
        }

        if(content.length() < instance.threshold){
            return new Outcome(content);
        }
        return new Outcome(ZstdUtils.compress(cntByte));
    }*/

}
