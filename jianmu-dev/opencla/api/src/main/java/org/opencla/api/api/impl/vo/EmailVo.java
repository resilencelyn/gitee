package org.opencla.api.api.impl.vo;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Getter;
import lombok.NoArgsConstructor;

import java.util.ArrayList;
import java.util.List;

/**
 * 邮箱vo
 */
@Builder
@Getter
@AllArgsConstructor
@NoArgsConstructor
public class EmailVo {
    /**
     * 邮箱
     */
    private String Email;
    /**
     * 是否是主邮箱
     */
    private String primary;
    /**
     * 是否是提交邮箱
     */
    private String committed;

    /**
     * 获取标签
     *
     * @return
     */
    public List<String> getLabels() {
        var labels = new ArrayList<String>();
        if ("primary".equals(this.primary)) {
            labels.add("主邮箱");
        }
        if ("committed".equals(this.committed)) {
            labels.add("提交邮箱");
        }
        return labels;
    }
}
