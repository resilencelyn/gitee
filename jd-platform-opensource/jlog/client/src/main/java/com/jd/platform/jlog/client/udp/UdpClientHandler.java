package com.jd.platform.jlog.client.udp;

import io.netty.channel.ChannelHandler;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;
import io.netty.channel.socket.DatagramPacket;
import io.netty.util.CharsetUtil;

/**
 * 客户端的handler（这里用不上这个类，因为不需要接收服务端的返回值）
 *
 * @author wuweifeng
 * @version 1.0
 * @date 2021-08-10
 */
@ChannelHandler.Sharable
public class UdpClientHandler extends SimpleChannelInboundHandler<DatagramPacket> {

    @Override
    protected void channelRead0(ChannelHandlerContext ctx, DatagramPacket msg) {
        //获得应答，DatagramPacket提供了content()方法取得报文的实际内容
        String response = msg.content().toString(CharsetUtil.UTF_8);
        System.out.println(Thread.currentThread().getName() + "--" + response);
        ctx.close();
    }

    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) {
        cause.printStackTrace();
        ctx.close();
    }
}