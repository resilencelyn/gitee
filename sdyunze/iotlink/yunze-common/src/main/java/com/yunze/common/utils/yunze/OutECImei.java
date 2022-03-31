package com.yunze.common.utils.yunze;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;

import java.util.*;

/**
 * @Auther: zhang feng
 * @Date: 2021/08/20/12:24
 * @Description:
 */
public class OutECImei {


    public String getSign(Map<String,Object> map,String key){//入参为：appid,password,version,iccid,timestamp,sign
        Iterator<String> iterator = map.keySet().iterator();
        while (iterator.hasNext()){// 循环取键值进行判断
            String m = iterator.next();// 键
            if(m.startsWith("sign")){
                iterator.remove();// 移除map中以a字符开头的键对应的键值对
            }
            if(m.startsWith("iccids")){
                iterator.remove();
            }
        }
        List<String> list=new ArrayList<>(map.keySet());
        Collections.sort(list);
        StringBuffer sb=new StringBuffer();
        for(int i=0;i<list.size();i++){
            String k =list.get(i);
            String v=(String )map.get(k);
            sb.append(k).append("=").append(v).append("&");
        }
        String signstr=sb.append("key=").append(key).toString();
        //System.out.println(signstr);
        String sign =MD5Util.MD5Encode(signstr).toUpperCase();
        //System.out.println(sign);
        return sign;
    }

    public static void main(String[] args) {
        //  String url="http://hsapi.e114.xin:9002/Api/yunze/v1";

        WriteCSV writeCSV = new WriteCSV();
        Map<String, Object> Yzmap = new HashMap<String, Object>();

        // for(int i=0;i<300;i++){
        //String geturl = "http://127.0.0.1:9080/route/Api/";
        String geturl = "http://127.0.0.1:9080/route/Api/";

        String appId = "adminOption";
        String password = "asdasdasczx";
        String access_key = "asdfdrqads69846532065";
        String timeStamp = System.currentTimeMillis() + "";

        Yzmap.put("password", password);
        Yzmap.put("appId", appId);
        Yzmap.put("timeStamp", timeStamp);

        OutECImei agentCheckShiro = new OutECImei();

        String sign = agentCheckShiro.getSign(Yzmap, access_key);
        Yzmap.put("sign", sign);

        Map<String, Object> map = new HashMap<String, Object>();
        Map<String, Object> Parmap = new HashMap<String, Object>();


        geturl +="queryCardImei";//单卡 imei查询




        List<String> sArr = new ArrayList<>();
        sArr.add("89860422152080038591");
        sArr.add("89860422152080038592");
        sArr.add("89860422152080038593");
        sArr.add("89860422152080038594");
        sArr.add("89860422152080038595");
        sArr.add("89860422152080038596");
        sArr.add("89860422152080038597");
        sArr.add("89860422152080038598");
        sArr.add("89860422152080038599");
        sArr.add("89860422152080038600");
        sArr.add("89860422152080038601");
        sArr.add("89860422152080038602");
        sArr.add("89860422152080038603");
        sArr.add("89860422152080038604");
        sArr.add("89860422152080038605");
        sArr.add("89860422152080038606");
        sArr.add("89860422152080038607");
        sArr.add("89860422152080038608");
        sArr.add("89860422152080038609");
        sArr.add("89860422152080038610");
        sArr.add("89860422152080038611");
        sArr.add("89860422152080038612");
        sArr.add("89860422152080038613");
        sArr.add("89860422152080038614");
        sArr.add("89860422152080038615");
        sArr.add("89860422152080038616");
        sArr.add("89860422152080038617");
        sArr.add("89860422152080038618");
        sArr.add("89860422152080038619");
        sArr.add("89860422152080038620");
        sArr.add("89860422152080038621");
        sArr.add("89860422152080038622");
        sArr.add("89860422152080038623");
        sArr.add("89860422152080038624");
        sArr.add("89860422152080038625");
        sArr.add("89860422152080038626");
        sArr.add("89860422152080038627");
        sArr.add("89860422152080038628");
        sArr.add("89860422152080038629");
        sArr.add("89860422152080038630");
        sArr.add("89860422152080038631");



        map.put("verify", Yzmap);
        Parmap.put("bind_type", "2");
        List<Map<String, Object>> list = new ArrayList<Map<String, Object>>();



        try {
            for (int i=0;i<sArr.size();i++){
                String result = null;
                Parmap.put("iccid", sArr.get(i));
                map.put("Param", Parmap);
                String data = AesEncryptUtil.encrypt(JSON.toJSONString(map), "Apishandongyunze", "2021_06_10_11_32");
                result = HttpUtil.post(geturl, data);// 返回结果字符串
                Map<String, Object> Robj = JSON.parseObject(result);
                System.out.println(Robj.get("data"));
                JSONObject Data = JSON.parseObject(((Map<String, Object>)Robj.get("data")).get("Data").toString());
                if(Data.get("result")!=null){
                    if(((List<Object>)Data.get("result")).size()>0){
                        Map<String, Object> Data_1 = ((List<Map<String, Object>>)Data.get("result")).get(0);
                        if(Data_1!=null){
                            String imei = "";
                            if(Data_1.get("imei")!=null){
                                imei = Data_1.get("imei").toString();
                            }
                            Map<String, Object> obj = new HashMap<>();
                            obj.put("iccid",sArr.get(i));
                            obj.put("imei",imei);
                            list.add(obj);
                        }
                    }
                }
            }
        } catch (Exception e) {
            System.out.println(e);
        }
        String Outcolumns[] = {"iccid","imei"};

        writeCSV.OutCSVObj(list, VeDate.getNo(4), Outcolumns, Outcolumns,50);


    }

}
