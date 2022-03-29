package cn.chongho.inf.flink.request;

import lombok.Data;

/**
 * @author ming
 */
@Data
public class JobListRequest extends BaseParameters{

    private String jobName = "";

    private Integer status = -1;

    private Integer jarId = -1;

    private String entryClass = "";

    private Integer flinkColonyId = -1;
}
