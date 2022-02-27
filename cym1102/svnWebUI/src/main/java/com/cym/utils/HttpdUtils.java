package com.cym.utils;

import java.io.File;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.List;

import org.noear.solon.annotation.Component;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import cn.hutool.core.io.FileUtil;
import cn.hutool.core.io.resource.ClassPathResource;
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
			if (line.contains("SVNUseUTF8")) {
				hasSvnCof = true;
			}
		}
		if (!hasSvnCof) {
			reLines.add("<IfModule mod_dav_svn.c>");
			reLines.add("SVNUseUTF8  On");
			reLines.add("</IfModule>");
		}
		FileUtil.writeLines(reLines, new File("/etc/apache2/ports.conf"), Charset.forName("UTF-8"));

	}

	public void releaseFile() {
		ClassPathResource resource = new ClassPathResource("file/dav_svn.conf");
		FileUtil.writeFromStream(resource.getStream(), "/etc/apache2/mods-available/dav_svn.conf");
	}

	public void start() {
		String[] env = new String[] { //
				"APACHE_RUN_USER=www-data", //
				"APACHE_RUN_GROUP=www-data", //
				"APACHE_PID_FILE=/var/run/apache2/apache2.pid", //
				"APACHE_RUN_DIR=/var/run/apache2", //
				"APACHE_LOCK_DIR=/var/lock/apache2", //
				"APACHE_LOG_DIR=/var/log/apache2", //
				"LANG=C" };

		RuntimeUtil.exec(env, "apache2 -k start");
	}

	public void stop() {
		RuntimeUtil.exec("pkill apache2");
	}

}
