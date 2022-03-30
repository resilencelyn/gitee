package com.yunze.common.utils.yunze;

import com.alibaba.fastjson.JSON;
import org.apache.commons.collections.CollectionUtils;
import org.apache.poi.hssf.usermodel.HSSFDateUtil;
import org.apache.poi.hssf.usermodel.HSSFWorkbook;
import org.apache.poi.ss.usermodel.*;
import org.apache.poi.xssf.usermodel.XSSFWorkbook;
import org.springframework.stereotype.Component;

import javax.servlet.http.HttpServletResponse;
import java.io.*;
import java.lang.reflect.Field;
import java.net.URLEncoder;
import java.text.SimpleDateFormat;
import java.util.*;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.stream.Collectors;



@Component
public class ExcelConfig {


    /**
     * 导出excel
     */
    public static <T> void writeExcel(HttpServletResponse response, List<T> dataList, Class<T> cls){
        Field[] fields = cls.getDeclaredFields();
        List<Field> fieldList = Arrays.stream(fields)
                .filter(field -> {
                    ExcelColumnAnnotation annotation = field.getAnnotation(ExcelColumnAnnotation.class);
                    if (annotation != null && annotation.col() > 0) {
                        field.setAccessible(true);
                        return true;
                    }
                    return false;
                }).sorted(Comparator.comparing(field -> {
                    int col = 0;
                    ExcelColumnAnnotation annotation = field.getAnnotation(ExcelColumnAnnotation.class);
                    if (annotation != null) {
                        col = annotation.col();
                    }
                    return col;
                })).collect(Collectors.toList());

        Workbook wb = new XSSFWorkbook();
        Sheet sheet = wb.createSheet("Sheet1");
        AtomicInteger ai = new AtomicInteger();
        {
            Row row = sheet.createRow(ai.getAndIncrement());
            AtomicInteger aj = new AtomicInteger();
            //写入头部
            fieldList.forEach(field -> {
                ExcelColumnAnnotation annotation = field.getAnnotation(ExcelColumnAnnotation.class);
                String columnName = "";
                if (annotation != null) {
                    columnName = annotation.value();
                }
                Cell cell = row.createCell(aj.getAndIncrement());

                CellStyle cellStyle = wb.createCellStyle();
                cellStyle.setFillForegroundColor(IndexedColors.WHITE.getIndex());

                Font font = wb.createFont();
                cellStyle.setFont(font);
                cell.setCellStyle(cellStyle);
                cell.setCellValue(columnName);
            });
        }
        if (CollectionUtils.isNotEmpty(dataList)) {
            dataList.forEach(t -> {
                Row row1 = sheet.createRow(ai.getAndIncrement());
                AtomicInteger aj = new AtomicInteger();
                fieldList.forEach(field -> {
                    Class<?> type = field.getType();
                    Object value = "";
                    try {
                        value = field.get(t);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    Cell cell = row1.createCell(aj.getAndIncrement());
                    if (value != null) {
                        if (type == Date.class) {
                            cell.setCellValue(value.toString());
                        } else {
                            cell.setCellValue(value.toString());
                        }
                        cell.setCellValue(value.toString());
                    }
                });
            });
        }
        //冻结窗格
        wb.getSheet("Sheet1").createFreezePane(0, 1, 0, 1);
        //生成excel文件
        String now=new SimpleDateFormat("yyyyMMddHHmmss").format(new Date());
        String exc ="exc-"+now+".xlsx";

        OutputStream os=null;
        try {
            response.setContentType("application/vnd.ms-excel");
            response.setHeader("Access-Control-Expose-Headers", "Content-Disposition");
            response.setHeader("Content-disposition", "attachment;filename="+URLEncoder.encode(exc,"utf-8"));
            os=response.getOutputStream();
            response.flushBuffer();
            wb.write(os);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    //**************************************************************************//

    public List<Map<String,String>> getExcelListMap(String file) {
        Workbook wb =null;
        Sheet sheet = null;
        Row row = null;
        List<Map<String,String>> list = null;
        String cellData = null;
       // String filePath = "C:/Users/Administrator/Desktop/1.xlsx";
        String columns[] = {"iccid","card_no","card_define_no","card_type","package_id"};
        wb = readExcel(file);
        if(wb != null){
            //用来存放表中数据
            list = new ArrayList<Map<String,String>>();
            //获取第一个sheet
            sheet = wb.getSheetAt(0);
            //获取最大行数
            int rownum = sheet.getPhysicalNumberOfRows();
            //获取第一行
            row = sheet.getRow(0);
            //获取最大列数
            int colnum = row.getPhysicalNumberOfCells();
            for (int i = 1; i<rownum; i++) {
                Map<String,String> map = new LinkedHashMap<String,String>();
                row = sheet.getRow(i);
                if (row== null) {
                    System.out.println("this row is null ----------------------");
                    continue;

                }
                for (int j=0;j<colnum;j++){
                    System.out.println("单元格："+row.getCell(j));
                    cellData = (String) getCellFormatValue(row.getCell(j));
                    cellData=cellData.replaceAll(" ","");
                    cellData=cellData.replace("\u00A0","");
                    if(cellData.equals("")||cellData==null){
                        continue;
                    }
                    map.put(columns[j], cellData);
                }

                list.add(map);
                System.out.println(list);
            }
        }
        //遍历解析出来的list
        /*for (Map<String,String> map : list) {
            for (Map.Entry<String,String> entry : map.entrySet()) {
                System.out.print(entry.getKey()+":"+entry.getValue()+",");
            }
            System.out.println();
        }*/
        return list;
    }


    public List<Map<String,Object>> getExcelListMap(String file,String[] columns) {
        Workbook wb =null;
        Sheet sheet = null;
        Row row = null;
        List<Map<String,Object>> list = null;
        String cellData = null;
        // String filePath = "C:/Users/Administrator/Desktop/1.xlsx";
        wb = readExcel(file);
        if(wb != null){
            //用来存放表中数据
            list = new ArrayList<Map<String,Object>>();
            //获取第一个sheet
            sheet = wb.getSheetAt(0);
            //获取最大行数
            int rownum = sheet.getPhysicalNumberOfRows();
            //获取第一行
            row = sheet.getRow(0);
            //获取最大列数
            int colnum = row.getPhysicalNumberOfCells();
            for (int i = 1; i<rownum; i++) {
                Map<String,Object> map = new LinkedHashMap<String,Object>();
                row = sheet.getRow(i);
                if (row== null) {
                    //System.out.println("this row is null ----------------------");
                    continue;

                }
                for (int j=0;j<colnum;j++){
                    //System.out.println("单元格 "+columns[j]+"："+row.getCell(j));
                    cellData = (String) getCellFormatValue(row.getCell(j));
                    cellData=cellData.replaceAll(" ","");
                    cellData=cellData.replace("\u00A0","");
                    if(cellData.equals("")||cellData==null){
                       // continue;
                        cellData = null;
                    }
                    map.put(columns[j], cellData);
                }
                list.add(map);
            }
        }
        //遍历解析出来的list
        /*for (Map<String,String> map : list) {
            for (Map.Entry<String,String> entry : map.entrySet()) {
                System.out.print(entry.getKey()+":"+entry.getValue()+",");
            }
            System.out.println();
        }*/
        return list;
    }

    /**
     * 追加 生成 Vid
     * @param file
     * @param columns
     * @param maxVid
     * @return
     */
    public List<Map<String,String>> getExcelListMap(String file,String[] columns,Long maxVid) {
        Workbook wb =null;
        Sheet sheet = null;
        Row row = null;
        List<Map<String,String>> list = null;
        String cellData = null;
        // String filePath = "C:/Users/Administrator/Desktop/1.xlsx";
        wb = readExcel(file);
        if(wb != null){
            //用来存放表中数据
            list = new ArrayList<Map<String,String>>();
            //获取第一个sheet
            sheet = wb.getSheetAt(0);
            //获取最大行数
            int rownum = sheet.getPhysicalNumberOfRows();
            //获取第一行
            row = sheet.getRow(0);
            //获取最大列数
            int colnum = row.getPhysicalNumberOfCells();
            for (int i = 1; i<rownum; i++) {
                Map<String,String> map = new LinkedHashMap<String,String>();
                row = sheet.getRow(i);
                if (row== null) {
                    //System.out.println("this row is null ----------------------");
                    continue;
                }
                for (int j=0;j<colnum;j++){
                    //System.out.println("单元格 "+columns[j]+"："+row.getCell(j));
                    cellData = (String) getCellFormatValue(row.getCell(j));
                    //cellData=cellData.replaceAll(" ","");
                    //cellData=cellData.replace("\u00A0","");
                    if(cellData.trim().equals("")||cellData==null){
                        // continue;
                        cellData = null;
                    }else{
                        cellData =  cellData.trim();
                    }
                    map.put(columns[j], cellData);
                }
                maxVid++;
                map.put("vid", ""+maxVid);//增长 vid
                list.add(map);
                //System.out.println(list);
            }
        }
        //遍历解析出来的list
        /*for (Map<String,String> map : list) {
            for (Map.Entry<String,String> entry : map.entrySet()) {
                System.out.print(entry.getKey()+":"+entry.getValue()+",");
            }
            System.out.println();
        }*/
        return list;
    }



    /**
     * 读取客户信息
     * @param file
     * @return
     */
    public Map<String,Object> getExcelCustomer(String file,String[] sheet1_columns,String[] sheet2_columns,String[] sheet3_columns) {
        Workbook wb =null;
        Sheet sheet = null;
        Sheet sheet2 = null;
        Sheet sheet3 = null;
        Row row = null;
        Row row2 = null;
        Row row3 = null;
        List<Map<String,String>> CMaplist = new ArrayList<Map<String,String>>();//主表信息
        List<Map<String,String>> CatArrslist = new ArrayList<Map<String,String>>();//账号
        List<Map<String,String>> CpeopleArrslist = new ArrayList<Map<String,String>>();//人员
        String cellData = null;
        // String filePath = "C:/Users/Administrator/Desktop/1.xlsx";
        wb = readExcel(file);
        Map<String,Object> AddMap = null;
        if(wb != null){
            //用来存放表中数据
            AddMap = new HashMap<>();


            //=============[主表信息获取]========================
                //获取sheet
                sheet = wb.getSheetAt(0);
                //获取最大行数
                int rownum = sheet.getPhysicalNumberOfRows();
                //获取第一行
                row = sheet.getRow(0);
                //获取最大列数
                int colnum = row.getPhysicalNumberOfCells();
                for (int i = 1; i<rownum; i++) {
                    Map<String,String> map = new LinkedHashMap<String,String>();
                    row = sheet.getRow(i);
                    if (row== null) {
                        //System.out.println("this row is null ----------------------");
                        continue;
                    }
                    for (int j=0;j<colnum;j++){
                        //System.out.println("单元格 "+columns[j]+"："+row.getCell(j));
                        cellData = (String) getCellFormatValue(row.getCell(j));
                        //cellData=cellData.replaceAll(" ","");
                        //cellData=cellData.replace("\u00A0","");
                        if(cellData.trim().equals("")||cellData==null){
                            // continue;
                            cellData = null;
                        }else{
                            cellData =  cellData.trim();
                        }
                        map.put(sheet1_columns[j], cellData);
                    }
                    CMaplist.add(map);
                    //System.out.println(list);
                }


            //=============[账号信息获取]========================
            //获取sheet
            sheet2 = wb.getSheetAt(1);
            //获取最大行数
            int rownum2 = sheet2.getPhysicalNumberOfRows();
            //获取第一行
            row2 = sheet2.getRow(0);
            //获取最大列数
            int colnum2 = row2.getPhysicalNumberOfCells();
            for (int i = 1; i<rownum2; i++) {
                Map<String,String> map = new LinkedHashMap<String,String>();
                row2 = sheet2.getRow(i);
                if (row2== null) {
                    //System.out.println("this row is null ----------------------");
                    continue;
                }
                for (int j=0;j<colnum2;j++){
                    cellData = (String) getCellFormatValue(row2.getCell(j));
                    if(cellData.trim().equals("")||cellData==null){
                        cellData = null;
                    }else{
                        cellData =  cellData.trim();
                    }
                    map.put(sheet2_columns[j], cellData);
                }
                CatArrslist.add(map);
                //System.out.println(list);
            }

            //=============[联系人 信息获取]========================
            //获取sheet
            sheet3 = wb.getSheetAt(2);
            //获取最大行数
            int rownum3 = sheet3.getPhysicalNumberOfRows();
            //获取第一行
            row3 = sheet3.getRow(0);
            //获取最大列数
            int colnum3 = row3.getPhysicalNumberOfCells();
            for (int i = 1; i<rownum3; i++) {
                Map<String,String> map = new LinkedHashMap<String,String>();
                row3 = sheet3.getRow(i);
                if (row3== null) {
                    continue;
                }
                for (int j=0;j<colnum3;j++){
                    cellData = (String) getCellFormatValue(row3.getCell(j));
                    if(cellData.trim().equals("")||cellData==null){
                        cellData = null;
                    }else{
                        cellData =  cellData.trim();
                    }
                    map.put(sheet3_columns[j], cellData);
                }
                CpeopleArrslist.add(map);
            }
            AddMap.put("CMaplist",CMaplist);
            AddMap.put("CatArrslist",CatArrslist);
            AddMap.put("CpeopleArrslist",CpeopleArrslist);

        }


        return AddMap;
    }




    //读取excel
    public static Workbook readExcel(String filePath){
        Workbook wb = null;
        if(filePath==null){
            return null;
        }
        String extString = filePath.substring(filePath.lastIndexOf("."));
        InputStream is = null;
        try {
            is = new FileInputStream(filePath);
            if(".xls".equals(extString)){
                return wb = new HSSFWorkbook(is);
            }else if(".xlsx".equals(extString)){
                return wb = new XSSFWorkbook(is);
            }else{
                return wb = null;
            }

        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return wb;
    }
    public static Object getCellFormatValue(Cell cell){
        Object cellValue = null;
        if(cell!=null){
            //判断cell类型
            switch(cell.getCellType()){
                case NUMERIC:{
                    if (HSSFDateUtil.isCellDateFormatted(cell)) {
                        // 获取日期类型的单元格的值
                        Date d = cell.getDateCellValue();
                        // 进行格式转换
                        SimpleDateFormat formatter = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
                        cellValue = formatter.format(d);
                    } else {// 如果不是日期类型的单元格,那么将单元格格式设置成String,然后取值,这里可能会处问题,需要自己多尝试一下
                        cell.setCellType(CellType.STRING);
                        cellValue = cell.getStringCellValue();
                    }
                    //cellValue = String.valueOf((int)cell.getNumericCellValue());
                    break;
                }
                case FORMULA:{
                    //判断cell是否为日期格式
                    if(DateUtil.isCellDateFormatted(cell)){
                        //转换为日期格式YYYY-mm-dd
                        cellValue = cell.getDateCellValue();
                    }else{
                        //数字
                        cellValue = String.valueOf(cell.getNumericCellValue());
                    }
                    break;
                }
                case STRING:{
                    cellValue = cell.getRichStringCellValue().getString();
                    break;
                }
                default:
                    cellValue = "";
            }
        }else{
            cellValue = "";
        }
        return cellValue;
    }

    /**
     * 导入换卡资源
     * @param file
     * @return
     */
    public List<Map<String,String>> getReplaceExcel(String file) {
        Workbook wb =null;
        Sheet sheet = null;
        Row row = null;
        List<Map<String,String>> list = null;
        String cellData = null;
        String columns[] = {"iccid","card_no","card_define_no","package_id","replace_price","express_price","is_system","agent_id"};
        wb = readExcel(file);
        if(wb != null){
            //用来存放表中数据
            list = new ArrayList<Map<String,String>>();
            //获取第一个sheet
            sheet = wb.getSheetAt(0);
            //获取最大行数
            int rownum = sheet.getPhysicalNumberOfRows();
            //获取第一行
            row = sheet.getRow(0);
            //获取最大列数
            int colnum = row.getPhysicalNumberOfCells();
            for (int i = 1; i<rownum; i++) {
                Map<String,String> map = new LinkedHashMap<String,String>();
                row = sheet.getRow(i);
                if (row == null) {
                    //System.out.println("this row is null ----------------------");
                    continue;
                }
                for (int j=0;j<colnum;j++){
                    //System.out.println("单元格："+row.getCell(j));
                    cellData = (String) getCellFormatValue(row.getCell(j));
                    cellData=cellData.replaceAll(" ","");
                    cellData=cellData.replace("\u00A0","");
                    map.put(columns[j], cellData);
                }

                list.add(map);
                // System.out.println(list);
            }
        }
        //遍历解析出来的list

        return list;
    }

    /**
     * pl导入表
     * @param file
     * @return
     */
    public List<Map<String,String>> getPlReplaceExcel(String file) {
        Workbook wb =null;
        Sheet sheet = null;
        Row row = null;
        List<Map<String,String>> list = null;
        String cellData = null;
        String columns[] = {"iccid","ordNo","newNumber","packageName","consignee","tel","address","expressNumber","express","createTime","info"};
        wb = readExcel(file);
        if(wb != null){
            //用来存放表中数据
            list = new ArrayList<Map<String,String>>();
            //获取第一个sheet
            sheet = wb.getSheetAt(0);
            //获取最大行数
            int rownum = sheet.getPhysicalNumberOfRows();
            //获取第一行
            row = sheet.getRow(0);
            //获取最大列数
            int colnum = row.getPhysicalNumberOfCells();
            for (int i = 1; i<rownum; i++) {
                Map<String,String> map = new LinkedHashMap<String,String>();
                row = sheet.getRow(i);
                if (row == null) {
                    System.out.println("this row is null ----------------------");
                    continue;

                }
                for (int j=0;j<colnum;j++){
                    System.out.println("单元格："+row.getCell(j));
                    cellData = (String) getCellFormatValue(row.getCell(j));
                    cellData=cellData.replaceAll(" ","");
                    cellData=cellData.replace("\u00A0","");
                    map.put(columns[j], cellData);
                }

                list.add(map);
                System.out.println(list);
            }
        }
        //遍历解析出来的list

        return list;
    }
    /**
     * initFlow导入表
     * @param file
     * @return
     */
    public List<Map<String,String>> getInitFlowExcel(String file) {
        Workbook wb =null;
        Sheet sheet = null;
        Row row = null;
        List<Map<String,String>> list = null;
        String cellData = null;
        String columns[] = {"new_iccid","old_iccid","reFlow","reMoney"};
        wb = readExcel(file);
        if(wb != null){
            //用来存放表中数据
            list = new ArrayList<Map<String,String>>();
            //获取第一个sheet
            sheet = wb.getSheetAt(0);
            //获取最大行数
            int rownum = sheet.getPhysicalNumberOfRows();
            //获取第一行
            row = sheet.getRow(0);
            //获取最大列数
            int colnum = row.getPhysicalNumberOfCells();
            for (int i = 1; i<rownum; i++) {
                Map<String,String> map = new LinkedHashMap<String,String>();
                row = sheet.getRow(i);
                if (row == null) {
                    System.out.println("this row is null ----------------------");
                    continue;

                }
                for (int j=0;j<colnum;j++){
                    System.out.println("单元格："+row.getCell(j));
                    cellData = (String) getCellFormatValue(row.getCell(j));
                    cellData=cellData.replaceAll(" ","");
                    cellData=cellData.replace("\u00A0","");
                    map.put(columns[j], cellData);
                }

                list.add(map);
                System.out.println(list);
            }
        }
        //遍历解析出来的list

        return list;
    }

    public static void main(String[] args) {
        /*ExcelConfig excelConfig=new ExcelConfig();
        List<Map<String ,String >> listMap=excelConfig.getExcelListMap("D:\\钉钉存档\\down.xlsx");
        System.out.println(listMap);*/

       /* String path="D:\\钉钉存档\\down.xlsx";
        EasyExcel.read(path, ExcelImportCard.class,new ExcelListener()).sheet().doRead();
*/
        ExcelConfig excelConfig=new ExcelConfig();

        String[] sheet1_columns = {"dept_name","dname","grade_id","province","city","district","sort_id","credit_code","id_card","address","remarks","affiliation_id","tax_number","sales_id"};
        String[] sheet2_columns ={"dept_name","Cat_Name","Cat_Bank_Name","Cat_Number","Cat_atId","Cat_Address","Cat_User_Name","Cat_Zip_Code"};
        String[] sheet3_columns ={"dept_name","name","is_maste","gender_id","telephone","qq","nailed","email","job_title"};
        Map<String,Object> Rmap = excelConfig.getExcelCustomer("C:\\Users\\54655\\Desktop\\ImportCustomer.xlsx",sheet1_columns,sheet2_columns,sheet3_columns);
        System.out.println(JSON.toJSON(Rmap));

    }

    /**
     * 读取 Excel 并赋值 传入 Pmap 属性
     * @param file
     * @param columns
     * @param Pmap
     * @return
     */
    public List<Map<String,Object>> getExcelListMap(String file,String[] columns,Map<String,Object> Pmap,String type) {
        Workbook wb =null;
        Sheet sheet = null;
        Row row = null;
        List<Map<String,Object>> list = null;
        String cellData = null;
        // String filePath = "C:/Users/Administrator/Desktop/1.xlsx";
        wb = readExcel(file);

        List<String> Exlist = new ArrayList<String>();

        if(wb != null){
            //用来存放表中数据
            list = new ArrayList<Map<String,Object>>();
            //获取第一个sheet
            sheet = wb.getSheetAt(0);
            //获取最大行数
            int rownum = sheet.getPhysicalNumberOfRows();
            //获取第一行
            row = sheet.getRow(0);
            //获取最大列数
            int colnum = row.getPhysicalNumberOfCells();
            String ID_type = "";
            if(type!=null && type.length()>0){
                ID_type = type ;
            }

            for (int i = 1; i<rownum; i++) {
                Map<String,Object> map = new HashMap<>();
                map.putAll(Pmap);
                row = sheet.getRow(i);
                if (row== null) {
                    System.out.println("this row is null ----------------------");
                    continue;

                }
                for (int j=0;j<colnum;j++){
                    cellData = (String) getCellFormatValue(row.getCell(j));
                    cellData=cellData.replaceAll(" ","");
                    cellData=cellData.replace("\u00A0","");
                    if(cellData.equals("")||cellData==null){
                        // continue;
                        cellData = null;
                    }
                    map.put(columns[j], cellData);
                }
                //生产ID
                switch (ID_type){
                    case "order":
                        String ord_no = VeDate.getNo(8);
                        while (true){
                            if(!CommonlyUsed.Val_Is_Arr(Exlist,ord_no)){
                                Exlist.add(ord_no);
                                break;
                            }else{
                                ord_no = VeDate.getNo(8);
                            }
                        }
                        map.put("ord_no",ord_no);
                        break;

                }
                list.add(map);
            }
        }
        return list;
    }



}

