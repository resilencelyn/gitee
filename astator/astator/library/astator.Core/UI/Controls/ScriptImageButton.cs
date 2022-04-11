﻿using System.IO;
using Android.Graphics;
using Android.Views;
using astator.Core.UI.Base;
using static Android.Views.ViewGroup;

namespace astator.Core.UI.Controls;
public class ScriptImageButton : AndroidX.AppCompat.Widget.AppCompatImageButton, IControl
{
    public string CustomId { get; set; }
    public OnCreatedListener OnCreatedListener { get; set; }

    private Color backgroundColor = DefaultTheme.LayoutBackgroundColor;

    private readonly string workDir;

    public ScriptImageButton(Android.Content.Context context, string workDir, ViewArgs args) : base(context)
    {
        this.workDir = workDir;
        this.LayoutParameters = new MarginLayoutParams(this.LayoutParameters ?? new(LayoutParams.WrapContent, LayoutParams.WrapContent));

        this.SetDefaultValue(ref args);
        foreach (var item in args)
        {
            SetAttr(item.Key.ToString(), item.Value);
        }
    }

    public override bool OnTouchEvent(MotionEvent e)
    {
        if (e.Action == MotionEventActions.Down)
        {
            SetBackgroundColor(DefaultTheme.ColorHint);
        }
        else
        {
            SetBackgroundColor(this.backgroundColor);
        }
        return base.OnTouchEvent(e);
    }

    public void SetAttr(string key, object value)
    {
        switch (key)
        {
            case "src":
                {
                    if (value is string temp)
                    {
                        var path = temp;
                        if (!path.StartsWith("/"))
                        {
                            path = System.IO.Path.Combine(this.workDir, "assets", path);
                        }
                        if (!File.Exists(path))
                        {
                            throw new FileNotFoundException(path + ": open failed! (No such file or directory)");
                        }
                        SetImageURI(Android.Net.Uri.FromFile(new Java.IO.File(path)));
                    }
                    break;
                }
            case "bg":
                {
                    if (value is string temp)
                    {
                        var bg = Color.ParseColor(temp);
                        this.backgroundColor = bg;
                        SetBackgroundColor(bg);
                    }
                    else if (value is Color color)
                    {
                        var bg = color;
                        this.backgroundColor = bg;
                        SetBackgroundColor(bg);
                    }
                    break;
                }
            case "scaleType":
                {
                    SetScaleType(Util.TypeParse<ScaleType>(value));
                    break;
                }
            default:
                {
                    Util.SetAttr(this, key, value);
                    break;
                }
        }
    }
    public object GetAttr(string key)
    {
        return key switch
        {
            "scaleType" => GetScaleType(),
            _ => Util.GetAttr(this, key)
        };
    }

    public void On(string key, object listener)
    {
        this.OnListener(key, listener);
    }
}
