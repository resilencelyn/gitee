package cn.chongho.inf.flink.model;

import lombok.Data;

import java.io.Serializable;
import java.util.List;

/**
 * @author ming
 */
@Data
public class SourceInfo implements Serializable {

    private static final long serialVersionUID = 1L;

    private Integer dbId;

    private Integer tableId;

    private String tablePriKey;

    private List<DbTable> dbTableList;
}
