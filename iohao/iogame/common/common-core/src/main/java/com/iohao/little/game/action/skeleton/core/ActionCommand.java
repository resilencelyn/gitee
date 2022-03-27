/*
 * # iohao.com . 渔民小镇
 * Copyright (C) 2021 - 2022 double joker （262610965@qq.com） . All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License..
 */
package com.iohao.little.game.action.skeleton.core;

import cn.hutool.core.util.StrUtil;
import com.esotericsoftware.reflectasm.ConstructorAccess;
import com.esotericsoftware.reflectasm.MethodAccess;
import com.iohao.little.game.action.skeleton.core.doc.ActionCommandDoc;
import com.iohao.little.game.action.skeleton.core.flow.FlowContext;
import lombok.AccessLevel;
import lombok.Getter;
import lombok.Setter;
import lombok.experimental.Accessors;
import lombok.experimental.FieldDefaults;

import java.lang.reflect.Method;
import java.lang.reflect.ParameterizedType;
import java.util.Arrays;
import java.util.List;

/**
 * 命令模式
 * <pre>
 * 将“请求”封装成对象，以便使用不同的请求、队列或者日志来参数化其他对象。命令模式也支持可撤销的操作。
 * 这个模式允许我们将动作封装成命令对象，这样一来就可以随心所欲地存储、传递和调用它们。
 * </pre>
 * 要点：
 * <pre>
 * 命令模式将发出请求的对象和执行请求的对象解耦。
 * 在被解耦的两者之间是通过命令对象进行沟通的。命令对象封装了接收者和一个或一组动作。
 * 调用者通过调用命令对象的execute()发出请求，这会使得接收者的动作被调用。
 * 调用者可以接受命令当作参数，甚至在运行时动态地进行。
 * 命令可以支持撤销，做法是实现一个undo()方法来回到execute()被执行前的状态。
 * 宏命令是命令的一种简单的延伸，允许调用多个命令。宏方法也可以支持撤销。
 * 实际操作时，很常见使用“聪明”命令对象，也就是直接实现了请求，而不是将工作委托给接收者。
 * 命令也可以用来实现日志和事物系统。
 * 这里把每个控制器中的方法封装成独立的命令. 用来处理客户端的请求
 * </pre>
 * <pre>
 * 这个类收集了类方法的详细信息
 * </pre>
 *
 * @author 洛朱
 * @Date 2021-12-12
 */
@Getter
public final class ActionCommand {
    /** cmdInfo */
    final CmdInfo cmdInfo;
    /** 构造方法器 */
    final ConstructorAccess<?> actionControllerConstructorAccess;
    /** 一个single控制器对象 */
    final Object actionController;
    /** 方法所在 class */
    final Class<?> actionControllerClazz;
    /** 默认tcp对象是single. 如果设置为false, 每次创建新的tcp对象. 默认:true */
    final boolean createSingleActionCommandController;

    /** 方法对象 */
    final Method actionMethod;
    /** 方法名 */
    final String actionMethodName;
    /** 方法下标 */
    final int actionMethodIndex;
    /** 方法访问器 */
    final MethodAccess actionMethodAccess;

    /** 方法参数信息 数组 */
    final ParamInfo[] paramInfos;
    /** 方法是否有参数: true 有参数 */
    final boolean hasMethodParam;
    /** 方法是否有异常抛出, 一般是错误码: true 有异常 */
    final boolean throwException;
    /** 返回类型 */
    final ActionMethodReturnInfo actionMethodReturnInfo;

    final ActionCommandDoc actionCommandDoc;

    /** 打印信息 */
    final String toStringInfo;

    /** true 表示交付给容器来管理 如 spring 等 */
    boolean deliveryContainer;

