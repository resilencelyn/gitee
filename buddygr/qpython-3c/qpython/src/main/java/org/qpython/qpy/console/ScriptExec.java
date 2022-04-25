package org.qpython.qpy.console;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Environment;
import android.os.Message;
import android.preference.PreferenceManager;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v4.util.ArrayMap;
import android.support.v7.app.AlertDialog;
import android.text.TextUtils;
import android.util.Log;

import com.quseit.util.NAction;

import org.qpython.qpy.R;
import org.qpython.qpy.console.util.TermSettings;
import org.qpython.qpy.main.app.CONF;
import org.qpython.qpy.main.utils.Utils;
import org.qpython.qpysdk.Exec;
import org.qpython.qpysdk.QPyConstants;
import org.qpython.qpysdk.utils.FileHelper;
import org.qpython.qpysdk.utils.StreamGobbler;
import org.qpython.qsl4a.QPyScriptService;
import org.qpython.qsl4a.qsl4a.util.SPFUtils;

import java.io.File;
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Map;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class ScriptExec {

    private static final String TAG                 = "ScriptExec";
    private static final int    SCRIPT_CONSOLE_CODE = 1237;
    private static final int    PID_INIT_VALUE      = -1;
    private static final int    DEFAULT_BUFFER_SIZE = 8192;
    private static final int    LOG_NOTIFICATION_ID = (int) System.currentTimeMillis();
    private InputStream  mIn;
    private OutputStream mOut;
    private String logFile = CONF.ABSOLUTE_LOG;

    private ScriptExec() { }

    public static ScriptExec getInstance() {
        return ScriptExecHolder.INSTANCE;
    }

    public void playScript(Context context, String script, String arg, boolean notify) {

        // confirm the SL4A Service is started
        context.startService(new Intent(context, QPyScriptService.class));

        String header_512 = FileHelper.getFileContents(script,512);
        boolean isWeb   = header_512.contains("#qpy:webapp");
        boolean isQApp  = header_512.contains("#qpy:quiet");
        //boolean isKivy  = header_512.contains("#qpy:kivy");
        //boolean isGame  = header_512.contains("#qpy:pygame");
        boolean isDaemon = header_512.contains("#qpy:daemon");

        //Log.d(TAG, "playScript:" + script+"|isWeb:"+isWeb+"|isQapp:"+isQApp+"|isKivy:"+isKivy+"|isGame:"+isGame+"|isDaemon:"+isDaemon);

        if (isWeb) {
            playWebApp(context,  script, arg);
        } /*else if (isKivy) {
            playKScript(context, script, arg, notify);
        } */else if (isQApp) {
            playQScript(context, script, arg);
        } /*else if (isGame) {
            playGScript(context, script, arg, notify);
        } */else if (isDaemon) {
            playDScript(context, script, arg, notify);
        } else {
            playCScript(context, script, arg);
        }
    }

    /*
        Play project Without args
     */
    public void playProject(Context context, String project, boolean notify) {
        playProject(context, project, null, notify);
    }

    /*
        Play project with args
     */

    public void playProject(Context context, String project, String args, boolean notify) {
        Log.d(TAG, "playProject:" + project);

        String script = project + "/main.py";
        File sf = new File(script);
        if (sf.exists()) {

            playScript(context, script, args, notify);

        } else {

            new AlertDialog.Builder(context, R.style.MyDialog)
                    .setTitle(R.string.notice)
                    .setMessage(R.string.project_main_error)
                    .setPositiveButton(R.string.close, (dialog1, which) ->  dialog1.dismiss())
                    .create()
                    .show();
        }
    }

    public String[] getPyEnv(Context context, String path, String term, String pyPath) {
        //boolean isQPy3 =  NAction.isQPy3(context);

        String pyVer = CONF.pyVer;//QPyConstants.py3Ver;
        File filesDir = context.getFilesDir();
        File externalStorage = new File(CONF.SCOPE_STORAGE_PATH);
        File externalLegacyStorage = new File(QPyConstants.LEGACY_PATH);
        boolean needCustom = !CONF.CUSTOM_PATH.equals(CONF.LEGACY_PATH);
        String commDir = filesDir+"/lib/"+pyVer+"/";

        String[] env = new String[25 + (needCustom?1:0)];

        env[0] = "TERM=" + term;
        env[1] = "PATH=" + CONF.filesDir+"/bin"+":"+path;

        env[2] = "LD_LIBRARY_PATH=.:" + filesDir + "/lib/" + ":" + filesDir + "/";

        env[3] = "PYTHONHOME="+filesDir;
        env[4] = "ANDROID_PRIVATE="+filesDir;

        if (!externalStorage.exists()) externalStorage.mkdir();

            env[5] = "PYTHONPATH="
                    +commDir+":"
                    +commDir+"lib-dynload/:"
                    +commDir+"site-packages/:"
                    +externalStorage+"/lib/"+pyVer+"/site-packages/:"
                    +externalLegacyStorage+"/lib/"+pyVer+"/site-packages/";

            if(needCustom) {
                env[5] += ":" + CONF.CUSTOM_PATH + "/lib/" + pyVer + "/site-packages/";
                env[25] = "ANDROID_PUBLIC_CUSTOM=" + CONF.CUSTOM_PATH;
            }

            env[14] = "PYTHONSTARTUP="+commDir+"site-packages/qpy.py";

        env[6] = "PYTHONOPTIMIZE=1";

        File td = new File(externalStorage+"/cache");
        if (!td.exists()) td.mkdir();
        env[7] = "TMPDIR="+td.getAbsolutePath();


        env[8] = "AP_HOST="+SPFUtils.getSP(context, "sl4a.hostname");
        env[9] = "AP_PORT="+SPFUtils.getSP(context, "sl4a.port");
        env[10] = "AP_HANDSHAKE="+SPFUtils.getSP(context, "sl4a.secue");

        env[11] = "ANDROID_PUBLIC="+externalStorage;
        env[12] = "ANDROID_PRIVATE="+filesDir;
        env[13] = "ANDROID_ARGUMENT=\""+pyPath+"\"";

        env[15] = "QPY_USERNO="+ NAction.getUserNoId(context);
        env[16] = "QPY_ARGUMENT="+NAction.getExtConf(context);
        env[17] = "PYTHONDONTWRITEBYTECODE=1";
        env[18] = "TMP="+externalStorage+"/cache";
        env[19] = "ANDROID_PUBLIC_LEGACY="+externalLegacyStorage+"";
        env[20] = "LANG="+context.getString(R.string.lang_env)+".UTF-8";
        env[21] = "HOME="+filesDir;
        env[22] = "ANDROID_DATA="+System.getenv("ANDROID_DATA");
        env[23] = "ANDROID_ROOT="+System.getenv("ANDROID_ROOT");
        env[24] = "EXTERNAL_STORAGE="+Environment.getExternalStorageDirectory();
        return env;
    }

    public String getPyBin(Context context, boolean bin) {
        String scmd = "";
        //String v = NAction.isQPy3(context) ? "3" : "";
        boolean isRootEnable = NAction.isRootEnable(context);
            if (bin) {
                scmd = CONF.pytho;
            } else {
                if (isRootEnable) {
                    scmd = CONF.qpyshr;
                } else {
                    scmd = CONF.qpysh;
                }
            }
        return scmd;
    }

    private void createWebTermSession(Context context, String[] mArgs) {
        SharedPreferences mPrefs = PreferenceManager.getDefaultSharedPreferences(context);
        TermSettings mSettings = new TermSettings(context.getResources(), mPrefs);

        String scmd = getPyBin(context, false);

        ShellTermSession session = createTermSession(context, mSettings, scmd + " \"" + mArgs[0] + "\"  " + mArgs[1] + " > \"" + mArgs[2] + "\" 2>&1", "");

        session.setFinishCallback(null);
        session.shellRun();
    }

    protected static ShellTermSession createTermSession(Context context, TermSettings settings, String initialCommand, String path) {
        ShellTermSession session = null;
        try {
            session = new ShellTermSession(context, settings, initialCommand, path);
            session.setProcessExitMessage(context.getString(R.string.process_exit_message));

        } catch (IOException e) {
            e.printStackTrace();
        }

        return session;
    }


    /*
    Run WEBAPP
     */
    private void playWebApp(Context context, String script, String argv) {
        String header_512 = FileHelper.getFileContents(script, 512);

        boolean isNoHead = header_512.contains("#qpy:fullscreen");
        boolean isDrawer = header_512.contains("#qpy:drawer");

        String title = "QWebApp";
        Pattern titlePattern = Pattern.compile("#qpy:webapp:(.+)", Pattern.CASE_INSENSITIVE);
        Matcher matcher0 = titlePattern.matcher(header_512);
        if (matcher0.find()) {
            title = matcher0.group(1);
        }

        String srv = "http://localhost";
        Pattern srvPattern = Pattern.compile("#qpy://(.+)[\\s]+", Pattern.CASE_INSENSITIVE);
        Matcher matcher1 = srvPattern.matcher(header_512);

        if (matcher1.find()) srv = "http://" + matcher1.group(1);
        logFile = Utils.getWebLog(srv,script);
        Utils.backTaskNotify(context);

        Utils.logFile[0] = logFile;
        playDScript(context, script, argv, false);
        Utils.startWebActivityWithUrl(context, title, srv, script, isNoHead, isDrawer);
//        QWebViewActivity.start(context, "main", title, srv, script);
    }


    /*
        Run KIVY Script
     * /
    public void playKScript(Context context, final String script, String argv, boolean notify) {
        if (Utils.isOpenGL2supported(context)) {
            File scriptParent = new File(script).getParentFile();
            String proj, log;
            log = QPyConstants.ABSOLUTE_LOG;

            if (scriptParent.getName().startsWith("scripts")) {
                proj = new File(script).getName();
            } else {
                proj = scriptParent.getName();
            }
            Intent intent = new Intent(context, PythonSDLActivity.class);
            intent.setData(Uri.fromFile(new File(script)));
            context.startActivity(intent);
            LogDialog event = new LogDialog(proj, log);
            EventBus.getDefault().post(event);

        } else {

            new AlertDialog.Builder(context, R.style.MyDialog)
                    .setTitle(R.string.notice)
                    .setMessage(R.string.open_gl2_is_not_supported)
                    .setPositiveButton(R.string.close, (dialog1, which) ->  dialog1.dismiss())
                    .create()
                    .show();

        }
    }*/

    /*
        Run Game Script
     * /
    public void playGScript(Context context, final String script, String argv, boolean notify) {
        playKScript(context,script,argv,notify);
    }*/


    /*
        Run Console Script
     */
    public void playCScript(Context context, final String scriptPath, String arg) {
        String[] args;
        if (TextUtils.isEmpty(arg)) {
            args = new String[]{scriptPath, scriptPath};
        } else {
            args = new String[]{scriptPath, scriptPath, arg};
        }
        execPyInConsole((Activity)context, args);
    }

    /*
        Run Daemon Script
     */

    public void playDScript(Context context, final String scriptPath, String argv1, boolean notify) {
        //String logFile = getLastLog();

        String[] mArgs = {scriptPath, " " + (argv1 != null ? argv1 : ""), logFile};

        Log.d(TAG, "playDScript:" + scriptPath + ":argv1:" + argv1 + " >" + logFile + " 2>&1");
        createWebTermSession(context, mArgs);
    }

    /*
        Run Quiet Script
     */
    FileDescriptor mFd;
    public int playQScript(Context context, final String script, String argv) {
        ArrayList<String> mArguments = new ArrayList<>();

        String binaryPath = getPyBin(context, true);

        Log.d(TAG, "playQScript:" + binaryPath + "|" + script + "|" + argv);

        File f = new File(script);

        int[] pid = new int[1];

        mArguments.add(script);
        mArguments.add(argv);

        String[] argumentsArray = mArguments.toArray(new String[mArguments.size()]);

        logFile = Utils.getQuietLog(script);
        Utils.backTaskNotify(context);

        final File mLog = new File(logFile);

        mFd = Exec.createSubprocess(binaryPath, argumentsArray, getPyEnv(context, System.getenv("PATH"), System.getenv("TERM"),f.getParentFile() + ""), Environment.getExternalStorageDirectory() + "/", pid);
        final AtomicInteger mPid = new AtomicInteger(PID_INIT_VALUE);

        mPid.set(pid[0]);
        mOut = new FileOutputStream(mFd);
        mIn = new StreamGobbler(new FileInputStream(mFd), mLog, DEFAULT_BUFFER_SIZE);
        long mStartTime = System.currentTimeMillis();

        new Thread(() -> {
            int returnValue = Exec.waitFor(mPid.get());

            //long mEndTime = System.currentTimeMillis();
            int pid1 = mPid.getAndSet(PID_INIT_VALUE);
            Log.d("", "out:" + mFd.out.toString());

            Message msg = new Message();
            msg.what = returnValue;
            msg.obj = mArguments.get(0);

            Log.d(TAG, "Process " + pid1 + " exited with result code " + returnValue + ".");

            try {
                mIn.close();
            } catch (IOException e) {
                Log.e(TAG, e.getMessage());
            }

            try {
                mOut.close();
            } catch (IOException e) {
                Log.e(TAG, e.getMessage());
            }
            //context.updateNotify(msg);
        }).start();

        return mPid.getAndSet(PID_INIT_VALUE);
    }
    

    public void execPyInConsole(Activity context, String[] args) {
        Intent intent = new Intent(context, TermActivity.class);
        intent.putExtra(TermActivity.ARGS, args);
        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        context.startActivityForResult(intent, SCRIPT_CONSOLE_CODE);
    }


/*    public void sendLogNotifiy(Context context, String filePath) {
        if (!PreferenceManager.getDefaultSharedPreferences(context).getBoolean(context.getString(R.string.key_hide_noti), true)) {
            return;
        }
        String title = FileHelper.getFileName(filePath);

        Intent resultIntent = new Intent(context, LogActivity.class);
        if (filePath.contains("/scripts/")) {
            String proj = new File(filePath).getName();

            resultIntent.putExtra(LogActivity.LOG_PATH, filePath.replace(title, "last.log"));
            resultIntent.putExtra(LogActivity.LOG_TITLE, proj);
        } else {
            String proj = new File(filePath).getParentFile().getName();
            resultIntent.putExtra(LogActivity.LOG_PATH, filePath.replace("main.py", "last.log"));
            resultIntent.putExtra(LogActivity.LOG_TITLE, proj);
        }

        PendingIntent pendingIntent = PendingIntent.getActivity(context, 0, resultIntent, PendingIntent.FLAG_ONE_SHOT);

//        PendingIntent resultPendingIntent = PendingIntent.getActivity(
//                this, 0, resultIntent, 0);

        Notification.Builder builder = new Notification.Builder(context)
                .setSmallIcon(R.drawable.img_home_logo)
                .setContentTitle(context.getString(R.string.app_name))
                .setContentText(context.getString(R.string.runtime_log, title))
                .setContentIntent(pendingIntent)
                .setAutoCancel(true);

        Notification notification;
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.JELLY_BEAN) {
            notification = builder.build();
        } else {
            notification = builder.getNotification();
        }
        NotificationManager mNotifyMgr = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);
        mNotifyMgr.notify(LOG_NOTIFICATION_ID, notification);
    }
*/
    public final void checkPermissionDo(Context context, String[] permissions, PermissionAction action) {
        Map<Integer, PermissionAction> mActionMap = new ArrayMap<>();
        if (Build.VERSION.SDK_INT >= 23) {
            boolean granted = true;
            for (int i = 0; i < permissions.length; i++) {
                String permission = permissions[i];
                int checkPermission = ContextCompat.checkSelfPermission(context, permission);
                if (checkPermission != PackageManager.PERMISSION_GRANTED) {
                    granted = false;

                } else {
                    granted = true;
                }
            }
            if (!granted) {
                int code = permissions.hashCode() & 0xffff;
                mActionMap.put(code, action);
                ActivityCompat.requestPermissions((Activity) context, permissions, code);
            } else {
                action.onGrant();
            }
        } else {
            action.onGrant();
        }
    }


    public interface PermissionAction {
        void onGrant();

        void onDeny();
    }

    private static class ScriptExecHolder {
        private static final ScriptExec INSTANCE = new ScriptExec();
    }

    public static class LogDialog {
        public LogDialog(String title, String path) {
            Log.e("EditorActivity", "LogUtil dialog event init");
            this.title = title;
            this.path = path;
        }

        public String title;
        public String path;
    }
}
