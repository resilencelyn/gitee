/**
 *    Copyright (c) [2022] [中和农信]
 *    [Software Name] is licensed under Mulan PSL v2.
 *    You can use this software according to the terms and conditions of the Mulan PSL v2.
 *    You may obtain a copy of Mulan PSL v2 at:
 *             http://license.coscl.org.cn/MulanPSL2
 *    THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *    See the Mulan PSL v2 for more details.
 *
 */
package cn.chongho.inf.flink.controller;

import cn.chongho.inf.flink.model.Jar;
import cn.chongho.inf.flink.model.WebResult;
import cn.chongho.inf.flink.request.JarListRequest;
import cn.chongho.inf.flink.restapi.JarApi;
import cn.chongho.inf.flink.service.JarService;
import cn.chongho.inf.flink.utils.HttpSessionUtils;
import cn.chongho.inf.flink.utils.StringUtils;
import cn.chongho.inf.flink.constants.Constant;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.multipart.MultipartFile;

import javax.annotation.Resource;
import javax.servlet.http.HttpSession;
import java.io.File;
import java.io.IOException;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;
import java.util.UUID;

/**
 * @author ming
 */
@Controller
@RequestMapping("/jars")
@Slf4j
public class JarsController {

    @Resource
    private JarService jarService;

    @Value("${sys.upload.jar-path}")
    private String uploadJarPath;

    @Autowired
    private JarApi jarApi;

    @GetMapping("/list")
    public String list(JarListRequest jarListRequest, ModelMap map) throws ParseException {
        Jar jar = new Jar();
        if(!StringUtils.isEmpty(jarListRequest.getFileName())){
            jar.setFileName(jarListRequest.getFileName());
        }
        int page = jarListRequest.getPage();
        int pageSize = jarListRequest.getPageSize();
        Date startTime = null;
        Date endTime = null;
        SimpleDateFormat timeFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        if(!StringUtils.isEmpty(jarListRequest.getStartTime())){
            startTime = timeFormat.parse(jarListRequest.getStartTime() + Constant.ADY_START_TIME);
        }
        if(!StringUtils.isEmpty(jarListRequest.getEndTime())){
            endTime = timeFormat.parse(jarListRequest.getEndTime() + Constant.ADY_END_TIME);
        }
        List<Jar> jars = jarService.getJarsByPage(page,  pageSize , jar ,startTime, endTime);
        int count = jarService.selectCount(jar, startTime, endTime);
        map.put("jars",jars);
        map.put("count",count);
        map.put("maxPage",count/10);
        map.put("page",page);
        map.put("pageSize",pageSize);
        map.put("nowBegin",pageSize * (page - 1 )+1);
        map.put("nowEnd",pageSize * (page - 1 )+jars.size());

        map.put("jarListRequest",jarListRequest);

        return "jar/jars-list";
    }


    @RequestMapping("delete")
    @ResponseBody
    public WebResult delete(int id , HttpSession httpSession){
        Jar dbJar = jarService.getJarOne(new Jar(id));
        if(jarApi.deleteJar(dbJar.getJobKey())){
            Jar jar = new Jar(id ,Constant.EnableFlag.NOT.ordinal() ,new Date() , HttpSessionUtils.getLoginUser(httpSession).getId());
            jarService.updateJar(jar);
            return WebResult.success();
        }
        return WebResult.error("删除失败");
    }


    @PostMapping("/upload")
    @ResponseBody
    public WebResult upload(@RequestParam("jarFile") MultipartFile file ,HttpSession httpSession) {
        if (file.isEmpty()) {
            return WebResult.error("文件不能为空");
        }
        String fileName = file.getOriginalFilename();
        String addUUIDfilename = UUID.randomUUID() + "_" + fileName;
        File saveFile = new File(uploadJarPath + addUUIDfilename);
        try {
            file.transferTo(saveFile);
            Integer loginUserId = HttpSessionUtils.getLoginUser(httpSession).getId();
            Jar jar = new Jar();
            jar.setJobKey(addUUIDfilename);
            jar.setFileName(fileName);
            jar.setCreateTime(new Date());
            jar.setEnableFlag(Constant.EnableFlag.ENABLE.ordinal());
            jar.setCreateUserId(loginUserId);
            jarService.addJar(jar);
        } catch (IOException e) {
            log.error("jar包上传出错",e);
            return WebResult.error(e.getMessage());
        }
        return WebResult.success();
    }
}
