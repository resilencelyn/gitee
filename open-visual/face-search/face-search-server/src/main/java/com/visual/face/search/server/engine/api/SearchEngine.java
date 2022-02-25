package com.visual.face.search.server.engine.api;

import com.visual.face.search.server.engine.model.MapParam;
import com.visual.face.search.server.engine.model.SearchResponse;

import java.util.List;

public interface SearchEngine {

    public boolean exist(String collectionName);

    public boolean dropCollection(String collectionName);

    public boolean createCollection(String collectionName, MapParam param);

    public boolean insertVector(String collectionName, Long keyId, String faceID, float[] vectors);

    public boolean deleteVectorByKey(String collectionName, Long keyId);

    public boolean deleteVectorByKey(String collectionName, List<Long> keyIds);

    public SearchResponse search(String collectionName, float[][] features, int topK);

}
