package cn.chongho.inf.flink.model;

import lombok.Data;

import javax.persistence.GeneratedValue;
import javax.persistence.Id;
import java.io.Serializable;

/**
 * @author ming
 */
@Data
public class DbTable implements Serializable  {

    private static final long serialVersionUID = 1L;

    @Id
    @GeneratedValue(generator = "JDBC")
    private Integer id;

    private Integer dbId;

    private String tableName;

    private String tableComment;

    public DbTable(Integer id) {
        this.id = id;
    }

    public DbTable() {
    }

    public DbTable(Integer dbId, String tableName) {
        this.dbId = dbId;
        this.tableName = tableName;
    }
}
