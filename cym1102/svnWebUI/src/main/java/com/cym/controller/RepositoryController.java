package com.cym.controller;

import java.io.File;
import java.io.IOException;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.List;

import org.noear.solon.annotation.Controller;
import org.noear.solon.annotation.Inject;
import org.noear.solon.annotation.Mapping;
import org.noear.solon.core.handle.Context;
import org.noear.solon.core.handle.ModelAndView;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.tmatesoft.svn.core.SVNException;
import org.tmatesoft.svn.core.SVNURL;
import org.tmatesoft.svn.core.auth.ISVNAuthenticationManager;
import org.tmatesoft.svn.core.io.SVNRepository;
import org.tmatesoft.svn.core.io.SVNRepositoryFactory;
import org.tmatesoft.svn.core.wc.SVNClientManager;
import org.tmatesoft.svn.core.wc.SVNRevision;
import org.tmatesoft.svn.core.wc.SVNWCUtil;
import org.tmatesoft.svn.core.wc.admin.SVNAdminClient;
import org.tmatesoft.svn.core.wc2.admin.SvnRepositoryDump;

import com.cym.config.HomeConfig;
import com.cym.config.InitConfig;
import com.cym.ext.RepositoryExt;
import com.cym.ext.RepositoryGroupExt;
import com.cym.ext.RepositoryUserExt;
import com.cym.ext.Select;
import com.cym.model.Group;
import com.cym.model.Repository;
import com.cym.model.RepositoryGroup;
import com.cym.model.RepositoryUser;
import com.cym.model.TreeNode;
import com.cym.model.User;
import com.cym.service.ConfigService;
import com.cym.service.RepositoryService;
import com.cym.service.SettingService;
import com.cym.sqlhelper.bean.Page;
import com.cym.utils.BaseController;
import com.cym.utils.BeanExtUtil;
import com.cym.utils.HttpdUtils;
import com.cym.utils.JsonResult;
import com.cym.utils.PathUtls;
import com.cym.utils.SvnAdminUtils;
import com.cym.utils.SystemTool;

import cn.hutool.core.io.FileUtil;
import cn.hutool.core.net.URLDecoder;
import cn.hutool.core.net.URLEncoder;
import cn.hutool.core.util.CharsetUtil;
import cn.hutool.core.util.RuntimeUtil;
import cn.hutool.core.util.StrUtil;

@Controller
@Mapping("/adminPage/repository")
public class RepositoryController extends BaseController {
	Logger logger = LoggerFactory.getLogger(RepositoryController.class);

	@Inject
	InitConfig projectConfig;
	@Inject
	RepositoryService repositoryService;
	@Inject
	ConfigService configService;
	@Inject
	SettingService settingService;
	@Inject
	PathUtls pathUtls;
	@Inject
	HomeConfig homeConfig;
	@Inject
	SvnAdminUtils svnAdminUtils;

	@Mapping("")
	public ModelAndView index(Page page, String keywords) {
		String port = settingService.get("port");

		page = repositoryService.search(page, keywords);

		Page<RepositoryExt> pageExt = BeanExtUtil.copyPageByProperties(page, RepositoryExt.class);
		for (RepositoryExt repositoryExt : (List<RepositoryExt>) pageExt.getRecords()) {
			String url = pathUtls.buildUrl(port);

			url += "/" + repositoryExt.getName();
			repositoryExt.setUrl(url);
		}

		ModelAndView modelAndView = new ModelAndView("/adminPage/repository/index.html");
		modelAndView.put("keywords", keywords);
		modelAndView.put("page", pageExt);
		modelAndView.put("home", homeConfig.home);
		return modelAndView;
	}

	@Mapping("addOver")
	public JsonResult addOver(String name) {
		if (StrUtil.isEmpty(name)) {
			return renderError("仓库名为空");
		}

		Repository repositoryOrg = repositoryService.getByName(name, null);
		if (repositoryOrg != null) {
			return renderError("此仓库名已存在");
		}

		if (repositoryService.hasDir(name)) {
			return renderError("该仓库文件夹已存在, 请使用扫描功能添加");
		}

		repositoryService.insertOrUpdate(name);

		configService.refresh();
		return renderSuccess();
	}

	@Mapping("detail")
	public JsonResult detail(String id) {
		Repository repository = sqlHelper.findById(id, Repository.class);
		RepositoryExt repositoryExt = BeanExtUtil.copyNewByProperties(repository, RepositoryExt.class);
		String url = pathUtls.buildUrl(settingService.get("port"));

		url += "/" + repositoryExt.getName();
		repositoryExt.setUrl(url);

		return renderSuccess(repositoryExt);
	}

