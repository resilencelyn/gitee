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
package com.iohao.little.game.action.skeleton.core.action;

import com.alipay.remoting.BizContext;
import com.iohao.little.game.action.skeleton.annotation.ActionController;
import com.iohao.little.game.action.skeleton.annotation.ActionMethod;
import com.iohao.little.game.action.skeleton.core.action.pojo.Bird;
import lombok.extern.slf4j.Slf4j;

import java.util.Collections;
import java.util.List;

@Slf4j
@ActionController(ExampleActionCont.BirdActionCont.cmd)
public class BirdAction {
    /**
     * <pre>
     *     打招呼
     *     实现注解 ActionMethod 告知框架这是一个对外开放的action (既一个方法就是一个对外的处理)
     * </pre>
     *
     * @param bird a
     * @return 返回具体信息
     */
    @ActionMethod(ExampleActionCont.BirdActionCont.bird_name)
    public String hello(List<Bird> birdList, Bird bird) {
        log.debug("bird: {}", bird, birdList);
        // 响应给客户端的数据 string 类型. 框架可根据返回参数类型将返回结果装到响应体中
        return " : BIRD_NAME hello: ";
    }

    @ActionMethod(ExampleActionCont.BirdActionCont.bird_age)
    public List<Bird> name(Bird bird, BizContext bizContext) {
        log.debug("bird: {}", bird);
        // 响应给客户端的数据 string 类型. 框架可根据返回参数类型将返回结果装到响应体中
//        return " : BIRD_AGE name: ";
        return Collections.emptyList();
    }

    @ActionMethod(ExampleActionCont.BirdActionCont.test_void)
    public void testVoid() {
        System.out.println("hello void");
    }
}
