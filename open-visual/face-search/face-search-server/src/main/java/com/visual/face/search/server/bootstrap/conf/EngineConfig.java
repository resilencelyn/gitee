package com.visual.face.search.server.bootstrap.conf;

import com.alibaba.proxima.be.client.ConnectParam;
import com.alibaba.proxima.be.client.ProximaGrpcSearchClient;
import com.alibaba.proxima.be.client.ProximaSearchClient;
import com.visual.face.search.server.engine.api.SearchEngine;
import com.visual.face.search.server.engine.impl.MilvusSearchEngine;
import com.visual.face.search.server.engine.impl.ProximaSearchEngine;
import io.milvus.client.MilvusServiceClient;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

@Configuration("visualEngineConfig")
public class EngineConfig {
    //日志
    public Logger logger = LoggerFactory.getLogger(getClass());

    @Value("${visual.engine.selected:proxima}")
    private String selected;

    @Value("${visual.engine.proxima.host}")
    private String proximaHost;
    @Value("${visual.engine.proxima.port:16000}")
    private Integer proximaPort;

    @Value("${visual.engine.milvus.host}")
    private String milvusHost;
    @Value("${visual.engine.milvus.port:19530}")
    private Integer milvusPort;

    @Bean(name = "visualSearchEngine")
    public SearchEngine getSearchEngine(){
        if(selected.equalsIgnoreCase("milvus")){
            logger.info("current vector engine is milvus");
            io.milvus.param.ConnectParam connectParam = io.milvus.param.ConnectParam.newBuilder().withHost(milvusHost).withPort(milvusPort).build();
            MilvusServiceClient client = new MilvusServiceClient(connectParam);
            return new MilvusSearchEngine(client);
        }else{
            logger.info("current vector engine is proxima");
            ConnectParam connectParam = ConnectParam.newBuilder().withHost(proximaHost).withPort(proximaPort).build();
            ProximaSearchClient client = new ProximaGrpcSearchClient(connectParam);
            return new ProximaSearchEngine(client);
        }
    }

}
