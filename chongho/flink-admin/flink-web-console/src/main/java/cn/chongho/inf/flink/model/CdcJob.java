package cn.chongho.inf.flink.model;

import lombok.Data;

import javax.persistence.Transient;

/**
 * @author ming
 */
@Data
public class CdcJob extends BaseJob {

    private static final long serialVersionUID = 1L;

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
}
