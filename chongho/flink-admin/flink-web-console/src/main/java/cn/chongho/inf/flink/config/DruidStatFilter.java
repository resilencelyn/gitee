package cn.chongho.inf.flink.config;

import com.alibaba.druid.support.http.WebStatFilter;

import javax.servlet.annotation.WebFilter;
import javax.servlet.annotation.WebInitParam;

@WebFilter(filterName="druidWebStatFilter",urlPatterns="/*",
initParams={
    @WebInitParam(name="exclusions",value="*.js,*.gif,*.jpg,*.bmp,*.png,*.css,*.ico,/druid/*")
})
/**
 * Druid拦截器，用于查看Druid监控
 * @author feihu.wang
 * @since 2022-02-14
 */
public class DruidStatFilter extends WebStatFilter {

}
