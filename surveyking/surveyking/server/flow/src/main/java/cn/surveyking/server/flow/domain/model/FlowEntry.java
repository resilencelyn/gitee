package cn.surveyking.server.flow.domain.model;

import cn.surveyking.server.flow.domain.handler.FlowEntryNodeTypeHandler;
import com.baomidou.mybatisplus.annotation.TableField;
import com.baomidou.mybatisplus.annotation.TableId;
import com.baomidou.mybatisplus.annotation.TableName;
import lombok.Data;

import java.io.Serializable;
import java.util.Date;
import java.util.List;

/**
 * 流程实体
 *
 * @TableName t_flow_entry
 */
@TableName(value = "t_flow_entry", autoResultMap = true)
@Data
public class FlowEntry implements Serializable {

	/**
	 * 主键
	 */
	@TableId(value = "id")
	private String id;

	/**
	 * 项目id, 等于流程定义key
	 */
	@TableField(value = "project_id")
	private String projectId;

	/**
	 * 流程定义 ID = processKey:version:deploymentId
	 */
	@TableField(value = "process_definition_id")
	private String processDefinitionId;

	/**
	 * 部署id
	 */
	@TableField(value = "deploy_id")
	private String deployId;

	/**
	 * 流程XML
	 */
	@TableField(value = "bpmn_xml")
	private String bpmnXml;

	/**
	 * 临时节点
	 */
	@TableField(typeHandler = FlowEntryNodeTypeHandler.class, value = "nodes")
	private List<FlowEntryNode> nodes;

	/**
	 * 流程图标
	 */
	@TableField(value = "icon")
	private String icon;

	/**
	 * 0未发布 1已发布
	 */
	@TableField(value = "status")
	private Integer status;

	/**
	 * 创建时间
	 */
	@TableField(value = "create_at")
	private Date createAt;

	/**
	 *
	 */
	@TableField(value = "create_by")
	private String createBy;

	/**
	 * 更新时间
	 */
	@TableField(value = "update_at")
	private Date updateAt;

	/**
	 *
	 */
	@TableField(value = "update_by")
	private String updateBy;

	@TableField(exist = false)
	private static final long serialVersionUID = 1L;

}