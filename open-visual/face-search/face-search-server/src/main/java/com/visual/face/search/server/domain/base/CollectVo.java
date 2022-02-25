package com.visual.face.search.server.domain.base;

import com.visual.face.search.server.domain.extend.FiledColumn;
import io.swagger.annotations.ApiModelProperty;
import org.hibernate.validator.constraints.Length;

import javax.validation.constraints.Min;
import java.util.List;

/***
 * 集合信息对象
 */
public class CollectVo<ExtendsVo extends CollectVo<ExtendsVo>> extends BaseVo {
    /**命名空间**/
    @Length(min = 1, max = 12, message = "namespace length is not in the range")
    @ApiModelProperty(value="命名空间：最大12个字符,支持小写字母、数字和下划线的组合", position = 0,required = true)
    private String namespace;
    /**集合名称**/
    @Length(min = 1, max = 24, message = "collectionName length is not in the range")
    @ApiModelProperty(value="集合名称：最大24个字符,支持小写字母、数字和下划线的组合", position = 1,required = true)
    private String collectionName;
    /**集合描述**/
    @Length(min = 0, max = 128, message = "collectionComment length is not in the range")
    @ApiModelProperty(value="集合描述：最大128个字符", position = 2,required = false)
    private String collectionComment;
    /**数据分片中最大的文件个数**/
    @Min(value = 0, message = "maxDocsPerSegment must greater than or equal to 0")
    @ApiModelProperty(value="数据分片中最大的文件个数，默认为0（不限制）,仅对Proxima引擎生效", position = 3,required = false)
    private Long maxDocsPerSegment;
    /**数据分片中最大的文件个数**/
    @Min(value = 0, message = "shardsNum must greater than or equal to 0")
    @ApiModelProperty(value="要创建的集合的分片数，默认为0（即系统默认）,仅对Milvus引擎生效", position = 4,required = false)
    private Integer shardsNum;
    /**自定义的样本字段**/
    @ApiModelProperty(value="自定义的样本属性字段", position = 5,required = false)
    private List<FiledColumn> sampleColumns;
    /**自定义的人脸字段**/
    @ApiModelProperty(value="自定义的人脸属性字段", position = 6,required = false)
    private List<FiledColumn> faceColumns;
    /**启用binlog同步**/
    @ApiModelProperty(value="启用binlog同步。扩展字段，暂不支持该功能。", position = 7,required = false)
    private Boolean syncBinLog;

    /**
     * 构建集合对象
     * @param namespace         命名空间
     * @param collectionName    集合名称
     * @return
     */
    public static CollectVo build(String namespace, String collectionName){
        return new CollectVo().setNamespace(namespace).setCollectionName(collectionName);
    }

    public String getNamespace() {
        return namespace;
    }

    public ExtendsVo setNamespace(String namespace) {
        this.namespace = namespace;
        return (ExtendsVo) this;
    }

    public String getCollectionName() {
        return collectionName;
    }

    public ExtendsVo setCollectionName(String collectionName) {
        this.collectionName = collectionName;
        return (ExtendsVo) this;
    }

    public String getCollectionComment() {
        return collectionComment;
    }

    public ExtendsVo setCollectionComment(String collectionComment) {
        this.collectionComment = collectionComment;
        return (ExtendsVo) this;
    }

    public Long getMaxDocsPerSegment() {
        return maxDocsPerSegment;
    }

    public ExtendsVo setMaxDocsPerSegment(Long maxDocsPerSegment) {
        this.maxDocsPerSegment = maxDocsPerSegment;
        return (ExtendsVo) this;
    }

    public Integer getShardsNum() {
        return shardsNum;
    }

    public ExtendsVo setShardsNum(Integer shardsNum) {
        this.shardsNum = shardsNum;
        return (ExtendsVo) this;
    }

    public List<FiledColumn> getSampleColumns() {
        return sampleColumns;
    }

    public ExtendsVo setSampleColumns(List<FiledColumn> sampleColumns) {
        this.sampleColumns = sampleColumns;
        return (ExtendsVo) this;
    }

    public List<FiledColumn> getFaceColumns() {
        return faceColumns;
    }

    public ExtendsVo setFaceColumns(List<FiledColumn> faceColumns) {
        this.faceColumns = faceColumns;
        return (ExtendsVo) this;
    }

    public boolean isSyncBinLog() {
        return null == syncBinLog ? false : syncBinLog;
    }

    public ExtendsVo setSyncBinLog(Boolean syncBinLog) {
        this.syncBinLog = syncBinLog;
        return (ExtendsVo) this;
    }
}
