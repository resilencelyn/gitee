package org.opencla.verification.service.email.impl;

import org.opencla.verification.enumeration.ErrorEnum;
import org.opencla.verification.model.Impl.EmailModel;
import org.opencla.verification.service.BaseVerificationService;
import org.opencla.verification.service.Result;
import org.opencla.verification.service.email.IEmailService;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import javax.mail.MessagingException;
import javax.mail.Session;
import javax.mail.Transport;
import javax.mail.internet.InternetAddress;
import javax.mail.internet.MimeMessage;
import java.io.UnsupportedEncodingException;
import java.util.Date;
import java.util.Objects;
import java.util.Properties;

/**
 * 邮箱服务
 */
public class EmailService extends BaseVerificationService<EmailConfig> implements IEmailService {

    private static final Logger LOGGER = LoggerFactory.getLogger(EmailService.class);

    public EmailService(EmailConfig config) {
        super(config);
    }

    /**
     * 创建一封只包含文本的简单邮件
     *
     * @param session     和服务器交互的会话
     * @param sendMail    发件人邮箱
     * @param receiveMail 收件人邮箱
     * @return
     * @throws Exception
     */
    public static MimeMessage createMimeMessage(Session session, String sendMail, String receiveMail, String subject, String content,String signature) {
        try {
            // 1. 创建一封邮件
            MimeMessage message = new MimeMessage(session);

            // 2. From: 发件人
            message.setFrom(new InternetAddress(sendMail, signature, "UTF-8"));

            // 3. To: 收件人（可以增加多个收件人、抄送、密送）
            message.setRecipient(MimeMessage.RecipientType.TO, new InternetAddress(receiveMail));

            // 4. Subject: 邮件主题
            message.setSubject(subject, "UTF-8");

            // 5. Content: 邮件正文（可以使用html标签）
            message.setContent(content, "text/html;charset=UTF-8");
            // 6. 设置发件时间
            message.setSentDate(new Date());

            // 7. 保存设置
            message.saveChanges();

            return message;
        } catch (MessagingException | UnsupportedEncodingException e) {
            LOGGER.error(e.getMessage(), e);
            return null;
        }

    }

    private static boolean isEmpty(Object str) {
        return str == null || "".equals(str);
    }

    @Override
    public Result<Void> send(String emailAddress, String code, String subject, String content, String imageCode, String flag,String signature) {

        try {
            EmailModel model = (EmailModel) REPOSITORY.get(emailAddress, flag);

            long now = System.currentTimeMillis();

            if (model == null) {
                //有效期内未创建

                model = new EmailModel(emailAddress, code, this.smsCodeExpiration(), this.smsCodeResendingTime(), 1, flag);

                //发送验证码
                this.send(emailAddress, subject, content, signature);

                //新增
                REPOSITORY.save(model);

                return Result.success();
            }

            //有效期内已创建

            if (now <= model.getIdentifyCodeResendingTime()) {
                //未到重发时间

                return Result.error(ErrorEnum.CODE_RESENDING_FAIL);
            }

            if (model.getSendingCount() >= CONFIG.getIdentifyMaxSendingCount()) {
                //限制次数

                return Result.error(ErrorEnum.CODE_EXCEED_SENDING_MAXIMUM);
            }

            if (model.getSendingCount() >= CONFIG.getImageCodeDisplayCount()) {
                //表示已发送邮件验证码次数大于等于imageCodeDisplayCount时，需输入图片验证码来获取邮件验证码

                //检查图片验证码是否存在
                if (isEmpty(imageCode)) {
                    return Result.error(ErrorEnum.IMAGE_CODE_NOT_EMPTY);
                }

                //验证图片验证码
                if (!Objects.equals(imageCode, model.getImageCode())) {
                    return Result.error(ErrorEnum.IMAGE_CODE_INCORRECT);
                }

                //验证图片验证码过期时间
                if (now > model.getImageCodeExpiration()) {
                    return Result.error(ErrorEnum.IMAGE_CODE_EXPIRED);
                }
            }

            model.refreshSmsCodeData(code, this.smsCodeExpiration(), this.smsCodeResendingTime(), 0);

            //发送验证码
            this.send(emailAddress, subject, content, signature);

            //增加发送次数
            model.increaseSendingNumber(model.getSendingCount() + 1);

            REPOSITORY.update(model);

            return Result.success();

        } catch (Exception e) {

            LOGGER.error(e.getMessage(), e);

            return Result.error(ErrorEnum.UNKNOWN);
        }
    }

