package org.opencla.verification.repository.impl;

import org.opencla.verification.model.BaseModel;
import org.opencla.verification.repository.BaseCaffeineRepository;
import org.opencla.verification.repository.IRepository;

/**
 * caffeine仓储
 * Created by liamjung on 2018/1/23.
 */
public class CaffeineRepository extends BaseCaffeineRepository implements IRepository {

    public CaffeineRepository(int identifyModelExpiration) {
        super(identifyModelExpiration);
    }


    @Override
    public void save(BaseModel model) {

        CACHE.put(model.id(), model);
    }

    @Override
    public void update(BaseModel model) {

        this.save(model);
    }

    @Override
    public BaseModel get(String identify, String flag) {

        return CACHE.getIfPresent(BaseModel.id(identify, flag));
    }

    @Override
    public void delete(String phoneNo, String flag) {

        //异步操作
        CACHE.invalidate(BaseModel.id(phoneNo, flag));

        //同步缓存
//        CACHE.cleanUp();
    }
}
