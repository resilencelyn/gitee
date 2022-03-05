package cn.surveyking.server.storage;

import lombok.Data;

/**
 * 存储文件的路径信息
 *
 * @author javahuang
 * @date 2021/9/8
 */
@Data
public class StorePath {

	private String fileName;

	private String filePath;

	private String thumbFilePath;

	public StorePath(String filePath) {
		this.filePath = filePath;
	}

	public StorePath(String fileNae, String filePath) {
		this.fileName = fileName;
		this.filePath = filePath;
	}

	public StorePath(String fileName, String filePath, String thumbFilePath) {
		this.fileName = fileName;
		this.filePath = filePath;
		this.thumbFilePath = thumbFilePath;
	}

}
