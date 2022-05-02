package org.qpython.qpysdk;

import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Environment;
import android.os.Message;
import android.util.Log;
import android.widget.Toast;

import org.qpython.qpysdk.utils.AssetExtract;
import org.qpython.qpysdk.utils.FileExtract;
import org.qpython.qpysdk.utils.FileUtils;
import org.qpython.qpysdk.utils.ResourceManager;
import org.qpython.qpysdk.utils.StreamGobbler;

import java.io.File;
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * Created by yhc on 16/2/22.
 */
public class QPySDK {

    private static final int    PID_INIT_VALUE      = -1;
    private static final int    DEFAULT_BUFFER_SIZE = 8192;
    private final        String TAG                 = "QPySDK";
    Context  context;
    Activity activity;
    ArrayList<String> mArguments = new ArrayList<String>();
    InputStream    mIn;
    OutputStream   mOut;
    FileDescriptor mFd;
    private ResourceManager resourceManager;

    public QPySDK(Context con, Activity act) {
        resourceManager = new ResourceManager(act);
        activity = act;
        context = con;
    }

    public static void  recursiveDelete(String path) {
        _recursiveDelete(new File(path));
    }
    static void _recursiveDelete(File f) {
        if (f.exists()) {
            if (f.isDirectory()) {
                if (f.listFiles() != null) {
                    for (File r : f.listFiles()) {
                        if (!f.getName().equals(".") && !f.getName().equals("..")) {
                            _recursiveDelete(r);
                        }
                    }
                }
            }
            f.delete();
        }
    }


    public void runPyService() {
        Log.d(TAG, "runPyService");
        String launchScript = this.context.getFilesDir() + "/main.py";
        playQScript(launchScript);
    }

    public void playQScript(final String script) {
        Log.d(TAG, "playQScript:" + script);
        String binaryPath = "";

        File f = new File(script);

        binaryPath = this.context.getFilesDir() + "/bin/python";

        int[] pid = new int[1];

        mArguments.add(script);
        String[] argumentsArray = mArguments.toArray(new String[mArguments.size()]);

        final File mLog = new File(String.format("%s", QPyConstants.LEGACY_LOG));
        File logDir = mLog.getParentFile();

        mFd = Exec.createSubprocess(binaryPath, argumentsArray, getEnvironmentArray(f.getParentFile() + ""), Environment.getExternalStorageDirectory() + "/", pid);
        final AtomicInteger mPid = new AtomicInteger(PID_INIT_VALUE);

        mPid.set(pid[0]);
        mOut = new FileOutputStream(mFd);
        mIn = new StreamGobbler(new FileInputStream(mFd), mLog, DEFAULT_BUFFER_SIZE);
        long mStartTime = System.currentTimeMillis();


        new Thread(new Runnable() {
            public void run() {
                int returnValue = Exec.waitFor(mPid.get());
                //long mEndTime = System.currentTimeMillis();
                int pid = mPid.getAndSet(PID_INIT_VALUE);
                Log.d("", "out:" + mFd.out.toString());

                Message msg = new Message();
                msg.what = returnValue;
                msg.obj = mArguments.get(0);

                Log.d(TAG, "Process " + pid + " exited with result code " + returnValue + ".");

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

            }
        }).start();
    }

