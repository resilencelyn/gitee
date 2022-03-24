package com.jd.platform.jlog.core;

import com.jd.platform.jlog.common.handler.CompressHandler;
import com.jd.platform.jlog.common.handler.CompressHandler.Outcome;
import com.jd.platform.jlog.common.handler.ExtractHandler;

import java.util.Map;

/**
 * @author tangbohu
 * @version 1.0.0
 * @ClassName ClientHandler.java
 * @createTime 2022年03月13日 16:53:00
 */
public class ClientHandler {


    public static Outcome processReq(Map<String, Object> reqMap){
        Map<String, Object> tag = ExtractHandler.extractReqTag(reqMap);
        return CompressHandler.compressReq(tag);
    }


    public static Outcome processLog(String content){
        Map<String, Object> tagMap = ExtractHandler.extractLogTag(content);
        return new Outcome(tagMap);
    }


    public static Outcome processResp(byte[] resp, Map<String, Object> respMap){
        Map<String, Object> tagMap = ExtractHandler.extractRespTag(respMap);
        Outcome outcome = CompressHandler.compressResp(resp);
        outcome.setMap(tagMap);
        return outcome;
    }




}
