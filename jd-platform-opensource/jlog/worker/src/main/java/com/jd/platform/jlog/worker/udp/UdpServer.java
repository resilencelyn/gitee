package com.jd.platform.jlog.worker.udp;

import com.jd.platform.jlog.worker.disruptor.Producer;
import com.jd.platform.jlog.worker.store.TracerBeanStore;
import io.netty.bootstrap.Bootstrap;
import io.netty.buffer.ByteBuf;
import io.netty.channel.*;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.DatagramPacket;
import io.netty.channel.socket.nio.NioDatagramChannel;
import io.netty.handler.codec.MessageToMessageDecoder;
import org.springframework.stereotype.Component;

import javax.annotation.Resource;
import java.util.Arrays;
import java.util.List;

/**
 * server接收器
 * https://zhuanlan.zhihu.com/p/33300266
 *
 * @author wifeng
 * @version 1.0
 * @date 2021-08-10
 */
@Component
public class UdpServer {
    /**
     * 信息存储器
     */
    @Resource
    private TracerBeanStore tracerBeanStore;
    /**
     * producer
     */
    @Resource
    private Producer producer;

    /**
     * 启动server监听器
     */
    public void startServer() {
        //1.NioEventLoopGroup是执行者
        NioEventLoopGroup group = new NioEventLoopGroup(1);
        //2.启动器
        Bootstrap bootstrap = new Bootstrap();
        //3.配置启动器
        bootstrap.group(group)//3.1指定group
                .channel(NioDatagramChannel.class)//3.2指定channel
                .option(ChannelOption.RCVBUF_ALLOCATOR, new FixedRecvByteBufAllocator(65535))
                .handler(new ChannelInitializer<NioDatagramChannel>() {
                    @Override
                    protected void initChannel(NioDatagramChannel nioDatagramChannel) {
                        //3.4在pipeline中加入解码器，和编码器（用来发送UDP）
                        nioDatagramChannel.pipeline().addLast(new TracerBeanDecoder());
                    }
                });
        try {
            //4.bind到指定端口，并返回一个channel，该端口就是监听UDP报文的端口
            Channel channel = bootstrap.bind(9999).sync().channel();
            //5.等待channel的close
            channel.closeFuture().sync();
            //6.关闭group
            group.shutdownGracefully();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    /**
     * 解码器，解析客户端发来的对象
     */
    private class TracerBeanDecoder extends MessageToMessageDecoder<DatagramPacket> {
        @Override
        protected void decode(ChannelHandlerContext channelHandlerContext, DatagramPacket datagramPacket, List<Object> list) {
            ByteBuf buf = datagramPacket.content();

            byte[] body = new byte[buf.readableBytes()];
            buf.readBytes(body);
            producer.sendData(body);
        }
    }

}