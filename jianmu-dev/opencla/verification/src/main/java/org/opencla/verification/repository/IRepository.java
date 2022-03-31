package org.opencla.verification.repository;

import org.opencla.verification.model.BaseModel;

/**
 * 仓储
 * Created by liamjung on 2018/1/19.
 */
public interface IRepository {

    /**
     * 新增
     *
     * @param model
     */
    void save(BaseModel model);

    /**
     * 修改
     *
     * @param model
     */
    void update(BaseModel model);

    /**
     * 获取
     *
     * @param identify 验证码标识
     * @param flag     业务标志
     * @return
     */
    BaseModel get(String identify, String flag);

    /**
     * 删除
     *
     * @param identify 验证码标识
     * @param flag     业务标志
     */
    void delete(String identify, String flag);
}
