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
package com.iohao.little.game.action.skeleton.core.doc;

import cn.hutool.core.io.FileUtil;
import com.iohao.little.game.common.kit.ClassScanner;
import com.thoughtworks.qdox.JavaProjectBuilder;
import com.thoughtworks.qdox.model.JavaClass;
import lombok.experimental.UtilityClass;

import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * @author 洛朱
 * @date 2022-01-28
 */
@UtilityClass
public class ActionCommandDocKit {

    /**
     * java class doc map
     * <pre>
     *     key : java class name (YourJavaFile.class)
     *     value : {@link JavaClassDocInfo}
     * </pre>
     *
     * @param controllerList classList
     * @return map
     */
    public Map<String, JavaClassDocInfo> getJavaClassDocInfoMap(List<Class<?>> controllerList) {
        JavaProjectBuilder javaProjectBuilder = new JavaProjectBuilder();
        final Map<String, JavaClassDocInfo> javaClassDocInfoMap = new HashMap<>();

        for (Class<?> actionClazz : controllerList) {

            try {
                String packagePath = actionClazz.getPackageName();
                ClassScanner classScanner = new ClassScanner(packagePath, null);
                List<URL> resources = classScanner.getResources();

                for (URL resource : resources) {

                    String path = resource.getPath();
                    String srcPath = path.replace("target/classes", "src/main/java");

                    File file = new File(srcPath);
                    if (!FileUtil.exist(file)) {
                        continue;
                    }

                    javaProjectBuilder.addSourceTree(file);
                }

                Collection<JavaClass> classes = javaProjectBuilder.getClasses();

                for (JavaClass javaClass : classes) {

                    JavaClassDocInfo javaClassDocInfo = new JavaClassDocInfo(javaClass);

                    javaClassDocInfoMap.put(javaClass.toString(), javaClassDocInfo);
                }

            } catch (IOException e) {
                e.printStackTrace();
            }

        }

        return javaClassDocInfoMap;

    }

    //    public ActionCommandDoc createActionCommandDoc(JavaClassDocInfo javaClassDocInfo, Method method) {
//
//        if (Objects.isNull(javaClassDocInfo)) {
//            return new ActionCommandDoc();
//        }
//
//        return javaClassDocInfo.createActionCommandDoc(method);
//    }
}
