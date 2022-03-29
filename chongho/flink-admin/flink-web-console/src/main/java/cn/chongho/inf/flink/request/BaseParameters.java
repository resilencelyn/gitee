package cn.chongho.inf.flink.request;

import lombok.Data;

/**
 * @author ming
 */
@Data
public class BaseParameters {

    private Integer page = 1;

    private Integer pageSize = 10;

    private String startTime = "";

    private String endTime = "";
}
