package com.jd.platform.jlog.client.cache;

import javax.servlet.ServletRequest;
import java.util.Map;

/**
 * @author tangbohu
 * @version 1.0.0
 * @ClassName ReqMap.java
 * @Description TODO
 * @createTime 2022年03月21日 19:18:00
 */
public interface ReqMap {


    /**
     * @param request
     * @return
     */
    Map<String, Object> appendResMap(ServletRequest request);

}
