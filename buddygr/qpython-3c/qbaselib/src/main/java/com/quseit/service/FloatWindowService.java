package com.quseit.service;

import android.app.Service;
import android.content.Intent;
import android.os.Handler;
import android.os.IBinder;


import java.util.Timer;
import java.util.TimerTask;

import static com.quseit.config.BASE_CONF.TIME_SPAN;

public class FloatWindowService extends Service  {

    private Handler handler = new Handler();
    private Timer timer;

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        if (timer == null) {
            timer = new Timer();
            timer.scheduleAtFixedRate(new RefreshTask(), 0L, (long) TIME_SPAN);
        }
        int result = super.onStartCommand(intent, flags, startId);
        return result;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        // Service被终止的同时也停止定时器继续运行
        timer.cancel();
        timer = null;
        SpeedWindowManager.getInstance().removeAllWindow(getApplicationContext());
    }

    class RefreshTask extends TimerTask {

        @Override
        public void run() {
            // 当前没有悬浮窗显示，则创建悬浮窗。
            if (!SpeedWindowManager.getInstance().isWindowShowing()) {
                handler.post(new Runnable() {
                    @Override
                    public void run() {
                        SpeedWindowManager.getInstance().initData();
                        SpeedWindowManager.getInstance().createWindow(getApplicationContext());
                    }
                });
            }
            // 当前有悬浮窗显示，则更新内存数据。
            else {
                handler.post(new Runnable() {
                    @Override
                    public void run() {
                        SpeedWindowManager.getInstance().updateViewData(getApplicationContext());
                    }
                });
            }
        }

    }
}
