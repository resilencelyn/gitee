package com.jd.platform.jlog.dashboard.utils;

import java.io.Serializable;

/**
 * 二元祖 TwoTuple
 *
 * @author wuweifeng
 * @version 1.0
 * @date 2020-12-16
 */
public class TwoTuple<K, V> implements Serializable {
    /**
     * Serializable
     */
    private static final long serialVersionUID = 1L;
    /**
     * first
     */
    private K first;
    /**
     * second
     */
    private V second;

    /**
     * TwoTuple
     */
    public TwoTuple() {
    }

    /**
     * getFirst
     */
    public K getFirst() {
        return this.first;
    }

    public void setFirst(K first) {
        this.first = first;
    }

    /**
     * getSecond
     */
    public V getSecond() {
        return this.second;
    }

    public void setSecond(V second) {
        this.second = second;
    }

    @Override
    public String toString() {
        return "[first = " + this.first + "," + "second = " + "]";
    }
}