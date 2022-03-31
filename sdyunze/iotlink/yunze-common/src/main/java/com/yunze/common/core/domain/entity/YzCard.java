package com.yunze.common.core.domain.entity;

import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.yunze.common.annotation.Excel;
import com.yunze.common.annotation.Excel.ColumnType;
import com.yunze.common.annotation.Excel.Type;
import com.yunze.common.annotation.Excels;
import com.yunze.common.core.domain.BaseEntity;
import org.apache.commons.lang3.builder.ToStringBuilder;
import org.apache.commons.lang3.builder.ToStringStyle;

import javax.validation.constraints.Email;
import javax.validation.constraints.NotBlank;
import javax.validation.constraints.Size;
import java.util.Date;
import java.util.List;

/**
 *  yz_card_info
 * 
 * @author root
 */
public class YzCard extends BaseEntity
{

    @Excel(name = "msisdn", cellType = ColumnType.STRING, prompt = "SIM 号码")
    private String  msisdn ;

    @Excel(name = "iccid", cellType = ColumnType.STRING, prompt = "SIM 卡号")
    private String  iccid;

    @Excel(name = "imsi", cellType = ColumnType.STRING, prompt = "SIM 识别码")
    private String imsi ;

    @Excel(name = "open_date", cellType = ColumnType.STRING, prompt = "开卡日期 yyyy-MM-dd")
    private String  open_date ;

    @Excel(name = "activate_date", cellType = ColumnType.STRING, prompt = "激活日期 yyyy-MM-dd")
    private String  activate_date;

    @Excel(name = "agent_id", cellType = ColumnType.STRING, prompt = "代理编号")
    private String agent_id ;

    @Excel(name = "channel_id", cellType = ColumnType.STRING, prompt = "通道编号")
    private String  channel_id ;

    @Excel(name = "is_pool", cellType = ColumnType.STRING, prompt = "是否流量池 0 否 1 是")
    private String  is_pool;

    @Excel(name = "batch_date", cellType = ColumnType.STRING, prompt = "入库日期  yyyy-MM-dd")
    private String batch_date ;

    @Excel(name = "remarks", cellType = ColumnType.STRING, prompt = "备注信息")
    private String  remarks ;


    @Excel(name = "status_id", cellType = ColumnType.STRING, prompt = "卡状态")
    private String status_id;

    @Excel(name = "package_id", cellType = ColumnType.STRING, prompt = "资费组 编号")
    private String  package_id ;



    @Excel(name = "imei", cellType = ColumnType.STRING, prompt = "IMEI")
    private String  imei ;

    @Excel(name = "type", cellType = ColumnType.STRING, prompt = "卡类型")
    private String  type;

    @Excel(name = "network_type", cellType = ColumnType.STRING, prompt = "网络类型 ")
    private String network_type;

    @Excel(name = "is_sms", cellType = ColumnType.STRING, prompt = "是否支持短信发送  0 否 1 是")
    private String  is_sms ;

    @Excel(name = "sms_number", cellType = ColumnType.STRING, prompt = "短信服务号码")
    private String  sms_number;

    @Excel(name = "gprs", cellType = ColumnType.STRING, prompt = "GPRS是否开启   0 否 1 是")
    private String gprs;

    @Excel(name = "user_id", cellType = ColumnType.STRING, prompt = "系统登录账户id ")
    private String user_id ;

    @Excel(name = "test_period_last_time", cellType = ColumnType.STRING, prompt = "测试期最后时间 格式化为 yyyy-MM-dd 如 2022-03-01 ")
    private String test_period_last_time ;

    @Excel(name = "silent_period_last_time", cellType = ColumnType.STRING, prompt = "沉默期最后时间 格式化为 yyyy-MM-dd 如 2022-03-01 ")
    private String silent_period_last_time ;

}
