package com.orion.ops.consts;

/**
 * redis key 常量
 *
 * @author Jiahang Li
 * @version 1.0.0
 * @since 2021/4/2 16:44
 */
public class KeyConst {

    private KeyConst() {
    }

    /**
     * 登陆 token
     * <p>
     * auth:login:{id}
     */
    public static final String LOGIN_TOKEN_KEY = "auth:info:{}";

    /**
     * 登陆token绑定 token
     * <p>
     * auth:login:{id}:{timestamp}
     */
    public static final String LOGIN_TOKEN_BIND_KEY = "auth:bind:{}:{}";

    /**
     * terminal 访问 token
     * <p>
     * terminal:access:{token}
     */
    public static final String TERMINAL_ACCESS_TOKEN = "terminal:access:{}";

    /**
     * 5min
     */
    public static final int TERMINAL_ACCESS_TOKEN_EXPIRE = 60 * 5;

    /**
     * terminal 绑定 token
     * <p>
     * terminal:bind:{token}
     */
    public static final String TERMINAL_BIND_TOKEN = "terminal:bind:{}";

    /**
     * 6 h
     */
    public static final int TERMINAL_BIND_EXPIRE = 60 * 60 * 6;

    /**
     * 文件 tail 访问 token
     * <p>
     * file:tail:access{token}
     */
    public static final String FILE_TAIL_ACCESS_TOKEN = "file:tail:access:{}";

    /**
     * 5 min
     */
    public static final int FILE_TAIL_ACCESS_EXPIRE = 60 * 5;

    /**
     * 文件 tail 绑定 token
     * <p>
     * file:tail:bind:{id}
     */
    public static final String FILE_TAIL_BIND_TOKEN = "file:tail:bind:{}";

    /**
     * 6 h
     */
    public static final int FILE_TAIL_BIND_EXPIRE = 60 * 60 * 6;

    /**
     * 文件下载 token
     * <p>
     * file:download:{token}
     */
    public static final String FILE_DOWNLOAD_TOKEN = "file:download:{}";

    /**
     * 5 min
     */
    public static final int FILE_DOWNLOAD_EXPIRE = 60 * 5;

    /**
     * sftp 会话 token
     * <p>
     * sftp:session:{token}
     */
    public static final String SFTP_SESSION_TOKEN = "sftp:session:{}";

    /**
     * 12 h
     */
    public static final int SFTP_SESSION_EXPIRE = 60 * 60 * 12;

    /**
     * sftp 上传请求 token
     * <p>
     * sftp:upload:{token}
     */
    public static final String SFTP_UPLOAD_ACCESS_TOKEN = "sftp:upload:{}";

    /**
     * 5 h
     */
    public static final int SFTP_UPLOAD_ACCESS_EXPIRE = 60 * 60 * 5;

    /**
     * 主页统计数量 key
     * <p>
     * data:statistics:home:count
     */
    public static final String HOME_STATISTICS_COUNT_KEY = "data:statistics:home:count";

    /**
     * 调度任务统计 key
     * <p>
     * data:statistics:scheduler:task:{id}
     */
    public static final String SCHEDULER_TASK_STATISTICS_KEY = "data:statistics:scheduler:task:{}";

    /**
     * 应用构建统计 key
     * <p>
     * data:statistics:app:build:{profileId}:{appId}
     */
    public static final String APP_BUILD_STATISTICS_KEY = "data:statistics:app:build:{}:{}";

    /**
     * 应用发布统计 key
     * <p>
     * data:statistics:app:release:{profileId}:{appId}
     */
    public static final String APP_RELEASE_STATISTICS_KEY = "data:statistics:app:release:{}:{}";

    /**
     * 环境缓存 key
     * <p>
     * data:profile
     */
    public static final String DATA_PROFILE_KEY = "data:profile";

    /**
     * 30 min
     */
    public static final int DATA_PROFILE_EXPIRE = 60 * 30;

}