    private ActionCommand(Builder builder, BarSkeletonSetting barSkeletonSetting) {
        // -------------- 路由相关 --------------
        this.cmdInfo = CmdInfoFlyweightFactory.me().getCmdInfo(builder.cmd, builder.subCmd);

        // -------------- 控制器相关 --------------
        this.actionControllerClazz = builder.actionControllerClazz;
        this.actionControllerConstructorAccess = builder.actionControllerConstructorAccess;
        this.actionController = this.actionControllerConstructorAccess.newInstance();
        this.createSingleActionCommandController = barSkeletonSetting.createSingleActionCommandController;

        // -------------- 控制器-方法相关 --------------
        this.actionMethod = builder.actionMethod;
        this.actionMethodName = builder.actionMethodName;
        this.actionMethodIndex = builder.actionMethodIndex;
        this.actionMethodAccess = builder.actionMethodAccess;

        // -------------- 控制器-方法参数相关 --------------
        this.paramInfos = builder.paramInfos;
        this.hasMethodParam = builder.paramInfos != null;
        this.throwException = builder.actionMethod.getExceptionTypes().length != 0;
        this.actionMethodReturnInfo = new ActionMethodReturnInfo(builder);

        this.actionCommandDoc = builder.actionCommandDoc;

        this.deliveryContainer = builder.deliveryContainer;

        this.toStringInfo = info();
    }

    public Object getActionController() {
        return actionController;
    }

    private String info() {
        String template = "ActionCommand(parameters={},clazz={}, methodName={}, hasThrowException={}, hasMethodParam={})";
        return StrUtil.format(template
                , Arrays.deepToString(paramInfos)
                , actionControllerClazz
                , actionMethodName
                , throwException
                , hasMethodParam
        );
    }

    /**
     * {@link ActionCommand} 命令的构建器
     * 因为 {@link ActionCommand} 遇到多个构造器参数, 这里考虑用构建器.
     */
    @Accessors(chain = true)
    @Setter
    @FieldDefaults(level = AccessLevel.PUBLIC)
    static final class Builder {
        /** 目标路由 */
        int cmd;
        /** 目标子路由 */
        int subCmd;
        /** 方法访问器 */
        MethodAccess actionMethodAccess;
        /** 类访问器 */
        ConstructorAccess<?> actionControllerConstructorAccess;
        /** 方法名 */
        String actionMethodName;
        /** tcp controller类 */
        Class<?> actionControllerClazz;
        /** 方法对象 */
        Method actionMethod;
        /** 参数列表信息 */
        ActionCommand.ParamInfo[] paramInfos;
        /** 方法下标 (配合 MethodAccess 使用) */
        int actionMethodIndex;
        /** 返回值信息 */
        Class<?> returnTypeClazz;
        /** action command 文档 */
        ActionCommandDoc actionCommandDoc;
        /** true 表示交付给容器来管理 如 spring 等 */
        boolean deliveryContainer;

        ActionCommand build(BarSkeletonSetting barSkeletonSetting) {
            return new ActionCommand(this, barSkeletonSetting);
        }
    }

    /**
     * 方法参数信息
     */
    @Getter
    public static final class ParamInfo {
        /** 参数名 */
        String name;
        /** 参数下标 */
        int index;
        /** 参数类型 */
        Class<?> paramClazz;
        /** true : 参数开启 JSR 303、JSR 349、JSR 380 验证规范 */
        boolean validator;

        ParamInfo() {
        }

        public String toStringShort() {
            return paramClazz.getSimpleName() + " " + name;
        }

        @Override
        public String toString() {
            return paramClazz.getName() + " " + name;
        }

        /**
         * 是否扩展属性
         *
         * @return true 是扩展属性
         */
        public boolean isExtension() {
            return FlowContext.class.equals(paramClazz);
        }
    }


    /**
     * action 方法参数返回值信息
     */
    @Getter
    @FieldDefaults(level = AccessLevel.PRIVATE)
    public static final class ActionMethodReturnInfo {
        /** 返回类型 */
        final Class<?> returnTypeClazz;
        /** list 泛型的类型 */
        final Class<?> actualTypeArgumentClazz;

        private ActionMethodReturnInfo(ActionCommand.Builder builder) {

            this.returnTypeClazz = builder.returnTypeClazz;

            if (List.class.isAssignableFrom(returnTypeClazz)) {
                ParameterizedType genericReturnType = (ParameterizedType) builder.actionMethod.getGenericReturnType();
                this.actualTypeArgumentClazz = (Class<?>) genericReturnType.getActualTypeArguments()[0];
            } else {
                this.actualTypeArgumentClazz = null;
            }
        }

        /**
         * 方法返回值类型是否 void
         *
         * @return true 是 void
         */
        public boolean isVoid() {
            return Void.TYPE == this.returnTypeClazz;
        }
    }
}
