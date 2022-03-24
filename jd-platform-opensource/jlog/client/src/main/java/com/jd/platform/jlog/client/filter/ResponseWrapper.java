package com.jd.platform.jlog.client.filter;

import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpServletResponseWrapper;
import java.io.*;

/**
 * ResponseWrapper
 *
 * @author wuweifeng
 * @version 1.0
 * @date 2021-08-16
 */
public class ResponseWrapper extends HttpServletResponseWrapper {
    /**
     * ByteArrayOutputStream
     */
    private ByteArrayOutputStream buffer;
    /**
     * ServletOutputStream
     */
    private ServletOutputStream out;
    /**
     * writer
     */
    private PrintWriter writer;

    /**
     * ResponseWrapper
     */
    public ResponseWrapper(HttpServletResponse resp) throws IOException {
        super(resp);
        buffer = new ByteArrayOutputStream();//真正存储数据的流
        out = new WrapperOutputStream(buffer);
        writer = new PrintWriter(new OutputStreamWriter(buffer, this.getCharacterEncoding()));
    }

    //重载父类获取outputstream的方法
    @Override
    public ServletOutputStream getOutputStream() throws IOException {
        return out;
    }

    //重载父类获取writer的方法
    @Override
    public PrintWriter getWriter() throws UnsupportedEncodingException {
        return writer;
    }

    //重载父类获取flushBuffer的方法
    @Override
    public void flushBuffer() throws IOException {
        if (out != null) {
            out.flush();
        }
        if (writer != null) {
            writer.flush();
        }
    }

    @Override
    public void reset() {
        buffer.reset();
    }

//    /**
//     * 获取response的字符串
//     */
//    public String getContent() throws IOException {
//        flushBuffer();//将out、writer中的数据强制输出到WapperedResponse的buffer里面，否则取不到数据
//        return new String(buffer.toByteArray());
//    }

    /**
     * 获取response的字符串
     */
    public byte[] getContent() throws IOException {
        flushBuffer();//将out、writer中的数据强制输出到WapperedResponse的buffer里面，否则取不到数据
        return buffer.toByteArray();
    }

    //内部类，对ServletOutputStream进行包装
    private class WrapperOutputStream extends ServletOutputStream {
        /**
         * ByteArrayOutputStream
         */
        private ByteArrayOutputStream bos;

        /**
         * WrapperOutputStream
         */
        WrapperOutputStream(ByteArrayOutputStream stream) {
            bos = stream;
        }

        @Override
        public void write(int b) {
            bos.write(b);
        }

    }
}

