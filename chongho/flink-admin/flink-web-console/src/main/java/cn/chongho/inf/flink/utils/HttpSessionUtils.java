package cn.chongho.inf.flink.utils;

import cn.chongho.inf.flink.model.AdminUser;

import javax.servlet.http.HttpSession;

/**
 * @author ming
 */
public class HttpSessionUtils {

    public static AdminUser getLoginUser(HttpSession session){
        return (AdminUser) session.getAttribute("loginUser");
    }
}
