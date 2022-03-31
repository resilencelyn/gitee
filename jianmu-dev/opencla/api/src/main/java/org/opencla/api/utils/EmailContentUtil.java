package org.opencla.api.utils;

import org.opencla.api.exception.EmailContentFileNotFoundException;
import org.springframework.core.io.ClassPathResource;

import java.io.IOException;
import java.nio.file.Files;
import java.util.stream.Stream;

/**
 * 读取Email内容的工具
 */
public class EmailContentUtil {
    public static final String EMAIL_CONTENT;

    static {
        StringBuilder emailContent = new StringBuilder();

        var path = new ClassPathResource("cla/email.html");

        // 读取文件内容到Stream流中，按行读取
        Stream<String> lines;
        try {
            lines = Files.lines(path.getFile().toPath());
            lines.forEachOrdered(emailContent::append);
        } catch (IOException e) {
            throw new EmailContentFileNotFoundException();
        }
        EMAIL_CONTENT = emailContent.toString();
    }

    public static String setEmailCode(String emailCode, String email, String team, String supportEmail) {
        return EMAIL_CONTENT.replace("${email_code}", emailCode)
                .replace("${email}", email)
                .replace("${team}", team)
                .replace("${support_email}", supportEmail)
                .replace("${identify_code_expiration}", "10");
    }
}
