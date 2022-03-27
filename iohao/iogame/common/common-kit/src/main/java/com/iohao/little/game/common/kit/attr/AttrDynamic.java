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
package com.iohao.little.game.common.kit.attr;

import java.lang.reflect.InvocationTargetException;
import java.util.Map;
import java.util.Objects;
import java.util.Optional;
import java.util.function.Supplier;

/**
 * 动态属性
 * <pre>
 *     实现该接口的对象, 都会提供动态属性机制
 *     避免类型转换
 * </pre>
 * AttrDynamic attr = ...;
 * <pre>
 *     使用示例 - get :
 *
 *     boolean val = attr.getAttr("exist", () -&gt; false);
 *
 *     int age = attr.getAttr("age", () -&gt; 18);
 *
 *     String name = attr.getAttr("name", () -&gt; "");
 *
 *     List&lt;Integer&gt; birdList = attr.getAttr("birds", Kit::newArrayList);
 *
 * </pre>
 * <pre>
 *     如果你使用了lombok, 推荐这种方式. 只需要在对象中新增此行代码
 *     final Map<String, Object> attr = new HashMap<>();
 *     or
 *     final Map<String, Object> attr = new ConcurrentHashMap<>();
 * </pre>
 *
 * @author 洛朱
 * @date 2022-01-02
 */
@SuppressWarnings("unchecked")
public interface AttrDynamic {
    Supplier<Boolean> SUPPLIER_BOOL = () -> false;
    Supplier<Integer> SUPPLIER_INT = () -> 0;
    Supplier<Long> SUPPLIER_LONG = () -> 0L;

    /**
     * 获取动态成员属性map
     *
     * @return 动态成员属性map
     */
    Map<String, Object> getAttr();

    /**
     * 获取动态成员属性
     * <pre>
     * 如果不存在. 从 supplier 中获取.
     * 并将 supplier 获取的值存入属性中
     * </pre>
     *
     * @param attrName 属性名
     * @param supplier supplier
     * @param <T>      t
     * @return val
     */
    default <T> T getAttr(String attrName, Supplier<T> supplier) {
        Map<String, Object> map = getAttr();
        T prop = (T) map.get(attrName);

        // 无锁化
        if (Objects.isNull(prop)) {
            prop = supplier.get();
            prop = (T) map.putIfAbsent(attrName, prop);
            if (Objects.isNull(prop)) {
                prop = (T) map.get(attrName);
            }
        }

        return prop;
    }

    /**
     * 是否有属性值
     *
     * @param attrName 属性名
     * @return true 表示有值 (只要不为null, 都视为有值)
     */
    default boolean hasAttr(String attrName) {
        return Objects.nonNull(getAttr().get(attrName));
    }

    /**
     * 属性名 key 是否存在
     *
     * @param attrName 属性名
     * @return true key 存在
     */
    default boolean containsAttrKey(String attrName) {
        return getAttr().containsKey(attrName);
    }

    /**
     * 获取属性值
     *
     * @param attrName 属性名
     * @param <T>      t
     * @return val
     */
    default <T> T getAttr(String attrName) {
        return (T) getAttr().get(attrName);
    }

    /**
     * 获取对象类型属性
     * <pre>
     *     方法内部根据 class 获取对应的对象;
     *     如果对象不存在,则会 new 该类型的对象
     * </pre>
     *
     * @param attrNameClazz 属性名 class
     * @param <T>           t
     * @return 类型对应的对象
     */
    default <T> T getAttr(Class<T> attrNameClazz) {
        String attrName = attrNameClazz.getSimpleName();

        Map<String, Object> attrMap = getAttr();
        T prop = (T) attrMap.get(attrName);

        // 无锁化
        if (Objects.isNull(prop)) {
            try {
                // 没有属性 则创建该类型的对象
                prop = attrNameClazz.getDeclaredConstructor().newInstance();
                prop = (T) attrMap.putIfAbsent(attrName, prop);
                if (Objects.isNull(prop)) {
                    prop = getAttr(attrName);
                }
            } catch (InstantiationException | InvocationTargetException | IllegalAccessException | NoSuchMethodException e) {
                e.printStackTrace();
            }


        }

        return prop;
    }

    /**
     * 获取bool值, 如果值不存在返回false
     *
     * @param attrName 属性名
     * @return bool
     */
    default boolean getAttrBool(String attrName) {
        return getAttr(attrName, SUPPLIER_BOOL);
    }

    /**
     * 获取long值, 如果值不存在返回 0
     *
     * @param attrName 属性名
     * @return long
     */
    default long getAttrLong(String attrName) {
        return getAttr(attrName, SUPPLIER_LONG);
    }

    /**
     * 获取int值, 如果值不存在返回 0
     *
     * @param attrName 属性名
     * @return int
     */
    default int getAttrInt(String attrName) {
        return getAttr(attrName, SUPPLIER_INT);
    }

    /**
     * 给属性添加值 , 一定不会超过最大值 Integer.MAX
     *
     * @param attrName 属性名
     * @param value    int 值
     */
    default void attrAddValue(String attrName, int value) {
        int theValue = getAttrInt(attrName);

        value = theValue + value;

        setAttr(attrName, value);
    }

    /**
     * 给属性减值
     *
     * @param attrName 属性名
     * @param value    int 值
     */
    default void attrMinusValue(String attrName, int value) {
        int theValue = getAttrInt(attrName);
        setAttr(attrName, theValue - value);
    }

    /**
     * 获取动态成员属性 optional 方式
     *
     * @param attrName 属性名
     * @param <T>      t
     * @return val
     */
    default <T> Optional<T> getAttrOptional(String attrName) {
        T prop = (T) getAttr().get(attrName);
        return Optional.ofNullable(prop);
    }

    /**
     * 移除动态属性成员
     *
     * @param attrName 属性名
     * @param <T>      t
     * @return 当前移除的对象
     */
    default <T> T removeAttr(String attrName) {
        return (T) getAttr().remove(attrName);
    }

    /**
     * 移除成员动态属性
     * <pre>
     * 并返回被移除的值`
     * 如果移除的属性不存在, 返回值从 supplier 中获取
     * </pre>
     *
     * @param attrName 属性名
     * @param supplier supplier
     * @param <T>      t
     * @return 移除的属性
     */
    default <T> T removeAttr(String attrName, Supplier<T> supplier) {
        T v = (T) getAttr().remove(attrName);

        if (Objects.nonNull(v)) {
            return v;
        }

        return supplier.get();
    }

    /**
     * 在动态属性中设置值
     *
     * @param attrName 属性名
     * @param value    属性值
     * @param <T>      t
     * @return 返回设置的值
     */
    default <T> T setAttr(String attrName, Object value) {
        this.getAttr().put(attrName, value);
        return (T) value;
    }
}
