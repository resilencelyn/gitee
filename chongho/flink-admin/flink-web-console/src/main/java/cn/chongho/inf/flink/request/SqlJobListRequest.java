package cn.chongho.inf.flink.request;

import lombok.Data;

/**
 * @author ming
 */
@Data
public class SqlJobListRequest extends BaseParameters {

    private String jobName = "";

    private Integer status = -1;

    private Integer flinkColonyId = -1;
}
