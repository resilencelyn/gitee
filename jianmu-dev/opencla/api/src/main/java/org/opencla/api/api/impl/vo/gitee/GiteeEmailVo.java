package org.opencla.api.api.impl.vo.gitee;

import lombok.*;

import java.util.List;

/**
 * gitee获取email
 */
@Builder
@Getter
@AllArgsConstructor
@NoArgsConstructor
public class GiteeEmailVo {
    /**
     * 邮箱
     */
    private String email;
    /**
     * scope
     */
    private List<String> scope;
}
