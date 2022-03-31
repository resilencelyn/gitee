package org.opencla.api.utils;

import org.opencla.api.config.AgreementProperties;
import org.opencla.api.exception.AgreementFileNotFoundException;
import org.springframework.core.io.ClassPathResource;

import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;

/**
 * @author Daihw
 * @class AgreementUtil
 * @description 协议工具类
 * @create 2022/1/10 6:12 下午
 */
public class AgreementUtil {

    /**
     * 协议内容
     */
    public final static String AGREEMENT_CONTENT;

    /**
     * 读取协议文件
     */
    static {
        var agreementProperties = ApplicationContextUtils.getBean(AgreementProperties.class);
        var path = new ClassPathResource(agreementProperties.getFilename());
        StringBuilder agreementContent = new StringBuilder();
        try (var bufferedReader = Files.newBufferedReader(path.getFile().toPath(), StandardCharsets.UTF_8)) {
            String line;
            while ((line = bufferedReader.readLine()) != null) {
                agreementContent.append(line).append(System.lineSeparator());
            }
        } catch (IOException e) {
            throw new AgreementFileNotFoundException();
        }
        AGREEMENT_CONTENT = agreementContent.toString()
                .replace("${COMMUNITY_NAME}", agreementProperties.getCommunityName())
                .replace("${COMMUNITY_OWNERSHIP}", agreementProperties.getCommunityOwnership())
                .replace("${CODE_HOSTING_PLATFORM}", agreementProperties.getCodeHostingPlatform())
                .replace("${CODE_HOSTING_PLATFORM_URL}", agreementProperties.getCodeHostingPlatformUrl());
    }
}
