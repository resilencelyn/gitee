package com.jd.platform.jlog.common.utils;

import com.alibaba.fastjson.JSON;

import java.util.*;
import java.util.function.Function;
import java.util.stream.Collectors;
import java.util.stream.Stream;

/**
 * @author tangbohu
 * @version 1.0.0
 * @ClassName CollectionUtil.java
 * @Description TODO
 * @createTime 2022年02月21日 17:14:00
 */
public class CollectionUtil {

    public static boolean isEmpty(Collection<?> collection) {
        return collection == null || collection.isEmpty();
    }

    public static boolean isNotEmpty(Collection<?> collection) {
        return collection != null && collection.isEmpty();
    }


    public static boolean isEmpty(Map<?, ?> map) {
        return !isNotEmpty(map);
    }


    public static boolean isNotEmpty(Map<?, ?> map) {
        return map != null && !map.isEmpty();
    }


    public static <T> boolean equals(List<T> c1, List<T> c2) {
        if (c1 == null && c2 == null) {
            return true;
        } else if (c1 != null && c2 != null) {
            if (c1.size() != c2.size()) {
                return false;
            } else {
                for(int i = 0; i < c1.size(); ++i) {
                    if (!Objects.equals(c1.get(i), c2.get(i))) {
                        return false;
                    }
                }

                return true;
            }
        } else {
            return false;
        }
    }

    public static <K, V> boolean equals(Map<K, V> c1, Map<K, V> c2) {
        if (c1 == null && c2 == null) {
            return true;
        } else if (c1 != null && c2 != null) {
            if (c1.size() != c2.size()) {
                return false;
            } else {
                Iterator var2 = c1.entrySet().iterator();

                Object v1;
                Object v2;
                do {
                    if (!var2.hasNext()) {
                        return true;
                    }

                    Map.Entry<K, V> entry = (Map.Entry)var2.next();
                    K k = entry.getKey();
                    v1 = entry.getValue();
                    v2 = c2.get(k);
                } while(Objects.equals(v1, v2));

                return false;
            }
        } else {
            return false;
        }
    }





    public static <K, V> Set<String> diffKeys(Map<K, V> m1, Map<K, V> m2){

        Set<String> diff = new HashSet<>(1);

        for (Map.Entry<K,V> kvEntry : m1.entrySet()) {
            V val = m2.get(kvEntry.getKey());
            if(!kvEntry.getValue().equals(val)){
                diff.add(kvEntry.getKey().toString());
            }
        }

        for (Map.Entry<K,V> kvEntry : m2.entrySet()) {
            V val = m1.get(kvEntry.getKey());
            if(!kvEntry.getValue().equals(val)){
                diff.add(kvEntry.getKey().toString());
            }
        }
        return diff;
    }




    public static <K, V> HashMap<K,V> diffMap(Map<K, V> m1, Map<K, V> m2){

        HashMap<K, V> diff = new HashMap<>(1);

        for (Map.Entry<K,V> kvEntry : m1.entrySet()) {
            V val = m2.get(kvEntry.getKey());
            if(!kvEntry.getValue().equals(val)){
                diff.put(kvEntry.getKey(), kvEntry.getValue());
            }
        }

        for (Map.Entry<K,V> kvEntry : m2.entrySet()) {
            V val = m1.get(kvEntry.getKey());
            if(!kvEntry.getValue().equals(val)){
                diff.put(kvEntry.getKey(), kvEntry.getValue());
            }
        }
        return diff;
    }


    public static void main(String[] args) {
        int fail = 0;
        int count = 10000;
        Set<Integer> set = new HashSet<>();
        for (int i = 0; i < count; i++) {
            int h = randomMac4Qemu().hashCode();
            int val = indexFor(h);
            boolean r = set.add(val);
          //  System.out.println(val);
            if(!r){
                fail++;
            }
        }

        int fail2 = 0;
        Set<Integer> set2 = new HashSet<>();
        for (int i = 0; i < count; i++) {
            int h = randomMac4Qemu().hashCode();
            Random rd = new Random(h);

            int val = rd.nextInt(16385);
            boolean r = set.add(val);
            System.out.println(val);
            if(!r){
                fail2++;
            }
        }

        System.out.println("11--》 "+fail);
       System.out.println("22-》 "+ fail2);
    }
    static int indexFor(int h) {
        return h & (16384-1);
    }
    public static String randomMac4Qemu() {
       /* if(1==1){
            return getRandomIp();
        }*/
        Random random = new Random();
        String[] mac = {
            /*  String.format("%02x", 0x52),
                String.format("%02x", 0x54),
                String.format("%02x", 0x00),*/
                String.format("%02x", random.nextInt(0xff)),
                String.format("%02x", random.nextInt(0xff)),
                String.format("%02x", random.nextInt(0xff))
        };
        return String.join(":", mac);
    }


    public static String getRandomIp(){

        //ip范围
        int[][] range = {{607649792,608174079},//36.56.0.0-36.63.255.255
                {1038614528,1039007743},//61.232.0.0-61.237.255.255
                {1783627776,1784676351},//106.80.0.0-106.95.255.255
                {2035023872,2035154943},//121.76.0.0-121.77.255.255
                {2078801920,2079064063},//123.232.0.0-123.235.255.255
                {-1950089216,-1948778497},//139.196.0.0-139.215.255.255
                {-1425539072,-1425014785},//171.8.0.0-171.15.255.255
                {-1236271104,-1235419137},//182.80.0.0-182.92.255.255
                {-770113536,-768606209},//210.25.0.0-210.47.255.255
                {-569376768,-564133889}, //222.16.0.0-222.95.255.255
        };

        Random rdint = new Random();
        int index = rdint.nextInt(10);
        return num2ip(range[index][0]+new Random().nextInt(range[index][1]-range[index][0]));
    }


    public static String num2ip(int ip) {
        int [] b=new int[4] ;
        String x = "";

        b[0] = (int)((ip >> 24) & 0xff);
        b[1] = (int)((ip >> 16) & 0xff);
        b[2] = (int)((ip >> 8) & 0xff);
        b[3] = (int)(ip & 0xff);
        x=Integer.toString(b[0])+"."+Integer.toString(b[1])+"."+Integer.toString(b[2])+"."+Integer.toString(b[3]);

        return x;
    }
}
