package com.jd.platform.jlog.dashboard.utils;

import com.jd.platform.jlog.dashboard.Starter;
import lombok.Data;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.io.Serializable;
import java.util.Random;
import java.util.concurrent.Delayed;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.ReentrantLock;

/**
 * 拉高cpu的工具类
 * @author shenkaiwen5
 * @version 1.0
 * @date 2021-11-09
 */
@Data
public class TaskUtil implements Runnable{

    /**
     * 开关
     */
    private int cpuOn = 1;

    /**
     * 重复次数，用于控制cpu
     */
    private int repeat = 100000;

    /**
     * logger
     */
    private Logger logger = LoggerFactory.getLogger(TaskUtil.class);

    @Data
    class DashboardTracerWrapper implements Serializable {
        public byte[] bytes;
    }

    private ReentrantLock lock = new ReentrantLock();

    @Override
    public void run() {
        if (cpuOn <= 0) {
            return;
        }

        String fileName="employee.ser";

        //执行10w次
        for (int i = 0; i < repeat; i++) {
            wrapper.setBytes(genRequest().getBytes());
            try {
                SerializationUtil.serialize(wrapper, fileName);
                wrapper = (DashboardTracerWrapper)SerializationUtil.deserialize(fileName);

                DashboardTracerWrapper wrapper1 = new DashboardTracerWrapper();
                wrapper1.setBytes(ZstdUtils.compress(wrapper.getBytes().clone()));
                byte[] bytes1 = wrapper1.getBytes();
                ZstdUtils.decompress(bytes1);

            } catch (IOException e) {
                e.printStackTrace();
            } catch (ClassNotFoundException e) {
                e.printStackTrace();
            }
        }

        lock.lock();
        try {
            for (int i = 0; i < 1; i++) {
                System.out.println("一条数据的HBase之旅，简明HBase入门教程-开篇");

                Starter.queue.put(new FakeTask());
            }
        } finally {
            lock.unlock();
        }

        logger.info("完成一次log");

    }


    public static void main(String[] args) {
        new Thread(new TaskUtil()).start();
    }

    private DashboardTracerWrapper wrapper = new DashboardTracerWrapper();;

    /**
     * 生成请求
     */
    private String genRequest() {
        StringBuilder stringBuilder = new StringBuilder();
        for (int i = 0; i < 8; i++) {
            stringBuilder.append("2018年3月21日 Jaison\n" +
                    "一条数据的HBase之旅，简明HBase入门教程-开篇\n" +
                    "这是HBase入门系列的第1篇文章，介绍HBase的数据模型、适用场景、集群关键角色、建表流程以及所涉及的HBase基础概念，本文内容基于HBase 2.0 beta2版本。本文既适用于HBase新手，也适用于已有一定经验的HBase开发人员。\n" +
                    "\n" +
                    "\n" +
                    "一些常见的HBase新手问题\n" +
                    "\n" +
                    "什么样的数据适合用HBase来存储？\n" +
                    "既然HBase也是一个数据库，能否用它将现有系统中昂贵的Oracle替换掉？\n" +
                    "存放于HBase中的数据记录，为何不直接存放于HDFS之上？\n" +
                    "能否直接使用HBase来存储文件数据？\n" +
                    "Region(HBase中的数据分片)迁移后，数据是否也会被迁移？\n" +
                    "为何基于Spark/Hive分析HBase数据时性能较差？\n" +
                    "2018年3月21日 Jaison\n");
        }
        return stringBuilder.toString();
    }

    class FakeTask implements Delayed {

        private  Random random = new Random();

        FakeTask() {
            String fileName="employee1.ser";
            try {
                SerializationUtil.serialize(wrapper, fileName);
                wrapper = (DashboardTracerWrapper)SerializationUtil.deserialize(fileName);
            } catch (IOException e) {
                e.printStackTrace();
            } catch (ClassNotFoundException e) {
                e.printStackTrace();
            }
        }

        @Override
        public long getDelay(TimeUnit unit) {
            return random.nextInt() % 125;
        }

        @Override
        public int compareTo(Delayed o) {
            return random.nextInt(65536)-32768;
        }
    }
}