    /**
     * 发送邮件
     *
     * @param emailAddress 邮箱地址
     * @param subject
     * @param content
     * @throws Exception
     */
    @Override
    public void send(String emailAddress, String subject, String content,String signature) throws MessagingException {
        try {
            // 发件人的 邮箱 和 密码（替换为自己的邮箱和密码）
            // PS: 某些邮箱服务器为了增加邮箱本身密码的安全性，给 SMTP 客户端设置了独立密码（有的邮箱称为“授权码”）,
            //     对于开启了独立密码的邮箱, 这里的邮箱密码必需使用这个独立密码（授权码）。
            String emailAccount = CONFIG.getSender();
            String authorizationCode = CONFIG.getAuthorizationCode();

            // 发件人邮箱的 SMTP 服务器地址, 必须准确, 不同邮件服务器地址不同, 一般(只是一般, 绝非绝对)格式为: smtp.xxx.com
            // 网易126邮箱的 SMTP 服务器地址为: smtp.126.com
            String emailSMTPHost = CONFIG.getHost();

            // 1. 创建参数配置, 用于连接邮件服务器的参数配置
            Properties props = new Properties();
            // 使用的协议（JavaMail规范要求）
            props.setProperty("mail.transport.protocol", "smtp");
            // 发件人的邮箱的 SMTP 服务器地址
            props.setProperty("mail.smtp.host", emailSMTPHost);
            // 需要请求认证
            props.setProperty("mail.smtp.auth", "true");
            //开启ssl
            props.setProperty("mail.smtp.ssl.enable", "true");

            // PS: 某些邮箱服务器要求 SMTP 连接需要使用 SSL 安全认证 (为了提高安全性, 邮箱支持SSL连接, 也可以自己开启),
            //     如果无法连接邮件服务器, 仔细查看控制台打印的 log, 如果有有类似 “连接失败, 要求 SSL 安全连接” 等错误,
            //     取消下面 /* ... */ 之间的注释代码, 开启 SSL 安全连接。
            // SMTP 服务器的端口 (非 SSL 连接的端口一般默认为 25, 可以不添加, 如果开启了 SSL 连接,
            //                  需要改为对应邮箱的 SMTP 服务器的端口, 具体可查看对应邮箱服务的帮助,
            //                  QQ邮箱的SMTP(SLL)端口为465或587, 其他邮箱自行去查看)
            String smtpPort = String.valueOf(CONFIG.getPort());
            props.setProperty("mail.smtp.port", smtpPort);
            props.setProperty("mail.smtp.socketFactory.port", smtpPort);

            //如果使用ssl连接邮件服务器
            if (CONFIG.getUseSsl()) {
                //使用JSSE的SSLSocketFactory来取代默认的socketFactory
                props.setProperty("mail.smtp.socketFactory.class", "javax.net.ssl.SSLSocketFactory");
                // 只处理SSL的连接,对于非SSL的连接不做处理
                props.setProperty("mail.smtp.socketFactory.fallback", "false");
            }

            // 2. 根据配置创建会话对象, 用于和邮件服务器交互
            Session session = Session.getInstance(props);
            // 设置为debug模式, 可以查看详细的发送 log
            session.setDebug(CONFIG.getDebug());

            // 3. 创建一封邮件
            MimeMessage message = createMimeMessage(session, emailAccount, emailAddress, subject, content, signature);

            // 4. 根据 Session 获取邮件传输对象
            Transport transport = session.getTransport();

            // 5. 使用 邮箱账号 和 密码 连接邮件服务器, 这里认证的邮箱必须与 message 中的发件人邮箱一致, 否则报错
            //
            //    PS_01: 如果连接服务器失败, 都会在控制台输出相应失败原因的log。
            //    仔细查看失败原因, 有些邮箱服务器会返回错误码或查看错误类型的链接,
            //    根据给出的错误类型到对应邮件服务器的帮助网站上查看具体失败原因。
            //
            //    PS_02: 连接失败的原因通常为以下几点, 仔细检查代码:
            //           (1) 邮箱没有开启 SMTP 服务;
            //           (2) 邮箱密码错误, 例如某些邮箱开启了独立密码;
            //           (3) 邮箱服务器要求必须要使用 SSL 安全连接;
            //           (4) 请求过于频繁或其他原因, 被邮件服务器拒绝服务;
            //           (5) 如果以上几点都确定无误, 到邮件服务器网站查找帮助。
            //
            transport.connect(emailAccount, authorizationCode);

            // 6. 发送邮件, 发到所有的收件地址, message.getAllRecipients() 获取到的是在创建邮件对象时添加的所有收件人, 抄送人, 密送人
            transport.sendMessage(message, message.getAllRecipients());

            // 7. 关闭连接
            transport.close();
        } catch (MessagingException e) {
            LOGGER.error(e.getMessage(), e);
        }

    }

    /**
     * 返回邮件验证码重发时间
     *
     * @return
     */
    private long smsCodeResendingTime() {
        return System.currentTimeMillis() + CONFIG.getIdentifyCodeResendingInterval();
    }

    /**
     * 返回邮件验证码过期时间
     *
     * @return
     */
    private long smsCodeExpiration() {
        return System.currentTimeMillis() + CONFIG.getIdentifyCodeExpiration();
    }
}