	@Mapping("del")
	public JsonResult del(String id, String pass) {
		User user = getLoginUser();
		if (!user.getPass().equals(pass)) {
			return renderError("密码错误，无法删除库!");
		}

		repositoryService.deleteById(id);
		configService.refresh();
		return renderSuccess();
	}

	@Mapping("allPermissionOver")
	public JsonResult allPermissionOver(String id, String allPermission) {

		repositoryService.allPermissionOver(id, allPermission);
		configService.refresh();
		return renderSuccess();
	}

	@Mapping("userPermission")
	public ModelAndView userPermission(Page page, String repositoryId) {
		String port = settingService.get("port");

		page = repositoryService.userPermission(page, repositoryId);
		Repository repository = sqlHelper.findById(repositoryId, Repository.class);

		Page<RepositoryUserExt> pageExt = BeanExtUtil.copyPageByProperties(page, RepositoryUserExt.class);
		for (RepositoryUserExt repositoryUserExt : (List<RepositoryUserExt>) pageExt.getRecords()) {
			repositoryUserExt.setUser(sqlHelper.findById(repositoryUserExt.getUserId(), User.class));

			String url = pathUtls.buildUrl(port);
			url += ("/" + repository.getName() + repositoryUserExt.getPath());
			if (url.endsWith("/")) {
				url = url.substring(0, url.length() - 1);
			}
			repositoryUserExt.setPath(url);
		}

		ModelAndView modelAndView = new ModelAndView("/adminPage/repository/userPermission.html");
		modelAndView.put("userList", sqlHelper.findAll(User.class));

		modelAndView.put("repositoryId", repositoryId);
		modelAndView.put("page", pageExt);
		return modelAndView;
	}

	@Mapping("addUser")
	public JsonResult addUser(RepositoryUser repositoryUser) {
		if (repositoryService.hasUser(repositoryUser.getUserId(), repositoryUser.getPath(), repositoryUser.getRepositoryId(), repositoryUser.getId())) {
			return renderError("该用户路径授权已存在");
		}
		repositoryService.addUser(repositoryUser);
		configService.refresh();
		return renderSuccess();
	}

	@Mapping("delUser")
	public JsonResult delUser(String id) {
		repositoryService.delUser(id);
		configService.refresh();
		return renderSuccess();
	}

	@Mapping("userDetail")
	public JsonResult userDetail(String id) {

		return renderSuccess(sqlHelper.findById(id, RepositoryUser.class));
	}

	@Mapping("groupPermission")
	public ModelAndView groupPermission(Page page, String repositoryId) {
		String port = settingService.get("port");

		page = repositoryService.groupPermission(page, repositoryId);
		Repository repository = sqlHelper.findById(repositoryId, Repository.class);

		Page<RepositoryGroupExt> pageExt = BeanExtUtil.copyPageByProperties(page, RepositoryGroupExt.class);
		for (RepositoryGroupExt repositoryGroupExt : (List<RepositoryGroupExt>) pageExt.getRecords()) {
			repositoryGroupExt.setGroup(sqlHelper.findById(repositoryGroupExt.getGroupId(), Group.class));

			String url = pathUtls.buildUrl(port);
			url += ("/" + repository.getName() + repositoryGroupExt.getPath());

			if (url.endsWith("/")) {
				url = url.substring(0, url.length() - 1);
			}
			repositoryGroupExt.setPath(url);
		}
		ModelAndView modelAndView = new ModelAndView("/adminPage/repository/groupPermission.html");
		modelAndView.put("groupList", sqlHelper.findAll(Group.class));

		modelAndView.put("repositoryId", repositoryId);
		modelAndView.put("page", pageExt);
		return modelAndView;
	}

	@Mapping("addGroup")
	public JsonResult addGroup(RepositoryGroup repositoryGroup) {
		if (repositoryService.hasGroup(repositoryGroup.getGroupId(), repositoryGroup.getPath(), repositoryGroup.getRepositoryId(), repositoryGroup.getId())) {
			return renderError("该小组路径授权已存在");
		}

		repositoryService.addGroup(repositoryGroup);
		configService.refresh();
		return renderSuccess();
	}

	@Mapping("delGroup")
	public JsonResult delGroup(String id) {
		repositoryService.delGroup(id);
		configService.refresh();
		return renderSuccess();
	}

	@Mapping("groupDetail")
	public JsonResult groupDetail(String id) {

		return renderSuccess(sqlHelper.findById(id, RepositoryGroup.class));
	}

