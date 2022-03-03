package cn.surveyking.server.api;

import cn.surveyking.server.core.constant.AppConsts;
import cn.surveyking.server.core.security.JwtTokenUtil;
import cn.surveyking.server.core.uitls.SecurityContextUtils;
import cn.surveyking.server.domain.dto.AuthRequest;
import cn.surveyking.server.domain.dto.UserInfo;
import cn.surveyking.server.domain.dto.UserRequest;
import cn.surveyking.server.domain.dto.UserTokenView;
import cn.surveyking.server.service.UserService;
import lombok.RequiredArgsConstructor;
import org.springframework.http.HttpCookie;
import org.springframework.http.HttpHeaders;
import org.springframework.http.ResponseCookie;
import org.springframework.http.ResponseEntity;
import org.springframework.security.access.AccessDeniedException;
import org.springframework.security.access.prepost.PreAuthorize;
import org.springframework.security.authentication.AuthenticationManager;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.Authentication;
import org.springframework.web.bind.annotation.*;

import javax.validation.Valid;

/**
 * @author javahuang
 * @date 2021/10/12
 */
@RestController
@RequiredArgsConstructor
@RequestMapping("${api.prefix}")
public class UserApi {

	private final UserService userService;

	private final AuthenticationManager authenticationManager;

	private final JwtTokenUtil jwtTokenUtil;

	@PostMapping("/public/login")
	public ResponseEntity login(@RequestBody @Valid AuthRequest request) {
		Authentication authentication;
		if (request.getAuthType() == AppConsts.AUTH_TYPE.PWD) {
			authentication = new UsernamePasswordAuthenticationToken(request.getUsername(), request.getPassword());
		}
		else {
			throw new AccessDeniedException("不支持的认证方式");
		}
		// 将 token 提交给 spring security 的 DaoAuthenticationProvider 进行认证
		Authentication authenticate = authenticationManager.authenticate(authentication);
		UserInfo user = (UserInfo) authenticate.getPrincipal();
		HttpCookie cookie = ResponseCookie
				.from(AppConsts.COOKIE_TOKEN_NAME,
						jwtTokenUtil.generateAccessToken(new UserTokenView(user.getUserId())))
				.path("/").httpOnly(true).build();
		return ResponseEntity.ok().header(HttpHeaders.SET_COOKIE, cookie.toString()).header(HttpHeaders.AUTHORIZATION,
				jwtTokenUtil.generateAccessToken(new UserTokenView(user.getUserId()))).build();
	}

	@PostMapping("/public/logout")
	public ResponseEntity logout() {
		HttpCookie cookie = ResponseCookie.from(AppConsts.COOKIE_TOKEN_NAME, "").path("/").httpOnly(true).maxAge(0)
				.build();
		return ResponseEntity.ok().header(HttpHeaders.SET_COOKIE, cookie.toString()).build();
	}

	@GetMapping("/currentUser")
	@PreAuthorize("isAuthenticated()")
	public UserInfo currentUser() {
		return userService.loadUserById(SecurityContextUtils.getUserId());
	}

	@PostMapping("/user")
	public UserInfo updateUser(@RequestBody UserRequest request) {
		// 只有本人才能通过调用这个接口修改个人信息
		request.setId(SecurityContextUtils.getUserId());
		userService.updateUser(request);
		return userService.loadUserById(SecurityContextUtils.getUserId());
	}

}
