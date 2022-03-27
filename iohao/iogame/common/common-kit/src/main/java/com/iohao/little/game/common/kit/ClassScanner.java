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
package com.iohao.little.game.common.kit;

import lombok.Data;

import java.io.File;
import java.io.IOException;
import java.net.JarURLConnection;
import java.net.URL;
import java.net.URLConnection;
import java.net.URLDecoder;
import java.nio.charset.StandardCharsets;
import java.util.*;
import java.util.function.Predicate;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;

/**
 * class 扫描
 *
 * @author 洛朱
 * @Date 2021-12-12
 */
@Data
public class ClassScanner {
    // 需要扫描的包名
    final String packagePath;
    // 存放扫描过的 clazz
    final Set<Class<?>> clazzSet = new HashSet<>();

    /**
     * true 保留符合条件的class
     */
    final Predicate<Class<?>> predicateFilter;

    ClassLoader classLoader;

    /**
     * 扫描
     *
     * @param packagePath     扫描路径
     * @param predicateFilter 过滤条件
     */
    public ClassScanner(String packagePath, Predicate<Class<?>> predicateFilter) {
        this.predicateFilter = predicateFilter;

        var path = packagePath.replace('.', '/');
        path = path.endsWith("/") ? path : path + '/';

        this.packagePath = path;
    }

    public List<Class<?>> listScan() {
        try {
            this.initClassLoad();

            List<URL> urlList = getResources();
            scanResources(urlList);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

        return new ArrayList<>(clazzSet);
    }

    private void initClassLoad() {
        if (Objects.nonNull(this.classLoader)) {
            return;
        }

        ClassLoader classLoader = Thread.currentThread().getContextClassLoader();
        this.classLoader = classLoader != null ? classLoader : ClassScanner.class.getClassLoader();

    }

    public List<URL> getResources() throws IOException {
        this.initClassLoad();

        Enumeration<URL> urlEnumeration = classLoader.getResources(packagePath);

        Set<URL> urlSet = new HashSet<>();
        while (urlEnumeration.hasMoreElements()) {
            URL url = urlEnumeration.nextElement();
            urlSet.add(url);
        }

        return new ArrayList<>(urlSet);
    }

    private void scanResources(List<URL> urlList) throws IOException {
        for (URL url : urlList) {
            String protocol = url.getProtocol();
            switch (protocol) {
                case "jar" -> scanJar(url);
                case "file" -> scanFile(url);
            }
        }
    }

    private void scanJar(URL url) throws IOException {
        URLConnection urlConn = url.openConnection();
        if (urlConn instanceof JarURLConnection jarUrlConn) {
            try (JarFile jarFile = jarUrlConn.getJarFile()) {
                Enumeration<JarEntry> jarEntryEnumeration = jarFile.entries();
                while (jarEntryEnumeration.hasMoreElements()) {
                    JarEntry jarEntry = jarEntryEnumeration.nextElement();
                    String en = jarEntry.getName();
                    // 扫描 packagePath 下的类
                    if (en.endsWith(".class") && en.startsWith(packagePath)) {
                        en = en.substring(0, en.length() - 6).replace('/', '.');
                        scanClazz(en);
                    }
                }
            }
        }


    }

    private void scanFile(URL url) {
        String path = url.getPath();
        path = URLDecoder.decode(path, StandardCharsets.UTF_8);
        File file = new File(path);
        String classPath = getClassPath(file);
        scanFile(file, classPath);
    }

    private void scanFile(File file, String classPath) {
        if (file.isDirectory()) {
            File[] files = file.listFiles();
            if (files != null) {
                for (File fi : files) {
                    scanFile(fi, classPath);
                }
            }
        } else if (file.isFile()) {
            String fullName = file.getAbsolutePath();
            if (fullName.endsWith(".class")) {
                String className = fullName.substring(classPath.length(), fullName.length() - 6).replace(File.separatorChar, '.');
                scanClazz(className);
            }
        }
    }

    private String getClassPath(File file) {
        String ret = file.getAbsolutePath();
        if (!ret.endsWith(File.separator)) {
            ret = ret + File.separator;
        }

        String bp = packagePath.replace('/', File.separatorChar);
        int index = ret.lastIndexOf(bp);
        if (index != -1) {
            ret = ret.substring(0, index);
        }

        return ret;
    }

    private void scanClazz(String className) {
        Class<?> clazz = null;
        try {
            clazz = classLoader.loadClass(className);
        } catch (Throwable e) {
            e.printStackTrace();
        }

        if (clazz != null && !clazzSet.contains(clazz)) {
            if (predicateFilter.test(clazz)) {
                clazzSet.add(clazz);
            }
        }
    }
}
