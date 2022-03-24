package com.jd.platform.jlog.client.udp;

import com.jd.platform.jlog.client.worker.WorkerInfoHolder;
import com.jd.platform.jlog.common.utils.AsyncPool;
import okhttp3.*;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.atomic.AtomicLong;

/**
 * Http对外发消息
 *
 * @author shenkaiwen5
 * @version 1.0
 * @date 2021-10-14
 */
public class HttpSender {
    /**
     * logger
     */
    private static Logger logger = LoggerFactory.getLogger(HttpSender.class);
    /**
     * 本地大对象队列满了后丢弃的数量
     */
    private static AtomicLong FAIL_OFFER_COUNT = new AtomicLong();
    /**
     * 本地大对象队列，已写入的总数量
     */
    private static AtomicLong SUCCESS_OFFER_COUNT = new AtomicLong();
    /**
     * okHttp客户端
     */
    private static OkHttpClient client = new OkHttpClient();

    /**
     * 出入参集中营，最多积压5万个
     */
    private static LinkedBlockingQueue<OneTracer> tracerDataQueue = new LinkedBlockingQueue<>(500);

    /**
     * 写入队列
     */
    public static void offerBean(byte[] compressBytes) {
        OneTracer oneTracer = new OneTracer();
        oneTracer.setBytes(compressBytes);
        //容量是否已满
        boolean success = tracerDataQueue.offer(oneTracer);
        if (!success) {
            long failCount = FAIL_OFFER_COUNT.incrementAndGet();
            if (failCount % 10 == 0) {
                logger.info("用户跟踪大对象队列已满，当前丢弃的数量为: " + failCount);
            }
        } else {
            long successCount = SUCCESS_OFFER_COUNT.incrementAndGet();
            if (successCount % 1000 == 0) {
                logger.info("用户跟踪大对象已产生数量：" + successCount + "，当前大对象队列积压数量：" + tracerDataQueue.size());
            }
        }
    }

    /**
     * 定时往worker发烧
     */
    public static void uploadToWorker() {
        //filter拦截到的出入参
        AsyncPool.asyncDo(() -> {
            while (true) {
                try {
                    List<OneTracer> tempTracers = new ArrayList<>();
                    //TODO 要不要多攒几个
                    OneTracer tracerBean = tracerDataQueue.take();
                    tempTracers.add(tracerBean);

                    send(tempTracers);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });
    }

    /**
     * 发送OkHttp请求
     */
    private static void send(List<OneTracer> tempTracers) {
        for (OneTracer one: tempTracers) {
            deal(one.getBytes());
        }
    }

    public static void main(String[] args) {
        deal(new byte[5]);
    }


    private static void deal(byte[] compressBytes) {
        /**
         * 所有HTTP请求的代理设置，超时，缓存设置等都需要在OkHttpClient中设置。 如果需要更改一个请求的配置，可以使用
         * OkHttpClient.newBuilder()获取一个builder对象，
         * 该builder对象与原来OkHttpClient共享相同的连接池，配置等。
         */
        client = client.newBuilder().build();

        RequestBody requestBody = RequestBody.create(MediaType.parse("application/octet-stream"), compressBytes);
        MultipartBody multipartBody = new MultipartBody.Builder()
                .setType(MultipartBody.FORM)
                .addFormDataPart("data", "data", requestBody)
                .build();

        //挑一个worker
        String rawIpPort = WorkerInfoHolder.chooseWorker();
        String ipPort = rawIpPort.substring(0, rawIpPort.lastIndexOf(":")) + ":8080";
        String url = "http://" + ipPort + "/big/receive";

        Request request = new Request.Builder()
                .url(url)
                .post(multipartBody)
                .build();
        Call call = client.newCall(request);
        call.enqueue(new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                logger.error("HttpSender  onFailure", e);
                call.cancel();
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                response.body().close();
            }
        });
    }
}
