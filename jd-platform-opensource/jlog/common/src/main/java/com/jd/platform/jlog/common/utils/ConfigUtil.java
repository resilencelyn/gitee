package com.jd.platform.jlog.common.utils;

import com.jd.platform.jlog.common.constant.Constant;
import com.jd.platform.jlog.common.handler.JcProperties;

import java.beans.IntrospectionException;
import java.beans.PropertyDescriptor;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.*;
import java.util.concurrent.ThreadLocalRandom;

/**
 * @author tangbohu
 * @version 1.0.0
 * @ClassName ConfigUtil.java
 * @createTime 2022年02月13日 21:40:00
 */
public class ConfigUtil {


    private static final String SERVER_SUFFIX = "Server";

    public static final char MID_LINE = '-';

    public static final ThreadLocalRandom RANDOM = ThreadLocalRandom.current();


    public static String escapeExprSpecialWord(String str) {

        if (str != null && str.length() > 0) {
            for (String s : Constant.SPECIAL_CHAR) {
                if (str.contains(s)) {
                    str = str.replace(s, "\\" + s);
                }
            }
        }
        return str;
    }


    public static String formatConfigStr(Properties properties) {

        StringBuilder sb = new StringBuilder();
        Enumeration<?> enumeration = properties.propertyNames();
        while (enumeration.hasMoreElements()) {
            String key = (String) enumeration.nextElement();
            Object property = properties.get(key);
            if (property != null) {
                property = String.valueOf(property);
            }
            sb.append(key).append("=").append(property).append("\n");
        }
        return sb.toString();
    }


    public static byte[] formatConfigByte(Properties properties) {
        return formatConfigStr(properties).getBytes();
    }


    //递归解析map对象
    public static void loadRecursion(Map<String, Object> map, String key, Properties props) {
        map.forEach((k, v) -> {
            System.out.println("k =>  " + k + "   v.class => " + v.getClass() + "   val => " + v.toString() + "  是不是爸爸 => " + isParent(v));
            if (isParent(v)) {
                if (v instanceof Boolean || v instanceof List) {
                    props.put(key + "." + k, v);
                } else {
                    Map<String, Object> nextValue = (Map<String, Object>) v;
                    loadRecursion(nextValue, (("".equals(key) ? "" : key + ".") + k), props);
                }
            } else {
                props.put(key + "." + k, v);
            }
        });
    }

    //判断是否还有子节点
    public static boolean isParent(Object o) {
        if (!(o instanceof String || o instanceof Character || o instanceof Byte)) {
            try {
                Number n = (Number) o;
                //  System.out.println("isParent ==> "+n);
            } catch (Exception e) {
                return true;
            }
        }
        return false;
    }

/*

    public static ConfigCenterEnum getCenter(CenterConfig config) throws Exception {
        Class<?> clz = config.getClass();
        Field[] fields = clz.getDeclaredFields();

        for (Field field : fields) {
            Method m = clz.getMethod("get" + getMethodName(field.getName()));
            String val = (String)m.invoke(config);
            if (val != null) {
                for (ConfigCenterEnum center : ConfigCenterEnum.values()) {
                    String fd = field.getName().replace(SERVER_SUFFIX, "");
                    if(center.name().equals(fd.toUpperCase())){
                        return center;
                    }
                }
            }
        }
        throw new Exception("Configuration center cannot be found");
    }
*/


    private static String getMethodName(String fieldName) {
        byte[] items = fieldName.getBytes();
        items[0] = (byte) ((char) items[0] - 'a' + 'A');
        return new String(items);
    }




    public static String camelToMidline(String param) {
        if (param == null || "".equals(param.trim())) {
            return "";
        }
        int len = param.length();
        StringBuilder sb = new StringBuilder(len);
        for (int i = 0; i < len; i++) {
            char c = param.charAt(i);
            if (Character.isUpperCase(c)) {
                sb.append("-");
                sb.append(Character.toLowerCase(c));
            } else {
                sb.append(c);
            }
        }
        return sb.toString();
    }

    public static String lowerFirst(String fromStr){
        char[] chars = fromStr.toCharArray();
        chars[0] += 32;
        return String.valueOf(chars);
    }


    /**
     * 只支持简单的对象形配置
     * @param model bean
     * @param properties 配置
     * @param prefix 前缀
     */
    @Deprecated
    public static void invoke(Object model, JcProperties properties, String prefix) throws
            IllegalAccessException, ClassNotFoundException, InstantiationException, ParseException {

        Class<?> clz = model.getClass();
        Field[] fields = model.getClass().getDeclaredFields();
        for (Field field : fields) {
            String type = field.getGenericType().toString();
            field.setAccessible(true);

            String curObjName = ConfigUtil.camelToMidline(lowerFirst(clz.getSimpleName()));

            prefix = StringUtil.isEmpty(prefix) ? curObjName : prefix;
            String fillName = !curObjName.equals(prefix) ? prefix +"."+ curObjName + "." + field.getName() : curObjName + "." + field.getName();

            switch (type){
                case "class java.lang.String":
                    field.set(model, properties.getString(fillName)) ;
                    break;
                case "byte":
                    field.setByte(model, Byte.valueOf(properties.getString(fillName)));
                    break;
                case "short":
                    field.setShort(model, Short.valueOf(properties.getString(fillName)));
                    break;
                case "int":
                    field.setInt(model, Integer.parseInt(properties.getString(fillName))) ;
                    break;
                case "long":
                    field.setLong(model, properties.getLong(fillName));
                    break;
                case "double":
                    field.setDouble(model, Double.valueOf(properties.getString(fillName)));
                    break;
                case "float":
                    field.setFloat(model, Float.valueOf(properties.getString(fillName)));
                    break;
                case "boolean":
                    field.setBoolean(model, Boolean.parseBoolean(properties.getString(fillName)));
                    break;
                case "class java.util.Date":
                    Date date = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss").parse(properties.getString(fillName));
                    field.set(model,date) ;
                    break;
                default:
                    String tn = field.getType().getTypeName();
                    if("java.util.List".equals(tn)){
                        String[] arr = fillName.split("\\[");
                        int index = 0;
                        String suffix;
                        String fastSuffix;
                        List<String> list = new ArrayList<>();
                        do{
                            suffix = "["+index+"]";
                            fastSuffix = "["+(index+1)+"]";
                            list.add(properties.getString(arr[0]+suffix));
                            index ++;
                        }while (properties.getString(arr[0]+fastSuffix) != null);
                        field.set(model, list);
                    }else if("java.util.Map".equals(tn)){
                        String val = properties.getString(fillName);
                        field.set(model,FastJsonUtils.toMap(val));
                    }else if(field.getType().isArray()){
                        String val = properties.getString(fillName);
                        field.set(model,FastJsonUtils.toArray(val));
                    }else{
                        String[] ar = type.split(" ");
                        Object tinyObj = Class.forName(ar[1]).newInstance();
                        invoke(tinyObj, properties, prefix);
                        field.set(model,tinyObj);
                    }
            }
        }

    }

}
