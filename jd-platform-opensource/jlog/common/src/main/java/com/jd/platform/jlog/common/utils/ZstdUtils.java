package com.jd.platform.jlog.common.utils;

import com.github.luben.zstd.Zstd;

import java.time.LocalDateTime;
import java.util.Arrays;
import java.util.Random;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * zstd压缩工具类
 *
 * @author wuweifeng
 * @version 1.0
 * @date 2021-08-16
 */
public class ZstdUtils {

    /**
     * 压缩
     */
    public static byte[] compress(byte[] bytes) {
        return Zstd.compress(bytes);
    }

    /**
     * 解压
     */
    public static String decompress(byte[] bytes) {
        int size = (int) Zstd.decompressedSize(bytes);
        byte[] ob = new byte[size];
        Zstd.decompress(ob, bytes);

        return new String(ob);
    }

    /**
     * 解压
     */
    public static byte[] decompressBytes(byte[] bytes) {
        int size = (int) Zstd.decompressedSize(bytes);
        byte[] ob = new byte[size];
        Zstd.decompress(ob, bytes);

        return ob;
    }



    public static void main(String[] args) throws InterruptedException, ClassNotFoundException, IllegalAccessException, InstantiationException {

        String s = "-75";
        char firstChar = s.charAt(0);
        System.out.println(firstChar);
        if (firstChar < '0') { // Possible leading "+" or "-"
            if (firstChar == '-') {
                System.out.println("=========");
            } else if (firstChar != '+')
                System.out.println("eeee");

        }
       // System.out.println(Integer.parseInt("-75"));
        String str1 = "1a";
        String str2 = "0a";

        String dbStr = Arrays.toString(compress(str1.getBytes()));
        System.out.println(dbStr);
        String[] arr0 = dbStr.split(",");

        byte[] bt = new byte[arr0.length];
        for (int i = 0; i < arr0.length; i++) {
            if(i == 0){
                String first = arr0[0].replace("[", "");
                bt[i] = Byte.valueOf(first);
                continue;
            }
            if(i ==  arr0.length-1){
                String last = arr0[arr0.length-1].replace("]", "");
                bt[i] = Byte.valueOf(last.trim());
                continue;
            }
            System.out.println(arr0[i]+"   len => "+arr0[i].length());
            bt[i] = Byte.parseByte(arr0[i].trim());
        }


        System.out.println(Arrays.toString(bt));
  //      System.out.println(Arrays.toString(compress(str2.getBytes())));

        AtomicInteger fail = new AtomicInteger(0);
        AtomicInteger ok = new AtomicInteger(0);

        for (int j = 0; j < 1; j++) {
            new Thread(() -> {
                for (int i = 0; i < 100; i++) {
                    int id = new Random().nextInt(99000000);
                /*    String ml = String.valueOf(getRandomChar()) +id+getRandomChar();
                    String ml2 = LocalDateTime.now().toString()+ getRandomChar();
                    ml = ml2 + ml;*/
                    // 压缩后的
                    String ml = ""+i;
                    byte[] mlc1= compress(ml.getBytes());

                    //存进去new String(mlc1)
                    byte[] resul = new String(mlc1).getBytes();
                    System.out.println(Arrays.toString(new String(mlc1).getBytes()));
                    byte[] arr = new byte[9];
                    arr[0] = 40;
                    arr[1] = -17;
                    arr[2] = -65;
                    arr[3] = -67;
                    arr[4] = 47;
                    arr[5] = -17;
                    arr[6] = -65;
                    arr[7] = -67;
                    arr[8] = 32;
                 //   System.out.println(new String(arr));

                    if(resul[0]==40
                            && resul[1]==-17
                            && resul[2]==-65
                            && resul[3]==-67
                            && resul[4]==47
                            && resul[5]==-17
                            && resul[6]==-65
                            && resul[7]==-67
                            && resul[8]==32 ){
                        ok.incrementAndGet();
                    }else{
                        fail.incrementAndGet();
                    }
                }
            }).start();
        }


        Thread.sleep(30000);


        System.out.println("ok == > "+ok.get());
        System.out.println("fail == > "+fail.get());

    }

    public static char getRandomChar() {
        return (char) (0x4e00 + (int) (Math.random() * (0x9fa5 - 0x4e00 + 1)));
    }
}