package com.yunze.common.utils.Email;

import com.alibaba.fastjson.JSON;
import com.yunze.common.utils.yunze.AesEncryptUtil;

import java.text.MessageFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;


/**
 * 
 * @ClassName: EmailSend   
 * @Description: TODO(邮件抄送类)   
 * @author zhangfeng
 * @date 2018年8月28日
 */
public class EmailSend {

	private static  String userName = "123123@5iot.cn"; // 用户邮箱地址
	private static	String password = "123@1111111"; // 密码或者授权码
	private static	String url  = "demo.5iot.com"; // 网页地址






	
	





	public static Map<String,Object> Shopping_text(String ord_no,String createTime,String shipping_Phone,String shipping_user,String province,String city,String district,String address,String by_count,String product_name,String info) {
		Map<String, Object> rmap = new HashMap<String, Object>();
		String template = "";
		String text ="<tbody><tr><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-corner-nw.png') top left no-repeat;\"width=\"6\"height=\"5\"><div></div></td><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-pixel-n.png') top center repeat-x;\"height=\"5\"><div></div></td><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-corner-ne.png') top right no-repeat;\"width=\"6\"height=\"5\"><div></div></td></tr><tr><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-pixel-w.png') center left repeat-y;\"width=\"6\"><div></div></td><td><div style=\"font-family: Roboto-Regular,Helvetica,Arial,sans-serif;padding-left: 20px; padding-right: 20px;border-bottom: thin solid #f0f0f0; color: rgba(0,0,0,0.87); font-size: 24px; padding-top: 40px; text-align: center; word-break: break-word;\"><div class=\"v2sp\"><h3>您有新的待发货订单</h3><p style=\"font-family: Roboto-Regular,Helvetica,Arial,sans-serif;color: rgba(0,0,0,0.87); font-size: 16px; line-height: 1.8;\">[<a href=\"http://"+url+"/order/OrderShopping/{0}\"rel=\"noopener\"target=\"_blank\">{0}</a>]</p></div></div><div style=\"font-family: Roboto-Regular,Helvetica,Arial,sans-serif; font-size: 13px; color: rgba(0,0,0,0.87); line-height: 1.6;padding-left: 20px; padding-right: 20px;padding-bottom: 32px; padding-top: 24px;\"><p class=\"v2sp\">下单时间：[{1}]</p><p>商品名：[{2}]</p><p>下单数量：[{3}]</p><p>收件人姓名：[{4}]</p><p>收件人联系电话：[{5}]</p><p>收件人地址：[{6}]-[{7}]-[{8}]({9})</p><p>订单备注：[{10}]</p><p class=\"v2sp\">如果这不是您本人所为，则可能是有人误输了您的电子邮件地址。请勿将此订单信息泄露给他人，并且您目前无需执行任何其它操作。</p></div></td><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-pixel-e.png') center left repeat-y;\"width=\"6\"><div></div></td></tr><tr><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-corner-sw.png') top left no-repeat;\"width=\"6\"height=\"5\"><div></div></td><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-pixel-s.png') top center repeat-x\"height=\"5\"><div></div></td><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-corner-se.png') top left no-repeat;\"width=\"6\"height=\"5\"><div></div></td></tr></tbody></table>";
		template = MessageFormat.format(text,ord_no,createTime,product_name,by_count,shipping_user,shipping_Phone,province,city,district,address,info);
		rmap.put("template", template);
		return rmap;

	}


	/**
	 * 有用量停机
	 * @param id
	 * @param createTime
	 * @param Count
	 * @return
	 */
	public static Map<String,Object> CardUsefulAmountDowntime_text(String id,String createTime,String Count) {
		Map<String, Object> rmap = new HashMap<String, Object>();
		String template = "";

		String text ="<tbody><tr><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-corner-nw.png') top left no-repeat;\"width=\"6\"height=\"5\"><div></div></td><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-pixel-n.png') top center repeat-x;\"height=\"5\"><div></div></td><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-corner-ne.png') top right no-repeat;\"width=\"6\"height=\"5\"><div></div></td></tr><tr><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-pixel-w.png') center left repeat-y;\"width=\"6\"><div></div></td><td><div style=\"font-family: Roboto-Regular,Helvetica,Arial,sans-serif;padding-left: 20px; padding-right: 20px;border-bottom: thin solid #f0f0f0; color: rgba(0,0,0,0.87); font-size: 24px; padding-top: 40px; text-align: center; word-break: break-word;\"><div class=\"v2sp\"><h3>已订购用量状态已停机</h3><p style=\"font-family: Roboto-Regular,Helvetica,Arial,sans-serif;color: rgba(0,0,0,0.87); font-size: 16px; line-height: 1.8;\">[<a href=\"http://"+url+"/card/ExecutionTask/{0}\"rel=\"noopener\"target=\"_blank\">点击查看详情</a>]</p></div></div><div style=\"font-family: Roboto-Regular,Helvetica,Arial,sans-serif; font-size: 13px; color: rgba(0,0,0,0.87); line-height: 1.6;padding-left: 20px; padding-right: 20px;padding-bottom: 32px; padding-top: 24px;\"><p class=\"v2sp\">通知时间：[{1}]</p><p>总数量：[{2}]</p><p class=\"v2sp\">如果这不是您本人所为，则可能是有人误输了您的电子邮件地址。请勿将此订单信息泄露给他人，并且您目前无需执行任何其它操作。</p></div></td><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-pixel-e.png') center left repeat-y;\"width=\"6\"><div></div></td></tr><tr><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-corner-sw.png') top left no-repeat;\"width=\"6\"height=\"5\"><div></div></td><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-pixel-s.png') top center repeat-x\"height=\"5\"><div></div></td><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-corner-se.png') top left no-repeat;\"width=\"6\"height=\"5\"><div></div></td></tr></tbody></table>";
		template = MessageFormat.format(text,id,createTime,Count);
		rmap.put("template", template);
		return rmap;
	}

