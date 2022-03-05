package com.visual.face.search.server.engine.impl;

import com.alibaba.proxima.be.client.*;
import com.visual.face.search.server.engine.api.SearchEngine;
import com.visual.face.search.server.engine.conf.Constant;
import com.visual.face.search.server.engine.model.*;

import java.util.*;

public class ProximaSearchEngine implements SearchEngine {

    private ProximaSearchClient client;

    /**
     * 构造获取连接对象
     * @param client
     */
    public ProximaSearchEngine(ProximaSearchClient client){
        this.client = client;
    }

    @Override
    public boolean exist(String collectionName) {
        DescribeCollectionResponse response = client.describeCollection(collectionName);
        return response.ok();
    }

    @Override
    public boolean dropCollection(String collectionName) {
        if(exist(collectionName)){
            Status status = client.dropCollection(collectionName);
            return status.ok();
        }
        return true;
    }

    @Override
    public boolean createCollection(String collectionName, MapParam param) {
        Long maxDocsPerSegment = param.getMaxDocsPerSegment();
        CollectionConfig config = CollectionConfig.newBuilder()
                .withCollectionName(collectionName)
                .withMaxDocsPerSegment(maxDocsPerSegment)
                .withForwardColumnNames(Arrays.asList(Constant.ColumnNameFaceId))
                .addIndexColumnParam(Constant.ColumnNameFaceIndex, DataType.VECTOR_FP32, 512).build();
        Status status = client.createCollection(config);
        if(status.ok()){
            return true;
        }else{
            throw new RuntimeException(status.getReason());
        }
    }

    @Override
    public boolean insertVector(String collectionName, Long keyId, String faceID, float[] vectors) {
        WriteRequest.Row insertRow = WriteRequest.Row.newBuilder()
                .withPrimaryKey(keyId)
                .addIndexValue(vectors)
                .addForwardValue(faceID)
                .withOperationType(WriteRequest.OperationType.INSERT)
                .build();
        WriteRequest writeRequest = WriteRequest.newBuilder()
                .withCollectionName(collectionName)
                .withForwardColumnList(Collections.singletonList(Constant.ColumnNameFaceId))
                .addIndexColumnMeta(Constant.ColumnNameFaceIndex, DataType.VECTOR_FP32, 512)
                .addRow(insertRow)
                .build();
        Status status = client.write(writeRequest);
        if(status.ok()){
            return true;
        }else{
            throw new RuntimeException(status.getReason());
        }
    }

    @Override
    public boolean deleteVectorByKey(String collectionName, Long keyId) {
        WriteRequest.Row deleteRow = WriteRequest.Row.newBuilder()
                .withPrimaryKey(keyId)
                .withOperationType(WriteRequest.OperationType.DELETE)
                .build();
        WriteRequest writeRequest = WriteRequest.newBuilder()
                .withCollectionName(collectionName)
                .withForwardColumnList(Collections.singletonList(Constant.ColumnNameFaceId))
                .addIndexColumnMeta(Constant.ColumnNameFaceIndex, DataType.VECTOR_FP32, 512)
                .addRow(deleteRow)
                .build();
        Status status = client.write(writeRequest);
        if(status.ok()){
            return true;
        }else{
            throw new RuntimeException(status.getReason());
        }
    }

    @Override
    public boolean deleteVectorByKey(String collectionName, List<Long> keyIds) {
        if(null == keyIds || keyIds.isEmpty()){
            return false;
        }
        List<Long> deleteIds = new ArrayList<>();
        for(Long keyId : keyIds){
            GetDocumentRequest r = GetDocumentRequest.newBuilder().withCollectionName(collectionName).withPrimaryKey(keyId).build();
            GetDocumentResponse p = client.getDocumentByKey(r);
            if(p.ok() && p.getDocument().getPrimaryKey() == keyId){
                deleteIds.add(keyId);
            }
        }
        if(deleteIds.isEmpty()){
            return true;
        }
        WriteRequest.Builder builder = WriteRequest.newBuilder().withCollectionName(collectionName);
        for(Long keyId : deleteIds){
            WriteRequest.Row deleteRow = WriteRequest.Row.newBuilder().withPrimaryKey(keyId).withOperationType(WriteRequest.OperationType.DELETE).build();
            builder.addRow(deleteRow);
        }
        Status status = client.write(builder.build());
        if(status.ok()){
            return true;
        }else{
            throw new RuntimeException(status.getReason());
        }
    }

    @Override
    public SearchResponse search(String collectionName, float[][] features, int topK) {
        QueryRequest queryRequest = QueryRequest.newBuilder()
                .withCollectionName(collectionName)
                .withKnnQueryParam(
                        QueryRequest.KnnQueryParam.newBuilder()
                                .withColumnName(Constant.ColumnNameFaceIndex)
                                .withTopk(topK)
                                .withFeatures(features)
                                .build())
                .build();
        //搜索向量
        QueryResponse queryResponse;
        try {
            queryResponse = client.query(queryRequest);
        } catch (Exception e) {
            return SearchResponse.build(SearchStatus.build(1, e.getMessage()), null);
        }
        //搜索失败
        if(!queryResponse.ok()){
            return SearchResponse.build(SearchStatus.build(1, "response status is error"), null);
        }
        //转换对象
        List<SearchResult> result = new ArrayList<>();
        SearchStatus status = SearchStatus.build(0, "success");
        for (int i = 0; i < queryResponse.getQueryResultCount(); ++i) {
            List<SearchDocument> documents = new ArrayList<>();
            QueryResult queryResult = queryResponse.getQueryResult(i);
            for (int d = 0; d < queryResult.getDocumentCount(); ++d) {
                Document document = queryResult.getDocument(d);
                long primaryKey = document.getPrimaryKey();
                float score = document.getScore();
                Set<String> forwardKeys = document.getForwardKeySet();
                String faceId = null;
                if(forwardKeys.contains(Constant.ColumnNameFaceId)){
                    faceId = document.getForwardValue(Constant.ColumnNameFaceId).getStringValue();
                }
                documents.add(SearchDocument.build(primaryKey, score, faceId));
            }
            result.add(SearchResult.build(documents));
        }
        //返回
        return SearchResponse.build(status, result);
    }

}
