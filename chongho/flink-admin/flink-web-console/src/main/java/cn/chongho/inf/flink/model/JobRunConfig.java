package cn.chongho.inf.flink.model;

import lombok.Data;

import javax.persistence.Id;
import javax.persistence.Table;
import javax.persistence.Transient;
import java.io.Serializable;
import java.util.Date;

/**
 * @author ming
 */
@Data
@Table(name = "cdc_job_run_config")
public class JobRunConfig implements Serializable {

    private static final long serialVersionUID = 1L;

    @Id
    private Integer id;

    private String jobType;

    private String entryClass;

    private Integer jarId;

    @Transient
    private String jarName;

    @Transient
    private Date jarTime;
}
