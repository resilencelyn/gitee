package cn.chongho.inf.flink.model;

import lombok.Data;

import javax.persistence.Id;
import java.io.Serializable;

/**
 * @author ming
 */
@Data
public class CdcJobRunConfig implements Serializable {

    private static final long serialVersionUID = 1L;

    @Id
    private Integer id;

    private String jobType;

    private String entryClass;

    private Integer jarId;
}
