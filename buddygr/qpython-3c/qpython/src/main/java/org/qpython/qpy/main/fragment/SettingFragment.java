package org.qpython.qpy.main.fragment;

import android.app.ActivityManager;
import android.app.AlarmManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.Resources;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.preference.CheckBoxPreference;
import android.preference.Preference;
import android.preference.PreferenceFragment;
import android.preference.PreferenceManager;
import android.preference.SwitchPreference;
import android.support.annotation.Nullable;
import android.support.v7.app.AlertDialog;
import android.text.InputType;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.widget.ListView;
import android.widget.Toast;

import com.loopj.android.http.JsonHttpResponseHandler;
import com.quseit.base.QBaseApp;
import com.quseit.common.updater.downloader.Downloader;
import com.quseit.util.NAction;
import com.quseit.util.NStorage;

import org.apache.http.Header;
import org.json.JSONException;
import org.json.JSONObject;
import org.qpython.qpy.R;
import org.qpython.qpy.main.activity.HomeMainActivity;
import org.qpython.qpy.main.app.App;
import org.qpython.qpy.main.app.CONF;
import org.qpython.qpy.main.auxActivity.ProtectActivity;
import org.qpython.qpy.main.auxActivity.ScreenRecordActivity;
import org.qpython.qpy.main.service.FTPServerService;
import org.qpython.qpy.main.widget.LoadingDialog;
import org.qpython.qpy.texteditor.ui.view.EnterDialog;
import org.qpython.qpysdk.QPyConstants;
import org.qpython.qpysdk.QPySDK;
import org.qpython.qpysdk.utils.Utils;
import org.qpython.qsl4a.QPyScriptService;

import java.io.File;
import java.net.InetAddress;

public class SettingFragment extends PreferenceFragment {
    private static final String TAG = "SettingFragment";

    private LoadingDialog mLoadingDialog;

    private SharedPreferences settings;
    private Resources         resources;
    private Preference        mPassWordPref, username_pref, portnum_pref, chroot_pref, lastlog, ipaddress, qpyCustom;
    private CheckBoxPreference sl4a, running_state, root, display_pwd, qpy_protect;//, notebook_run;

    //private PreferenceScreen py_inter,notebook_page;
    //private Preference py3,py2; //notebook_res, py2compatible
    //private Preference update_qpy3,update_qpy2compatible;

    private void viewWebSite(int resId) {
        startActivity(new Intent(Intent.ACTION_VIEW,Uri.parse(getString(resId))));
    }

