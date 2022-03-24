package com.jd.platform.jlog.client.worker;


import java.util.Collections;
import java.util.List;
import java.util.concurrent.CopyOnWriteArrayList;

/**
 * 存储该客户端支持的worker列表
 * @author wuweifeng
 * @date  2020-01-13
 * @version 1.0
 */
public class WorkerInfoHolder {

    /**
     * 保存worker的ip地址和Channel的映射关系，这是有序的。每次client发送消息时，都会根据该map的size进行hash
     * 如key-1就发送到workerHolder的第1个Channel去，key-2就发到第2个Channel去
     */
    private static final List<String> WORKER_HOLDER = new CopyOnWriteArrayList<>();
    /**
     * 坐标
     */
    private static int index = 0;

    /**
     * 发消息到哪个worker，在这里挑一个
     */
    public static String chooseWorker() {
        String workerIp = "127.0.0.1:9999";

        int size = WORKER_HOLDER.size();
        if (size == 0) {
            return workerIp;
        }
        //按本机ip对worker数量进行hash
//        int index = Math.abs(IpUtils.getIp().hashCode() % size);
        if (index >= WORKER_HOLDER.size()) {
            index = 0;
        }

        try {
            workerIp = WORKER_HOLDER.get(index);
        } catch (Exception e) {
            return WORKER_HOLDER.get(0);
        }

        index++;

        return workerIp;
    }

    /**
     * 监听到worker信息变化后
     * 将新的worker信息和当前的进行合并，并且连接新的address
     * address例子：10.12.139.152:11111
     */
    public static void mergeAndConnectNew(List<String> allAddresses) {
        if (allAddresses.size() == 0) {
            WORKER_HOLDER.clear();
            return;
        }
        //将新的进行排序
        Collections.sort(allAddresses);
        //对比是否两个list是否一样，如果一样就什么也不干
        if (WORKER_HOLDER.equals(allAddresses)) {
            return;
        }
        //变更WORKER_HOLDER
        WORKER_HOLDER.clear();
        WORKER_HOLDER.addAll(allAddresses);
    }

}
