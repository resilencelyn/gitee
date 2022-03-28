package org.noear.esearchx;


import org.noear.esearchx.exception.NoExistException;
import org.noear.esearchx.model.EsAliases;
import org.noear.esearchx.model.EsSetting;
import org.noear.snack.ONode;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Properties;
import java.util.function.Consumer;

/**
 * ElasticSearch 执行上下文（支持 7.x +）
 *
 * @author noear
 * @since 1.0
 */
public class EsContext {
    private final String[] urls;
    private int urlIndex;
    private final String username;
    private final String paasword;

    @Deprecated
    public EsCommand lastCommand;

    public EsContext(Properties prop) {
        this(prop.getProperty("url"), prop.getProperty("username"), prop.getProperty("paasword"));
    }

    public EsContext(String url) {
        this(url, null, null);
    }

    public EsContext(String url, String username, String paasword) {
        this.username = username;
        this.paasword = paasword;

        List<String> urlAry = new ArrayList<>();
        for (String ser : url.split(",")) {
            if (ser.contains("://")) {
                urlAry.add(ser);
            } else {
                urlAry.add("http://" + ser);
            }
        }
        this.urls = urlAry.toArray(new String[urlAry.size()]);
    }

    private String getUrl() {
        if (urls.length == 0) {
            return urls[0];
        } else {
            if (urlIndex > 10000000) {
                urlIndex = 0;
            }

            return urls[urlIndex % urls.length];
        }
    }

    protected PriHttpUtils getHttp(String path) {
        PriHttpUtils http = PriHttpUtils.http(getUrl() + path);

        if (PriUtils.isNotEmpty(username)) {
            String token = PriUtils.b64Encode(username + ":" + paasword);
            String auth = "Basic " + token;

            http.header("Authorization", auth);
        }

        return http;
    }

    /**
     * 执行并返回结果体
     *
     * @param cmd 命令
     */
    public String execAsBody(EsCommand cmd) throws IOException {
        lastCommand = cmd;

        if (PriUtils.isEmpty(cmd.dsl)) {
            return getHttp(cmd.path).execAsBody(cmd.method);
        } else {
            return getHttp(cmd.path).bodyTxt(cmd.dsl, cmd.dslType).execAsBody(cmd.method);
        }
    }

    /**
     * 执行并返回状态码
     *
     * @param cmd 命令
     */
    public int execAsCode(EsCommand cmd) throws IOException {
        lastCommand = cmd;

        if (PriUtils.isEmpty(cmd.dsl)) {
            return getHttp(cmd.path).execAsCode(cmd.method);
        } else {
            return getHttp(cmd.path).bodyTxt(cmd.dsl, cmd.dslType).execAsCode(cmd.method);
        }
    }

    /**
     * 获取索引操作
     */
    public EsQuery indice(String indiceName) {
        return new EsQuery(this, indiceName, false);
    }

    /**
     * 索引创建
     *
     * @param indiceName 索引名字
     */
    public String indiceCreate(String indiceName, String dsl) throws IOException {
        EsCommand cmd = new EsCommand();
        cmd.method = PriWw.method_put;
        cmd.path = String.format("/%s", indiceName);
        cmd.dsl = dsl;
        cmd.dslType = PriWw.mime_json;

        String tmp = execAsBody(cmd);

        return tmp;
    }

    /**
     * 索引是否存在
     *
     * @param indiceName 索引名字
     */
    public boolean indiceExist(String indiceName) throws IOException {
        EsCommand cmd = new EsCommand();
        cmd.method = PriWw.method_head;
        cmd.path = String.format("/%s", indiceName);

        int tmp = execAsCode(cmd);

        return tmp == 200; //404不存在
    }

    /**
     * 索引删除
     *
     * @param indiceName 索引名字
     */
    public boolean indiceDrop(String indiceName) throws IOException {
        EsCommand cmd = new EsCommand();
        cmd.method = PriWw.method_delete;
        cmd.path = String.format("/%s", indiceName);

        try {
            execAsBody(cmd);
            return true;
        } catch (NoExistException e) {
            return true;
        }
    }

    /**
     * 索引设置修改
     *
     * @param indiceName 索引名字
     */
    public String indiceSettings(String indiceName, Consumer<EsSetting> setting) throws IOException {
        ONode oNode1 = PriUtils.newNode();
        EsSetting s = new EsSetting(oNode1);
        setting.accept(s);

        EsCommand cmd = new EsCommand();
        cmd.method = PriWw.method_put;
        cmd.path = String.format("/%s/_settings", indiceName);
        cmd.dsl = oNode1.toJson();

        String tmp = execAsBody(cmd);

        return tmp;
    }

