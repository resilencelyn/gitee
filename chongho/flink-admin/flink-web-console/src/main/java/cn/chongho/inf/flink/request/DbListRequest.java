package cn.chongho.inf.flink.request;

import lombok.Data;

/**
 * @author ming
 */
@Data
public class DbListRequest extends BaseParameters{

    private String name = "";

    private String providerType = "";

    private String url = "";

    private String remark = "";
}
