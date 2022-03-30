package cn.chongho.inf.flink.model;

import lombok.Data;

import java.util.List;

/**
 * @author ming
 */
@Data
public class DingTalkMsg {

    private String groupId;

    private String message;

    private List<String> atUserList;
}