    private SwitchPreference log, app;
    private BroadcastReceiver ftpServerReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            Log.v(TAG, "FTPServerService action received: " + intent.getAction());
            switch (intent.getAction()) {
                case FTPServerService.ACTION_STARTED:
                    running_state.setChecked(true);
                    // Fill in the FTP server address
                    setFtpAddress();
                    break;
                case FTPServerService.ACTION_STOPPED:
                    running_state.setChecked(false);
                    running_state.setSummary(org.swiftp.R.string.running_summary_stopped);
                    break;
                case FTPServerService.ACTION_FAILEDTOSTART:
                    running_state.setChecked(false);
                    running_state.setSummary(org.swiftp.R.string.running_summary_failed);
                    Toast.makeText(getActivity(), R.string.ip_address_need_wifi_or_ap, Toast.LENGTH_LONG).show();
                    break;
            }
        }
    };

    public SettingFragment() {
    }

    static private String transformPassword(String password) {
        StringBuilder sb = new StringBuilder(password.length());
        for (int i = 0; i < password.length(); ++i)
            sb.append('*');
        return sb.toString();
    }

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        addPreferencesFromResource(R.xml.qpython_setting);
        settings = PreferenceManager.getDefaultSharedPreferences(getActivity());
        resources = getResources();
        initSettings();
        initListener();
    }

    @Override
    public void onViewCreated(View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        //Disable scrollbar
        ListView listView = view.findViewById(android.R.id.list);
        if (listView != null) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                listView.setFocusedByDefault(false);
            }
        }
        mLoadingDialog = new LoadingDialog(getActivity());
    }

    @Override
    public void onResume() {
        super.onResume();
    }

    private boolean showIpAddress(){
        InetAddress ip;
        try {
            ip = FTPServerService.getWifiAndApIp();
        } catch (NullPointerException e) {
            ip = null;

        }
        if (ip!=null) {
            ipaddress.setSummary(ip.getHostAddress());
            return true;
        } else {
            ipaddress.setSummary(R.string.ip_address_need_wifi_or_ap);
            return false;
        }
    }

    private void initSettings() {
        ipaddress = findPreference("ipaddress");
        showIpAddress();

        lastlog = findPreference("lastlog");
        //py_inter = (PreferenceScreen) findPreference(getString(R.string.key_py_inter));
        /*notebook_page = (PreferenceScreen) findPreference(getString(R.string.key_notebook_page));
        notebook_page.setTitle(MessageFormat.format(getString(R.string.notebook_for_py), NAction.getPyVer(getActivity())));
        if (NAction.isQPy3(getActivity())) {
            notebook_page.setSummary(NotebookUtil.isNotebookLibInstall(getActivity()) ? R.string.notebook_installed : R.string.notebook_not_started);

        } else {
            notebook_page.setSummary( R.string.notebook_py3_support);

        }*/

        //notebook_res = (Preference) findPreference(getString(R.string.key_notebook));
        //notebook_res.setSummary((NotebookUtil.isNotebookLibInstall(getActivity())||NotebookUtil.isNotebookInstall(getActivity()))?R.string.choose_notebook_inter:R.string.install_notebook_first);
        //notebook_run = (CheckBoxPreference) findPreference(getString(R.string.key_notebook_run));

        //update_qpy3 = (Preference)findPreference(getString(R.string.key_update_qpy3));
        //update_qpy2compatible = (Preference)findPreference(getString(R.string.key_update_qpy2compatible));
        //py2 = (Preference) findPreference(getString(R.string.key_py2));
        //py2compatible = (Preference) findPreference(getString(R.string.key_py2compatible));
        //py3 = (Preference) findPreference(getString(R.string.key_py3));


        root = (CheckBoxPreference) findPreference(resources.getString(R.string.key_root));
        qpyCustom = findPreference(resources.getString(R.string.qpy_custom_dir));
        sl4a = (CheckBoxPreference) findPreference(resources.getString(R.string.key_sl4a));
        qpy_protect = (CheckBoxPreference) findPreference("qpython_protect");
        app = (SwitchPreference) findPreference(getString(R.string.key_hide_push));
        log = (SwitchPreference) findPreference(resources.getString(R.string.key_hide_noti));
        username_pref = findPreference(resources.getString(R.string.key_username));
        running_state = (CheckBoxPreference) findPreference(resources.getString(R.string.key_ftp_state));
        display_pwd = (CheckBoxPreference) findPreference(resources.getString(R.string.key_show_pwd));
        mPassWordPref = findPreference(resources.getString(R.string.key_ftp_pwd));
        portnum_pref = findPreference(resources.getString(R.string.key_port_num));
        chroot_pref = findPreference(resources.getString(R.string.key_root_dir));

        boolean isRoot, isRunning;
        isRoot = settings.getBoolean(getString(R.string.key_root), false);
        root.setChecked(isRoot);
        root.setSummary(isRoot ? R.string.enable_root : R.string.disable_root);

        isRunning = isMyServiceRunning(QPyScriptService.class);
        sl4a.setChecked(isRunning);
        sl4a.setSummary(isRunning ? R.string.sl4a_running : R.string.sl4a_un_running);

        qpy_protect.setChecked(settings.getBoolean("qpython_protect",false));

        app.setChecked(settings.getBoolean(getString(R.string.key_hide_push), true));
        log.setChecked(settings.getBoolean(getString(R.string.key_hide_noti), true));

        //qpypi.setSummary(settings.getString(getString(R.string.key_qpypi), org.qpython.qpy.main.app.CONF.QPYPI_URL));
        username_pref.setSummary(settings.getString(resources.getString(R.string.key_username), resources.getString(org.swiftp.R.string.username_default)));
        boolean isFTPRunning = FTPServerService.isRunning();
        running_state.setChecked(isFTPRunning);
        if (isFTPRunning) {
            setFtpAddress();
        } else {
            running_state.setSummary(R.string.running_summary_stopped);
        }
        portnum_pref.setSummary(settings.getString(resources.getString(R.string.key_port_num),
                resources.getString(org.swiftp.R.string.portnumber_default)));
        chroot_pref.setSummary(settings.getString(resources.getString(R.string.key_root_dir),
                Environment.getExternalStorageDirectory().getPath()));
        qpyCustom.setSummary(CONF.CUSTOM_PATH);

        //py_inter.setSummary(NAction.isQPy3(getActivity()) ? R.string.py3_now : R.string.py2_now);
        //setNotebookCheckbox();

        SharedPreferences.Editor editor = settings.edit();
        editor.putBoolean(getString(R.string.key_root), root.isChecked());
        //editor.putString(getString(R.string.key_qpypi), qpypi.getSummary().toString());
        editor.putString(getString(R.string.key_username), username_pref.getSummary().toString());
        editor.putString(getString(R.string.key_ftp_pwd), settings.getString(mPassWordPref.getKey(), "ftp"));
        editor.putString(getString(R.string.key_port_num), portnum_pref.getSummary().toString());
        editor.putString(getString(R.string.key_root_dir), chroot_pref.getSummary().toString());
        editor.putBoolean(getString(R.string.key_hide_noti), log.isChecked());
        //editor.putBoolean(getString(R.string.key_py3), py3.isChecked());
        editor.apply();
    }

    private void setFtpAddress() {
        InetAddress address = FTPServerService.getWifiAndApIp();
        if (address == null) {
            Log.v(TAG, "Unable to retreive wifi ip address");
            running_state.setSummary(org.swiftp.R.string.cant_get_url);
            Toast.makeText(getActivity(),"FTP: "+getString(R.string.ip_address_need_wifi_or_ap),Toast.LENGTH_LONG).show();
        } else {
            String iptext = "ftp://" + address.getHostAddress() + ":"
                    + FTPServerService.getPort() + "/";
            String summary = getString(R.string.running_summary_started,
                    iptext);
            running_state.setSummary(summary);
        }
    }

    private boolean isMyServiceRunning(Class<?> serviceClass) {
        ActivityManager manager = (ActivityManager) getActivity().getSystemService(Context.ACTIVITY_SERVICE);
        for (ActivityManager.RunningServiceInfo service : manager.getRunningServices(Integer.MAX_VALUE)) {
            if (serviceClass.getName().equals(service.service.getClassName())) {
                return true;
            }
        }
        return false;
    }

    private void initListener() {

        lastlog.setOnPreferenceClickListener(preference -> {
            File logFolder = new File(CONF.SCOPE_STORAGE_PATH,"log");
            String[] logFiles = logFolder.list();
            if (logFiles == null || logFiles.length==0){return false;}
            android.app.AlertDialog.Builder alert;
            alert = new android.app.AlertDialog.Builder(getActivity(),R.style.MyDialog);
            alert.setTitle(R.string.choose_file);
            alert.setItems(logFiles, (dialogInterface, i) -> {
                Utils.checkRunTimeLog(getActivity(), getString(R.string.last_log),
                        logFolder.toString()+"/"+logFiles[i]);
                    });
            alert.setNegativeButton(getString(R.string.close),
                    (dialogInterface, i) -> dialogInterface.dismiss());
            alert.create().show();
            return false;
        });

        ipaddress.setOnPreferenceClickListener(preference -> showIpAddress());

        /*if (!NAction.isQPy3(getActivity())) {
            notebook_run.setSummary(getString(R.string.notebook_py3_support));
            notebook_run.setEnabled(false);

        } else {
            notebook_run.setChecked(NotebookUtil.isNBSrvSet(getActivity()));

            notebook_run.setOnPreferenceChangeListener((preference, newValue) -> {
                if ((boolean)newValue) {
                    NotebookUtil.startNotebookService2(getActivity());

                } else {
                    NotebookUtil.killNBSrv(getActivity());
                }
                notebook_page.setSummary(NotebookUtil.isNotebookEnable(getActivity())?R.string.notebook_installed : R.string.notebook_not_started);

                return true;
            });
        }*/

        /*py2.setOnPreferenceClickListener(preference -> {
            Log.d(TAG, "py2.setOnPreferenceClickListener");
            NotebookUtil.killNBSrv(getActivity());

            releasePython2Standard();
            return false;
        });

        py3.setOnPreferenceClickListener(preference -> {
            NotebookUtil.killNBSrv(getActivity());
            releasePython3();

            return false;
        });*/

        root.setOnPreferenceChangeListener((preference, newValue) ->
        {
            if ((boolean) newValue) {
                if (NAction.isRootSystem()) {
                    NStorage.setSP(getActivity(), "app.root", "1");
                    return true;
                } else {
                    Toast.makeText(getActivity(), R.string.not_root_yet, Toast.LENGTH_SHORT).show();
                    return false;
                }
            } else {
                NStorage.setSP(getActivity(), "app.root", "0");
                return true;
            }
        });

        sl4a.setOnPreferenceChangeListener((preference, newValue) ->

        {
            boolean isCheck = (boolean) newValue;
            if (isCheck) {
                getActivity().startService(new Intent(getActivity(), QPyScriptService.class));
            } else {
                getActivity().stopService(new Intent(getActivity(), QPyScriptService.class));
            }
            return true;
        });

        app.setOnPreferenceChangeListener((preference, newValue) ->

        {
            boolean isCheck = (boolean) newValue;
            settings.edit().putBoolean(getString(R.string.key_hide_push), isCheck).apply();
            return true;
        });
        log.setOnPreferenceChangeListener((preference, newValue) ->

        {
            boolean isCheck = (boolean) newValue;
            settings.edit().putBoolean(getString(R.string.key_hide_noti), isCheck).apply();
            return true;
        });

        findPreference(resources.getString(R.string.key_reset)).
            setOnPreferenceClickListener(preference ->
            {
                NAction.startInstalledAppDetailsActivity(getActivity());
                return false;
            });

        /*findPreference(resources.getString(R.string.key_about)).
            setOnPreferenceClickListener(preference ->
            {
                AboutActivity.start(getActivity());
                return true;
            });

        findPreference("course_official").
            setOnPreferenceClickListener(preference ->
            {
                viewWebSite(R.string.qpython_edu);
                return true;
            });*/

        findPreference("community").
            setOnPreferenceClickListener(preference ->
            {
                viewWebSite(R.string.community_website);
                return true;
            });

        findPreference("video_account").
                setOnPreferenceClickListener(preference ->
                {
                    viewWebSite(R.string.wechat_video_website);
                    return true;
                });

        findPreference("open_source_library").
            setOnPreferenceClickListener(preference ->
            {
                viewWebSite(R.string.bilibili_website);
                return true;
            });

        findPreference("screen_record").
                setOnPreferenceClickListener(preference ->
                {
                    startActivity(new Intent(getActivity(), ScreenRecordActivity.class));
                    return true;
                });

        qpy_protect.setOnPreferenceChangeListener((preference,newValue) -> {
            Context context = getActivity();
            boolean result = (boolean) newValue;
            settings.edit().putBoolean("qpython_protect",result).apply();
            if (result) {
                ProtectActivity.DoProtect(context);
                } else {
                Toast.makeText(context,getString(R.string.restart_valid),Toast.LENGTH_SHORT).show();
                ProtectActivity.UndoProtect();
            }
            return true;
        });

        /*  ====================FTP====================   */
        running_state.setOnPreferenceChangeListener((preference, newValue) ->
        {
            if ((Boolean) newValue) {
                startServer();
            } else {
                stopServer();
            }
            return true;
        });

        username_pref.setOnPreferenceClickListener(preference ->

        {
            new EnterDialog(getActivity())
                    .setTitle(preference.getKey())
                    .setText(preference.getSummary().toString())
                    .setConfirmListener(name -> {
                        preference.setSummary(name);
                        updatePreference(preference);
                        stopServer();
                        return true;
                    })
                    .show();
            return true;
        });

        display_pwd.setOnPreferenceChangeListener(((preference, newValue) ->

        {
            boolean check = (boolean) newValue;
            String pwd = settings.getString(getString(R.string.key_ftp_pwd), "ftp");
            if (check) {
                mPassWordPref.setSummary(pwd);
            } else {
                mPassWordPref.setSummary(transformPassword(pwd));
            }
            return true;
        }));

        String password = settings.getString(resources.getString(R.string.key_ftp_pwd), "ftp");
        mPassWordPref.setSummary(display_pwd.isChecked() ? password :

                transformPassword(password));
        mPassWordPref.setOnPreferenceClickListener(preference ->

        {
            new EnterDialog(getActivity())
                    .setTitle(mPassWordPref.getKey())
                    .setText(password)
                    .setEnterType(InputType.TYPE_TEXT_VARIATION_PASSWORD)
                    .setConfirmListener(name -> {
                        settings.edit().putString(mPassWordPref.getKey(), name).apply();
                        mPassWordPref.setSummary(display_pwd.isChecked() ? name : transformPassword(name));
                        stopServer();
                        return true;
                    })
                    .show();
            return true;
        });

        portnum_pref.setOnPreferenceClickListener(preference ->

        {
            new EnterDialog(getActivity())
                    .setTitle(preference.getKey())
                    .setText(preference.getSummary().toString())
                    .setEnterType(InputType.TYPE_CLASS_NUMBER)
                    .setConfirmListener(name -> {
                        int portNum = Integer.parseInt(name);
                        if (portNum <= 0 || portNum > 65535) {
                            Toast.makeText(getActivity(), org.swiftp.R.string.port_validation_error, Toast.LENGTH_LONG).show();
                            return false;
                        }
                        preference.setSummary(name);
                        updatePreference(preference);
                        stopServer();
                        return true;
                    })
                    .show();
            return true;
        });

        chroot_pref.setOnPreferenceClickListener(preference ->

        {
            new EnterDialog(getActivity())
                    .setTitle(preference.getKey())
                    .setText(preference.getSummary().toString())
                    .setConfirmListener(name -> {
                        if (preference.getSummary().equals(name)) {
                            return true;
                        }
                        File chrootTest = new File(name);
                        if (!chrootTest.isDirectory() || !chrootTest.canRead()) {
                            Toast.makeText(getActivity(), R.string.dir_not_valid, Toast.LENGTH_SHORT).show();
                            return false;
                        }
                        preference.setSummary(name);
                        updatePreference(preference);
                        stopServer();
                        return true;
                    })
                    .show();
            return true;

        });

        qpyCustom.setOnPreferenceClickListener(preference ->

        {
            new EnterDialog(getActivity())
                    .setTitle(preference.getKey())
                    .setText(preference.getSummary().toString())
                    .setConfirmListener(name -> {
                        if (preference.getSummary().equals(name)) {
                            return true;
                        }
                        File customTest = new File(name);
                        if (!(customTest.isDirectory() && customTest.canRead())) {
                            Toast.makeText(getActivity(), R.string.dir_not_valid, Toast.LENGTH_SHORT).show();
                            return false;
                        }
                        CONF.CUSTOM_PATH = customTest.getAbsolutePath();
                        preference.setSummary(CONF.CUSTOM_PATH);
                        updatePreference(preference);
                        return true;
                    })
                    .show();
            return true;
        });

        final CheckBoxPreference wakelock_pref = (CheckBoxPreference) findPreference(resources.getString(R.string.key_stay_awake));
        wakelock_pref.setOnPreferenceChangeListener((preference, newValue) ->
        {
            stopServer();
            return true;
        });
    }

    /*private void releaseNotebook(Preference preference) {
        Observable.create((Observable.OnSubscribe<Boolean>) subscriber -> {
            try {
                String nbfile = NStorage.getSP(App.getContext(), NotebookUtil.getNbResFk(getActivity()));
                if (!nbfile.equals("") && new File(nbfile).exists()) {    //
                    extractNotebookRes(nbfile);
                }
                if (!NotebookUtil.isNotebookLibInstall(getActivity())) {
                    NotebookUtil.extraData(getActivity());
                }

                subscriber.onNext(true);
                subscriber.onCompleted();
            } catch (Exception e) {
                e.printStackTrace();
            }

        })
        .subscribeOn(Schedulers.io())
        .doOnSubscribe(() -> mLoadingDialog.show())
        .subscribeOn(AndroidSchedulers.mainThread())
        .observeOn(AndroidSchedulers.mainThread())
        .doOnTerminate(() -> mLoadingDialog.dismiss())
        .subscribe(new Observer<Boolean>() {
            @Override
            public void onCompleted() {

            }

            @Override
            public void onError(Throwable e) {

            }

            @Override
            public void onNext(Boolean aBoolean) {
                Log.d(TAG, "onNext");

                NotebookUtil.startNotebookService2(getActivity());
                notebook_page.setSummary(NotebookUtil.isNotebookLibInstall(getActivity())?R.string.notebook_installed : R.string.notebook_not_started);


            }
        });
    }

    private void installNotebook() {
        new Thread(() -> {
            QPySDK qpySDK = new QPySDK(App.getContext(), getActivity());
            qpySDK.extractRes("notebook" + (NAction.isQPy3(getActivity()) ? "3" : "2"), new File(NotebookUtil.RELEASE_PATH));
        }).run();
//        new AlertDialog.Builder(getActivity(), R.style.MyDialog)
//            .setTitle(R.string.notice)
//            .setMessage(R.string.install_notebook_hint)
//            .setPositiveButton(R.string.download_it, (dialog1, which)->getNotebook())
//            .setNegativeButton(R.string.cancel, (dialog1, which) -> dialog1.dismiss())
//            .create()
//            .show();
    }


    private void extractNotebookRes(String path) {
        final String extarget = NotebookUtil.RELEASE_PATH;

        if (path!=null && !path.equals("")) {
            File resf = new File(path);
            if (resf.exists()) {
                QPySDK qpySDK = new QPySDK(App.getContext(), getActivity());
                qpySDK.extractRes(resf, new File(extarget), false);
            }
        }
    }*/


    private void releaseQPycRes(String path) {
        final String extarget = CONF.PY_CACHE_PATH;

        if (path!=null && !path.equals("")) {
            File res = new File(path);

            if (res.exists()) {
                QPySDK qpySDK = new QPySDK(App.getContext(), getActivity());
                qpySDK.extractRes(res, new File(extarget), false);
            }
        }
    }

    /*private void releasePython2Standard() {
        Observable.create((Observable.OnSubscribe<Boolean>) subscriber -> {
            try {
                File filesDir = getActivity().getFilesDir();
                //removeQPyc2Core();
                QPySDK qpysdk = new QPySDK(getActivity(), getActivity());
                qpysdk.extractRes("private1", filesDir, true);
                qpysdk.extractRes("private2", filesDir, true);
                qpysdk.extractRes("private~", new File(filesDir,"lib/python2.7/site-packages"), true);
                qpysdk.extractRes("private3", filesDir,true);

                File externalStorage = new File(Environment.getExternalStorageDirectory(), "qpython");
                qpysdk.extractRes("public", new File(externalStorage + "/lib"),true);

                subscriber.onNext(true);
                subscriber.onCompleted();
            } catch (Exception e) {
                e.printStackTrace();
                subscriber.onError(new Throwable("Failed to release Py2 resources"));
            }
        })
        .subscribeOn(Schedulers.io())
        .doOnSubscribe(() -> mLoadingDialog.show())
        .subscribeOn(AndroidSchedulers.mainThread())
        .observeOn(AndroidSchedulers.mainThread())
        .doOnTerminate(() -> mLoadingDialog.dismiss())
        .subscribe(new Observer<Boolean>() {
            @Override
            public void onCompleted() {

            }

            @Override
            public void onError(Throwable e) {
                Toast.makeText(getActivity(), "Faild to extract Py2 resource", Toast.LENGTH_SHORT).show();
            }

            @Override
            public void onNext(Boolean aBoolean) {
                NAction.setQPyInterpreter(getActivity(), "2.x");
                py_inter.setSummary(R.string.py2_now);

                getActivity().recreate();
            }
        });
    }*/

    /*private void releasePython2Compatable(Preference preference) {
        Observable.create((Observable.OnSubscribe<Boolean>) subscriber -> {
            try {

                //removeQPyc2Core();

                extractQPyCore(true);

                subscriber.onNext(true);
                subscriber.onCompleted();
            } catch (Exception e) {
                e.printStackTrace();
                subscriber.onError(new Throwable("Failed to release Py2 resources"));
            }
        })
        .subscribeOn(Schedulers.io())
        .doOnSubscribe(() -> mLoadingDialog.show())
        .subscribeOn(AndroidSchedulers.mainThread())
        .observeOn(AndroidSchedulers.mainThread())
        .doOnTerminate(() -> mLoadingDialog.dismiss())
        .subscribe(new Observer<Boolean>() {
            @Override
            public void onCompleted() {

            }

            @Override
            public void onError(Throwable e) {
                Toast.makeText(getActivity(), "Faild to extract Py2 resource", Toast.LENGTH_SHORT).show();
            }

            @Override
            public void onNext(Boolean aBoolean) {
                NAction.setQPyInterpreter(getActivity(), "2.x");
                py_inter.setSummary(R.string.py2_now);

                getActivity().recreate();
            }
        });
    }*/


    /*private void releasePython3() {
        QPySDK qpysdk = new QPySDK(this.getActivity(), this.getActivity());
        Observable.create((Observable.OnSubscribe<Boolean>) subscriber -> {
            try {
                File filesDir = getActivity().getFilesDir();
                releaseQPycRes(NStorage.getSP(App.getContext(),QPyConstants.KEY_PY3_RES));
                //extractQPyCore(false);

                qpysdk.extractRes("private31", filesDir, true);
                qpysdk.extractRes("private32", filesDir, true);
                qpysdk.extractRes("private~", new File(filesDir,"lib/python"+QPyConstants.py3Ver+"/site-packages"), true);
                qpysdk.extractRes("private33", filesDir,true);
                //qpysdk.extractRes("notebook3", filesDir, true);

                File externalStorage = new File(Environment.getExternalStorageDirectory(), "qpython");
                qpysdk.extractRes("public3", new File(externalStorage + "/lib"));

                subscriber.onNext(true);
                subscriber.onCompleted();
            } catch (Exception e) {
                e.printStackTrace();
                subscriber.onError(new Throwable("Failed to release Py3 resources"));
            }
        })
        .subscribeOn(Schedulers.io())
        .doOnSubscribe(() -> mLoadingDialog.show())
        .subscribeOn(AndroidSchedulers.mainThread())
        .observeOn(AndroidSchedulers.mainThread())
        .doOnTerminate(() -> mLoadingDialog.dismiss())
        .subscribe(new Observer<Boolean>() {
            @Override
            public void onCompleted() {

            }

            @Override
            public void onError(Throwable e) {
                Toast.makeText(getActivity(), "Faild to extract Py3 resource", Toast.LENGTH_SHORT).show();
            }

            @Override
            public void onNext(Boolean aBoolean) {
                NAction.setQPyInterpreter(getActivity(), "3.x");
                py_inter.setSummary(R.string.py3_now);

                getActivity().recreate();
            }
        });

    }*/

    private void updatePreference(Preference preference) {
        SharedPreferences.Editor editor = settings.edit();
        if (preference instanceof CheckBoxPreference) {
            editor.putBoolean(preference.getKey(), ((CheckBoxPreference) preference).isChecked());
        } else {
            editor.putString(preference.getKey(), preference.getSummary().toString());
        }
        editor.apply();
    }

    private void restartApp() {
        Intent mStartActivity = new Intent(getActivity(), HomeMainActivity.class);
        int mPendingIntentId = 123456;
        PendingIntent mPendingIntent = PendingIntent.getActivity(getActivity(), mPendingIntentId, mStartActivity, PendingIntent.FLAG_CANCEL_CURRENT);
        AlarmManager mgr = (AlarmManager) getActivity().getSystemService(Context.ALARM_SERVICE);
        mgr.set(AlarmManager.RTC, System.currentTimeMillis() + 100, mPendingIntent);
        System.exit(0);
    }

    private void startServer() {
        Context context = getActivity();
        Intent serverService = new Intent(context, FTPServerService.class);
        if (!FTPServerService.isRunning()) {
            warnIfNoExternalStorage();
            context.startService(serverService);
        }
    }

    private void stopServer() {
        Context context = getActivity();
        Intent serverService = new Intent(context, FTPServerService.class);
        context.stopService(serverService);
    }

    /**
     * Will check if the device contains external storage (sdcard) and display a warning
     * for the user if there is no external storage. Nothing more.
     */
    private void warnIfNoExternalStorage() {
        String storageState = Environment.getExternalStorageState();
        if (!storageState.equals(Environment.MEDIA_MOUNTED)) {
            Log.v(TAG, "Warning due to storage state " + storageState);
            Toast toast = Toast.makeText(getActivity(), org.swiftp.R.string.storage_warning,
                    Toast.LENGTH_LONG);
            toast.setGravity(Gravity.CENTER, 0, 0);
            toast.show();
        }
    }

    public BroadcastReceiver getFtpServerReceiver() {
        return ftpServerReceiver;
    }

    /*
    TODO: Make it configuration

    private void getNotebook() {

        mLoadingDialog.show();

        QBaseApp.getInstance().getAsyncHttpClient().get(getActivity(), NotebookUtil.getNBLink(getActivity()),
        null, new JsonHttpResponseHandler() {
            @Override
            public void onSuccess(int statusCode, Header[] headers, JSONObject result) {
                final String KEY_RES = "setting.notebookresource.ver";

                try {

                    final String notebook_resource_ver = NStorage.getSP(getActivity(), KEY_RES);
                    final String url = result.getString("link");
                    final String target = result.getString("target");
                    final String vercode = result.getString("vercode");
                    final String title = result.getString("title");
                    final String vername = result.getString("vername");
                    final String path = NotebookUtil.RELEASE_PATH+"/"+target;

                    NStorage.setSP(App.getContext(),  NotebookUtil.getNbResFk(getActivity()), path);

                    Log.d(TAG, "getNotebook:onSuccess:"+notebook_resource_ver+"["+vercode+"]");

                    if (notebook_resource_ver.equals(vercode) && new File(path).exists()) {
                        mLoadingDialog.dismiss();

                        new AlertDialog.Builder(getActivity(), R.style.MyDialog)
                            .setTitle(title)
                            .setMessage(R.string.newest_resource)
                            .setPositiveButton(R.string.ok, (dialog1, which) -> {
                                try {
                                    extractNotebookRes(path);
                                } catch (Exception e) {

                                }
                                dialog1.dismiss();
                            })
                            .create()
                            .show();

                    } else {
                        App.getDownloader().download(getString(R.string.download_notebook), url, path, new Downloader.Callback() {

                            @Override
                            public void pending(String name) {
                                mLoadingDialog.cancel();

                                new AlertDialog.Builder(getActivity(), R.style.MyDialog)
                                    .setTitle(R.string.notice)
                                    .setMessage(R.string.download_progress_need_sometime)
                                    .setPositiveButton(R.string.ok, (dialog1, which) -> {
                                    })
                                    .create()
                                    .show();
                            }

                            @Override
                            public void complete(String name, File installer) {

                                Log.d(TAG, "getNotebook:complete:" + name + "[" + installer.getAbsolutePath() + "]");
                                mLoadingDialog.dismiss();


                                NStorage.setSP(App.getContext(),KEY_RES, vercode);

                                // UNZIP resources && install
                                try {
                                    extractNotebookRes(installer.getAbsolutePath());
                                    Toast.makeText(App.getContext(), R.string.file_downloaded, Toast.LENGTH_SHORT).show();

                                } catch (Exception e) {

                                }
                            }

                            @Override
                            public void error(String err) {
                                mLoadingDialog.cancel();
                                try {
                                    Toast.makeText(getActivity(), err, Toast.LENGTH_SHORT).show();
                                } catch (Exception e) {

                                }
                            }
                        });
                    }
                } catch (JSONException e) {
                    e.printStackTrace();
                }
            }
            @Override
            public void onFailure(int statusCode, Header[] headers, String responseString, Throwable throwable) {
                // waitingWindow.dismiss();
                Log.d(TAG, "Error in checkConfUpdate:" + throwable.getMessage());
            }
        });

    }*/
    private void getQPYC(boolean ispy2compatible) {
        mLoadingDialog.show();

        String conf_url = (ispy2compatible?QPyConstants.QPYC2COMPATIBLE:QPyConstants.QPYC3)+"?"+NAction.getUserUrl(getActivity());

        QBaseApp.getInstance().getAsyncHttpClient().get(getActivity(), conf_url,
        null, new JsonHttpResponseHandler() {
                @Override
                public void onSuccess(int statusCode, Header[] headers, JSONObject result) {
                    final String KEY_RES = ispy2compatible?QPyConstants.QPYC2COMPATIBLE_VER_KEY:QPyConstants.QPYC3_VER_KEY;

                    try {

                        final String py_resource_ver = NStorage.getSP(getActivity(), KEY_RES);

                        final String url = result.getString("link");
                        final String target = result.getString("target");
                        final String vercode = result.getString("vercode");
                        final String title = result.getString("title");
                        final String vername = result.getString("vername");
                        final String path = CONF.PY_CACHE_PATH + "/" + target;

                        NStorage.setSP(App.getContext(), QPyConstants.KEY_PY3_RES, path);

                        Log.d(TAG, "getQPYC:onSuccess:"+py_resource_ver+"["+vercode+"]");

                        if (py_resource_ver.equals(vercode) && new File(path).exists()) {
                            mLoadingDialog.dismiss();

                            new AlertDialog.Builder(getActivity(), R.style.MyDialog)
                                .setTitle(title)
                                .setMessage(R.string.newest_resource)
                                .setPositiveButton(R.string.ok, (dialog1, which) -> {
                                    try {
                                        releaseQPycRes(path);
                                    } catch (Exception e) {

                                    }

                                    dialog1.dismiss();
                                })
                                .create()
                                .show();

                        } else {

                            App.getDownloader().download(getString(R.string.download_py), url, path, new Downloader.Callback() {

                                @Override
                                public void pending(String name) {
                                    mLoadingDialog.cancel();

                                    new AlertDialog.Builder(getActivity(), R.style.MyDialog)
                                            .setTitle(R.string.notice)
                                            .setMessage(R.string.download_progress_need_sometime)
                                            //.setNegativeButton(R.string.cancel, (dialog1, which) -> dialog1.dismiss())
                                            .setPositiveButton(R.string.ok, (dialog1, which) -> {
                                            })
                                            .create()
                                            .show();
                                }

                                @Override
                                public void complete(String name, File installer) {

                                    Log.d(TAG, "getQPYC:complete:" + name + "[" + installer.getAbsolutePath() + "]");
                                    mLoadingDialog.dismiss();

                                    NStorage.setSP(App.getContext(),KEY_RES, vercode);
                                    // UNZIP resources && install
                                    try {
                                        releaseQPycRes(installer.getAbsolutePath());

                                    } catch (Exception e) {

                                    }
                                }

                                @Override
                                public void error(String err) {
                                    mLoadingDialog.cancel();
                                    try {
                                        Toast.makeText(getActivity(), err, Toast.LENGTH_SHORT).show();
                                    } catch (Exception e) {

                                    }
                                }
                            });
                        }
                } catch (JSONException e) {
                    e.printStackTrace();
                }
            }
            @Override
            public void onFailure(int statusCode, Header[] headers, String responseString, Throwable throwable) {
                // waitingWindow.dismiss();
                Log.d(TAG, "Error in getQPYC:" + throwable.getMessage());
            }
        });
    }

    private boolean isQPycRelease(boolean ispy2compatible) {
        boolean isRelease = true;
        String[] py3Mp3File = getActivity().getResources().getStringArray(R.array.qpy3_zip);
        for (String s : py3Mp3File) {
            isRelease = isRelease && new File(CONF.PY_CACHE_PATH + "/" + s).exists();
        }
        return isRelease;
    }

    /*private void removeQPyc2Core() {
        Log.d(TAG, "removeQPyc2Core");
        String files = getActivity().getFilesDir().getAbsolutePath();
        String[] files2del = {files+"/lib/notebook.zip", files+"/lib/python27.zip", files+"/lib/python2.7"};
        for (int i=0;i<files2del.length;i++) {
            QPySDK.recursiveDelete(files2del[i]);
        }
    }*/

    /**
     * 应该在工作线程处理文件的解压释放
     */
    private void extractQPyCore(Boolean ispy2Compatible) {
        QPySDK qpySDK = new QPySDK(getActivity(), getActivity());
        File libFolder = new File(CONF.SCOPE_STORAGE_PATH, QPyConstants.PY_CACHE);
        String[] pyMp3File = getActivity().getResources().getStringArray(R.array.qpy3_zip);

        for (String s : pyMp3File) {

            File unzipFile = new File(libFolder, s);
            if (s.contains("public")) {
                File externalStorage = new File(CONF.SCOPE_STORAGE_PATH);

                qpySDK.extractRes(unzipFile, new File(externalStorage + "/lib"), false);

            } else {
                qpySDK.extractRes(unzipFile, getActivity().getFilesDir(), false);
            }

        }
    }

}