/*
 * # iohao.com . 渔民小镇
 * Copyright (C) 2021 - 2022 double joker （262610965@qq.com） . All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.iohao.little.game.net.external.session;

import com.iohao.little.game.action.skeleton.protocol.RequestMessage;
import io.netty.channel.Channel;
import lombok.AccessLevel;
import lombok.Getter;
import lombok.Setter;
import lombok.ToString;
import lombok.experimental.Accessors;
import lombok.experimental.FieldDefaults;

import java.net.InetSocketAddress;
import java.util.Objects;

/**
 * 用户的唯一 session 信息
 * <pre>
 *     与 channel 是 1:1 的关系，可取到对应的 userId、channel 等信息
 *
 *     关于用户管理 UserSessions 和 UserSession 可以参考这里：
 *     https://www.yuque.com/iohao/game/wg6lk7
 * </pre>
 *
 * @author 洛朱
 * @date 2022-03-15
 */
@Getter
@ToString
@Accessors(chain = true)
@FieldDefaults(level = AccessLevel.PRIVATE)
public class UserSession {
    /** userId */
    long userId;
    /** user channel */
    final Channel channel;
    /** 用户状态 */
    @Setter
    UserSessionState state;

    public UserSession(Channel channel) {
        this.channel = channel;

        // false 没有进行身份验证
        this.channel.attr(UserSessionAttr.verifyIdentity).set(false);
        // channel 中也保存 UserSession 的引用
        this.channel.attr(UserSessionAttr.userSession).set(this);

        this.state = UserSessionState.ACTIVE;
    }

    public void employ(RequestMessage requestMessage) {

        if (!this.isVerifyIdentity()) {
            // 只有没进行验证的，才给 userChannelId
            String channelId = this.getChannelId();
            requestMessage.setUserChannelId(channelId);
        }

        // 设置请求用户的id
        requestMessage.setUserId(this.userId);
    }

    public void setUserId(long userId) {
        this.userId = userId;
        this.channel.attr(UserSessionAttr.verifyIdentity).set(true);
    }

    public UserChannelId getUserChannelId() {
        String channelId = this.getChannelId();
        return new UserChannelId(channelId);
    }

    /**
     * 获取玩家ip
     *
     * @return 获取玩家ip
     */
    public String getIp() {
        if (Boolean.FALSE.equals(isActiveChannel())) {
            return "";
        }

        InetSocketAddress inetSocketAddress = (InetSocketAddress) channel.remoteAddress();
        return inetSocketAddress.getHostString();
    }


    private String getChannelId() {
        return this.channel.id().asLongText();
    }

    public boolean isVerifyIdentity() {
        return this.channel.attr(UserSessionAttr.verifyIdentity).get();
    }

    public boolean isActiveChannel() {
        return Objects.nonNull(channel) && channel.isActive();
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }

        if (!(o instanceof UserSession that)) {
            return false;
        }

        return userId == that.userId;
    }

    @Override
    public int hashCode() {
        return Objects.hash(userId);
    }
}
