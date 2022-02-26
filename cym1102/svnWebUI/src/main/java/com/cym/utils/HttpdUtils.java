package com.cym.utils;

import java.io.File;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.List;

import org.noear.solon.annotation.Component;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.cym.config.InitConfig;

import cn.hutool.core.io.FileUtil;
import cn.hutool.core.util.RuntimeUtil;

@Component
public class HttpdUtils {
	Logger logger = LoggerFactory.getLogger(HttpdUtils.class);
	
	public void modHttpdPort(String port) {
		List<String> lines = FileUtil.readLines(new File("/etc/apache2/ports.conf"), Charset.forName("UTF-8"));
		List<String> reLines = new ArrayList<String>();
		boolean hasSvnCof = false;
		for (String line : lines) {
			if (line.startsWith("Listen ")) {
				line = "Listen " + port;
			}
			reLines.add(line);
			if(line.contains("SVNUseUTF8")) {
				hasSvnCof = true;
			}
		}
		if(!hasSvnCof) {
			reLines.add("<IfModule mod_dav_svn.c>");
			reLines.add("SVNUseUTF8  On");
			reLines.add("</IfModule>");
		}
		FileUtil.writeLines(reLines, new File("/etc/apache2/ports.conf"), Charset.forName("UTF-8"));
	}

	public void start() {
		String rs = RuntimeUtil.execForStr("apache2 -k start");
		logger.info(rs);
	}

	public void stop() {
		RuntimeUtil.execForStr("pkill apache2");
	}

}
