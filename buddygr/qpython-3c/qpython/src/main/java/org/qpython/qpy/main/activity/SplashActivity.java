package org.qpython.qpy.main.activity;

import android.app.ActivityManager;
import android.content.Intent;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;

import org.qpython.qpy.R;

import java.util.Timer;
import java.util.TimerTask;

public class SplashActivity extends AppCompatActivity {

    private static int delay = 1000;

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_splash);
        new Timer().schedule(new TimerTask() {
            @Override
            public void run() {
                Intent intent = new Intent(SplashActivity.this,HomeMainActivity.class);
                intent.setAction(getIntent().getAction());
                startActivity(intent);
                finish();
            }
        },delay);
        setTaskDescription(new ActivityManager.TaskDescription(getString(R.string.main_window)));
        delay = 100;
    }

}