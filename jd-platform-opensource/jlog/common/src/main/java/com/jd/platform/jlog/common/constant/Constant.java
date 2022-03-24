package com.jd.platform.jlog.common.constant;

import java.util.HashSet;
import java.util.Set;

/**
 * 常量工具类Constant
 *
 * @author wuweifeng wrote on 2019-12-05
 * @version 1.0
 */
public class Constant {
    /**
     * netty通信端口
     */
    public static int NETTY_PORT = 9999;
    /**
     * 冒号
     */
    public static String SPLITER = ":";
    /**
     * 所有的workers，存这里
     */
    public static String WORKER_PATH = "/userTracer/workers/";

    /**
     * 当客户端要删除某个key时，就往etcd里赋值这个value，设置1秒过期，就算删除了
     */
    public static String DEFAULT_DELETE_VALUE = "#[DELETE]#";


    public static byte[] DEFAULT_BYTE = "default".getBytes();


    public static final String REQ = "request";

    public static final String RESP = "response";


    /**
     * 普通tag最大长度 超过则不会单独存储，不超过则会存入tag：{"normal" : "logContent"}
     */
    public static int TAG_NORMAL_KEY_MAX_LEN = 20;

    /**
     * 可供提取的日志最小长度
     */
    public static int EXTRACT_MIN_LEN = 5;

    /**
     * 符合正则但不构成kv结构的普通日志，如「xx查询为空」
     */
    public static final String TAG_NORMAL_KEY = "normal";

    /**
     * 小于1000不考虑压缩
     */
    public static final long MIN = 1000;


    /**
     * 默认大于10000才考虑压缩
     */
    public static final long THRESHOLD = 10000;


    /**
     * 正则里需要特殊处理的字符串
     */
    public static final Set<String> SPECIAL_CHAR = new HashSet<String>() {
        {
            add("*");
            add(".");
            add("?");
            add("+");
            add("$");
            add("^");
            add("[");
            add("]");
            add("(");
            add(")");
            add("{");
            add("}");
            add("|");
        }
    };

}
