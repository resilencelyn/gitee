package cn.chongho.inf.flink.request;

import lombok.Data;

/**
 * @author ming
 */
@Data
public class CdcJobListRequest extends BaseParameters{

    private String jobName = "";

    private Integer jobType = -1;

    private Integer targetDbId = -1;

    private Integer status = -1;

    private Integer flinkColonyId = -1;
}
