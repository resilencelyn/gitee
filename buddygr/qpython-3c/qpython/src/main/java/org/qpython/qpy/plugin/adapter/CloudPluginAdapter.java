package org.qpython.qpy.plugin.adapter;

import android.content.Context;
import android.support.v4.app.FragmentManager;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.PopupMenu;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import com.quseit.common.updater.Updater;

import org.qpython.qpy.R;

import org.qpython.qpy.plugin.manager.CloudPluginManager;
import org.qpython.qpy.plugin.fragment.PluginDetailDialogFragment;
import org.qpython.qpy.plugin.model.CloudPluginBean;

import java.util.ArrayList;
import java.util.List;


public class CloudPluginAdapter extends RecyclerView.Adapter<CloudPluginAdapter.PluginHolder> {
    private Context               mContext;
    private List<CloudPluginBean> mPluginBeen;

    public CloudPluginAdapter(Context context) {
        mContext = context;
        mPluginBeen = new ArrayList<>();
    }

    @Override
    public PluginHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        LayoutInflater inflater = LayoutInflater.from(mContext);
        View v = inflater.inflate(R.layout.item_plugin_cloud, null);
        return new PluginHolder(v);
    }

    @Override
    public void onBindViewHolder(PluginHolder holder, int position) {
        CloudPluginBean bean = mPluginBeen.get(position);
        holder.name.setText(bean.getName());
        holder.description.setText(bean.getDescription());

        PopupMenu menu = new PopupMenu(mContext, holder.menu);
        boolean install = CloudPluginManager.checkInstalled(bean);
        boolean update = CloudPluginManager.checkUpdate(bean);
        if (install && !update) {
            holder.menu.setImageResource(R.drawable.ic_plugin);
            menu.inflate(R.menu.plugin_installed_menu);
            menu.setOnMenuItemClickListener(item -> {
                        switch (item.getItemId()) {
                            case R.id.detail:
                                showDetail(bean);
                                break;
                            case R.id.uninstall:
                                uninstall(bean);
                                break;
                        }
                        return true;
                    }
            );
        } else if (install) {
            holder.menu.setImageResource(R.drawable.ic_library_upgrade);
            menu.inflate(R.menu.plugin_update_menu);
            menu.setOnMenuItemClickListener(item -> {
                switch (item.getItemId()) {
                    case R.id.detail:
                        showDetail(bean);
                        break;
                    case R.id.update:
                        update(bean);
                        break;
                    case R.id.uninstall:
                        uninstall(bean);
                        break;
                }
                return true;
            });
        } else {
            holder.menu.setImageResource(R.drawable.ic_library_download);
            menu.inflate(R.menu.plugin_uninstalled_menu);
            menu.setOnMenuItemClickListener(item -> {
                switch (item.getItemId()) {
                    case R.id.install:
                        install(bean);
                        break;
                }
                return true;
            });
        }
        holder.itemView.setOnClickListener(v -> menu.show());
    }

    private void showDetail(CloudPluginBean plugin) {
        FragmentManager manager = ((AppCompatActivity) mContext).getSupportFragmentManager();
        PluginDetailDialogFragment.newInstance(plugin.getName(), CloudPluginManager.getCompletePath(plugin))
                .show(manager, PluginDetailDialogFragment.TAG);
    }

    private void install(CloudPluginBean plugin) {
        update(plugin);
    }

    private void update(CloudPluginBean plugin) {
        Updater.update(plugin);
    }

    private void uninstall(CloudPluginBean plugin) {
        mPluginBeen.remove(plugin);
        CloudPluginManager.uninstall(plugin);
    }

    @Override
    public int getItemCount() {
        return mPluginBeen == null ? 0 : mPluginBeen.size();
    }

    public void setData(List<CloudPluginBean> plugins) {
        mPluginBeen.clear();
        mPluginBeen.addAll(plugins);
        notifyDataSetChanged();
    }

    public static class PluginHolder extends RecyclerView.ViewHolder {
        TextView name;

        TextView description;

        ImageView menu;

        public PluginHolder(View itemView) {
            super(itemView);
            name = (TextView) itemView.findViewById(R.id.plugin_name);
            description = (TextView) itemView.findViewById(R.id.plugin_description);
            menu = (ImageView) itemView.findViewById(R.id.plugin_menu);
        }
    }
}
