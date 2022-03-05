package com.visual.face.search.server.engine.impl;

import com.visual.face.search.server.engine.api.SearchEngine;
import com.visual.face.search.server.engine.conf.Constant;
import com.visual.face.search.server.engine.model.*;
import com.visual.face.search.server.engine.utils.VectorUtils;
import io.milvus.Response.SearchResultsWrapper;
import io.milvus.client.MilvusServiceClient;
import io.milvus.grpc.*;
import io.milvus.param.*;
import io.milvus.param.collection.*;
import io.milvus.param.dml.DeleteParam;
import io.milvus.param.dml.InsertParam;
import io.milvus.param.dml.SearchParam;
import io.milvus.param.index.CreateIndexParam;
import java.util.*;

public class MilvusSearchEngine implements SearchEngine {

    private static final Integer SUCCESS_STATUE = 0;

    private MilvusServiceClient client;

    public MilvusSearchEngine(MilvusServiceClient client) {
        this.client = client;
    }

    @Override
    public boolean exist(String collectionName) {
        HasCollectionParam param = HasCollectionParam.newBuilder().withCollectionName(collectionName).build();
        R<Boolean> response = this.client.hasCollection(param);
        if(SUCCESS_STATUE.equals(response.getStatus())){
            return response.getData();
        }else{
            throw new RuntimeException(response.getMessage(), response.getException());
        }
    }

    @Override
    public boolean dropCollection(String collectionName) {
        DropCollectionParam param = DropCollectionParam.newBuilder().withCollectionName(collectionName).build();
        R<RpcStatus> response = this.client.dropCollection(param);
        if(SUCCESS_STATUE.equals(response.getStatus())){
            return true;
        }else{
            throw new RuntimeException(response.getMessage(), response.getException());
        }
    }

    @Override
    public boolean createCollection(String collectionName, MapParam param) {
        FieldType keyFieldType = FieldType.newBuilder()
                .withName(Constant.ColumnPrimaryKey)
                .withDescription("id")
                .withDataType(DataType.Int64)
                .withPrimaryKey(true)
                .withAutoID(false)
                .build();

        FieldType indexFieldType = FieldType.newBuilder()
                .withName(Constant.ColumnNameFaceIndex)
                .withDescription("face vector")
                .withDataType(DataType.FloatVector)
                .withDimension(512)
                .build();

        CreateCollectionParam createCollectionReq = CreateCollectionParam.newBuilder()
                .withCollectionName(collectionName)
                .withDescription(collectionName)
                .addFieldType(keyFieldType)
                .addFieldType(indexFieldType)
                .withShardsNum(param.getShardsNum())
                .build();

        CreateIndexParam indexParam = CreateIndexParam.newBuilder()
                .withCollectionName(collectionName)
                .withFieldName(Constant.ColumnNameFaceIndex)
                .withIndexType(IndexType.IVF_FLAT)
                .withMetricType(MetricType.L2)
                .withExtraParam("{\"nlist\":128}")
                .withSyncMode(Boolean.TRUE)
                .build();

        LoadCollectionParam loadParam = LoadCollectionParam.newBuilder()
                .withCollectionName(collectionName)
                .build();

        R<RpcStatus> response = this.client.createCollection(createCollectionReq);
        if(SUCCESS_STATUE.equals(response.getStatus())){
            R<RpcStatus> indexResponse = this.client.createIndex(indexParam);
            if(SUCCESS_STATUE.equals(indexResponse.getStatus())){
                R<RpcStatus> loadResponse = this.client.loadCollection(loadParam);
                if(SUCCESS_STATUE.equals(loadResponse.getStatus())){
                    return true;
                }else{
                    this.dropCollection(collectionName);
                    return false;
                }
            }else{
                this.dropCollection(collectionName);
                return false;
            }
        }else{
            throw new RuntimeException(response.getMessage(), response.getException());
        }
    }

    @Override
    public boolean insertVector(String collectionName, Long keyId, String faceID, float[] vectors) {
        List<InsertParam.Field> fields = new ArrayList<>();
        fields.add(new InsertParam.Field(Constant.ColumnPrimaryKey, DataType.Int64, Collections.singletonList(keyId)));
        fields.add(new InsertParam.Field(Constant.ColumnNameFaceIndex, DataType.FloatVector, Collections.singletonList(VectorUtils.convertVector(vectors))));

        InsertParam insertParam = InsertParam.newBuilder()
                .withCollectionName(collectionName)
                .withFields(fields)
                .build();

        R<MutationResult> response = this.client.insert(insertParam);
        if(SUCCESS_STATUE.equals(response.getStatus())){
            return true;
        }else{
            throw new RuntimeException(response.getMessage(), response.getException());
        }
    }

    @Override
    public boolean deleteVectorByKey(String collectionName, Long keyId) {
        String deleteExpr = Constant.ColumnPrimaryKey + " in " + "[" + keyId + "]";
        DeleteParam build = DeleteParam.newBuilder()
                .withCollectionName(collectionName)
                .withExpr(deleteExpr)
                .build();

        R<MutationResult> response = this.client.delete(build);
        if(SUCCESS_STATUE.equals(response.getStatus())){
            return true;
        }else{
            throw new RuntimeException(response.getMessage(), response.getException());
        }
    }

    @Override
    public boolean deleteVectorByKey(String collectionName, List<Long> keyIds) {
        String deleteExpr = Constant.ColumnPrimaryKey + " in " + keyIds.toString();
        DeleteParam build = DeleteParam.newBuilder()
                .withCollectionName(collectionName)
                .withExpr(deleteExpr)
                .build();

        R<MutationResult> response = this.client.delete(build);
        if(SUCCESS_STATUE.equals(response.getStatus())){
            return true;
        }else{
            throw new RuntimeException(response.getMessage(), response.getException());
        }
    }

    @Override
    public SearchResponse search(String collectionName, float[][] features, int topK) {
        SearchParam searchParam = SearchParam.newBuilder()
                .withCollectionName(collectionName)
                .withMetricType(MetricType.L2)
                .withOutFields(Collections.singletonList(Constant.ColumnPrimaryKey))
                .withTopK(topK)
                .withVectors(VectorUtils.convertVector(features))
                .withVectorFieldName(Constant.ColumnNameFaceIndex)
                .build();

        R<SearchResults> response = this.client.search(searchParam);
        if(SUCCESS_STATUE.equals(response.getStatus())){
            SearchStatus status = SearchStatus.build(0, "success");
            List<SearchResult> result = new ArrayList<>();
            if(response.getData().getResults().hasIds()){
                SearchResultsWrapper wrapper = new SearchResultsWrapper(response.getData().getResults());
                for (int i = 0; i < features.length; ++i) {
                    List<SearchDocument> documents = new ArrayList<>();
                    List<SearchResultsWrapper.IDScore> scores = wrapper.GetIDScore(i);
                    for(SearchResultsWrapper.IDScore scoreId : scores){
                        long primaryKey = scoreId.getLongID();
                        float score = scoreId.getScore();
                        documents.add(SearchDocument.build(primaryKey, score, null));
                    }
                    result.add(SearchResult.build(documents));
                }
            }
            return SearchResponse.build(status, result);
        }else{
            throw new RuntimeException(response.getMessage(), response.getException());
        }
    }

}
