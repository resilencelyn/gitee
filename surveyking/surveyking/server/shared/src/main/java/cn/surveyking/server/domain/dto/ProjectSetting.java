package cn.surveyking.server.domain.dto;

import cn.surveyking.server.core.constant.AnswerFreqEnum;
import lombok.Data;

import java.util.LinkedHashMap;

/**
 * @author javahuang
 * @date 2021/8/6
 */
@Data
public class ProjectSetting {

	/**
	 * 1开启 0停止
	 */
	private Integer status = 0;

	private AnswerSetting answerSetting = new AnswerSetting();

	private SubmittedSetting submittedSetting = new SubmittedSetting();

	@Data
	public static class AnswerSetting {

		/**
		 * 需要密码填写
		 */
		private String password;

		/**
		 * 显示填写进度条
		 */
		private Boolean progressBar;

		/**
		 * 是否需要登录才能作答。 需要登录答卷的几种场景：1、问卷设置直接设置 2、工作流起始节点指定发起人 3、问卷里面有成员和部门题
		 */
		private Boolean loginRequired;

		/**
		 * 是否显示题号
		 */
		private Boolean questionNumber = true;

		/**
		 * 自动保存结果
		 */
		private Boolean autoSave;

		private LinkedHashMap initialValues;

		/**
		 * 回收答案条数限制,
		 */
		private Long maxAnswers;

		/**
		 * 截止回收时间
		 */
		private Long endTime;

		/**
		 * 只能通过微信作答
		 */
		private Boolean wechatOnly;

		/**
		 * ip 答题限制
		 */
		private UniqueLimitSetting ipLimit;

		/**
		 * cookie 答题限制
		 */
		private UniqueLimitSetting cookieLimit;

		/**
		 * 登录之后答题限制
		 */
		private UniqueLimitSetting loginLimit;

	}

	@Data
	public static class SubmittedSetting {

		/**
		 * 答题完成后跳转自定义页面
		 */
		private String contentHtml;

		/**
		 * 允许更新答案
		 */
		private Boolean enableUpdate;

	}

	@Data
	public static class UniqueLimitSetting {

		/**
		 * 限制数量
		 */
		private Integer limitNum;

		/**
		 * 限制频率
		 */
		private AnswerFreqEnum limitFreq;

		public UniqueLimitSetting() {
		}

		public UniqueLimitSetting(Integer limitNum, AnswerFreqEnum limitFreq) {
			this.limitNum = limitNum;
			this.limitFreq = limitFreq;
		}

	}

}