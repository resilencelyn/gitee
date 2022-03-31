package org.opencla.api.api.impl;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.core.type.TypeReference;
import com.fasterxml.jackson.databind.DeserializationFeature;
import com.fasterxml.jackson.databind.ObjectMapper;
import org.opencla.api.api.impl.config.GiteeConfigProperties;
import org.opencla.api.api.impl.vo.EmailVo;
import org.opencla.api.api.impl.vo.UserInfoVo;
import org.opencla.api.api.impl.vo.gitee.GiteeEmailVo;
import org.opencla.api.api.impl.vo.gitee.GiteeLoginVo;
import org.opencla.api.api.impl.vo.gitee.GiteeTokenVo;
import org.opencla.api.api.impl.vo.gitee.GiteeUserInfoVo;
import org.opencla.api.enumeration.GiteeGrantTypeEnum;
import org.opencla.api.exception.AccessTokenDoesNotExistException;
import org.opencla.api.exception.GetTokenRequestParameterErrorException;
import org.opencla.api.exception.JsonParseException;
import org.opencla.api.exception.MailboxAcquisitionFailedException;
import org.opencla.api.api.AuthApi;
import org.springframework.http.*;
import org.springframework.stereotype.Component;
import org.springframework.web.client.HttpClientErrorException;
import org.springframework.web.client.HttpServerErrorException;
import org.springframework.web.client.RestTemplate;

import java.net.URLEncoder;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;

/**
 * giteeApi
 */
@Component
public class GiteeApi implements AuthApi {
    private final RestTemplate restTemplate;
    private final GiteeConfigProperties configProperties;

    public GiteeApi(RestTemplate restTemplate, GiteeConfigProperties configProperties) {
        this.restTemplate = restTemplate;
        this.configProperties = configProperties;
    }

    @Override
    public String getAuthUrl(String redirectUri) {
        redirectUri = URLEncoder.encode(redirectUri, StandardCharsets.UTF_8);
        String clientId = URLEncoder.encode(this.configProperties.getClientId(), StandardCharsets.UTF_8);
        String responseType = URLEncoder.encode(this.configProperties.getResponseType(), StandardCharsets.UTF_8);
        return this.configProperties.getCodeUrl() + "?" +
                "client_id=" + clientId +
                "&redirect_uri=" + redirectUri +
                "&response_type=" + responseType;
    }


    @Override
    public String getAccessToken(String code, String redirectUri) {
        // 封装请求条件
        GiteeLoginVo giteeLoginVo = GiteeLoginVo.builder()
                .client_id(this.configProperties.getClientId())
                .client_secret(this.configProperties.getClientSecret())
                .code(code)
                .grant_type(GiteeGrantTypeEnum.AUTHORIZATION_CODE.getTypeName())
                .redirect_uri(redirectUri)
                .build();

        ObjectMapper mapper = new ObjectMapper();
        String giteeLoginJson;
        try {
            giteeLoginJson = mapper.writeValueAsString(giteeLoginVo);
        } catch (JsonProcessingException e) {
            throw new JsonParseException();
        }
        HttpHeaders headers = new HttpHeaders();
        headers.setContentType(MediaType.APPLICATION_JSON);
        HttpEntity<String> entity = new HttpEntity<>(giteeLoginJson, headers);

        // 发送请求, 获取token
        ResponseEntity<String> tokenEntity;
        try {
            tokenEntity = this.restTemplate.exchange(this.configProperties.getTokenUrl(), HttpMethod.POST, entity, String.class);
        } catch (HttpClientErrorException e) {
            throw new GetTokenRequestParameterErrorException();
        } catch (HttpServerErrorException e) {
            throw new GetTokenRequestParameterErrorException("连不上GITEE（" + e.getStatusCode() + "），请稍后再试");
        }

        GiteeTokenVo giteeTokenVo;
        try {
            giteeTokenVo = mapper.readValue(tokenEntity.getBody(), GiteeTokenVo.class);
        } catch (JsonProcessingException e) {
            throw new JsonParseException();
        }
        return giteeTokenVo.getAccess_token();
    }

    @Override
    public UserInfoVo getUserInfo(String token) {
        String userTokenInfoUrl = this.configProperties.getUserInfoUrl() + "?access_token=" + token;

        ResponseEntity<String> userInfoEntity = this.getGiteeResponseResult(userTokenInfoUrl);

        String userInfo = userInfoEntity.getBody();
        GiteeUserInfoVo giteeUserInfoVo;
        ObjectMapper mapper = new ObjectMapper();
        try {
            giteeUserInfoVo = mapper.readValue(userInfo, GiteeUserInfoVo.class);
        } catch (JsonProcessingException e) {
            throw new JsonParseException();
        }

        return UserInfoVo.builder()
                .headUrl(giteeUserInfoVo.getAvatar_url())
                .username(giteeUserInfoVo.getLogin())
                .build();
    }

    @Override
    public List<EmailVo> getEmail(String token) {
        String userEmailInfoUrl = this.configProperties.getEmails() + "?access_token=" + token;
        ResponseEntity<String> emailInfoEntity = this.getGiteeResponseResult(userEmailInfoUrl);

        String emailInfo = emailInfoEntity.getBody();
        List<GiteeEmailVo> giteeEmailVos;
        GiteeEmailVo giteeEmailVo;
        List<EmailVo> emailsVos = new ArrayList<>();
        ObjectMapper mapper = new ObjectMapper();
        mapper.configure(DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES, false);
        TypeReference<GiteeEmailVo> type = new TypeReference<>() {
        };
        try {
            giteeEmailVos = mapper.readValue(emailInfo, new TypeReference<>() {
            });
            for (GiteeEmailVo emailVo : giteeEmailVos) {
                emailsVos.add(EmailVo.builder().Email(emailVo.getEmail())
                        .committed(emailVo.getScope().contains("committed") ? "committed" : null)
                        .primary(emailVo.getScope().contains("primary") ? "primary" : null).build());
            }

        } catch (JsonProcessingException e1) {
            try {
                giteeEmailVo = mapper.readValue(emailInfo, type);
                emailsVos.add(EmailVo.builder().Email(giteeEmailVo.getEmail()).build());
                return emailsVos;
            } catch (JsonProcessingException e2) {
                throw new MailboxAcquisitionFailedException();
            }
        }
        if (giteeEmailVos.isEmpty()) {
            throw new MailboxAcquisitionFailedException("请先绑定邮箱地址，或勾选《Gitee设置 -> 邮箱管理 -> 不公开我的邮箱地址》后，重试");
        }
        return emailsVos;
    }

    /**
     * get方法获取gitee请求结果
     *
     * @param url
     * @return
     */
    private ResponseEntity<String> getGiteeResponseResult(String url) {
        ResponseEntity<String> entity;
        try {
            entity = this.restTemplate.exchange(
                    url,
                    HttpMethod.GET,
                    null,
                    String.class);
        } catch (HttpClientErrorException e) {
            throw new AccessTokenDoesNotExistException();
        } catch (HttpServerErrorException e) {
            throw new GetTokenRequestParameterErrorException("连不上GITEE（" + e.getStatusCode() + "），请稍后再试");
        }
        return entity;
    }
}
