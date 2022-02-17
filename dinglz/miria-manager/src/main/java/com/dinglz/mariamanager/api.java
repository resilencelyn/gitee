package com.dinglz.mariamanager;

import com.alibaba.fastjson.JSON;
import com.moandjiezana.toml.Toml;
import com.github.kevinsawicki.http.HttpRequest;
import io.swagger.annotations.*;
import net.mamoe.mirai.Bot;
import net.mamoe.mirai.BotFactory;
import net.mamoe.mirai.event.events.FriendMessageEvent;
import net.mamoe.mirai.event.events.GroupMessageEvent;
import net.mamoe.mirai.message.data.MessageChain;
import net.mamoe.mirai.message.data.QuoteReply;
import net.mamoe.mirai.utils.BotConfiguration;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.web.bind.annotation.*;

import java.io.File;
import java.util.ArrayList;
import java.util.List;



class ThreadSendWeb implements Runnable{
    private Thread t;
    private String url;
    private String data;

    public ThreadSendWeb(String url, String data) {
        this.url = url;
        this.data = data;
    }

    public void run() {
        System.out.println("正在向"+url+"投递数据:"+data);
        HttpRequest.get(url,true,"data",data).body();
    }

    public void start() {
        t = new Thread(this);
        t.start();
    }
}

@ApiModel(value = "返回实体类")
class JsonAll<T>
{
    @ApiModelProperty(value = "状态")
    private int status;

    @ApiModelProperty(value = "详细信息")
    private String msg;

    @ApiModelProperty(value = "具体数据")
    private T data;

    public JsonAll(int status, String msg, T data) {
        this.status = status;
        this.msg = msg;
        this.data = data;
    }

    public int getStatus() {
        return status;
    }

    public void setStatus(int status) {
        this.status = status;
    }

    public String getMsg() {
        return msg;
    }

    public void setMsg(String msg) {
        this.msg = msg;
    }

    public T getData() {
        return data;
    }

    public void setData(T data) {
        this.data = data;
    }
}

@ApiModel(value = "版本号")
class VersionData
{
    @ApiModelProperty(value = "版本号")
    private String version;

    public VersionData(String version) {
        this.version = version;
    }

    public String getVersion() {
        return version;
    }

    public void setVersion(String version) {
        this.version = version;
    }
}

@ApiModel("机器人信息")
class BotInfo{
    @ApiModelProperty(value = "机器人账号")
    private Long id;

    @ApiModelProperty(value = "机器人昵称")
    private String nickname;

    public Long getId() {
        return id;
    }

    public void setId(Long id) {
        this.id = id;
    }

    public String getNickname() {
        return nickname;
    }

    public void setNickname(String nickname) {
        this.nickname = nickname;
    }

    public BotInfo(Long id, String nickname) {
        this.id = id;
        this.nickname = nickname;
    }
}

@RestController
@RequestMapping("/api")
@Api(tags = "机器人操作Api")
public class api {

    public List<Bot> MariaBots = new ArrayList<Bot>();

    @Value("${version.data}")
    private String versionData;

    //toml配置文件
    private Toml toml = new Toml().read(new File("setting.toml"));

    private final static Logger logger = LoggerFactory.getLogger(api.class);

    @PostMapping("/version")
    @ApiOperation(value = "获取版本号",tags = "版本")
    public JsonAll version()
    {
        return new JsonAll(0,"ok",new VersionData(versionData));
    }

