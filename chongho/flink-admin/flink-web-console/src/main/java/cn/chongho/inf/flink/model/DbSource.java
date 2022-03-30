package cn.chongho.inf.flink.model;

import lombok.Data;

import javax.persistence.GeneratedValue;
import javax.persistence.Id;
import java.io.Serializable;
import java.util.Date;

/**
 * @author ming
 */
@Data
public class DbSource implements Serializable {

    private static final long serialVersionUID = 1L;

    @Id
    @GeneratedValue(generator = "JDBC")
    private Integer id;

    private String name;

    private String providerType;

    private String url;

    private String username;

    private Integer enableFlag;

    private String password;

    private String remark;

    private Date createTime;

    private Integer createUserId;

    private Date updateTime;

    private Integer updateUserId;

    public DbSource(String name, String providerType, String url, String username, Integer enableFlag, String password) {
        this.name = name;
        this.providerType = providerType;
        this.url = url;
        this.username = username;
        this.enableFlag = enableFlag;
        this.password = password;
    }

    public DbSource(Integer id) {
        this.id = id;
    }

    public DbSource() {
    }
}
