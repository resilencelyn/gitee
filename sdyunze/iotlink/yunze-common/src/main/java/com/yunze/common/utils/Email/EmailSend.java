package com.yunze.common.utils.Email;

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
	private static	String Url = "demo.5iot.com"; // 网页地址


	/**
	 * 发送注册邮件验证码
	 * @param USER_NAME
	 * @param Email
	 * @return
	 */
	public static Map<String,Object> EmailVerification(String USER_NAME,String Email) {
		Map<String, Object> rmap = new HashMap<String, Object>();
		boolean bool=true;
		Map<String, Object> Text =GetEmailVFText(USER_NAME, Email);
		try {
			MimeMessageDTO mimeDTO = new MimeMessageDTO();
			mimeDTO.setSentDate(new Date());
			mimeDTO.setSubject("验证您的电子邮箱地址");
			mimeDTO.setText(Text.get("template").toString());
			bool=MailUtil.sendEmail(userName, password, Email, mimeDTO);
		}catch(Exception e) {
			bool=false;
		}
		rmap.put("bool", bool);
		rmap.put("random", Text.get("random"));
		return rmap;
	}




	
	
	
	/**
	 * 获取注册时的邮件内容
	 * @param USER_NAME 收件人姓名
	 * @param Email 收件人邮箱
	 * @return
	 */
	public static Map<String,Object> GetEmailVFText(String USER_NAME,String Email) {
		Map<String, Object> rmap = new HashMap<String, Object>();
		String random=(int)((Math.random()*9+1)*100000)+"";
		rmap.put("random", random);
		String template="<table cellpadding=\"0\" cellspacing=\"0\">" +
				"			<tbody>" +
				"				<tr>" +
				"					<td></td>" +
				"					<td>" +
				"						<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\" style=\"direction: ltr; padding-bottom: 7px;\">" +
				"							<tbody>" +
				"								<tr>" +
				"									<td align=\"left\"><font color=\"#ffb64b\">Bi</font><font color=\"#1ba7e6\">llie</font></td>" +
				"									<td align=\"right\" style=\"font-family: Roboto-Light,Helvetica,Arial,sans-serif;\">"+USER_NAME+"</td>" +
				"									<td align=\"right\" width=\"35\"><img width=\"92\" height=\"32\" src=\"http://ssl.gstatic.com/images/branding/product/2x/avatar_circle_blue_32dp.png\" style=\"width:32px;height:32px;\"></td>" +
				"								</tr>" +
				"							</tbody>" +
				"						</table>" +
				"					</td>" +
				"					<td></td>" +
				"				</tr>" +
				"				<tr>" +
				"					<td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-corner-nw.png') top left no-repeat;\" width=\"6\" height=\"5\">" +
				"						<div></div>" +
				"					</td>" +
				"					<td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-pixel-n.png') top center repeat-x;\" height=\"5\">" +
				"						<div></div>" +
				"					</td>" +
				"					<td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-corner-ne.png') top right no-repeat;\" width=\"6\" height=\"5\">" +
				"						<div></div>" +
				"					</td>" +
				"				</tr>" +
				"				<tr>" +
				"					<td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-pixel-w.png') center left repeat-y;\" width=\"6\">" +
				"						<div></div>" +
				"					</td>" +
				"					<td>" +
				"						<div style=\"font-family: Roboto-Regular,Helvetica,Arial,sans-serif;padding-left: 20px; padding-right: 20px;border-bottom: thin solid #f0f0f0; color: rgba(0,0,0,0.87); font-size: 24px; padding-top: 40px; text-align: center; word-break: break-word;\">" +
				"							<div class=\"v2sp\">" +
				"								<h3>验证您是该电子邮件地址的所有者</h3>" +
				"								<p style=\"font-family: Roboto-Regular,Helvetica,Arial,sans-serif;color: rgba(0,0,0,0.87); font-size: 16px; line-height: 1.8;\">" +
				"									<a href=\"mailto:"+Email+"\" rel=\"noopener\" target=\"_blank\">"+Email+"</a>" +
				"								</p>" +
				"							</div>" +
				"						</div>" +
				"						<div style=\"font-family: Roboto-Regular,Helvetica,Arial,sans-serif; font-size: 13px; color: rgba(0,0,0,0.87); line-height: 1.6;padding-left: 20px; padding-right: 20px;padding-bottom: 32px; padding-top: 24px;\">" +
				"							<p class=\"v2sp\">验证该邮箱的目的是为了更好的为您服务。</p>" +
				"							<p>您可以使用此验证码来验证您是该电子邮件地址的所有者。</p>" +
				"							<div style=\"font-size:24px;padding-top:20px;padding-bottom:20px;font-weight:bold;text-align:center;\"><span style=\"border-bottom:1px dashed #ccc;z-index:1\" t=\"7\" onclick=\"return false;\" data=\""+random+"\">"+random+"</span></div>" +
				"							<p class=\"v2sp\">如果这不是您本人所为，则可能是有人误输了您的电子邮件地址。请勿将此验证码泄露给他人，并且您目前无需执行任何其它操作。</p>" +
				"						</div>" +
				"					</td>" +
				"					<td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-pixel-e.png') center left repeat-y;\" width=\"6\">" +
				"						<div></div>" +
				"					</td>" +
				"				</tr>" +
				"				<tr>" +
				"					<td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-corner-sw.png') top left no-repeat;\" width=\"6\" height=\"5\">" +
				"						<div></div>" +
				"					</td>" +
				"					<td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-pixel-s.png') top center repeat-x\" height=\"5\">" +
				"						<div></div>" +
				"					</td>" +
				"					<td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-corner-se.png') top left no-repeat;\" width=\"6\" height=\"5\">" +
				"						<div></div>" +
				"					</td>" +
				"				</tr>" +
				"				<tr>" +
				"					<td></td>" +
				"					<td>" +
				"						<div style=\"text-align: left;\">" +
				"							<div style=\"font-family: Roboto-Regular,Helvetica,Arial,sans-serif;color: rgba(0,0,0,0.54); font-size: 12px; line-height: 20px; padding-top: 10px;\">" +
				"								<div>我们向您发送这封电子邮件，目的是让您了激活您的报名 帐号。</div>" +
				"							</div>" +
				"						</div>" +
				"					</td>" +
				"					<td></td>" +
				"				</tr>" +
				"			</tbody>" +
				"		</table>";
		rmap.put("template", template);
		return rmap;
		
	}






	public static Map<String,Object> Shopping_text(String ord_no,String createTime,String shipping_Phone,String shipping_user,String province,String city,String district,String address,String by_count,String product_name,String info) {
		Map<String, Object> rmap = new HashMap<String, Object>();
		String template = "";
		String text ="<tbody><tr><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-corner-nw.png') top left no-repeat;\"width=\"6\"height=\"5\"><div></div></td><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-pixel-n.png') top center repeat-x;\"height=\"5\"><div></div></td><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-corner-ne.png') top right no-repeat;\"width=\"6\"height=\"5\"><div></div></td></tr><tr><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-pixel-w.png') center left repeat-y;\"width=\"6\"><div></div></td><td><div style=\"font-family: Roboto-Regular,Helvetica,Arial,sans-serif;padding-left: 20px; padding-right: 20px;border-bottom: thin solid #f0f0f0; color: rgba(0,0,0,0.87); font-size: 24px; padding-top: 40px; text-align: center; word-break: break-word;\"><div class=\"v2sp\"><h3>您有新的待发货订单</h3><p style=\"font-family: Roboto-Regular,Helvetica,Arial,sans-serif;color: rgba(0,0,0,0.87); font-size: 16px; line-height: 1.8;\">[<a href=\"http://"+Url+"/order/OrderShopping/{0}\"rel=\"noopener\"target=\"_blank\">{0}</a>]</p></div></div><div style=\"font-family: Roboto-Regular,Helvetica,Arial,sans-serif; font-size: 13px; color: rgba(0,0,0,0.87); line-height: 1.6;padding-left: 20px; padding-right: 20px;padding-bottom: 32px; padding-top: 24px;\"><p class=\"v2sp\">下单时间：[{1}]</p><p>商品名：[{2}]</p><p>下单数量：[{3}]</p><p>收件人姓名：[{4}]</p><p>收件人联系电话：[{5}]</p><p>收件人地址：[{6}]-[{7}]-[{8}]({9})</p><p>订单备注：[{10}]</p><p class=\"v2sp\">如果这不是您本人所为，则可能是有人误输了您的电子邮件地址。请勿将此订单信息泄露给他人，并且您目前无需执行任何其它操作。</p></div></td><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-pixel-e.png') center left repeat-y;\"width=\"6\"><div></div></td></tr><tr><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-corner-sw.png') top left no-repeat;\"width=\"6\"height=\"5\"><div></div></td><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-pixel-s.png') top center repeat-x\"height=\"5\"><div></div></td><td style=\"background:url('https://www.gstatic.com/accountalerts/email/hodor/4-corner-se.png') top left no-repeat;\"width=\"6\"height=\"5\"><div></div></td></tr></tbody></table>";
		template = MessageFormat.format(text,ord_no,createTime,product_name,by_count,shipping_user,shipping_Phone,province,city,district,address,info);
		rmap.put("template", template);
		return rmap;

	}




	
}
