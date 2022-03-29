package cn.chongho.inf.flink.model;

import lombok.Data;

import javax.persistence.GeneratedValue;
import javax.persistence.Id;
import java.io.Serializable;
import java.util.Date;

/**
 * @author ming
 */
@Data
public class CheckPointInfo implements Serializable {

    private static final long serialVersionUID = 1L;

    @Id
    @GeneratedValue(generator = "JDBC")
    private Integer id;

    private Integer jobId;

    private Integer jobForm;

    private Integer pointType;

    private String triggerId;

    private String location;

    private Date createTime;

    private Integer createUserId;

    public CheckPointInfo(Integer jobId, String triggerId) {
        this.jobId = jobId;
        this.triggerId = triggerId;
        this.createTime = new Date();
    }

    public CheckPointInfo() {
    }
}
