package org.opencla.api.utils;

import org.opencla.api.config.EmailProperties;
import org.opencla.api.exception.EnterpriseMailConfigurationFileNotFoundException;
import org.springframework.core.io.ClassPathResource;

import java.io.File;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;

/**
 * @author Daihw
 * @class EnterpriseMailUtil
 * @description EnterpriseMailUtil
 * @create 2022/3/29 10:50 上午
 */
public class EnterpriseMailUtil {

    /**
     * 企业邮件内容
     */
    public final static String ENTERPRISE_MAIL;

    /**
     * 读取企业邮件内容
     */
    static {
        var emailProperties = ApplicationContextUtils.getBean(EmailProperties.class);
        var path = new ClassPathResource("cla" + File.separator + "enterprise_mail_configuration.md");
        StringBuilder agreementContent = new StringBuilder();
        try (var bufferedReader = Files.newBufferedReader(path.getFile().toPath(), StandardCharsets.UTF_8)) {
            String line;
            while ((line = bufferedReader.readLine()) != null) {
                agreementContent.append(line).append(System.lineSeparator());
            }
        } catch (IOException e) {
            throw new EnterpriseMailConfigurationFileNotFoundException();
        }
        ENTERPRISE_MAIL = agreementContent.toString()
                .replace("${SUPPORT_EMAIL}", emailProperties.getSupportEmail());
    }
}