    @PostMapping("/login")
    @ApiOperation(value = "登录一个机器人",tags = "login")
    public JsonAll LoginAbot(@RequestParam(value = "id") @ApiParam(value = "qq账号") Long qqid,@RequestParam(value = "password") @ApiParam(value = "密码") String password)
    {
        Bot bot = BotFactory.INSTANCE.newBot(qqid, password, new BotConfiguration() {{
            fileBasedDeviceInfo();
            setProtocol(MiraiProtocol.ANDROID_PHONE);
        }});
        bot.login();
        bot.getEventChannel().subscribeAlways(FriendMessageEvent.class,event->{
            MessageChain message = event.getMessage();
            if(message.contentToString().equals("manager"))
            {
                event.getSubject().sendMessage(new QuoteReply(message).plus("MiraiManager "+versionData+"\nCopyRight by 丁丁（dinglz）"));
            }
            String senddata = JSON.toJSONString(new HttpData(message.contentToString(), event.getBot().getId(), event.getFriend().getId()));
            ThreadSendWeb threadSendWeb = new ThreadSendWeb(toml.getString("http.url", "http://localhost:999/")+"friend_msg", senddata);
            threadSendWeb.start();
        });
        bot.getEventChannel().subscribeAlways(GroupMessageEvent.class,event->{
            MessageChain message = event.getMessage();
            String senddata = JSON.toJSONString(new HttpDataGroupMsg(event.getGroup().getId(),event.getBot().getId(),event.getSender().getId(),message.contentToString()));
            ThreadSendWeb threadSendWeb = new ThreadSendWeb(toml.getString("http.url", "http://localhost:999/") + "group_msg", senddata);
            threadSendWeb.start();
        });
        MariaBots.add(bot);
        return new JsonAll(0,"ok","login ok!");
    }

    @PostMapping("/send_friend_msg")
    @ApiOperation(value = "发送好友消息（所有机器人）",tags = "消息")
    public JsonAll SendFriendMsg(@RequestParam(value = "id") @ApiParam(value = "目标qq号") Long id,@RequestParam(value = "msg") @ApiParam(value = "发送消息内容") String sendmsg)
    {
        for(Bot abot:MariaBots)
        {
            abot.getFriend(id).sendMessage(sendmsg);
        }
        return new JsonAll(0,"操作完成","");
    }

    @PostMapping("/send_friend_msg_single")
    @ApiOperation(value = "指定机器人账号发送好友消息（单个机器人）",tags = "消息")
    public JsonAll SendFriendMsgSingle(@RequestParam(value = "id") @ApiParam(value = "目标qq号") Long id,@RequestParam(value = "msg") @ApiParam(value = "发送消息内容") String sendmsg,@RequestParam(value = "use") @ApiParam(value = "使用QQ") Long useid)
    {
        for(Bot abot:MariaBots)
        {
            if(abot.getId()==useid)
            {
                abot.getFriend(id).sendMessage(sendmsg);
                break;
            }
        }
        return new JsonAll(0,"ok","ok");
    }

    @PostMapping("/send_group_msg")
    @ApiOperation(value = "发送群消息（所有机器人）",tags = "消息")
    public JsonAll SendGroupMessage(@RequestParam(value = "id") @ApiParam(value = "群聊账号") Long group_id,@RequestParam(value = "msg") @ApiParam(value = "发送信息内容") String msg)
    {
        for(Bot abot:MariaBots)
        {
            abot.getGroup(group_id).sendMessage(msg);
        }
        return new JsonAll(0,"ok","ok");
    }

    @PostMapping("/send_group_msg_single")
    @ApiOperation(value = "单机器人发送群消息（单个机器人）",tags = "消息")
    public JsonAll SendGroupMessageSingle(@RequestParam(value = "id") @ApiParam(value = "群聊账号") Long group_id,@RequestParam(value = "msg") @ApiParam(value = "发送信息内容") String msg,@RequestParam(value = "use") @ApiParam(value = "机器人账号") Long useid)
    {
        for(Bot abot:MariaBots)
        {
            if(abot.getId()==useid)
            {
                abot.getGroup(group_id).sendMessage(msg);
                break;
            }
        }
        return new JsonAll(0,"ok","ok");
    }

    @PostMapping("/close_bot")
    @ApiOperation(value = "下线一个机器人",tags = "login")
    public JsonAll CloseBot(@RequestParam(value = "id") @ApiParam(value = "下线qq机器人账号") Long id)
    {
        for(Bot abot:MariaBots)
        {
            abot.close();
            MariaBots.remove(abot);
        }
        return new JsonAll(0,"操作完成","logout");
    }

    @PostMapping("/get_all_bot")
    @ApiOperation(value = "获取所有机器人信息",tags = "login")
    public JsonAll GetAllBot()
    {
        List<BotInfo> bots = new ArrayList<BotInfo>();
        for(Bot abot:MariaBots)
        {
            bots.add(new BotInfo(abot.getId(),abot.getNick()));
        }
        return new JsonAll(0,"ok",bots);
    }
}
