package cn.chongho.inf.flink.model;

import lombok.Data;

import javax.persistence.GeneratedValue;
import javax.persistence.Id;
import java.io.Serializable;
import java.time.LocalDateTime;

/**
 * @author ming
 */
@Data
public class DataAuthority implements Serializable {

    private static final long serialVersionUID = 1L;

    @Id
    @GeneratedValue(generator = "JDBC")
    private Integer id;

    private Integer dataId;

    private Integer dataType;

    private Integer userId;

    private Integer enableFlag;

    private LocalDateTime createTime;

    private LocalDateTime updateTime;

    public DataAuthority(Integer dataId, Integer dataType, Integer userId) {
        this.dataId = dataId;
        this.dataType = dataType;
        this.userId = userId;
        this.enableFlag = 1;
        this.createTime = LocalDateTime.now();
        this.updateTime = LocalDateTime.now();
    }

    public DataAuthority() {
    }
}
