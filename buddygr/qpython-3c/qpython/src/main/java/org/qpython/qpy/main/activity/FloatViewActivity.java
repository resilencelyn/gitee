package org.qpython.qpy.main.activity;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Intent;
import android.graphics.PixelFormat;
import android.icu.text.SimpleDateFormat;
import android.os.Build;
import android.os.Bundle;
import android.support.annotation.RequiresApi;
import android.text.Html;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.Toast;

import org.qpython.qpy.R;
import org.qpython.qpy.console.ScriptExec;

import java.util.ArrayList;
import java.util.Date;

public class FloatViewActivity extends Activity
    {
        //按钮数组
        static final ArrayList<Button> buttons = new ArrayList<>();
        //参数数组
        static final ArrayList<WindowManager.LayoutParams> params = new ArrayList<>();
        //时间数组
        static final ArrayList<String> times = new ArrayList<>();
        //操作类型数组
        static final ArrayList<String> operations = new ArrayList<>();
        static WindowManager windowManager;

        @SuppressLint({"ClickableViewAccessibility", "SimpleDateFormat"})
        @Override
        public void onCreate(Bundle savedInstanceState) {
            super.onCreate(savedInstanceState);
            if (Build.VERSION.SDK_INT<26){
                Toast.makeText(this,
                        getString(R.string.float_view_android),
                        Toast.LENGTH_LONG).show();
                finish();
                return;
            }
        Intent intent = getIntent();
        if (intent==null) intent = new Intent();
        WindowManager.LayoutParams layoutParams = null;
        int index;
        //返回结果
        index=intent.getIntExtra("result",-2);
        if (index>-2){
            if(index<0) index=buttons.size()-1;
            if(index<0 || index>=buttons.size()){
                Toast.makeText(this,R.string.float_view_out_range,Toast.LENGTH_LONG).show();
                finish();
                return;
            }
            Intent intentR = new Intent();
            //返回横坐标，原点为屏幕中心
            layoutParams = params.get(index);
            intentR.putExtra("x", layoutParams.x);
            //返回纵坐标，原点为屏幕中心
            intentR.putExtra("y", layoutParams.y);
            intentR.putExtra("time",times.get(index));
            intentR.putExtra("operation",operations.get(index));
            intentR.putExtra("index",index);
            setResult(RESULT_OK,intentR);
            finish();
            return;
        }
        windowManager = (WindowManager) this.getSystemService(WINDOW_SERVICE);
        //删除悬浮窗
        index=intent.getIntExtra("remove",-2);
        if (index>-2){
        if(index==-1){//删除所有悬浮窗
            for(Button button : buttons)
                windowManager.removeView(button);
            buttons.clear();
            params.clear();
            times.clear();
            operations.clear();
        } else {//删除一个悬浮窗
            try {
                removeButton(index);
            } catch (Exception e){
                Toast.makeText(this,e.toString(),Toast.LENGTH_LONG).show();
            }
        }
          finish();
          return;
        }
        //悬浮按钮
        Button floatButton = null;
        //悬浮窗文本
        String text=intent.getStringExtra("text");
        //是否彩色文本
        boolean isHtml = false;
        if (text == null) {
            text = intent.getStringExtra("html");
            if (text == null)
                text = "drag move\nlong click close";
            else isHtml = true;
        }
        //悬浮窗背景色 格式:aarrggbb或rrggbb
        int backColor=colorToInt(intent.getStringExtra("backColor"),"7f7f7f7f");
        //悬浮窗文字颜色 格式:aarrggbb或rrggbb
        int textColor=colorToInt(intent.getStringExtra("textColor"),"ff000000");
        //字体大小
        int textSize=intent.getIntExtra("textSize",10);
        //脚本路径
        final String script=intent.getStringExtra("script");
        //脚本参数
        final String arg=intent.getStringExtra("arg");
        final boolean clickRemove = intent.getBooleanExtra("clickRemove",true);
        //moveTaskToBack(true);
        //索引参数
        index = intent.getIntExtra("index",-1);
        if (index>=0) {
            try {
                floatButton = buttons.get(index);
                layoutParams = params.get(index);
            } catch (Exception e){
                index=-1;
            }
        }
        if(index<0){
            floatButton = new Button(this);
            layoutParams = new WindowManager.LayoutParams();
            index = buttons.size();
        }
            WindowManager.LayoutParams finalLayoutParams = layoutParams;
            floatButton.setOnTouchListener(new View.OnTouchListener() {
            private int x;
            private int y;

            @RequiresApi(api = Build.VERSION_CODES.N)
            @Override
            public boolean onTouch(View view, MotionEvent event) {
                int index;
                for(index =0; index < buttons.size(); index++){
                    if(view==buttons.get(index))
                        break;
                }
                switch (event.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        x = (int) event.getRawX();
                        y = (int) event.getRawY();
                        break;
                    case MotionEvent.ACTION_MOVE:
                        int nowX = (int) event.getRawX();
                        int nowY = (int) event.getRawY();
                        int movedX = nowX - x;
                        int movedY = nowY - y;
                        x = nowX;
                        y = nowY;
                        finalLayoutParams.x = finalLayoutParams.x + movedX;
                        finalLayoutParams.y = finalLayoutParams.y + movedY;
                        // 更新悬浮窗控件布局
                        if (movedX!=0 || movedY!=0){
                            windowManager.updateViewLayout(view, finalLayoutParams);
                            operations.set(index, "move");
                        } else {
                            operations.set(index, "click");
                        }
                        times.set(index, getTime());
                        break;
                    case MotionEvent.ACTION_UP:
                    if (operations.get(index).equals("click")) {
                            if(script!=null)
                                ScriptExec.getInstance().playScript(FloatViewActivity.this,
                                        script, arg,false);
                            if (clickRemove) removeButton(index);
                            FloatViewActivity.this.finish();
                        }
                    default:
                        break;
                }
                return false;
            }
        });
        if(isHtml)
            floatButton.setText(Html.fromHtml(text));
        else
            floatButton.setText(text);
        floatButton.setBackgroundColor(backColor);
        floatButton.setTextColor(textColor);
        floatButton.setTextSize(textSize);
        layoutParams.type=WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY;//下拉通知栏不可见
        // 设置Window flag,锁定悬浮窗 ,若不设置，悬浮窗会占用整个屏幕的点击事件，FLAG_NOT_FOCUSABLE不设置会导致菜单键和返回键失效
        layoutParams.flags = WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL
                | WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE;
        layoutParams.format = PixelFormat.RGBA_8888; // 设置图片格式，效果为背景透明
        //悬浮窗宽度
        int width = intent.getIntExtra("width",300);
        if(width!=Integer.MIN_VALUE)
            layoutParams.width = width;
        //悬浮窗高度
        int height = intent.getIntExtra("height",150);
        if (height!=Integer.MIN_VALUE)
            layoutParams.height = height;
        //起始横坐标，原点为屏幕中心
        int x = intent.getIntExtra("x",0);
        if (x!=Integer.MIN_VALUE)
            layoutParams.x=x;
        //起始纵坐标，原点为屏幕中心
        int y = intent.getIntExtra("y",0);
        if (y!=Integer.MIN_VALUE)
            layoutParams.y=y;
        //记录结果
        try {
            if(index>=buttons.size()){
                windowManager.addView(floatButton, layoutParams);
                buttons.add(floatButton);
                params.add(layoutParams);
                times.add(getTime());
                operations.add("initial");
            } else {
                params.set(index,layoutParams);
                times.set(index,getTime());
                windowManager.updateViewLayout(floatButton, layoutParams);
                operations.set(index,"modify");
            }
        } catch (Exception e) {
            Toast.makeText(this,getString(R.string.float_view_permission)+"\n"+ e,Toast.LENGTH_LONG).show();
        }
        finish();
        }

        private int colorToInt(String color,String defaultColor){
        if (color == null) {
            color = defaultColor;
        } else {
            int len = color.length();
            if (len <= 6) {
                color = defaultColor.substring(0,2) + "000000".substring(len) + color;
            }
        }
        long l;
        try {
            l = Long.valueOf(color,16);
            return (int) l;
        }
        catch (Exception e) {
            l = Long.valueOf(defaultColor,16);
            return (int) l;
        }
    }

    private void removeButton(int index){
        windowManager.removeView(buttons.get(index));
        buttons.remove(index);
        params.remove(index);
        times.remove(index);
        operations.remove(index);
    }

    @SuppressLint("NewApi")
    private String getTime(){
            return new SimpleDateFormat("yyyyMMdd-HHmmss-SSS").format(new Date(System.currentTimeMillis()));
    }
}
