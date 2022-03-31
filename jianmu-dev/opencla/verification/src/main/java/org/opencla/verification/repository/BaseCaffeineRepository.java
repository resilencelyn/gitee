package org.opencla.verification.repository;

import com.github.benmanes.caffeine.cache.Cache;
import com.github.benmanes.caffeine.cache.Caffeine;
import org.opencla.verification.model.BaseModel;

import java.util.concurrent.TimeUnit;

/**
 * caffeine仓储抽象类
 * Created by liamjung on 2018/1/23.
 */
public abstract class BaseCaffeineRepository {

    protected final Cache<String, BaseModel> CACHE;

    protected BaseCaffeineRepository(int identifyModelExpiration) {

        CACHE = Caffeine.newBuilder()
                .maximumSize(10000)
                .expireAfterWrite(identifyModelExpiration, TimeUnit.SECONDS)
//                .refreshAfterWrite(1, TimeUnit.SECONDS)
                .build();
    }
}
