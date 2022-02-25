package com.visual.face.search.server.engine.model;

public class SearchDocument {
    private long primaryKey;
    private float score;
    private String faceId;

    public SearchDocument(){}

    public SearchDocument(long primaryKey, float score, String faceId) {
        this.primaryKey = primaryKey;
        this.score = score;
        this.faceId = faceId;
    }

    public static SearchDocument build(long primaryKey, float score, String faceId){
        return new SearchDocument(primaryKey, score, faceId);
    }

    public long getPrimaryKey() {
        return primaryKey;
    }

    public void setPrimaryKey(long primaryKey) {
        this.primaryKey = primaryKey;
    }

    public float getScore() {
        return score;
    }

    public void setScore(float score) {
        this.score = score;
    }

    public String getFaceId() {
        return faceId;
    }

    public void setFaceId(String faceId) {
        this.faceId = faceId;
    }
}
