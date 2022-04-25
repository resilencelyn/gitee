package org.qpython.qpy.main.activity;

import android.app.AlertDialog;
import android.app.LoaderManager;
import android.content.Context;
import android.content.Intent;
import android.content.Loader;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v7.widget.GridLayoutManager;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.widget.TextView;

import com.quseit.util.FileHelper;
import com.quseit.util.FolderUtils;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;
import org.qpython.qpy.R;
import org.qpython.qpy.console.ScriptExec;
import org.qpython.qpy.main.adapter.AppListAdapter;
import org.qpython.qpy.main.app.CONF;
import org.qpython.qpy.main.event.AppsLoader;
import org.qpython.qpy.main.model.AppModel;
import org.qpython.qpy.main.model.QPyScriptModel;
import org.qpython.qpysdk.QPyConstants;
import org.qpython.qpysdk.utils.Utils;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * Local App list
 * Created by Hmei on 2017-05-22.
 * Edit by 乘着船 2021-2022
 */

public class AppListActivity extends BaseActivity implements LoaderManager.LoaderCallbacks<ArrayList<AppModel>> {
    public static final String TYPE_SCRIPT = "script";

    private List<AppModel> dataList;
    private AppListAdapter adapter;

    public static void start(Context context, String type) {
        Intent starter = new Intent(context, AppListActivity.class);
        starter.putExtra("type", type);
        context.startActivity(starter);
    }

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_local_app);
        initView();
        EventBus.getDefault().register(this);
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (needLog && event != null) {
            showLogDialog(event);
            needLog = false;
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        EventBus.getDefault().unregister(this);
    }

    private void initView() {
        dataList = new ArrayList<>();
        adapter = new AppListAdapter(dataList, getIntent().getStringExtra("type"), this);
        adapter.setCallback(new AppListAdapter.Callback() {
            @Override
            public void runScript(QPyScriptModel item) {
                ScriptExec.getInstance().playScript(AppListActivity.this, item.getPath(), null, false);
            }

            @Override
            public void runProject(QPyScriptModel item) {
                ScriptExec.getInstance().playProject(AppListActivity.this, item.getPath(), false);
            }

            @Override
            public void exit() {
                AppListActivity.this.finish();
            }
        });

        RecyclerView appsView = findViewById(R.id.rv_app);
        GridLayoutManager mLayoutManager = new GridLayoutManager(this, 3, LinearLayoutManager.VERTICAL, false);
        appsView.setLayoutManager(mLayoutManager);
        appsView.setAdapter(adapter);

        ((TextView) findViewById(R.id.tv_folder_name)).setText(R.string.qpy_app);
        ((TextView) findViewById(R.id.tv_ar_back)).setOnClickListener(view -> finish());
        getProjectScriptList();
    }

    private void getProjectList(File projectFile) {
        File[] projectFiles = projectFile.listFiles();
        if (projectFiles != null) {
            Arrays.sort(projectFiles, FolderUtils.sortByName);
            for (File file : projectFiles) {
                if (file.isDirectory()) {
                    if ((new File(file, "main.py")).exists())
                        dataList.add(new QPyScriptModel(file));
                    else {
                        getProjectList(file);
                    }
                }
            }
        }
    }

    private void getProjectScriptList() {
        for (String path : CONF.PATHS()){
            File projectFile = new File(path,QPyConstants.DFROM_PRJ3);
            getProjectList(projectFile);
            getScriptList(path);
        }
    }

    private void getScriptList(String path) {
        try {
            File[] files = FileHelper.getFilesByType(new File(path + "/" + QPyConstants.DFROM_QPY3));
            if (files!=null && files.length > 0) {
                Arrays.sort(files, FolderUtils.sortByName);
                for (File file : files) {
                    dataList.add(new QPyScriptModel(file));
                }
            }
            adapter.notifyDataSetChanged();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public Loader<ArrayList<AppModel>> onCreateLoader(int id, Bundle args) {
        return new AppsLoader(this);
    }

    @Override
    public void onLoadFinished(Loader<ArrayList<AppModel>> loader, ArrayList<AppModel> data) {
        dataList.addAll(data);
        adapter.notifyDataSetChanged();
    }

    @Override
    public void onLoaderReset(Loader<ArrayList<AppModel>> loader) {
        dataList.clear();
        adapter.notifyDataSetChanged();
    }

    boolean              needLog;
    ScriptExec.LogDialog event;

    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onEventMainThread(ScriptExec.LogDialog event) {
        this.event = event;
        needLog = true;
    }

    public void showLogDialog(ScriptExec.LogDialog event) {
        AlertDialog alertDialog = new AlertDialog.Builder(this, R.style.MyDialog)
                .setTitle(R.string.last_log)
                .setMessage(com.quseit.qpyengine.R.string.open_log)
                .setNegativeButton(R.string.no, ((dialog, which) -> dialog.dismiss()))
                .setPositiveButton(R.string.ok, (dialog, which) -> {

                    Utils.checkRunTimeLog(this, event.title, event.path);

                    dialog.dismiss();
                })
                .create();
        alertDialog.show();
    }

    @Override
    public void finish() {
        super.finish();
        overridePendingTransition(0, R.anim.fade_out);
    }
}