    private String[] getEnvironmentArray(String pyPath) {
        String path = System.getenv("PATH");

        File filesDir = this.context.getFilesDir();

        List<String> environmentVariables = new ArrayList<String>();

        environmentVariables.add("PATH=" + filesDir + "/bin" + ":" + path);
        environmentVariables.add("LD_LIBRARY_PATH=" + ".:" + filesDir + "/lib/" + ":" + filesDir + "/:" + filesDir.getParentFile() + "/lib/");
        environmentVariables.add("PYTHONHOME=" + filesDir);
        environmentVariables.add("ANDROID_PRIVATE=" + filesDir);

        File externalStorage = new File(Environment.getExternalStorageDirectory(), "org.qpython.qpy");

        /*environmentVariables.add("PYTHONPATH=" + externalStorage + "/lib/python2.7/site-packages/:"
                + filesDir + "/lib/python2.7/site-packages/:"
                + filesDir + "/lib/python2.7/:"
                + filesDir + "/lib/python27.zip:"
                + filesDir + "/lib/python2.7/lib-dynload/:"
                + pyPath);*/

        //environmentVariables.add("PYTHONSTARTUP=" + externalStorage + "/lib/python2.7/site-packages/qpythoninit.py");
        environmentVariables.add("PYTHONOPTIMIZE=1");

        environmentVariables.add("TMPDIR=" + externalStorage + "/cache");
        environmentVariables.add("TEMP=" + externalStorage + "/cache");

        File td = new File(externalStorage + "/cache");
        if (!td.exists()) {
            td.mkdirs();
        }

        environmentVariables.add("ANDROID_PUBLIC=" + externalStorage);
        environmentVariables.add("ANDROID_PRIVATE=" + this.context.getFilesDir().getAbsolutePath());
        environmentVariables.add("ANDROID_ARGUMENT=" + pyPath);
        environmentVariables.add("PYTHONDONTWRITEBYTECODE=1");

        for (Map.Entry<String, String> entry : System.getenv().entrySet()) {
            environmentVariables.add(entry.getKey() + "=" + entry.getValue());
        }
        String[] environment = environmentVariables.toArray(new String[environmentVariables.size()]);
        return environment;
    }

    public String getVersion(){
        try {
            return activity.getPackageManager().getPackageInfo(activity.getPackageName(),0).versionName;
        } catch (PackageManager.NameNotFoundException e) {
            return "0";
        }
    }

    public String readDiskVersion(String filename){
        String disk_version = "0";
        try {
            byte buf[] = new byte[64];
            InputStream is = new FileInputStream(filename);
            int len = is.read(buf);
            disk_version = new String(buf, 0, len);
            is.close();
            return disk_version;
        } catch (Exception e) {
            return "0";
        }
    }

    public void writeDataVersion(String diskVerFn,String dataVer) throws IOException {
        // Write version file.
        FileOutputStream os = new FileOutputStream(diskVerFn);
        os.write(dataVer.getBytes());
        os.close();
    }

    public boolean needUpdateRes(){
        String fn = context.getFilesDir()+"/text/ver/qpython";
        if (readDiskVersion(fn).equals(getVersion())){
            return false;
        } else {
            return true;
        }
    }

    public String[] getPyVer(){
        String verPath = context.getFilesDir() + "/text/ver/python";
        String[] pyVer = new String[2];
        pyVer[1] = readDiskVersion(verPath);
        pyVer[0] = "python"+pyVer[1].substring(0, pyVer[1].lastIndexOf("."));
        pyVer[1] = "Python"+pyVer[1];
        return pyVer;
    }


    public void extractRes(File file, File target, boolean forceExtrac) {
        String fileName = file.getName().startsWith(".") ? file.getName().substring(1, file.getName().length()) : file.getName();
        fileName = fileName.substring(0, !file.getName().contains(".") ? file.getName().length() : file.getName().indexOf("."));

        String data_version = getVersion();
        // resourceManager.getString(fileName + "_version");
        /*if (data_version==null) {
            data_version = "0";
        }*/

        //Log.d(TAG, "extractRes:"+fileName+"["+data_version+"]"+"["+disk_version+"]");

        String disk_version_fn = target.getAbsolutePath() + "/" + fileName + ".version";
        /*try {
            byte buf[] = new byte[64];
            InputStream is = new FileInputStream(disk_version_fn);
            int len = is.read(buf);
            disk_version = new String(buf, 0, len);
            is.close();
        } catch (FileNotFoundException e) {
            disk_version = "0";
        } catch (IOException e) {
            disk_version = "0";
        }*/
        String disk_version = readDiskVersion(disk_version_fn);

        if (!data_version.equals(disk_version) || disk_version.equals("0") || forceExtrac) {
            target.mkdirs();
            if (!new FileExtract().extractTar(file, target.getAbsolutePath())) {
                Log.d(TAG,"Could not extract " + fileName + " data.");
            }
        }

        chmodX();
    }

