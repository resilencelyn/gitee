package cn.chongho.inf.flink.model;

import lombok.Data;

/**
 * @author ming
 */
@Data
public class Progress {

    /**
     *已读取文件的比特数
     */
    private long bytesRead;

    /**
     * 文件总比特数
     */
    private long contentLength;
}
