package com.jd.platform.jlog.common.handler;

/**
 * @author tangbohu
 * @version 1.0.0
 * @ClassName CollectMode.java
 * @Description 定义采集模式的组合 用于以后method级别的tag注解
 * @createTime 2022年02月24日 23:10:00
 */

public class CollectMode {

    /**
     * 挂起 不提取不压缩
     */
    public static final long SUSPEND = 0L;

    /**
     * 提取req
     */
    public static final long E_REQ = 1L;

    /**
     * 压缩req
     */
    public static final long C_REQ = 1L << 2;

    /**
     * 提取log
     */
    public static final long E_LOG = 1L << 3;

    /**
     * 压缩log
     */
    public static final long C_LOG = 1L << 4;

    /**
     * 提取resp
     */
    public static final long E_RESP = 1L << 5;

    /**
     * 压缩resp
     */
    public static final long C_RESP = 1L << 6;



    /**  =======================================  下面是组合  ======================================= */



    /**
     * 提取req+log
     */
    public static final long EXTRACT_REQ_LOG = E_REQ | E_LOG;

    /**
     * 提取req+resp
     */
    public static final long EXTRACT_REQ_REQP = E_REQ | E_RESP;

    /**
     * 提取req+resp
     */
    public static final long EXTRACT_REQ_RESP = E_REQ | E_RESP;

    /**
     * 提取req+log+resp
     */
    public static final long EXTRACT_ALL = E_REQ | E_LOG | E_RESP;


    /**
     * 压缩req+log
     */
    public static final long COMPRESS_REQ_LOG = C_REQ | C_LOG;

    /**
     * 压缩req+resp
     */
    public static final long COMPRESS_REQ_REQP = C_REQ | C_RESP;

    /**
     * 压缩log+resp
     */
    public static final long COMPRESS_LOG_RESP = C_REQ | C_RESP;

    /**
     * 压缩req+log+resp
     */
    public static final long COMPRESS_ALL = C_REQ | C_LOG | C_RESP;


    public static boolean isMatched(long indicator, long position) {
        return (indicator & position) == position;
    }

}