    public void extractRes(final String resource, File target, boolean force) {
        // The version of data in memory and on disk.
        String data_version = getVersion();

        //LogUtil.d(TAG, "data_version:"+data_version+"-"+resource + "_version"+"-"+resourceManager);
        // If no version, no unpacking is necessary.
        /*f (data_version == null) {
            return;
        }*/

        // Check the current disk version, if any.
        String filesDir = target.getAbsolutePath();
        String disk_version_fn = filesDir + "/" + resource + ".version";

        String disk_version = readDiskVersion(disk_version_fn);

        //LogUtil.d(TAG, "data_version:"+Math.round(Double.parseDouble(data_version))+"-disk_version:"+Math.round(Double.parseDouble(disk_version))+"-RET:"+(int)(Double.parseDouble(data_version)-Double.parseDouble(disk_version)));
        if ((!data_version.equals(disk_version) || disk_version.equals("0"))
                || force) {
            //Log.v(TAG, "Extracting " + resource + " assets.");

            //recursiveDelete(target);
            target.mkdirs();

            AssetExtract ae = new AssetExtract(this.activity);
            boolean ret = ae.extractTar(resource + ".mp3", target.getAbsolutePath());
            if (!ret) {
                //Toast.makeText(this.context, "Could not extract " + resource + " data, please reinstall and make sure your device have enough space", Toast.LENGTH_SHORT).show();
            } else {

                try {
                /*if (resource.equals("private")) {
                    Toast.makeText(getApplicationContext(), R.string.first_load, Toast.LENGTH_SHORT).show();
            	}*/
                    // Write .nomedia.
                    new File(target, ".nomedia").createNewFile();

                    /*/ Write version file.
                    FileOutputStream os = new FileOutputStream(disk_version_fn);
                    os.write(data_version.getBytes());
                    os.close();*/
                    writeDataVersion(disk_version_fn,data_version);
                } catch (Exception e) {
                    Log.w("python", e);
                    Toast.makeText(this.context, "Could not extract " + resource + " data, make sure your device have enough space.", Toast.LENGTH_LONG).show();
                }
            }
        } else {
            Log.d(TAG, "NO EXTRACT");

        }
        if (resource.startsWith("resource")) {
            chmodX();
        }
    }

    public void extractRes(final String resource, File target) {
        extractRes( resource, target, false);
    }


    /*public void extractRes2(final String resource, File target) {
        Log.d(TAG, "extractRes:" + target);
        // The version of data in memory and on disk.
        String data_version = getVersion();

        // If no version, no unpacking is necessary.
        / *if (data_version == null) {
            return;
        }* /

        // Check the current disk version, if any.
        String filesDir = target.getAbsolutePath();
        String disk_version_fn = filesDir + "/" + resource + ".version";

        String disk_version = readDiskVersion(disk_version_fn);

        // If the disk data is out of date, extract it and write the
        // version file.
        if (!data_version.equals(disk_version)) {
            Log.v("python", "Extracting " + resource + " assets.");

            //_recursiveDelete(target);
            target.mkdirs();

            AssetExtract ae = new AssetExtract(activity);
            if (!ae.extractTar(resource + ".mp3", target.getAbsolutePath())) {
                //toastError("Could not extract " + resource + " data.");
//            if (!ae.copyFromAssetsToInternalStorage(resource + ".zip")) {
            } else {

                try {
                    new File(target, ".nomedia").createNewFile();

                    // Write version file.
                    / *FileOutputStream os = new FileOutputStream(disk_version_fn);
                    os.write(data_version.getBytes());
                    os.close();* /
                    writeDataVersion(disk_version_fn,data_version);
                } catch (Exception e) {
                    Log.w("unpackData", e);
                }
            }
        }

        if (resource.startsWith("private") || resource.startsWith("notebook")) {
            chmodX();
        }
    }*/

    private void chmodX () {
        File bind = new File(this.context.getFilesDir() , "bin");
        File[] fileList = bind.listFiles();
        if (fileList != null) {
            for (File bin : fileList) {
                try {
                    //LogUtil.d(TAG, "chmod:"+bin.getAbsolutePath());
                    FileUtils.chmod(bin, 0777);
                } catch (Exception e1) {
                    e1.printStackTrace();
                }
            }
        }
    }
}