package cn.chongho.inf.flink.model;

import lombok.Data;

import javax.persistence.Id;
import javax.persistence.Transient;
import java.io.Serializable;
import java.util.Date;

/**
 * @author ming
 */
@Data
public class CdcJob implements Serializable {

    private static final long serialVersionUID = 1L;

    @Id
    private Integer id;

    private Integer jobType;

    private String sourceInfo;

    private Integer targetDbId;

    private Integer targetTableId;

    @Transient
    private String targetDbName;

    @Transient
    private String targetTableName;

    /**
     * 同步的列对应信息
     */
    private String columnAssociation;

    private String primaryColumn;

    private String scanStartupMode;

    private String skippedOperations;

    private String jobId;

    private String jobName;

    private Integer parallelism;

    private String savepointPath;

    private Integer status;

    private Integer enableFlag;

    private Integer flinkColonyId;

    @Transient
    private String flinkColonyName;

    @Transient
    private String flinkColonyUrl;

    private String remark;

    private Date createTime;

    private Integer createUserId;

    private Date updateTime;

    private Integer updateUserId;
}
