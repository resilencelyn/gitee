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


public interface ExampleActionCont {
    /**
     * bee 模块功能
     */
    interface BeeActionCont {
        /**
         * bee 模块 - 主 cmd
         */
        int cmd = 10;

        int hello = 0;
        int name = 1;
    }

    /**
     * bee 模块功能
     */
    interface BirdActionCont {
        /**
         * bee 模块 - 主 cmd
         */
        int cmd = 11;

        int bird_name = 0;
        int bird_age = 1;
        int test_void = 2;
    }

    interface DemoActionCont {
        /** 主 cmd */
        int cmd = 1;

        int hello = 0;

    }

}
