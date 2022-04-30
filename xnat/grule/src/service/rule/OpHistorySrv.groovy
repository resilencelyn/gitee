package service.rule

import cn.xnatural.app.ServerTpl
import cn.xnatural.app.Utils
import cn.xnatural.enet.event.EL
import cn.xnatural.jpa.IEntity
import cn.xnatural.jpa.Repo
import com.alibaba.fastjson.JSON
import com.alibaba.fastjson.serializer.SerializerFeature
import entity.OpHistory
import entity.OpHistoryType

/**
 * 记录实体的修改历史
 */
class OpHistorySrv extends ServerTpl {

    @Lazy repo = bean(Repo, 'jpa_rule_repo')


    /**
     * 操作历史
     * @param entity 实体
     */
    @EL(name = 'enHistory', async = true)
    void enHistory(IEntity entity, String operatorId) {
        final data = Utils.copier(entity, [:]).ignore("metaClass")
            .addConverter('createTime', {v -> if (v instanceof Date) v.time else v})
            .addConverter('updateTime', {v -> if (v instanceof Date) v.time else v})
            .build()
        repo.saveOrUpdate(
            new OpHistory(type: OpHistoryType.valueOf(entity.class.simpleName), operator: operatorId, dataId: data['id'], content: JSON.toJSONString(data, SerializerFeature.WriteMapNullValue, SerializerFeature.DisableCircularReferenceDetect))
        )
    }
}
