package cn.chongho.inf.flink.model;

import lombok.Data;

import javax.persistence.Entity;
import javax.persistence.Id;
import javax.persistence.Transient;
import java.util.Date;

/**
 * @author ming
 */
@Data
@Entity
public class Job {
    @Id
    private Integer id;

    private String jobName;

    @Transient
    private String jobKey;

    private Integer jarId;

    @Transient
    private String fileName;

    private String entryClass;

    private String args;

    private Integer targetDbId;

    private Integer parallelism;

    private String savepointPath;

    private String jobId;

    private Integer status;

    private Integer enableFlag;

    private Integer flinkColonyId;

    @Transient
    private String flinkColonyUrl;

    @Transient
    private String flinkColonyName;

    private String remark;

    private Date createTime;

    private Integer createUserId;

    private Date updateTime;

    private Integer updateUserId;

}
