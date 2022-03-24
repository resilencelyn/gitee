package com.jd.platform.jlog.dashboard.utils;


import com.jd.platform.jlog.dashboard.entity.TracerListVO;

/**
 * 返回值处理类
 * @author shenkaiwen5
 * @version 1.0
 * @date 2021-09-02
 */
public class ResultHelper {
    /**
     * 成功
     */
    public static TracerListVO success(TracerListVO listVO) {
        return success(listVO, 0);
    }

    /**
     * 成功
     */
    public static TracerListVO success(TracerListVO listVO, Integer msg) {
        listVO.setCode(0);
        listVO.setMsg(msg);

        return listVO;
    }

    /**
     * 失败
     */
    public static TracerListVO fail() {
        TracerListVO listVO = new TracerListVO();
        listVO.setCode(1);
        listVO.setMsg(0);

        return listVO;
    }
}
