package org.qpython.qpy.main.fragment;

import static com.quseit.util.FolderUtils.sortTypeByName;

import android.databinding.DataBindingUtil;
import android.graphics.Color;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v7.app.AlertDialog;
import android.support.v7.widget.LinearLayoutManager;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;

import com.quseit.util.FileHelper;
import com.quseit.util.ImageUtil;
import com.yanzhenjie.recyclerview.swipe.SwipeMenuCreator;
import com.yanzhenjie.recyclerview.swipe.SwipeMenuItem;

import org.qpython.qpy.R;
import org.qpython.qpy.databinding.FragmentExplorerBinding;
import org.qpython.qpy.main.app.CONF;
import org.qpython.qpy.texteditor.EditorActivity;
import org.qpython.qpy.texteditor.common.CommonEnums;
import org.qpython.qpy.texteditor.common.RecentFiles;
import org.qpython.qpy.texteditor.ui.adapter.FolderAdapter;
import org.qpython.qpy.texteditor.ui.adapter.bean.FolderBean;
import org.qpython.qpy.texteditor.ui.view.EnterDialog;
import org.qpython.qpy.texteditor.widget.crouton.Crouton;
import org.qpython.qpy.texteditor.widget.crouton.Style;
import org.qpython.qpy.utils.FileUtils;
import org.qpython.qpysdk.QPyConstants;

import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;

public class ExplorerFragment extends Fragment {
    private static final int REQUEST_SAVE_AS   = 107;
    private static final int REQUEST_HOME_PAGE = 109;
    private static final int REQUEST_RECENT    = 111;
    public static final int REQUEST_CUSTOM_FOLDER = 113;

    private static final String TYPE = "type";
    private static final String FOLDER_NAME = "folder_name";
    private static final String STORAGE_FOLDER = "/storage";
    private static final String MNT_FOLDER = "/mnt";
    //private static final String EMULATED_FOLDER = STORAGE_FOLDER + "/emulated";
    private static final String SDCARD = "/sdcard";
    private static String ANDROID_DATA;
    private static String TOP_FOLDER;
    private static final String EXT_PATH = Environment.getExternalStorageDirectory().toString();
    private static boolean needCustom;

    private int WIDTH = (int) ImageUtil.dp2px(60);

    private FragmentExplorerBinding binding;
    private List<FolderBean>        folderList;
    private FolderAdapter           adapter;
    private Map<String, Boolean> cloudedMap = new HashMap<>();

    private boolean openable = true; // 是否可打开文件

    private int    type;
    private String curPath;

    public static ExplorerFragment newInstance(int type) {
        ExplorerFragment myFragment = new ExplorerFragment();

        Bundle args = new Bundle();
        getTopFolder();
        args.putInt(TYPE, type);
        myFragment.setArguments(args);

        return myFragment;
    }

    public static ExplorerFragment newInstance(int type, String folder_name) {
        ExplorerFragment myFragment = new ExplorerFragment();

        Bundle args = new Bundle();
        getTopFolder();
        args.putInt(TYPE, type);
        args.putString(FOLDER_NAME, folder_name);
        myFragment.setArguments(args);

        return myFragment;
    }

