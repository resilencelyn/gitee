package com.jd.platform.jlog.client.cache;

import org.reflections.Reflections;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import javax.servlet.ServletRequest;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * @author tangbohu
 * @version 1.0.0
 * @ClassName ExtParamFactory.java
 * @Description TODO
 * @createTime 2022年03月21日 20:10:00
 */
public class ExtParamFactory {


    private static Logger LOGGER = LoggerFactory.getLogger(ExtParamFactory.class);

    private static volatile ReqMap reqMap = null;

    private static volatile RespMap respMap = null;

    private static AtomicBoolean reqHasLoad = new AtomicBoolean(false);

    private static AtomicBoolean respHasLoad = new AtomicBoolean(false);


    public static Map<String, Object> getReqMap(ServletRequest request) {
        try {
            if(!reqHasLoad.get() && reqMap == null){
                synchronized (ExtParamFactory.class){
                    if(reqMap == null){
                        Reflections reflections = new Reflections();
                        Set<Class<? extends ReqMap>> subTypes = reflections.getSubTypesOf(ReqMap.class);
                        for (Class<? extends ReqMap> subType : subTypes) {
                            reqMap = subType.newInstance();
                            break;
                        }
                    }
                }
                reqHasLoad.set(true);
            }
            return reqMap == null ? new HashMap<>(0) : reqMap.appendResMap(request);
        }catch (Exception e){
            LOGGER.error("反射获取入参异常",e);
        }
        return new HashMap<>(0);
    }

    public static Map<String, Object> getRespMap(String cnt) {

        try {
            if(!respHasLoad.get() && respMap == null){
                synchronized (ExtParamFactory.class){
                    if(respMap == null){
                        Reflections reflections = new Reflections();
                        Set<Class<? extends RespMap>> subTypes = reflections.getSubTypesOf(RespMap.class);
                        for (Class<? extends RespMap> subType : subTypes) {
                            respMap = subType.newInstance();
                            break;
                        }
                    }
                }
                respHasLoad.set(true);
            }
            return respMap == null ? new HashMap<>(0) : respMap.appendRespMap(cnt);
        }catch (Exception e){
            LOGGER.error("反射获取出参异常",e);
        }
       return new HashMap<>(0);
    }

}
