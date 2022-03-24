package com.jd.platform.jlog.client.filter;

import com.jd.platform.jlog.client.Context;
import com.jd.platform.jlog.client.cache.ExtParamFactory;
import com.jd.platform.jlog.client.percent.DefaultTracerPercentImpl;
import com.jd.platform.jlog.client.percent.ITracerPercent;
import com.jd.platform.jlog.client.tracerholder.TracerHolder;
import com.jd.platform.jlog.client.udp.UdpSender;
import com.jd.platform.jlog.common.handler.CompressHandler.Outcome;
import com.jd.platform.jlog.common.model.TracerBean;
import com.jd.platform.jlog.common.utils.CollectionUtil;
import com.jd.platform.jlog.common.utils.IdWorker;
import com.jd.platform.jlog.common.utils.IpUtils;
import com.jd.platform.jlog.core.ClientHandler;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import javax.servlet.*;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.*;

import static com.jd.platform.jlog.common.constant.Constant.REQ;
import static com.jd.platform.jlog.common.constant.Constant.RESP;


/**
 * HttpFilter
 * http://blog.chinaunix.net/uid-20783755-id-4729930.html
 *
 * @author wuweifeng
 * @version 1.0
 * @date 2021-08-16
 */
public class HttpFilter implements Filter {
    /**
     * 获取切量百分比的
     */
    private ITracerPercent iTracerPercent;
    private Logger logger = LoggerFactory.getLogger(getClass());


    /**
     * 传入百分比实现类
     */
    public HttpFilter(ITracerPercent iTracerPercent) {
        this.iTracerPercent = iTracerPercent;
    }

    public HttpFilter() {
        iTracerPercent = new DefaultTracerPercentImpl();
    }

    public void setTracerPercent(ITracerPercent iTracerPercent) {
        this.iTracerPercent = iTracerPercent;
    }

    @Override
    public void init(FilterConfig filterConfig) {

    }

    @Override
    public void doFilter(ServletRequest servletRequest, ServletResponse servletResponse, FilterChain filterChain) throws IOException, ServletException {
        try {
            HttpServletRequest httpRequest = (HttpServletRequest) servletRequest;
            HttpServletResponse resp = (HttpServletResponse) servletResponse;
            String uri = httpRequest.getRequestURI().replace("/", "");
            long currentTImeMills = System.currentTimeMillis();

            //设置随机数
            Random random = new Random(currentTImeMills);
            //1-100之间
            int number = random.nextInt(100) + 1;
            //此处要有个开关，控制百分比
            if (iTracerPercent.percent() < number) {
                filterChain.doFilter(servletRequest, servletResponse);
                return;
            }

            //如果是要忽略的接口，就继续执行，不搜集信息
            if (iTracerPercent.ignoreUriSet() != null && iTracerPercent.ignoreUriSet().contains(uri)) {
                filterChain.doFilter(servletRequest, servletResponse);
                return;
            }

            //链路唯一Id
            long tracerId = IdWorker.nextId();
            TracerHolder.setTracerId(tracerId);

            //传输对象基础属性设置
            TracerBean tracerBean = new TracerBean();
            tracerBean.setCreateTime(System.currentTimeMillis());
            List<Map<String, Object>> tracerObject = new ArrayList<>();
            tracerBean.setTracerObject(tracerObject);
            tracerBean.setTracerId(tracerId + "");

            //处理request的各个入参
            dealRequestMap(servletRequest, tracerObject, tracerId, uri);

            //处理response
            dealResponseMap(servletRequest, servletResponse, resp, tracerObject, filterChain);

            //设置耗时
            tracerBean.setCostTime((int) (System.currentTimeMillis() - tracerBean.getCreateTime()));

            //udp发送
            UdpSender.offerBean(tracerBean);
        } catch (Exception e) {
            filterChain.doFilter(servletRequest, servletResponse);
        }
    }

    /**
     * 处理出参相关信息
     */
    private void dealResponseMap(ServletRequest servletRequest, ServletResponse servletResponse, HttpServletResponse resp,
                                 List<Map<String, Object>> tracerObject, FilterChain filterChain) throws IOException, ServletException {
        // 包装响应对象 resp 并缓存响应数据
        ResponseWrapper mResp = new ResponseWrapper(resp);
        filterChain.doFilter(servletRequest, mResp);
        byte[] contentBytes = mResp.getContent();
        String content = new String(contentBytes);
        Map<String, Object> responseMap = new HashMap<>(8);

        Map<String, Object> map = ExtParamFactory.getRespMap(content);
        Outcome outcome = ClientHandler.processResp(contentBytes, map);
        responseMap.put(RESP, outcome.getContent());
        if(CollectionUtil.isNotEmpty(outcome.getTagMap())){
            responseMap.putAll(outcome.getTagMap());
        }
        tracerObject.add(responseMap);

        //此处可以对content做处理,然后再把content写回到输出流中
        servletResponse.setContentLength(-1);
        PrintWriter out = servletResponse.getWriter();
        out.write(content);
        out.flush();
        out.close();
    }

    /**
     * 处理入参相关信息
     */
    private void dealRequestMap(ServletRequest servletRequest, List<Map<String, Object>> tracerObject,
                                long tracerId, String uri) throws IllegalAccessException, InstantiationException {
        //request的各个入参
        Map<String, String[]> params = servletRequest.getParameterMap();
        Map<String, Object> requestMap = new HashMap<>(params.size());
        for (String key : params.keySet()) {
            requestMap.put(key, params.get(key)[0]);
        }
        requestMap.put("appName", Context.APP_NAME);
        requestMap.put("serverIp", IpUtils.getIp());
        requestMap.put("tracerId", tracerId);
        requestMap.put("uri", uri);
        // 自定义的其他的参数对
        requestMap.putAll(ExtParamFactory.getReqMap(servletRequest));

        Outcome out = ClientHandler.processReq(requestMap);
        requestMap.put(REQ, out.getContent());
        if(CollectionUtil.isNotEmpty(out.getTagMap())){
            tracerObject.add(out.getTagMap());
        }
    }

    @Override
    public void destroy() {

    }
}
