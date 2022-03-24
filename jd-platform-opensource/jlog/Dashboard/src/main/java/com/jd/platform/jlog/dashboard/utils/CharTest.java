package com.jd.platform.jlog.dashboard.utils;

import com.alibaba.fastjson.JSON;
import org.springframework.util.StringUtils;
import ru.yandex.clickhouse.ClickHouseConnection;
import ru.yandex.clickhouse.ClickHouseDataSource;
import ru.yandex.clickhouse.settings.ClickHouseProperties;

import java.io.UnsupportedEncodingException;
import java.nio.charset.StandardCharsets;
import java.sql.*;
import java.util.*;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * 测试类 别用
 */
@Deprecated
public class CharTest {
    public static void main2(String[] args) throws SQLException, UnsupportedEncodingException, InterruptedException {

        List<Map<String, Object>> list = new ArrayList<>();
        String sql = "select * from tracer_model";

        Connection connection = getConn();
        Statement statement = connection.createStatement();

        try {
            int id = new Random().nextInt(10000);
            String str = "SOSOSOSOSA1232哈哈哈";
            byte[] bt = ZstdUtils.compress(str.getBytes());

            PreparedStatement pstmt = connection.prepareStatement("insert into test values(?, ?)");
            for (int i = 0; i < 1; i++) {
                pstmt.setInt(1, id);
                pstmt.setString(2, new String(bt,"ISO8859-1"));
                pstmt.addBatch();
            }
           // pstmt.executeBatch();
        } catch (SQLException e) {
            e.printStackTrace();
        }

        ResultSet results = statement.executeQuery(sql);
        ResultSetMetaData rsmd = results.getMetaData();

        while(results.next()){
            Map<String, Object> row = new HashMap<>();
            for(int i = 1; i <= rsmd.getColumnCount(); i++){
                row.put(rsmd.getColumnName(i), results.getObject(rsmd.getColumnName(i)));
            }
            list.add(row);
        }


        String str4 = "SOSOSOSOSA1232哈哈哈";
        byte[] bt4 = ZstdUtils.compress(str4.getBytes());
        System.out.println("压缩后的 bt4 ==>  "+Arrays.toString(bt4));
        System.out.println("list => "+list);
        for (Map<String, Object> objectMap : list) {
            if("7648".equals((objectMap.get("id").toString()))){
                System.out.println("bt4 ==> "+Arrays.toString(objectMap.get("name").toString()
                        .getBytes("ISO8859-1")));
                String result = ZstdUtils.decompress(objectMap.get("name").toString()
                        .getBytes("ISO8859-1"));
                System.out.println("解密之后的结果： "+result);
            }

        }


        System.out.println("==================================");
        cutString(str4,8);

    }

    public static Connection getConn() {

        String username = "default";
      //  String password = "123456";
        String address = "jdbc:clickhouse://101.42.242.201:8123";
        String db = "default";
        int socketTimeout = 600000;

        ClickHouseProperties properties = new ClickHouseProperties();
        properties.setUser(username);
      //  properties.setPassword(password);
        properties.setDatabase(db);
        properties.setSocketTimeout(socketTimeout);
        ClickHouseDataSource clickHouseDataSource = new ClickHouseDataSource(address, properties);

        ClickHouseConnection conn = null;
        try {
            conn = clickHouseDataSource.getConnection();
            return conn;
        } catch (SQLException e) {
            e.printStackTrace();
        }

        return null;
    }

