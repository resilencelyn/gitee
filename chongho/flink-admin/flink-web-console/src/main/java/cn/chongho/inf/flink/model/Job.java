package cn.chongho.inf.flink.model;

import lombok.Data;

import javax.persistence.Entity;
import javax.persistence.Transient;

/**
 * @author ming
 */
@Data
@Entity
public class Job extends BaseJob {

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
}