    /**
     * 索引结构显示
     *
     * @param indiceName 索引名字
     */
    public String indiceShow(String indiceName) throws IOException {
        EsCommand cmd = new EsCommand();
        cmd.method = PriWw.method_get;
        cmd.path = String.format("/%s", indiceName);


        String tmp = execAsBody(cmd);

        return tmp;
    }

    /////////////////

    /**
     * 索引别名处理
     */
    public String indiceAliases(Consumer<EsAliases> aliases) throws IOException {
        ONode oNode1 = PriUtils.newNode();
        EsAliases e = new EsAliases(oNode1);
        aliases.accept(e);

        ONode oNode = PriUtils.newNode().build(n -> n.set("actions", oNode1));

        EsCommand cmd = new EsCommand();
        cmd.method = PriWw.method_post;
        cmd.dslType = PriWw.mime_json;
        cmd.dsl = oNode.toJson();
        cmd.path = "/_aliases";

        String tmp = execAsBody(cmd);

        return tmp;
    }

    //=======================

    public EsQuery stream(String streamName) {
        return new EsQuery(this, streamName, true);
    }


    /**
     * 模板创建
     */
    public String templateCreate(String templateName, String dsl) throws IOException {
        EsCommand cmd = new EsCommand();
        cmd.method = PriWw.method_put;
        cmd.path = String.format("/_index_template/%s", templateName);

        cmd.dsl = dsl;
        cmd.dslType = PriWw.mime_json;

        String tmp = execAsBody(cmd);

        return tmp;
    }

    /**
     * 模板是否创建
     *
     * @param templateName 模板名称
     */
    public boolean templateExist(String templateName) throws IOException {
        EsCommand cmd = new EsCommand();
        cmd.method = PriWw.method_head;
        cmd.path = String.format("/_index_template/%s", templateName);

        int tmp = execAsCode(cmd);

        return tmp == 200; //404不存在
    }

    /**
     * 模板结构显示
     *
     * @param templateName 模板名称
     */
    public String templateShow(String templateName) throws IOException {
        EsCommand cmd = new EsCommand();
        cmd.method = PriWw.method_get;
        cmd.path = String.format("/_index_template/%s", templateName);


        String tmp = execAsBody(cmd);

        return tmp;
    }

    /**
     * 模板创建
     */
    public String componentCreate(String componentName, String dsl) throws IOException {
        EsCommand cmd = new EsCommand();
        cmd.method = PriWw.method_put;
        cmd.path = String.format("/_component_template/%s", componentName);

        cmd.dsl = dsl;
        cmd.dslType = PriWw.mime_json;

        String tmp = execAsBody(cmd);

        return tmp;
    }

    /**
     * 模板是否创建
     *
     * @param componentName 模板名称
     */
    public boolean componentExist(String componentName) throws IOException {
        EsCommand cmd = new EsCommand();
        cmd.method = PriWw.method_head;
        cmd.path = String.format("/_component_template/%s", componentName);

        int tmp = execAsCode(cmd);

        return tmp == 200; //404不存在
    }

    /**
     * 模板结构创建
     *
     * @param componentName 模板名称
     */
    public String componentShow(String componentName) throws IOException {
        EsCommand cmd = new EsCommand();
        cmd.method = PriWw.method_get;
        cmd.path = String.format("/_component_template/%s", componentName);


        String tmp = execAsBody(cmd);

        return tmp;
    }

    /**
     * 索引生命周期策略创建
     *
     * @param policyName 策略名称
     */
    public String policyCreate(String policyName, String dsl) throws IOException {
        EsCommand cmd = new EsCommand();
        cmd.method = PriWw.method_put;
        cmd.path = String.format("/_ilm/policy/%s", policyName);
        cmd.dsl = dsl;
        cmd.dslType = PriWw.mime_json;

        String tmp = execAsBody(cmd);

        return tmp;
    }

    /**
     * 索引生命周期策略是否存在？
     *
     * @param policyName 策略名称
     */
    public boolean policyExist(String policyName) throws IOException {
        EsCommand cmd = new EsCommand();
        cmd.method = PriWw.method_get; //用 head 会出错
        cmd.path = String.format("/_ilm/policy/%s", policyName);

        int tmp = execAsCode(cmd);

        return tmp == 200; //404不存在
    }

    /**
     * 索引生命结构显示
     *
     * @param policyName 策略名称
     */
    public String policyShow(String policyName) throws IOException {
        EsCommand cmd = new EsCommand();
        cmd.method = PriWw.method_get;
        cmd.path = String.format("/_ilm/policy/%s", policyName);


        String tmp = execAsBody(cmd);

        return tmp;
    }
}