	/**
	 * 未划分资费组
	 * @param parMap
	 * @param createTime
	 * @param Count
	 * @return
	 */
	public static Map<String,Object> NotDividedIntoTariffGroups_text(Map<String,Object> parMap,String createTime,String Count) {
		Map<String, Object> rmap = new HashMap<String, Object>();
		String template = "";

		String text ="<tbody><tr><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-corner-nw.png') top left no-repeat;\"width=\"6\"height=\"5\"><div></div></td><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-pixel-n.png') top center repeat-x;\"height=\"5\"><div></div></td><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-corner-ne.png') top right no-repeat;\"width=\"6\"height=\"5\"><div></div></td></tr><tr><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-pixel-w.png') center left repeat-y;\"width=\"6\"><div></div></td><td><div style=\"font-family: Roboto-Regular,Helvetica,Arial,sans-serif;padding-left: 20px; padding-right: 20px;border-bottom: thin solid #f0f0f0; color: rgba(0,0,0,0.87); font-size: 24px; padding-top: 40px; text-align: center; word-break: break-word;\"><div class=\"v2sp\"><h3>未划分资费组</h3><p style=\"font-family: Roboto-Regular,Helvetica,Arial,sans-serif;color: rgba(0,0,0,0.87); font-size: 16px; line-height: 1.8;\">[<a href=\"http://"+url+"/card/card/{0}\"rel=\"noopener\"target=\"_blank\">点击查看详情</a>]</p></div></div><div style=\"font-family: Roboto-Regular,Helvetica,Arial,sans-serif; font-size: 13px; color: rgba(0,0,0,0.87); line-height: 1.6;padding-left: 20px; padding-right: 20px;padding-bottom: 32px; padding-top: 24px;\"><p class=\"v2sp\">通知时间：[{1}]</p><p>总数量：[{2}]</p><p class=\"v2sp\">如果这不是您本人所为，则可能是有人误输了您的电子邮件地址。请勿将此订单信息泄露给他人，并且您目前无需执行任何其它操作。</p></div></td><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-pixel-e.png') center left repeat-y;\"width=\"6\"><div></div></td></tr><tr><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-corner-sw.png') top left no-repeat;\"width=\"6\"height=\"5\"><div></div></td><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-pixel-s.png') top center repeat-x\"height=\"5\"><div></div></td><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-corner-se.png') top left no-repeat;\"width=\"6\"height=\"5\"><div></div></td></tr></tbody></table>";
		try {
			template = MessageFormat.format(text, AesEncryptUtil.encrypt(JSON.toJSONString(parMap)),createTime,Count);
		}catch (Exception e){
			System.out.println(e.getMessage());
		}
		rmap.put("template", template);
		return rmap;
	}

