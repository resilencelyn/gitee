package org.opencla.api.controller;

import io.swagger.v3.oas.annotations.Operation;
import io.swagger.v3.oas.annotations.tags.Tag;
import org.opencla.api.application.AuthService;
import org.opencla.api.dto.AuthorizationUrlGettingDto;
import org.opencla.api.dto.LoggingDto;
import org.opencla.api.dto.vo.AuthorizationUrlVo;
import org.opencla.api.dto.vo.TokenVo;
import org.springframework.validation.annotation.Validated;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import javax.validation.Valid;

/**
 * auth控制器
 */
@Validated
@RestController
@RequestMapping("auth")
@Tag(name = "auth控制器", description = "auth控制器")
// todo email cor svc
public class AuthController {
    private final AuthService authService;

    public AuthController(AuthService authService) {
        this.authService = authService;
    }

    /**
     * 获取令牌
     *
     * @param loggingDto
     */
    @GetMapping("login")
    @Operation(summary = "返回令牌", description = "返回令牌")
    public TokenVo create(@Valid LoggingDto loggingDto) {
        return this.authService.login(loggingDto);
    }

    /**
     * 获取url
     *
     * @param authorizationUrlGettingDto
     * @return
     */
    @GetMapping("url")
    @Operation(summary = "获取授权url", description = "获取授权url")
    public AuthorizationUrlVo getAuthorizationUrl(@Valid AuthorizationUrlGettingDto authorizationUrlGettingDto) {
        return this.authService.getAuthorizationUrl(authorizationUrlGettingDto);
    }
}
