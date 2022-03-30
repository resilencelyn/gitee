package com.yunze.common.utils.yunze;

import com.alibaba.fastjson.JSON;
import org.apache.commons.csv.CSVFormat;
import org.apache.commons.csv.CSVPrinter;
import org.springframework.stereotype.Component;

import java.io.*;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.*;
import java.util.stream.Collectors;

/**
 * 写入CSV文件
 * @Auther: zhang feng
 * @Date: 2021/03/06/13:39
 * @Description:
 */
@Component
public class WriteCSV {

    /**
     * 输出CSV 文件 重名 文件 追加写入
     * @param fileName 文件名
     * @param mapList 写入数据
     * @param withHeader 表头
     * @param OutUrl 写入地址 /a/ 默认 /mnt/yunze/download/
     */
    public  void  Write (String fileName , List<Map<String,Object>> mapList,String[]  withHeader,String OutUrl,String[]  keys){

        try {
            File file2 = new File("");
            String filePath = file2.getCanonicalPath();
            if(OutUrl!=null){
                filePath +=OutUrl;
            }
            filePath +="/mnt/yunze/download/csv/";
            //System.out.println(filePath);
            String agent_rul = fileName.split("/")[0];

            File Url=new File(filePath+"/1.txt");//tomcat 路径
            Upload.mkdirsmy(Url);//创建文件夹
            File file = new File(filePath+"/"+fileName+".csv");
            FileOutputStream fos = null;
            CSVFormat csvFormat = null ;
            if (file.exists()) {
                csvFormat = CSVFormat.DEFAULT.withIgnoreHeaderCase();
                fos = new FileOutputStream(filePath+"/"+fileName+".csv",true);
            }else {
                //设置表头
                csvFormat = CSVFormat.DEFAULT.withHeader(withHeader);
                fos = new FileOutputStream(filePath+"/"+fileName+".csv");
            }
            if(fos!=null){
                OutputStreamWriter osw = new OutputStreamWriter(fos, "GBK");// GBK UTF-8

                CSVPrinter csvPrinter = new CSVPrinter(osw, csvFormat);
//				    csvPrinter = CSVFormat.DEFAULT.withHeader("姓名", "年龄", "家乡").print(osw);

                //循环输出表格数据
                for (int j = 0; j < mapList.size(); j++) {
                    Map<String,Object> objectMap = mapList.get(j);
                    Object oarr[] = objectMap.values().toArray();
                    //int index = 0;
                    Object[] Wtarr = {};
                    for (int i = 0; i < keys.length; i++) {
                       /* String value = ""+;
                        value = value != null && value !="null"?value:"";*/
                        Wtarr = Different.StringArrAdd(Wtarr, objectMap.get(keys[i]));//数组添加
                    }
                    csvPrinter.printRecord(Wtarr);//最后一个写入
                    /**for(String key:objectMap.keySet()){
                     //System.out.println("key="+key+"and value=" +objectMap.get(key));
                     Wtarr = Different.StringArrAdd(Wtarr,objectMap.get(key).toString());//数组添加
                     index++;
                     if(oarr.length==index){
                     csvPrinter.printRecord(Wtarr);//最后一个写入
                     }
                     }*/
                }
                csvPrinter.flush();
                csvPrinter.close();
            }else{
                System.err.println("============== [FileOutputStream = null !!!] ==============");
            }

        } catch (Exception e) {
            System.err.println("==============WriteCSV导出CVS数据异常！==============");
            System.out.println(e);
        }

    }


