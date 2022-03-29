package cn.chongho.inf.flink.model;

import lombok.Data;

import javax.persistence.Id;
import javax.persistence.Table;
import java.io.Serializable;

/**
 * @author ming
 */
@Data
@Table(name = "cluster")
public class Cluster implements Serializable {

    private static final long serialVersionUID = 1L;

    @Id
    private Integer id;

    private String name;

    private String url;

    private Integer enableFlag;

    public Cluster(Integer id) {
        this.id = id;
    }

    public Cluster() {
    }
}