    private static void getTopFolder(){
        ANDROID_DATA = "/data/" + CONF.packageName;
        if (Build.VERSION.SDK_INT>=30)
            TOP_FOLDER = MNT_FOLDER;
        else TOP_FOLDER = STORAGE_FOLDER;
        needCustom = !CONF.CUSTOM_PATH.equals(CONF.LEGACY_PATH);
    }

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_explorer, null);
    }

    @Override
    public void onViewCreated(View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        binding = DataBindingUtil.bind(view);
        type = getArguments().getInt(TYPE);
        if (type == REQUEST_CUSTOM_FOLDER) {
            curPath = getArguments().getString(FOLDER_NAME);
        }
        initView();
        initListener();
        //initCloud();
        switch (type) {
            case REQUEST_RECENT:
                binding.rlPath.setVisibility(View.GONE);
                binding.prevFolder.setVisibility(View.GONE);
                break;
            case REQUEST_SAVE_AS:
                binding.ivNewFolder.setVisibility(View.VISIBLE);
                break;
            case REQUEST_HOME_PAGE:
                binding.ivNewFolder.setVisibility(View.VISIBLE);
                break;
        }
    }

    private void initView() {
        SwipeMenuCreator swipeMenuCreator = (leftMenu, rightMenu, viewType) -> {
//            SwipeMenuItem uploadItem = new SwipeMenuItem(getContext())
//                    .setBackgroundColor(Color.parseColor("#FF4798F3"))
//                    .setImage(R.drawable.ic_cloud_upload)
//                    .setHeight(ViewGroup.LayoutParams.MATCH_PARENT)
//                    .setWidth(WIDTH);

            SwipeMenuItem renameItem = new SwipeMenuItem(getContext())
                    .setBackgroundColor(Color.parseColor("#FF4BAC07"))
                    .setImage(R.drawable.ic_file_rename)
                    .setHeight(ViewGroup.LayoutParams.MATCH_PARENT)
                    .setWidth(WIDTH);

            SwipeMenuItem deleteItem = new SwipeMenuItem(getContext())
                    .setBackgroundColor(Color.parseColor("#FFD14136"))
                    .setImage(R.drawable.ic_editor_filetree_close)
                    .setHeight(ViewGroup.LayoutParams.MATCH_PARENT)
                    .setWidth(WIDTH);

            switch (type) {
                case REQUEST_RECENT:
                    rightMenu.addMenuItem(deleteItem);
                    break;
                case REQUEST_SAVE_AS:
                    rightMenu.addMenuItem(deleteItem);
                    break;
                case REQUEST_HOME_PAGE:
                case REQUEST_CUSTOM_FOLDER:
                    //rightMenu.addMenuItem(uploadItem);
                    rightMenu.addMenuItem(renameItem);
                    rightMenu.addMenuItem(deleteItem);
                    break;
            }
        };

        folderList = new ArrayList<>();
        adapter = new FolderAdapter(folderList, getArguments().getInt(TYPE) == REQUEST_RECENT);
        adapter.setCloudMap(cloudedMap);
        binding.swipeList.setLayoutManager(new LinearLayoutManager(getContext()));
        binding.swipeList.setSwipeMenuCreator(swipeMenuCreator);
        if (type != REQUEST_CUSTOM_FOLDER) {
            openDir(CONF.SCOPE_STORAGE_PATH);
        } else {
            openDir(curPath);
        }
    }

    private String getParentPath(String curPath) {
        String parentPath;
        if(curPath.equals(EXT_PATH) || curPath.equals(SDCARD)){
            parentPath = TOP_FOLDER;
        } else {
            parentPath = new File(curPath).getParent();
        }
        return parentPath;
    }

    private void initListener() {
        binding.ivNewFolder.setOnClickListener(v -> doNewDir());
        binding.prevFolder.setOnClickListener(v -> {
            String parentPath = getParentPath(curPath);
            openDir(parentPath);
            /*if (parentPath.length()>=Environment.getExternalStorageDirectory().getAbsolutePath().length()) {
                openDir(parentPath);
            }*/
        });
        binding.swipeList.setSwipeMenuItemClickListener(menuBridge -> {
            binding.swipeList.smoothCloseMenu();
            switch (menuBridge.getPosition()) {
                case 0:
                    renameFile(menuBridge.getAdapterPosition());
                    break;
                case 1:
                    deleteFile(menuBridge.getAdapterPosition());
                    break;
            }
        });
        adapter.setClickListener(new FolderAdapter.Click() {
            @Override
            public void onItemClick(int position) {
                FolderBean item = folderList.get(position);
                if (item.getType().equals(CommonEnums.FileType.FILE)) {
                    if (!openable) {
                        Toast.makeText(getActivity(), R.string.cant_open, Toast.LENGTH_SHORT).show();
                        return;
                    }
                    //判断文件类型
                    int lastDot = item.getName().lastIndexOf(".");
                    if (lastDot != -1) {
                        String ext = item.getName().substring(lastDot + 1);
                        openFile(item.getFile(), ext);
                    }
                } else {
                    openDir(item.getPath());
                }
            }

            @Override
            public void onLongClick(int position) {
                binding.swipeList.smoothOpenRightMenu(position);
            }
        });

        binding.swipeList.setAdapter(adapter);
    }

    private void gotoSetting() {
        org.qpython.qpy.main.activity.SettingActivity.startActivity(getActivity());
    }
    private void openFile(File file, String ext) {
        List<String> textExts = Arrays.asList(getContext().getResources().getStringArray(R.array.text_ext));
        if (textExts.contains(ext)) {
            EditorActivity.start(getContext(), Uri.fromFile(file));
        } /*else if (ext.equals("ipynb")) {
            boolean notebookenable = NotebookUtil.isNotebookEnable(getActivity());
            if (notebookenable) {
                NotebookActivity.start(getActivity(), file.getAbsolutePath(), false);
            } else {

                new AlertDialog.Builder(getActivity(),R.style.MyDialog)
                        .setTitle(R.string.dialog_alert)
                        .setMessage(getString(R.string.ennable_notebook_first))
                        .setNegativeButton(R.string.cancel, (dialog1, which) -> dialog1.dismiss())
                        .setPositiveButton(R.string.ok, (dialog1, which) -> gotoSetting())
                        .create()
                        .show();

                //Toast.makeText(getActivity(), R.string.ennable_notebook_first, Toast.LENGTH_SHORT).show();
            }
        }*/ else {
            FileUtils.openFile(getContext(), file);
        }
    }

    private void uploadFile(int adapterPosition) {
        File file = folderList.get(adapterPosition).getFile();

        // only support type in <R.array.support_file_ext>
        String ext = "";
        if (file.getName().lastIndexOf(".") > 0) {
            ext = file.getName().substring(file.getName().lastIndexOf(".") + 1);
        }
        boolean isSupport = false;
        if (!file.isDirectory()) {
            for (String s : getResources().getStringArray(R.array.support_file_ext)) {
                if (s.equals(ext)) {
                    isSupport = true;
                    break;
                }
            }
        } else {
            isSupport = true;
        }

        if (!isSupport) {
            Toast.makeText(getContext(), R.string.not_support_type_hint, Toast.LENGTH_SHORT).show();
            return;
        }

        /* only available for already login user
        if (App.getUser() == null) {
            new AlertDialog.Builder(getActivity(), R.style.MyDialog)
                    .setTitle(R.string.need_login)
                    .setMessage(R.string.upload_login_hint)
                    .setNegativeButton(R.string.no, null)
                    .setPositiveButton(getString(R.string.login_now), (dialog, which) -> {
                        //startActivityForResult(new Intent(getActivity(), SignInActivity.class), LOGIN_REQUEST)
                    })
                    .create()
                    .show();
            return;
        }

        // upload

        File folder = folderList.get(adapterPosition).getFile();
        folderList.get(adapterPosition).setUploading(true);
        adapter.notifyItemChanged(adapterPosition);
        if (folder.isDirectory()) {
            ShareCodeUtil.getInstance().uploadFolder(folder, callback, 0);
        } else {
            ShareCodeUtil.getInstance().uploadFile(folder, callback);
        }*/
    }

    private void renameFile(int adapterPosition) {
        new EnterDialog(getContext())
            .setTitle(getString(R.string.rename))
            .setConfirmListener(name -> {
                File oldFile = folderList.get(adapterPosition).getFile();
                File newFile = new File(oldFile.getParent(), name);
                boolean renameSuc = oldFile.renameTo(newFile);
                if (renameSuc) {
                    folderList.set(adapterPosition, new FolderBean(newFile));
                    adapter.notifyItemChanged(adapterPosition);
                    return true;
                } else {
                    Toast.makeText(getActivity(), R.string.rename_fail, Toast.LENGTH_SHORT).show();
                    return false;
                }
            })
            .setText(folderList.get(adapterPosition).getName())
            .show();
    }

    private void deleteFile(int adapterPosition) {
        switch (type) {
            case REQUEST_RECENT:
                RecentFiles.removePath(folderList.get(adapterPosition).getPath());
                folderList.remove(adapterPosition);
                adapter.notifyDataSetChanged();
                break;
            case REQUEST_HOME_PAGE:
            case REQUEST_SAVE_AS:
                AlertDialog.Builder builder = new AlertDialog.Builder(getActivity(), R.style.MyDialog);
                builder.setTitle(R.string.warning)
                        .setMessage(R.string.delete_file_hint)
                        .setNegativeButton(R.string.no, null)
                        .setPositiveButton(R.string.yes, (dialog, which) -> {
                            FileHelper.clearDir(folderList.get(adapterPosition).getFile().getAbsolutePath(), 0, true);
                            folderList.remove(adapterPosition);
                            adapter.notifyItemRemoved(adapterPosition);
                            adapter.notifyDataSetChanged();
                        })
                        .show();
                break;
        }
    }

    private void openDir(String dirPath) {
        /*if (dirPath.equals(EMULATED_FOLDER)){
            dirPath = SDCARD;
        }*/
        if (type == REQUEST_RECENT) {
            folderList.clear();
            for (String path : RecentFiles.getRecentFiles()) {
                folderList.add(new FolderBean(new File(path)));
            }

            if (folderList.size() == 0) {
                binding.emptyHint.setVisibility(View.VISIBLE);
            } else {
                binding.emptyHint.setVisibility(View.GONE);
                adapter.notifyDataSetChanged();
            }
        } else {
            File dir = new File(dirPath);
            if (dir.exists()) {
                File[] files = dir.listFiles();
                if (files != null) {
                    Arrays.sort(files, sortTypeByName);
                    folderList.clear();
                    if (dirPath.equals(TOP_FOLDER)) {
                        folderList.add(new FolderBean(new File(SDCARD), ""));
                    } else if (dirPath.endsWith("/Android") && new File(dirPath,"data").exists())
                        folderList.add(new FolderBean(new File(dirPath,ANDROID_DATA),""));
                    else if (dirPath.endsWith(ANDROID_DATA)) {
                        folderList.add(new FolderBean(new File(dirPath.substring(0, dirPath.lastIndexOf("/data"))), ""));
                        folderList.add(new FolderBean(new File(QPyConstants.LEGACY_PATH), ""));
                        if (needCustom)
                            folderList.add(new FolderBean(new File(CONF.CUSTOM_PATH), ""));
                    } else if (dirPath.equals(QPyConstants.LEGACY_PATH) || (needCustom && dirPath.equals(CONF.CUSTOM_PATH)))
                        folderList.add(new FolderBean(new File(CONF.SCOPE_STORAGE_PATH), ""));
                    for (File file : files) {
                        //if (!file.getName().startsWith(".")) {
                        folderList.add(new FolderBean(file));
                        //}
                    }
                    //adapter.notifyDataSetChanged();
                    binding.tvPath.setText(dirPath);
                    curPath = dirPath;
                    adapter.notifyDataSetChanged();

                }
            } else {
                Toast.makeText(getContext(), R.string.file_not_exists, Toast.LENGTH_SHORT).show();
            }
        }
    }

    private void doNewDir() {
        new EnterDialog(getContext())
                .setTitle(getString(R.string.new_folder))
                .setHint(getString(R.string.folder_name))
                .setConfirmListener(name -> {
                    File dirN = new File(curPath, name.equals("") ? getString(R.string.untitled_folder) : name);
                    if (dirN.exists()) {
                        Crouton.makeText(getActivity(), getString(R.string.toast_folder_exist), Style.ALERT).show();
                        return false;
                    } else {
                        if (dirN.mkdirs()) {
                            openDir(curPath + "/" + name);
                        } else {
                            Toast.makeText(getContext(), R.string.mkdir_fail, Toast.LENGTH_SHORT).show();
                        }
                        return true;
                    }
                })
                .show();
    }

    public void backToPrev() {

        //String qpyDir = Environment.getExternalStorageDirectory().getAbsolutePath()+"/qpython";

        //if (curPath == null || qpyDir.equals(curPath) || Environment.getExternalStorageDirectory().getAbsolutePath().equals(curPath)) {
        if (curPath == null || curPath.equals(TOP_FOLDER)) {
            Objects.requireNonNull(getActivity()).finish();
        } else {
            String parentPath = getParentPath(curPath);
            openDir(parentPath);
        }
    }

    /*private void updateClouded(File file) {
        cloudedMap.put(file.getAbsolutePath(), true);
        if (file.isDirectory()) {
            for (File file1 : file.listFiles()) {
                if (file1.isDirectory()) {
                    updateClouded(file1);
                } else {
                    cloudedMap.put(file.getAbsolutePath(), true);
                }
            }
        }
    }*/

    public String getCurPath() {
        return curPath;
    }

    /*private void initCloud() {
        if (App.getUser() == null) {
            return;
        }
        //获取云端脚本

    }*/

    private void savePath(String path) {
        String[] paths = path.split("/");
        StringBuffer realPath = new StringBuffer(CONF.SCOPE_STORAGE_PATH);
        for (String p : paths) {
            if (!TextUtils.isEmpty(p)) {
                realPath.append("/");
                realPath.append(p);
                if (!p.equals("projects") && !p.equals("scripts") && !cloudedMap.keySet().contains(realPath.toString()))
                    cloudedMap.put(realPath.toString(), true);
            }
        }
    }

    /*public void deleteCloudedMap(String absolutePath) {
        if (cloudedMap.containsKey(absolutePath)) {
            cloudedMap.remove(absolutePath);
            adapter.notifyDataSetChanged();
        }
    }

    public void updateCloudedFiles(Map<String, Boolean> map) {
        if (map != null) {
            cloudedMap.putAll(map);
        }
        adapter.notifyDataSetChanged();
    }*/
}
// GIT TEST