    /**
     * 输出CSV 文件 重名 文件 追加写入
     * @param fileName 文件名
     * @param mapList 写入数据
     * @param withHeader 表头
     * @param OutUrl 写入地址 /a/ 默认 /mnt/yunze/download/
     * @param Flieurl
     */
    public  void  Write (String fileName , List<Map<String,Object>> mapList,String[]  withHeader,String OutUrl,String[]  keys,String Flieurl){

        try {
            File file2 = new File("");
            String filePath = file2.getCanonicalPath();
            if(OutUrl!=null){
                filePath +=OutUrl;
            }
            filePath +="/mnt/yunze/backup/csv/"+Flieurl;
            //System.out.println(filePath);
            String agent_rul = fileName.split("/")[0];

            File Url=new File(filePath+"/1.txt");//tomcat 路径
            Upload.mkdirsmy(Url);//创建文件夹
            File file = new File(filePath+"/"+fileName+".csv");
            FileOutputStream fos = null;
            CSVFormat csvFormat = null ;
            if (file.exists()) {
                csvFormat = CSVFormat.DEFAULT.withIgnoreHeaderCase();
                fos = new FileOutputStream(filePath+"/"+fileName+".csv",true);
            }else {
                //设置表头
                csvFormat = CSVFormat.DEFAULT.withHeader(withHeader);
                fos = new FileOutputStream(filePath+"/"+fileName+".csv");
            }
            if(fos!=null){
                OutputStreamWriter osw = new OutputStreamWriter(fos, "GBK");// GBK UTF-8

                CSVPrinter csvPrinter = new CSVPrinter(osw, csvFormat);
//				    csvPrinter = CSVFormat.DEFAULT.withHeader("姓名", "年龄", "家乡").print(osw);

                //循环输出表格数据
                for (int j = 0; j < mapList.size(); j++) {
                    Map<String,Object> objectMap = mapList.get(j);
                    Object oarr[] = objectMap.values().toArray();
                    //int index = 0;
                    Object[] Wtarr = {};
                    for (int i = 0; i < keys.length; i++) {
                       /* String value = ""+;
                        value = value != null && value !="null"?value:"";*/
                        Wtarr = Different.StringArrAdd(Wtarr, objectMap.get(keys[i]));//数组添加
                    }
                    csvPrinter.printRecord(Wtarr);//最后一个写入
                    /**for(String key:objectMap.keySet()){
                     //System.out.println("key="+key+"and value=" +objectMap.get(key));
                     Wtarr = Different.StringArrAdd(Wtarr,objectMap.get(key).toString());//数组添加
                     index++;
                     if(oarr.length==index){
                     csvPrinter.printRecord(Wtarr);//最后一个写入
                     }
                     }*/
                }
                csvPrinter.flush();
                csvPrinter.close();
            }else{
                System.err.println("============== [FileOutputStream = null !!!] ==============");
            }

        } catch (Exception e) {
            System.err.println("==============WriteCSV导出CVS数据异常！==============");
            System.out.println(e);
        }

    }