    private static byte[] format(String dbStr){
        String[] arr0 = dbStr.split(",");

        byte[] bt = new byte[arr0.length];
        for (int i = 0; i < arr0.length; i++) {
            if(i == 0){
                String first = arr0[0].replace("[", "");
                System.out.println("first --> "+first);
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
        return bt;
    }




    public static void cutString(String str, int n) throws UnsupportedEncodingException, InterruptedException {


        AtomicInteger fail = new AtomicInteger(0);
        AtomicInteger ok = new AtomicInteger(0);

        for (int j = 0; j < 2; j++) {
            new Thread(() -> {
                for (int i = 0; i < 5; i++) {
                    int id = new Random().nextInt(99000000);
                    String ml = String.valueOf(getRandomChar()) +getRandomChar()+getRandomChar()+getRandomChar()+id;
                    System.out.println("old--> "+ml);
                    byte[] zipBt= ZstdUtils.compress(ml.getBytes());
                    String newStr = null;

                    try {
                        newStr = new String(zipBt,StandardCharsets.ISO_8859_1);
                    } catch (Exception e) {}

                  //  byte[] resul = newStr.getBytes();
                  //  System.out.println("新字符串的bt => "+Arrays.toString(resul));

                    try {
                        byte[] resul = sub(newStr, 4);
                        if(resul[0]==40 && resul[1]==-62 && resul[2]==-75 && resul[3]==47 && resul[4]==-61 && resul[5]==-67 && resul[6]==32){
                            ok.incrementAndGet();
                        }else{
                            fail.incrementAndGet();
                        }

                    } catch (UnsupportedEncodingException e) {
                        e.printStackTrace();
                    }
                    try {
                        String val = ZstdUtils.decompress(newStr.getBytes(StandardCharsets.ISO_8859_1));
                        System.out.println("result--> "+val);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
            }).start();
        }


        Thread.sleep(5000);

        System.out.println("ok == > "+ok.get());
        System.out.println("fail == > "+fail.get());

    }

    private static byte[] sub(String str, int n) throws UnsupportedEncodingException {
       // System.out.println("str => "+str);
        StringBuilder sb = new StringBuilder();
        int count=0;
        for (int i = 0; i < str.length(); i++) {
            String c = String.valueOf(str.charAt(i));
            sb.append(c);
            count+=c.getBytes("ISO8859-1").length;
            if(count>n){
               // System.out.println("count=> "+count);
                break;
            }
        }
        return sb.toString().getBytes();
      //  System.out.println(" bs==> "+ Arrays.toString(sb.toString().getBytes()));
    }

    public static char getRandomChar() {
        return (char) (0x4e00 + (int) (Math.random() * (0x9fa5 - 0x4e00 + 1)));
    }


    public static void main(String[] args) throws UnsupportedEncodingException, InterruptedException, SQLException {
        testChar();


        String str = "SOSOSOSOSA1232哈哈哈";
        byte[] bt = ZstdUtils.compress(str.getBytes());

        String newnewStr = new String(bt, "ISO8859-1");
        System.out.println("bt4 ==> "+Arrays.toString(newnewStr
                .getBytes("ISO8859-1")));
        String result = ZstdUtils.decompress(newnewStr
                .getBytes("ISO8859-1"));
        System.out.println("解密之后的结果： "+result);

        if(1==1){
            return;
        }
       // insert();
        List<Map<String, Object>> list = new ArrayList<>();
        String sql = "select * from tracer_model";

        Connection connection = getConn();
        Statement statement = connection.createStatement();
        ResultSet results = statement.executeQuery(sql);
        ResultSetMetaData rsmd = results.getMetaData();

        while(results.next()){
            Map<String, Object> row = new HashMap<>();
            for(int i = 1; i <= rsmd.getColumnCount(); i++){
                Object obj = results.getObject(rsmd.getColumnName(i));
                if(isZip(obj.toString())){
                    System.out.println("%%%%%%%%### is zip"+obj.toString());
                    obj = obj.toString().getBytes(StandardCharsets.ISO_8859_1);
                }
               // System.out.println("obj=> "+obj);
                row.put(rsmd.getColumnName(i), obj);
            }
            list.add(row);
        }
        System.out.println(JSON.toJSONString(list));
    }

    public static boolean isZip(String str) throws UnsupportedEncodingException {

        if(StringUtils.isEmpty(str)){
            return false;
        }
        StringBuilder sb = new StringBuilder();
        int count=0;
        for (int i = 0; i < str.length(); i++) {
            String c = String.valueOf(str.charAt(i));
            sb.append(c);
            count += c.getBytes("ISO8859-1").length;
            if(count > 4){
                break;
            }
        }
        byte[] bs = str.getBytes("ISO8859-1");
       // byte[] bs = sb.toString().getBytes("ISO8859-1");
        System.out.println(str+ " <=== ### ==> "+ Arrays.toString(bs));

        return bs[0] == 40 && bs[1] == -62 && bs[2] == -75 && bs[3] == 47 && bs[4] == -61 && bs[5] == -67 && bs[6] == 32;
    }

    private static void insert() throws SQLException, UnsupportedEncodingException {
        int id = new Random().nextInt(10000);
        String str = "滴滴员工tangbohu的终身代号是什么？？？是9527";
        byte[] bt = ZstdUtils.compress(str.getBytes());

        PreparedStatement pstmt = getConn().prepareStatement("insert into tracer_model (responseContent) values(?)");
        for (int i = 0; i < 1; i++) {
            pstmt.setString(1, new String(bt,"ISO8859-1"));
            pstmt.addBatch();
        }
         pstmt.executeBatch();
    }

    //[40, -75, 47, -3, 32
    private static void testChar() throws UnsupportedEncodingException {
        for (int i = 0; i < 10; i++) {
            String str =String.valueOf(getRandomChar())+ String.valueOf(getRandomChar()) +i;
            byte[] initBs = ZstdUtils.compress(str.getBytes());
            String zipStr = new String(initBs, "ISO8859-1");
            System.out.println("@@@ => "+ Arrays.toString(zipStr.getBytes("ISO8859-1")));
        }

        String str = "滴滴员工tangbohu的终身代号是什么？？？是9527";
        byte[] initBs = ZstdUtils.compress(str.getBytes());
        String zipStr = new String(initBs, "ISO8859-1");
        // [40, -75, 47, -3, 32,
        System.out.println("@@@ => "+ Arrays.toString(zipStr.getBytes("ISO8859-1")));
        System.out.println("### "+ ZstdUtils.decompress(zipStr.getBytes("ISO8859-1")));

    }
}
