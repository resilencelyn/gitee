package com.visual.face.search.server.utils;

import com.visual.face.search.core.utils.JsonUtil;
import com.visual.face.search.server.domain.extend.FieldKeyValue;
import com.visual.face.search.server.domain.extend.FieldKeyValues;
import com.visual.face.search.server.domain.extend.FiledColumn;
import com.visual.face.search.server.domain.response.CollectRepVo;
import com.visual.face.search.server.model.Collection;
import org.apache.commons.collections4.MapUtils;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class ValueUtil {

    public static List<FiledColumn> getFaceColumns(Collection collection){
        if(null != collection.getSchemaInfo() && !collection.getSchemaInfo().isEmpty()){
            CollectRepVo collectVo = JsonUtil.toEntity(collection.getSchemaInfo(), CollectRepVo.class);
            return collectVo.getFaceColumns();
        }else{
            return new ArrayList<>();
        }
    }

    public static List<FiledColumn> getSampleColumns(Collection collection){
        if(null != collection.getSchemaInfo() && !collection.getSchemaInfo().isEmpty()){
            CollectRepVo collectVo = JsonUtil.toEntity(collection.getSchemaInfo(), CollectRepVo.class);
            return collectVo.getSampleColumns();
        }else{
            return new ArrayList<>();
        }
    }

    public static FieldKeyValues getFieldKeyValues(Map<String, Object> map , List<FiledColumn> columns){
        Map<String, String> keyMap = new HashMap<>();
        for(FiledColumn column : columns){
            for(String dataKey : map.keySet()){
                if(column.getName().toLowerCase().equalsIgnoreCase(dataKey.toLowerCase())){
                    keyMap.put(column.getName(), dataKey);
                }
            }
        }
        FieldKeyValues values = FieldKeyValues.build();
        for(FiledColumn column : columns){
            if(keyMap.containsKey(column.getName())){
                values.add(FieldKeyValue.build(column.getName(), map.get(keyMap.get(column.getName()))));
            }else{
                values.add(FieldKeyValue.build(column.getName(), (String)null));
            }
        }
        return values;
    }

    public static Map<String, Map<String, Object>> mapping(List<Map<String, Object>> list, String key){
        Map<String, Map<String, Object>> res = new HashMap<>();
        for(Map<String, Object> item : list){
            String value = MapUtils.getString(item, key);
            if(null != value){
                res.put(value, item);
            }
        }
        return res;
    }

    public static Map<Long, String> mapping(List<Map<String, Object>> list, String idKey, String faceKey){
        Map<Long, String> res = new HashMap<>();
        for(Map<String, Object> item : list){
            Long idValue = MapUtils.getLongValue(item, idKey);
            String faceValue = MapUtils.getString(item, faceKey);
            if(idValue>0 && null != faceValue && !faceValue.isEmpty()){
                res.put(idValue, faceValue);
            }
        }
        return res;
    }


    public static float[] convertVector(String faceVector){
        List<Float> value = JsonUtil.toList(faceVector, Float.class);
        float[] arr = new float[value.size()];
        for(int i=0; i< value.size(); i++){
            arr[i] = value.get(i);
        }
        return arr;
    }
}
