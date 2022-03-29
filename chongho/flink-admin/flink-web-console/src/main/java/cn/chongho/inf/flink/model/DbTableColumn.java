package cn.chongho.inf.flink.model;

import lombok.Data;

import javax.persistence.Id;
import javax.persistence.Transient;
import java.io.Serializable;

/**
 * @author ming
 */
@Data
public class DbTableColumn implements Serializable  {

    private static final long serialVersionUID = 1L;

    @Id
    private Integer id;

    private Integer tableId;

    private String columnName;

    private String columnType;

    private String columnKey;

    private String columnComment;

    @Transient
    private String targetColumnName;


    public DbTableColumn(Integer id) {
        this.id = id;
    }

    public DbTableColumn() {
    }

    public DbTableColumn(Integer tableId, String columnName) {
        this.tableId = tableId;
        this.columnName = columnName;
    }
}
