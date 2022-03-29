package cn.chongho.inf.flink.model;

import lombok.Data;

import javax.persistence.Id;
import java.io.Serializable;
import java.util.Date;

/**
 * @author ming
 */
@Data
public class Jar implements Serializable  {

    private static final long serialVersionUID = 1L;

    @Id
    private Integer id;

    private String jobKey;

    private Integer enableFlag;

    private String fileName;

    private Date createTime;

    private Integer createUserId;

    private Date updateTime;

    private Integer updateUserId;

    public Jar(Integer id) {
        this.id = id;
    }

    public Jar() {
    }

    public Jar(Integer id, Integer enableFlag, Date updateTime, Integer updateUserId) {
        this.id = id;
        this.enableFlag = enableFlag;
        this.updateTime = updateTime;
        this.updateUserId = updateUserId;
    }

    public Jar(Integer id, Integer enableFlag) {
        this.id = id;
        this.enableFlag = enableFlag;
    }
}
