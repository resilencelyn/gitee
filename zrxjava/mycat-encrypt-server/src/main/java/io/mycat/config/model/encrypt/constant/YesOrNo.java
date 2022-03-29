package io.mycat.config.model.encrypt.constant;

/**
 * @author zrx
 */
public enum YesOrNo {
	/**
	 * 否
	 */
	NO(0, "否"),
	/**
	 * 是
	 */
	YES(1, "是");

	private Integer code;
	private String value;

	YesOrNo(Integer code, String value) {
		this.code = code;
		this.value = value;
	}

	public String getValue() {
		return this.value;
	}

	public Integer getCode() {
		return this.code;
	}
}
