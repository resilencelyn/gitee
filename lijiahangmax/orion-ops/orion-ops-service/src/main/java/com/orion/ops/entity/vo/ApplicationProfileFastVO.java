package com.orion.ops.entity.vo;

import com.orion.ops.entity.dto.ApplicationProfileDTO;
import com.orion.utils.convert.TypeStore;
import lombok.Data;

/**
 * 应用环境
 *
 * @author Jiahang Li
 * @version 1.0.0
 * @since 2022/1/20 15:06
 */
@Data
public class ApplicationProfileFastVO {

    /**
     * id
     */
    private Long id;

    /**
     * 名称
     */
    private String name;

    /**
     * tag
     */
    private String tag;

    static {
        TypeStore.STORE.register(ApplicationProfileDTO.class, ApplicationProfileFastVO.class, p -> {
            ApplicationProfileFastVO vo = new ApplicationProfileFastVO();
            vo.setId(p.getId());
            vo.setName(p.getProfileName());
            vo.setTag(p.getProfileTag());
            return vo;
        });
    }

}
