package com.cym.utils;

import org.noear.solon.annotation.Component;
import org.noear.solon.annotation.Init;

import cn.hutool.core.lang.UUID;

@Component
public class SvnAdminUtils {
	public String adminUserName;
	public String adminUserPass;
	
	@Init
	public void init() {
		adminUserName = "svnWebUISuperAdministrator";
		adminUserPass = UUID.randomUUID().toString();
	}

	public String getAdminUserName() {
		return adminUserName;
	}

	public void setAdminUserName(String adminUserName) {
		this.adminUserName = adminUserName;
	}

	public String getAdminUserPass() {
		return adminUserPass;
	}

	public void setAdminUserPass(String adminUserPass) {
		this.adminUserPass = adminUserPass;
	}

}
