package com.visual.face.search.core.utils;

public class Similarity {

    /**
     * 向量余弦相似度
     * @param leftVector
     * @param rightVector
     * @return
     */
    public static float cosineSimilarity(float[] leftVector, float[] rightVector) {
        double dotProduct = 0;
        for (int i=0; i< leftVector.length; i++) {
            dotProduct += leftVector[i] * rightVector[i];
        }
        double d1 = 0.0d;
        for (float value : leftVector) {
            d1 += Math.pow(value, 2);
        }
        double d2 = 0.0d;
        for (float value : rightVector) {
            d2 += Math.pow(value, 2);
        }
        double cosineSimilarity;
        if (d1 <= 0.0 || d2 <= 0.0) {
            cosineSimilarity = 0.0;
        } else {
            cosineSimilarity = (dotProduct / (Math.sqrt(d1) * Math.sqrt(d2)));
        }
        return (float) cosineSimilarity;
    }
    

}
