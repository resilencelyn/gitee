package com.poetry.jianjia.net;

import com.poetry.jianjia.bean.Article;
import com.poetry.jianjia.bean.Banner;

import java.util.List;
import java.util.Map;

import okhttp3.MultipartBody;
import okhttp3.RequestBody;
import okhttp3.ResponseBody;
import poetry.jianjia.Call;
import poetry.jianjia.http.BaseUrl;
import poetry.jianjia.http.Field;
import poetry.jianjia.http.FieldMap;
import poetry.jianjia.http.FormUrlEncoded;
import poetry.jianjia.http.GET;
import poetry.jianjia.http.Multipart;
import poetry.jianjia.http.POST;
import poetry.jianjia.http.Part;
import poetry.jianjia.http.PartMap;
import poetry.jianjia.http.Path;
import poetry.jianjia.http.Query;
import poetry.jianjia.http.QueryMap;
import poetry.jianjia.http.SkipCallbackExecutor;
import poetry.jianjia.http.Url;

/**
 * @author 裴云飞
 * @date 2021/1/23
 */

public interface Wan {

    @GET("article/list/{page}/json")
    Call<ResponseBody> getArticle(@Path("page") int page);

    @GET("article/list/{page}/json")
    Call<Article> getHomeArticle(@Path("page") int page);

    @GET()
    Call<ResponseBody> getArticle(@Url String url);

    @GET("wxarticle/list/405/1/json")
    Call<ResponseBody> search(@Query("k") String k);

    @GET("wxarticle/list/405/1/json")
    Call<ResponseBody> search(@Query("k") String... k);

    @GET("wxarticle/list/405/1/json")
    Call<ResponseBody> search(@Query("k") List<String> k);

    @GET("wxarticle/list/405/1/json")
    Call<ResponseBody> search(@QueryMap Map<String, String> param);

    @GET("article/list/0/json")
    Call<ResponseBody> getArticle(@QueryMap Map<String, String> param);

    @BaseUrl("https://api.apiopen.top")
    @GET("getJoke")
    Call<ResponseBody> getJoke(@QueryMap Map<String, String> param);

    @POST("user/login")
    @FormUrlEncoded
    Call<ResponseBody> login(@Field("username") String username, @Field("password") String password);

    @POST("user/login")
    @FormUrlEncoded
    Call<ResponseBody> login(@FieldMap Map<String, String> map);

    @GET("banner/json")
    Call<Banner> getBanner();

    /**
     * 上传文件，需要使用Multipart注解和Part注解
     * 注意：上传文件的接口并不存在，开发者可以将示例中的接口替换成真实的上传文件接口
     *
     * @param photo 本地文件的路径
     * @return
     */
    @Multipart
    @POST("upload")
    Call<ResponseBody> upload(@Part MultipartBody.Part photo);

    /**
     * 使用PartMap注解上传多个文件
     * 注意：上传文件的接口并不存在，开发者可以将示例中的接口替换成真实的上传文件接口
     * @param params
     * @return
     */
    @Multipart
    @POST("upload")
    Call<ResponseBody> upload(@PartMap Map<String, RequestBody> params);

    /**
     * 蒹葭的onResponse方法默认在主线程中执行，SkipCallbackExecutor注解会让onResponse方法在子线程执行
     * 注意：下载文件的接口并不存在，开发者可以将示例中的接口替换成真实的下载文件接口
     *
     * @return
     */
    @SkipCallbackExecutor
    @GET("download")
    Call<ResponseBody> download();
}