	@Mapping("loadOver")
	public JsonResult loadOver(String id, String dirTemp) {

		Repository repository = sqlHelper.findById(id, Repository.class);

		String rs = "";
		String home = homeConfig.home;

		if (SystemTool.isWindows()) {

			String cmd = "svnadmin.exe load " + (home + File.separator + "repo" + File.separator + repository.getName() + File.separator).replace("/", "\\") + " < " + dirTemp;
			FileUtil.writeString(cmd, home + File.separator + "dump.bat", CharsetUtil.GBK);

			rs = RuntimeUtil.execForStr(home + File.separator + "dump.bat");
		} else {
			String sh = "svnadmin load " + (home + File.separator + "repo" + File.separator + repository.getName() + File.separator) + " < " + dirTemp;
			FileUtil.writeString(sh, home + File.separator + "dump.sh", CharsetUtil.CHARSET_UTF_8);

			rs = RuntimeUtil.execForStr("sh", home + File.separator + "dump.sh");
		}
		logger.info(rs);
		FileUtil.del(dirTemp);

		return renderSuccess(rs.replace("\n", "<br>"));
	}

	@Mapping("dumpOver")
	public void dumpOver(String id, Context context) throws Exception {
		Repository repository = sqlHelper.findById(id, Repository.class);

		String rs = "";
		String home = homeConfig.home;
		String dumpTemp = null;

		if (SystemTool.isWindows()) {

			dumpTemp = home + File.separator + "temp" + File.separator + repository.getName() + ".dump";
			String cmd = "svnadmin.exe dump " + (home + File.separator + "repo" + File.separator + repository.getName() + File.separator).replace("/", "\\") + " > " + dumpTemp;

			FileUtil.writeString(cmd, home + File.separator + "dump.bat", CharsetUtil.GBK);

			rs = RuntimeUtil.execForStr(home + File.separator + "dump.bat");
		} else {

			dumpTemp = home + File.separator + "temp" + File.separator + repository.getName() + ".dump";
			String sh = "svnadmin dump " + (home + File.separator + "repo" + File.separator + repository.getName() + File.separator) + " > " + dumpTemp;

			FileUtil.writeString(sh, home + File.separator + "dump.sh", CharsetUtil.CHARSET_UTF_8);

			rs = RuntimeUtil.execForStr("sh", home + File.separator + "dump.sh");
		}

		logger.info(rs);

		if (FileUtil.exist(dumpTemp)) {
			Context.current().outputAsFile(new File(dumpTemp));
			FileUtil.del(dumpTemp);
		}

	}

	@Mapping("getFileList")
	public List<TreeNode> getFileList(String id, String url) {
		if (StrUtil.isEmpty(id)) {
			id = url;
		}
		id = URLDecoder.decode(id, Charset.forName("UTF-8"));
		List<TreeNode> list = pathUtls.getPath(id, svnAdminUtils.adminUserName, svnAdminUtils.adminUserPass);

		sortFile(list);

		return list;
	}

	@Mapping("download")
	public void download(String url, Context context) throws SVNException, IOException {
		url = URLDecoder.decode(url, Charset.forName("UTF-8"));

		SVNRepository svnRepository = SVNRepositoryFactory.create(SVNURL.parseURIEncoded(url));
		ISVNAuthenticationManager authManager = SVNWCUtil.createDefaultAuthenticationManager(svnAdminUtils.adminUserName, svnAdminUtils.adminUserPass.toCharArray());
		svnRepository.setAuthenticationManager(authManager);

		String fileName = getFileName(url);

		context.headerAdd("Accept-Ranges", "bytes");
		context.headerAdd("Content-Type", "application/octet-stream");
		context.headerAdd("Content-Disposition", "attachment;filename=" + URLEncoder.createDefault().encode(fileName, Charset.forName("UTF-8")));
		svnRepository.getFile(pathUtls.getRelativePath(url), -1, null, context.outputStream());

	}

	@Mapping("getUserList")
	public JsonResult getUserList() {

		List<User> users = sqlHelper.findAll(User.class);

		List<Select> selects = new ArrayList<>();
		for (User user : users) {
			Select select = new Select();
			select.setName(user.getTrueName());
			select.setValue(user.getId());
			selects.add(select);
		}

		return renderSuccess(selects);
	}

	@Mapping("getGroupList")
	public JsonResult getGroupList() {

		List<Group> groups = sqlHelper.findAll(Group.class);

		List<Select> selects = new ArrayList<>();
		for (Group group : groups) {
			Select select = new Select();
			select.setName(group.getName());
			select.setValue(group.getId());
			selects.add(select);
		}

		return renderSuccess(selects);
	}

	@Mapping("scan")
	public JsonResult scan() {
		repositoryService.scan();
		configService.refresh();
		return renderSuccess();
	}

}
