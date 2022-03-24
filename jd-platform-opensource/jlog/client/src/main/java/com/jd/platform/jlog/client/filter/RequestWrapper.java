package com.jd.platform.jlog.client.filter;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import javax.servlet.ServletInputStream;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletRequestWrapper;
import java.io.*;
import java.util.Collections;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Map;

import static com.alibaba.fastjson.util.IOUtils.UTF8;

/**
 * 对请求入参的包装类
 */
public class RequestWrapper extends HttpServletRequestWrapper {
    /**
     * log
     */
    private static final Logger log = LoggerFactory.getLogger(RequestWrapper.class);
    /**
     * application/json型的入参
     */
    private final String body;
    /**
     * key-value型所有的入参
     */
    private Map<String, String[]> params = new HashMap<>();


    public RequestWrapper(HttpServletRequest request) {
        super(request);
        this.params.putAll(request.getParameterMap());
        StringBuilder stringBuilder = new StringBuilder();
        BufferedReader bufferedReader = null;
        InputStream inputStream = null;
        try {
            inputStream = request.getInputStream();
            if (inputStream != null) {
                bufferedReader = new BufferedReader(new InputStreamReader(inputStream, UTF8));
                char[] charBuffer = new char[512];
                int bytesRead;
                while ((bytesRead = bufferedReader.read(charBuffer)) > 0) {
                    stringBuilder.append(charBuffer, 0, bytesRead);
                }
            }
        } catch (IOException ex) {
            log.error("requestBody read error ignore,message:" + ex.getMessage());
        } finally {
            if (inputStream != null) {
                try {
                    inputStream.close();
                } catch (IOException e) {
                    log.error("requestBody inputStream close error,ignore");
                }
            }
            if (bufferedReader != null) {
                try {
                    bufferedReader.close();
                } catch (IOException e) {
                    log.error("requestBody bufferedReader close error,ignore");
                }
            }
        }
        body = stringBuilder.toString();
    }

    @Override
    public ServletInputStream getInputStream() {
        final ByteArrayInputStream byteArrayInputStream = new ByteArrayInputStream(body.getBytes(UTF8));
        ServletInputStream servletInputStream = new ServletInputStream() {

            @Override
            public int read() throws IOException {
                return byteArrayInputStream.read();
            }
        };
        return servletInputStream;

    }

    @Override
    public BufferedReader getReader() throws IOException {
        return new BufferedReader(new InputStreamReader(this.getInputStream(), UTF8));
    }

    /**
     * 获取requestBody
     */
    public String getBody() {
        return this.body;
    }

    @Override
    public String getParameter(String name) {
        String[] values = params.get(name);
        if (values == null || values.length == 0) {
            return null;
        }
        return values[0];
    }

    @Override
    public String[] getParameterValues(String name) {
        return params.get(name);
    }


    @Override
    public Enumeration<String> getParameterNames() {
        return Collections.enumeration(params.keySet());
    }

}