	/**
	 * 未划分通道
	 * @param emailMap
	 * @param createTime
	 * @param Count
	 * @return
	 */
	public static Map<String,Object> UndividedChannel_text(Map<String,Object> emailMap,String createTime,String Count) {
		Map<String, Object> rmap = new HashMap<String, Object>();
		String template = "";

		String text ="<tbody><tr><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-corner-nw.png') top left no-repeat;\"width=\"6\"height=\"5\"><div></div></td><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-pixel-n.png') top center repeat-x;\"height=\"5\"><div></div></td><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-corner-ne.png') top right no-repeat;\"width=\"6\"height=\"5\"><div></div></td></tr><tr><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-pixel-w.png') center left repeat-y;\"width=\"6\"><div></div></td><td><div style=\"font-family: Roboto-Regular,Helvetica,Arial,sans-serif;padding-left: 20px; padding-right: 20px;border-bottom: thin solid #f0f0f0; color: rgba(0,0,0,0.87); font-size: 24px; padding-top: 40px; text-align: center; word-break: break-word;\"><div class=\"v2sp\"><h3>未划分通道</h3><p style=\"font-family: Roboto-Regular,Helvetica,Arial,sans-serif;color: rgba(0,0,0,0.87); font-size: 16px; line-height: 1.8;\">[<a href=\"http://"+url+"/card/card/{0}\"rel=\"noopener\"target=\"_blank\">点击查看详情</a>]</p></div></div><div style=\"font-family: Roboto-Regular,Helvetica,Arial,sans-serif; font-size: 13px; color: rgba(0,0,0,0.87); line-height: 1.6;padding-left: 20px; padding-right: 20px;padding-bottom: 32px; padding-top: 24px;\"><p class=\"v2sp\">通知时间：[{1}]</p><p>总数量：[{2}]</p><p class=\"v2sp\">如果这不是您本人所为，则可能是有人误输了您的电子邮件地址。请勿将此订单信息泄露给他人，并且您目前无需执行任何其它操作。</p></div></td><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-pixel-e.png') center left repeat-y;\"width=\"6\"><div></div></td></tr><tr><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-corner-sw.png') top left no-repeat;\"width=\"6\"height=\"5\"><div></div></td><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-pixel-s.png') top center repeat-x\"height=\"5\"><div></div></td><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-corner-se.png') top left no-repeat;\"width=\"6\"height=\"5\"><div></div></td></tr></tbody></table>";
		try {
			template = MessageFormat.format(text, AesEncryptUtil.encrypt(JSON.toJSONString(emailMap)),createTime,Count);

		}catch (Exception e){
			System.out.println(e.getMessage());
		}
		rmap.put("template", template);
		return rmap;
	}



	/**
	 * 归属为总平台
	 * @param findCardMap
	 * @param createTime
	 * @param Count
	 * @return
	 */
	public static Map<String,Object> CardConsumption_text(Map<String,Object> findCardMap,String createTime,String Count) {
		Map<String, Object> rmap = new HashMap<String, Object>();
		String template = "";

		String text ="<tbody><tr><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-corner-nw.png') top left no-repeat;\"width=\"6\"height=\"5\"><div></div></td><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-pixel-n.png') top center repeat-x;\"height=\"5\"><div></div></td><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-corner-ne.png') top right no-repeat;\"width=\"6\"height=\"5\"><div></div></td></tr><tr><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-pixel-w.png') center left repeat-y;\"width=\"6\"><div></div></td><td><div style=\"font-family: Roboto-Regular,Helvetica,Arial,sans-serif;padding-left: 20px; padding-right: 20px;border-bottom: thin solid #f0f0f0; color: rgba(0,0,0,0.87); font-size: 24px; padding-top: 40px; text-align: center; word-break: break-word;\"><div class=\"v2sp\"><h3>归属为总平台</h3><p style=\"font-family: Roboto-Regular,Helvetica,Arial,sans-serif;color: rgba(0,0,0,0.87); font-size: 16px; line-height: 1.8;\">[<a href=\"http://"+url+"/card/card/{0}\"rel=\"noopener\"target=\"_blank\">点击查看详情</a>]</p></div></div><div style=\"font-family: Roboto-Regular,Helvetica,Arial,sans-serif; font-size: 13px; color: rgba(0,0,0,0.87); line-height: 1.6;padding-left: 20px; padding-right: 20px;padding-bottom: 32px; padding-top: 24px;\"><p class=\"v2sp\">通知时间：[{1}]</p><p>总数量：[{2}]</p><p class=\"v2sp\">如果这不是您本人所为，则可能是有人误输了您的电子邮件地址。请勿将此订单信息泄露给他人，并且您目前无需执行任何其它操作。</p></div></td><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-pixel-e.png') center left repeat-y;\"width=\"6\"><div></div></td></tr><tr><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-corner-sw.png') top left no-repeat;\"width=\"6\"height=\"5\"><div></div></td><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-pixel-s.png') top center repeat-x\"height=\"5\"><div></div></td><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-corner-se.png') top left no-repeat;\"width=\"6\"height=\"5\"><div></div></td></tr></tbody></table>";
		try {
			template = MessageFormat.format(text, AesEncryptUtil.encrypt(JSON.toJSONString(findCardMap)),createTime,Count);

		}catch (Exception e){
			System.out.println(e.getMessage());
		}
		rmap.put("template", template);
		return rmap;
	}



	public static void main(String[] args) {
		Map<String, Object> rmap = new HashMap<String, Object>();
		boolean bool=true;
		Map<String, Object> Text =CardUsefulAmountDowntime_text("1","2021-09-24 13:48:04","53");
		try {
			MimeMessageDTO mimeDTO = new MimeMessageDTO();
			mimeDTO.setSentDate(new Date());
			mimeDTO.setSubject("用量已订购状态已停机数量["+"53"+"]");
			mimeDTO.setText(Text.get("template").toString());
			bool=MailUtil.sendEmail(userName, password, "123@qq.com", mimeDTO);
		}catch(Exception e) {
			bool=false;
		}
		rmap.put("bool", bool);
		System.out.println(rmap);
	}





	
}
