package cn.chongho.inf.flink.model;

import lombok.Data;

import java.io.Serializable;

/**
 * @author ming
 */
@Data
public class AddDbTable implements Serializable  {

    private static final long serialVersionUID = 1L;

    private Integer dbId;

    private Integer tableId;

    private String tableName;

    private String tableComment;

    private String columnAssociation;
}
