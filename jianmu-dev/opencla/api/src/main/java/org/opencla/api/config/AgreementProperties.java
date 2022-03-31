package org.opencla.api.config;

import lombok.Getter;
import lombok.Setter;
import org.springframework.boot.context.properties.ConfigurationProperties;
import org.springframework.stereotype.Component;

import java.io.File;

/**
 * @author Daihw
 * @class AgreementConfig
 * @description AgreementConfig
 * @create 2022/1/11 10:03 上午
 */
@Component
@ConfigurationProperties(prefix = "opencla.agreement")
@Getter
@Setter
public class AgreementProperties {
    /**
     * 版本
     */
    private String version;
    /**
     * 项目名称
     */
    private String projectName;
    /**
     * 社区名称
     */
    private String communityName;
    /**
     * 社区归属
     */
    private String communityOwnership;
    /**
     * 代码托管平台
     */
    private String codeHostingPlatform;
    /**
     * 代码托管平台URL
     */
    private String codeHostingPlatformUrl;

    /**
     * 获取协议文件名
     *
     * @return
     */
    public String getFilename() {
        return "cla" + File.separator + this.version + ".md";
    }
}
