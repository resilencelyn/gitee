package com.jd.platform.jlog.client.cache;

import javax.servlet.ServletResponse;
import java.util.Map;


/**
 * @author tangbohu
 * @version 1.0.0
 * @ClassName RespMap.java
 * @Description TODO
 * @createTime 2022年03月21日 19:18:00
 */
public interface RespMap {


    /**
     *
     * @param cnt
     * @return
     */
    Map<String, Object> appendRespMap(String cnt);
}
