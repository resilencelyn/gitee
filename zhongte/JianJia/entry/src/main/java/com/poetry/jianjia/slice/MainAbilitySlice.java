package com.poetry.jianjia.slice;

import com.poetry.jianjia.BaseApplication;
import com.poetry.jianjia.ResourceTable;
import com.poetry.jianjia.bean.Article;
import com.poetry.jianjia.provider.HomeArticleProvider;
import com.poetry.jianjia.util.LogUtils;

import java.io.File;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import ohos.aafwk.ability.AbilitySlice;
import ohos.aafwk.content.Intent;
import ohos.agp.components.ListContainer;
import okhttp3.MediaType;
import okhttp3.MultipartBody;
import okhttp3.RequestBody;
import okhttp3.ResponseBody;
import poetry.jianjia.Call;
import poetry.jianjia.Callback;
import poetry.jianjia.Response;

public class MainAbilitySlice extends AbilitySlice {

    private ListContainer mListContainer;
    private HomeArticleProvider mHomeArticleProvider;
    List<Article.Data.Datas> mDatas;

    @Override
    public void onStart(Intent intent) {
        super.onStart(intent);
        super.setUIContent(ResourceTable.Layout_ability_main);
        mListContainer = (ListContainer) findComponentById(ResourceTable.Id_list);
        mDatas = new ArrayList<>();
        mHomeArticleProvider = new HomeArticleProvider(this, mDatas);
        mListContainer.setItemProvider(mHomeArticleProvider);
        // 从服务端获取数据
        getHomeArticle();
    }

    /**
     * 下载文件，注意：下载文件的接口并不存在，开发者可以将示例中的接口替换成真实的下载文件接口
     */
    private void download() {
        BaseApplication.getInstance().getWan().download().enqueue(new Callback<ResponseBody>() {
            @Override
            public void onResponse(Call<ResponseBody> call, Response<ResponseBody> response) {
                // onResponse方法在子线程中执行
                // 得到文件输入流后，就可以写文件了
                InputStream inputStream = response.body().byteStream();

            }

            @Override
            public void onFailure(Call<ResponseBody> call, Throwable throwable) {

            }
        });
    }

    /**
     * 上传多个文件，注意：上传文件的接口并不存在，开发者可以将示例中的接口替换成真实的上传文件接口
     */
    private void uploadMultiFile() {
        File photoFile = new File(getExternalCacheDir(), "photo.png");
        RequestBody photoBody = RequestBody.create(MediaType.parse("image/png"), photoFile);

        File avatarFile = new File(getExternalCacheDir(), "avatar.png");
        RequestBody avatarBody = RequestBody.create(MediaType.parse("image/png"), avatarFile);

        Map<String, RequestBody> photos = new HashMap<>();
        photos.put("photo", photoBody);
        photos.put("avatar", avatarBody);
        BaseApplication.getInstance().getWan().upload(photos).enqueue(new Callback<ResponseBody>() {
            @Override
            public void onResponse(Call<ResponseBody> call, Response<ResponseBody> response) {
                // 上传成功
            }

            @Override
            public void onFailure(Call<ResponseBody> call, Throwable throwable) {
                // 上传失败
            }
        });
    }

    /**
     * 上传单个文件，注意：上传文件的接口并不存在，开发者可以将示例中的接口替换成真实的上传文件接口
     */
    private void uploadFile() {
        // 文件路径
        File file = new File(getExternalCacheDir(), "icon.png");
        // 创建请求体对象
        RequestBody photoBody = RequestBody.create(MediaType.parse("image/png"), file);
        MultipartBody.Part photo = MultipartBody.Part.createFormData("photos", "icon.png", photoBody);
        BaseApplication.getInstance().getWan().upload(photo).enqueue(new Callback<ResponseBody>() {
            @Override
            public void onResponse(Call<ResponseBody> call, Response<ResponseBody> response) {
                // 上传成功
            }

            @Override
            public void onFailure(Call<ResponseBody> call, Throwable throwable) {
                // 上传失败
            }
        });
    }

    /**
     * 从服务端获取数据
     */
    public void getHomeArticle() {
        BaseApplication.getInstance().getWan().getHomeArticle(0).enqueue(new Callback<Article>() {
            @Override
            public void onResponse(Call<Article> call, Response<Article> response) {
                if (response.isSuccessful()) {
                    // 请求成功
                    setHomeArticle(response.body());
                }
            }

            @Override
            public void onFailure(Call<Article> call, Throwable t) {
                // 请求失败
                LogUtils.info("yunfei", t.getMessage());
            }
        });
    }

    @Override
    public void onActive() {
        super.onActive();
    }

    @Override
    public void onForeground(Intent intent) {
        super.onForeground(intent);
    }

    public void setHomeArticle(Article article) {
        if (article == null || article.data == null || article.data.datas == null) {
            return;
        }
        mDatas.addAll(article.data.datas);
        // 刷新列表
        mHomeArticleProvider.notifyDataChanged();
    }

}
