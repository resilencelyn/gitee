package com.jd.platform.jlog.common.utils;

import org.xerial.snappy.Snappy;

import java.io.IOException;
import java.nio.charset.StandardCharsets;

/**
 * snappy压缩工具类
 *
 * @author wuweifeng
 * @version 1.0
 * @date 2021-08-16
 */
public class SnappyUtils {
    /**
     * 压缩
     */
    public static byte[] compress(String html) {
        return compress(html.getBytes(StandardCharsets.UTF_8));
    }

    /**
     * 压缩
     */
    public static byte[] compress(byte[] bytes) {
        try {
            return Snappy.compress(bytes);
        } catch (IOException e) {
            e.printStackTrace();
            return null;
        }
    }

    /**
     * 解压
     */
    public static String decompress(byte[] bytes) {
        try {
            return new String(Snappy.uncompress(bytes));
        } catch (IOException e) {
            e.printStackTrace();
            return null;
        }
    }
}
