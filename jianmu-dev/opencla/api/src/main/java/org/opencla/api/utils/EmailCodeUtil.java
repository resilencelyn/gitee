package org.opencla.api.utils;

import java.util.Random;

/**
 * 短信工具类
 */
public final class EmailCodeUtil {

    /**
     * 生成随机验证码（6位）
     *
     * @return
     */
    public static String generateRandomCode() {
        StringBuilder sb = new StringBuilder();
        Random rand = new Random();
        for (int i = 0; i < 6; i++) {
            sb.append(rand.nextInt(9));
        }
        return sb.toString();
    }
}
