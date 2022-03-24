package com.jd.platform.jlog.dashboard.utils;

import java.io.*;

/**
 * 序列化方法
 * @author shenkaiwen5
 * @version 1.0
 * @date 2021-11-09
 */
public class SerializationUtil {
    /**
     * deserialize to Object from given file
     */
     public static Object deserialize(String fileName) throws IOException, ClassNotFoundException {
         FileInputStream fis = new FileInputStream(fileName);
         ObjectInputStream ois = new ObjectInputStream(fis);
         Object obj = ois.readObject();
         ois.close();
         return obj;
     }

    /**
     * serialize the given object and save it to file
     */
     public static void serialize(Object obj, String fileName) throws IOException {
         FileOutputStream fos = new FileOutputStream(fileName);
         ObjectOutputStream oos = new ObjectOutputStream(fos);
         oos.writeObject(obj);
         fos.close();
     }
}
