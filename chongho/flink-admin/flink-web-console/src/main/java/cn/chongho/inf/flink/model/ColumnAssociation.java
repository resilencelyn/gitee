package cn.chongho.inf.flink.model;

import lombok.Data;

/**
 * @author ming
 */
@Data
public class ColumnAssociation {

    private String sourceColumnName;

    private String sourceColumnType;

    private String targetColumnName;
}
