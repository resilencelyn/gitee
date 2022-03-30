package com.yunze.common.core.controller;

import java.beans.PropertyEditorSupport;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.alibaba.fastjson.JSON;
import com.yunze.common.utils.yunze.AesEncryptUtil;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.web.bind.WebDataBinder;
import org.springframework.web.bind.annotation.InitBinder;
import com.github.pagehelper.PageHelper;
import com.github.pagehelper.PageInfo;
import com.yunze.common.constant.HttpStatus;
import com.yunze.common.core.domain.AjaxResult;
import com.yunze.common.core.page.PageDomain;
import com.yunze.common.core.page.TableDataInfo;
import com.yunze.common.core.page.TableSupport;
import com.yunze.common.utils.DateUtils;
import com.yunze.common.utils.StringUtils;
import com.yunze.common.utils.sql.SqlUtil;

/**
 * web层通用数据处理
 * 
 * @author yunze
 */
public class BaseController
{

    protected final Logger logger = LoggerFactory.getLogger(this.getClass());

    /**
     * 将前台传递过来的日期格式的字符串，自动转化为Date类型
     */
    @InitBinder
    public void initBinder(WebDataBinder binder)
    {
        // Date 类型转换
        binder.registerCustomEditor(Date.class, new PropertyEditorSupport()
        {
            @Override
            public void setAsText(String text)
            {
                setValue(DateUtils.parseDate(text));
            }
        });
    }

    /**
     * 设置请求分页数据
     */
    protected void startPage()
    {
        PageDomain pageDomain = TableSupport.buildPageRequest();
        Integer pageNum = pageDomain.getPageNum();
        Integer pageSize = pageDomain.getPageSize();
        if (StringUtils.isNotNull(pageNum) && StringUtils.isNotNull(pageSize))
        {
            String orderBy = SqlUtil.escapeOrderBySql(pageDomain.getOrderBy());
            PageHelper.startPage(pageNum, pageSize, orderBy);
        }
    }

    /**
     * my 成功返回函数
     */
    @SuppressWarnings({ "rawtypes", "unchecked" })
    protected String MyRetunSuccess(Object obj,String msg)
    {
        msg = msg!=null?msg:"操作成功";
        Map<String, Object> Rmap =  new HashMap<String, Object>();
        Rmap.put("code",HttpStatus.SUCCESS);
        Rmap.put("msg",msg);
        Rmap.put("Data",obj);
        try {
            return AesEncryptUtil.encrypt(JSON.toJSONString(Rmap));
        }catch (Exception e){
            System.err.println(e);
            return "数据返回加密失败……";
        }
    }


    protected String Myerr(String Msg)
    {
        Map<String ,Object> map = new HashMap<>();
        map.put("code",HttpStatus.ERROR);
        map.put("msg",Msg);
        try {
            return AesEncryptUtil.encrypt(JSON.toJSONString(map));
            //return JSON.toJSONString(map);
        }catch (Exception e){
            System.err.println(e);
            return "数据返回加密失败……";
        }
    }

    /**
     * 设置请求排序数据
     */
    protected void startOrderBy()
    {
        PageDomain pageDomain = TableSupport.buildPageRequest();
        if (StringUtils.isNotEmpty(pageDomain.getOrderBy()))
        {
            String orderBy = SqlUtil.escapeOrderBySql(pageDomain.getOrderBy());
            PageHelper.orderBy(orderBy);
        }
    }

    /**
     * 响应请求分页数据
     */
    @SuppressWarnings({ "rawtypes", "unchecked" })
    protected TableDataInfo getDataTable(List<?> list)
    {
        TableDataInfo rspData = new TableDataInfo();
        rspData.setCode(HttpStatus.SUCCESS);
        rspData.setMsg("查询成功");
        rspData.setRows(list);
        rspData.setTotal(new PageInfo(list).getTotal());
        return rspData;
    }

    /**
     * 响应返回结果
     * 
     * @param rows 影响行数
     * @return 操作结果
     */
    protected AjaxResult toAjax(int rows)
    {
        return rows > 0 ? AjaxResult.success() : AjaxResult.error();
    }

    /**
     * 响应返回结果
     * 
     * @param result 结果
     * @return 操作结果
     */
    protected AjaxResult toAjax(boolean result)
    {
        return result ? success() : error();
    }

    /**
     * 返回成功
     */
    public AjaxResult success()
    {
        return AjaxResult.success();
    }

    /**
     * 返回失败消息
     */
    public AjaxResult error()
    {
        return AjaxResult.error();
    }

    /**
     * 返回成功消息
     */
    public AjaxResult success(String message)
    {
        return AjaxResult.success(message);
    }

    /**
     * 返回失败消息
     */
    public AjaxResult error(String message)
    {
        return AjaxResult.error(message);
    }

    /**
     * 页面跳转
     */
    public String redirect(String url)
    {
        return StringUtils.format("redirect:{}", url);
    }
}