    /**
     * 输出 操作数据
     * @param list 输出数据
     * @param fileName_flow 文件名
     * @param description  操作描述
     * @param agentName 执行人
     * @param result 执行结果
     * @param Outcolumns   输出表头
     * @param keys  输出列
     */
    public void OutCSV(List<Map<String, String>> list , String fileName_flow, String description, String agentName, String result,String Outcolumns[],String keys[],String IdKey){

        List<Map<String, Object>> out_list = new ArrayList<>();

        for (int i = 0; i < list.size(); i++) {
            Map<String, Object> out_map= new HashMap<String, Object>();

            Map<String,String> cardIndex=list.get(i);
            String iccid =cardIndex.get(IdKey).toString();

            String Time= LocalDateTime.now().format(DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss"));
            out_map.put(IdKey,iccid);
            out_map.put("description",description);
            out_map.put("time",Time);
            out_map.put("result",result);
            out_map.put("agentName",agentName);

            out_list.add(out_map);
            if ((i+1)%50==0 || (i+1)==list.size()){
                //执行导出
                if(out_list.size()>0){
                    Write(fileName_flow,out_list,Outcolumns,null,keys);
                    out_list = new ArrayList<>();
                }

            }
        }

    }






    /**
     * 输出 操作数据
     * @param list 输出数据
     * @param fileName_flow 文件名
     * @param description  操作描述
     * @param agentName 执行人
     * @param result 执行结果
     * @param Outcolumns   输出表头
     * @param keys  输出列
     */
    public void OutCSV(List<Map<String, String>> list , String fileName_flow, String description, String agentName, String result,String Outcolumns[],String keys[]){
        OutCSV(list,fileName_flow,description,agentName,result,Outcolumns,keys,"iccid");
    }



    /**
     * 输出 操作数据
     * @param list 输出数据
     * @param fileName_flow 文件名
     * @param description  操作描述
     * @param agentName 执行人
     * @param result 执行结果
     * @param Outcolumns   输出表头
     * @param keys  输出列
     */
    public void OutCSVObj(List<Map<String, Object>> list , String fileName_flow, String description, String agentName, String result,String Outcolumns[],String keys[]){
        OutCSVObj(list,fileName_flow,description,agentName,result,Outcolumns,keys,"iccid");
    }


    /**
     * 输出 操作数据
     * @param list 输出数据
     * @param fileName_flow 文件名
     * @param description  操作描述
     * @param agentName 执行人
     * @param result 执行结果
     * @param Outcolumns   输出表头
     * @param keys  输出列
     */
    public void OutCSVObj(List<Map<String, Object>> list , String fileName_flow, String description, String agentName, String result,String Outcolumns[],String keys[],String IdKey){

        List<Map<String, Object>> out_list = new ArrayList<>();

        for (int i = 0; i < list.size(); i++) {
            Map<String, Object> out_map= new HashMap<String, Object>();

            Map<String,Object> cardIndex=list.get(i);
            String iccid =cardIndex.get(IdKey).toString();

            String Time= LocalDateTime.now().format(DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss"));
            out_map.put(IdKey,iccid);
            out_map.put("description",description);
            out_map.put("time",Time);
            out_map.put("result",result);
            out_map.put("agentName",agentName);

            out_list.add(out_map);
            if ((i+1)%50==0 || (i+1)==list.size()){
                //执行导出
                if(out_list.size()>0){
                    Write(fileName_flow,out_list,Outcolumns,null,keys);
                    out_list = new ArrayList<>();
                }

            }
        }

    }




    /**
     * 输出 操作数据
     * @param list 输出数据
     * @param fileName_flow 文件名
     * @param description  操作描述
     * @param agentName 执行人
     * @param result 执行结果
     * @param Outcolumns   输出表头
     * @param keys  输出列
     * @param OutSize  多少条数据进行一次输出
     */
    public void OutCSVObj(List<Map<String, Object>> list , String fileName_flow, String description, String agentName, String result,String Outcolumns[],String keys[],int OutSize){

        List<Map<String, Object>> out_list = new ArrayList<>();

        for (int i = 0; i < list.size(); i++) {
            Map<String, Object> out_map= new HashMap<String, Object>();
            Map<String,Object> cardIndex=list.get(i);
            String iccid =cardIndex.get("iccid").toString();
            String Time= LocalDateTime.now().format(DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss"));
            out_map.put("iccid",iccid);
            out_map.put("description",description);
            out_map.put("result",result);
            out_map.put("agentName",agentName);
            out_map.put("OptionTime",Time);//加上操作时间
            out_list.add(out_map);
            if ((i+1)%OutSize==0 || (i+1)==list.size()){
                //执行导出
                if(out_list.size()>0){
                    Write(fileName_flow,out_list,Outcolumns,null,keys);
                    out_list = new ArrayList<>();
                }

            }
        }
    }

    /**
     * 输出 操作数据
     * @param list 输出数据
     * @param fileName_flow 文件名
     * @param Outcolumns   输出表头
     * @param keys  输出列
     * @param OutSize  多少条数据进行一次输出
     */
    public void OutCSVObj(List<Map<String, Object>> list , String fileName_flow,String Outcolumns[],String keys[],int OutSize){
        List<Map<String, Object>> out_list = new ArrayList<>();
        for (int i = 0; i < list.size(); i++) {
            Map<String, Object> out_map= new HashMap<String, Object>();
            Map<String,Object> cardIndex=list.get(i);
            String Time= LocalDateTime.now().format(DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss"));
            for (int j = 0; j <keys.length ; j++) {
                Object oVlue = cardIndex.get(""+keys[j]);
                String SVlue = oVlue!=null?oVlue.toString():"";
                out_map.put(""+keys[j],SVlue);
            }
            out_map.put("OptionTime",Time);//加上操作时间
            out_list.add(out_map);
            if ((i+1)%OutSize==0 || (i+1)==list.size()){
                //执行导出
                if(out_list.size()>0){
                    Write(fileName_flow,out_list,Outcolumns,null,keys);
                    out_list = new ArrayList<>();
                }

            }
        }

    }



    /**
     * 输出 操作数据
     * @param list 输出数据
     * @param fileName_flow 文件名
     * @param Outcolumns   输出表头
     * @param keys  输出列
     * @param defOutcolumns   默认输出 【默认】该批次都会写入的数据
     * @param OutSize  多少条数据进行一次输出
     */
    public void OutCSVObj(List<Map<String, Object>> list , String fileName_flow,String Outcolumns[],String keys[],Map<String, Object> defOutcolumns,int OutSize){
        List<Map<String, Object>> out_list = new ArrayList<>();
        for (int i = 0; i < list.size(); i++) {
            Map<String, Object> out_map= new HashMap<String, Object>();
            Map<String,Object> cardIndex=list.get(i);
            String Time= LocalDateTime.now().format(DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss"));
            for (int j = 0; j <keys.length ; j++) {
                Object oVlue = cardIndex.get(""+keys[j]);
                String SVlue = oVlue!=null?oVlue.toString():"";
                out_map.put(""+keys[j],SVlue);
            }
            if(defOutcolumns!=null){
                for(String key:defOutcolumns.keySet()){
                    //System.out.println("key="+key+"and value=" +defOutcolumns.get(key));
                    out_map.put(""+key,defOutcolumns.get(key));
                }
            }
            out_map.put("OptionTime",Time);//加上操作时间
            out_list.add(out_map);
            if ((i+1)%OutSize==0 || (i+1)==list.size()){
                //执行导出
                if(out_list.size()>0){
                    Write(fileName_flow,out_list,Outcolumns,null,keys);
                    out_list = new ArrayList<>();
                }
            }
        }

    }




    /**
     * 输出 操作数据
     * @param list 输出数据
     * @param fileName_flow 文件名
     * @param Outcolumns   输出表头
     * @param keys  输出列
     * @param defOutcolumns   默认输出 【默认】该批次都会写入的数据
     * @param OutSize  多少条数据进行一次输出
     */
    public void OutCSVStr(List<Map<String, String>> list , String fileName_flow,String Outcolumns[],String keys[],Map<String, Object> defOutcolumns,int OutSize){
        List<Map<String, Object>> out_list = new ArrayList<>();
        for (int i = 0; i < list.size(); i++) {
            Map<String, Object> out_map= new HashMap<String, Object>();
            Map<String,String> cardIndex=list.get(i);
            String Time= LocalDateTime.now().format(DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss"));
            for (int j = 0; j <keys.length ; j++) {
                Object oVlue = cardIndex.get(""+keys[j]);
                String SVlue = oVlue!=null?oVlue.toString():"";
                out_map.put(""+keys[j],SVlue);
            }
            if(defOutcolumns!=null){
                for(String key:defOutcolumns.keySet()){
                    //System.out.println("key="+key+"and value=" +defOutcolumns.get(key));
                    out_map.put(""+key,""+defOutcolumns.get(key));
                }
            }
            out_map.put("OptionTime",Time);//加上操作时间
            out_list.add(out_map);
            if ((i+1)%OutSize==0 || (i+1)==list.size()){
                //执行导出
                if(out_list.size()>0){
                    Write(fileName_flow,out_list,Outcolumns,null,keys);
                    out_list = new ArrayList<>();
                }
            }
        }

    }




    /**
     * 读取
     * @param readpath 读取路径
     * @param Redcolumns 列名
     * @return
     * @throws Exception
     */
    public static List<Map<String,Object>> readCSV(String readpath,List<String> Redcolumns)  {
        List<Map<String,Object>> Rlist = new ArrayList<>();
        File inFile = new File(readpath);
        try
        {
            boolean sign = Redcolumns!=null&&Redcolumns.size()>0?true:false;
            BufferedReader reader = new BufferedReader(new InputStreamReader(new FileInputStream(inFile),"GB2312"));
            while(reader.ready())
            {
                String line = reader.readLine();
                StringTokenizer st = new StringTokenizer(line, ",");

                if (st.hasMoreTokens() && sign)
                {
                    Map<String,Object> obj = new HashMap<>();

                    for (int i = 0; i < Redcolumns.size(); i++) {
                        obj.put(""+Redcolumns.get(i),st.nextToken());
                    }
                    System.out.println(obj);
                    Rlist.add(obj);
                }
                else
                {
                    Redcolumns = new ArrayList<>();
                    while (st.hasMoreTokens()){ // 判断是否已经到结尾
                        Redcolumns.add(st.nextToken()); // 打印下一个字段
                    }
                    sign = true;
                }
            }
            reader.close();
        }
        catch (FileNotFoundException e)
        {
            e.printStackTrace();
        }
        catch (IOException e)
        {
            e.printStackTrace();
        }
        return Rlist;
    }










    public static void main(String[] args) {
       /* String path = "/getcsv/2a8984d344e04b878956521960d03fa5_CardOut.csv";
        String Prefix = path.split("/")[1];
        path = path.substring(Prefix.length()+2,path.length());
        System.out.println(path);*/
        List<Map<String,Object>>   dataList = WriteCSV.readCSV("F:\\谷歌下载\\新建文本文档 (3).csv",null);
        System.out.println(dataList);



        Map<String, List<Map<String,Object>>> collect = dataList.stream().collect(Collectors.groupingBy(scope->scope.get("dept_id").toString()+'-'+scope.get("user_id").toString()));


        System.out.println(JSON.toJSONString(collect));


    }


}
