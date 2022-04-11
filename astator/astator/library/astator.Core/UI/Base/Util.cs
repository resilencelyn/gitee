﻿using System;
using System.Collections.Generic;
using Android.Graphics;
using Android.Graphics.Drawables;
using Android.Text.Util;
using Android.Views;
using Android.Widget;
using astator.Core.Exceptions;
using astator.Core.Script;
using astator.Core.UI.Controls;
using static Android.Text.TextUtils;
using static Android.Views.ViewGroup;

namespace astator.Core.UI.Base;

public static class Util
{
    public static T TypeParse<T>(object value)
    {
        var str = value.ToString().Trim().ToLower();
        var properties = typeof(T).GetProperties();
        foreach (var p in properties)
        {
            if (p.Name.ToString().ToLower().Equals(str))
            {
                return (T)p.GetValue(null);
            }
        }
        throw new AttributeNotExistException(str);
    }

    public static T EnumParse<T>(object value)
    {
        var list = new List<int>();
        if (value is string strs)
        {
            var array = strs.Trim().ToLower().Split("|");
            foreach (var item in Enum.GetNames(typeof(T)))
            {
                foreach (var str in array)
                {
                    if (item.ToLower().Equals(str))
                    {
                        list.Add((int)Enum.Parse(typeof(T), item));
                    }
                };
            }
            if (list.Count != 0)
            {
                var result = 0;
                foreach (var v in list)
                {
                    result |= v;
                }
                return (T)(object)result;
            }
            else
            {
                throw new AttributeNotExistException(strs);
            }
        }
        throw new AttributeNotExistException(value.ToString() ?? string.Empty);
    }

    public static int Dp2Px(object value)
    {
        return (int)(Devices.Dp * Convert.ToSingle(value));
    }
    public static int Px2Dp(object value)
    {
        return (int)(Convert.ToSingle(value) / Devices.Dp);
    }
    public static T Dp2Px<T>(object value)
    {
        return (T)(object)(Devices.Dp * Convert.ToSingle(value));
    }
    public static T Px2Dp<T>(object value)
    {
        return (T)(object)(Convert.ToSingle(value) / Devices.Dp);
    }


    public static void OnListener(this IView view, string key, object listener)
    {
        var v = view as View;
        switch (key)
        {
            case "click":
                {
                    if (listener is OnClickListener temp) v.SetOnClickListener(temp);
                    break;
                }
            case "longClick":
                {
                    if (listener is OnLongClickListener temp) v.SetOnLongClickListener(temp);
                    break;
                }
            case "touch":
                {
                    if (listener is OnTouchListener temp) v.SetOnTouchListener(temp);
                    break;
                }
            case "created":
                {
                    if (listener is OnCreatedListener temp) view.OnCreatedListener = temp;
                    break;
                }
            default: throw new AttributeNotExistException(key);
        }
    }
    public static void OnListener(this ILayout view, string key, object listener)
    {
        var v = view as View;
        switch (key)
        {
            case "scrollChange":
                {
                    if (listener is OnScrollChangeListener temp) v.SetOnScrollChangeListener(temp);
                    break;
                }
            default:
                var iView = view as IView;
                iView.OnListener(key, listener);
                break;
        }
    }

    public static void SetAttr(IView view, string key, object value)
    {
        if (view is TextView tv)
        {
            switch (key)
            {
                case "w":
                    {
                        tv.SetWidth(Dp2Px(value));
                        return;
                    }
                case "h":
                    {
                        tv.SetHeight(Dp2Px(value));
                        return;
                    }
                case "text":
                    {
                        tv.Text = value.ToString();
                        return;
                    }
                case "paintFlags":
                    {
                        tv.PaintFlags = EnumParse<PaintFlags>(value);
                        return;
                    }
                case "textSize":
                    {
                        tv.SetTextSize(Android.Util.ComplexUnitType.Dip, Convert.ToInt32(value));
                        return;
                    }
                case "textColor":
                    {
                        if (value is string temp) tv.SetTextColor(Color.ParseColor(temp));
                        else if (value is Color color) tv.SetTextColor(color);
                        return;
                    }
                case "lines":
                    {
                        tv.SetLines(Convert.ToInt32(value));
                        return;
                    }
                case "maxLines":
                    {
                        tv.SetMaxLines(Convert.ToInt32(value));
                        return;
                    }
                case "typeface":
                    {
                        if (value is string path)
                        {
                            var tf = Typeface.CreateFromFile(new Java.IO.File(path));
                            var style = tv.Typeface?.Style ?? tf.Style;
                            tv.SetTypeface(tf, style);
                        }
                        else if (value is Typeface tf)
                        {
                            var style = tv.Typeface?.Style ?? tf.Style;
                            tv.SetTypeface(tf, style);
                        }
                        return;
                    }
                case "textStyle":
                    {
                        tv.SetTypeface(tv.Typeface, EnumParse<TypefaceStyle>(value));
                        return;
                    }
                case "ems":
                    {
                        tv.SetEms(Convert.ToInt32(value));
                        return;
                    }
                case "autoLink":
                    {
                        if (value is string temp)
                        {
                            if (temp == "web") tv.AutoLinkMask = MatchOptions.WebUrls;
                            else if (temp == "email") tv.AutoLinkMask = MatchOptions.EmailAddresses;
                            else if (temp == "phone") tv.AutoLinkMask = MatchOptions.PhoneNumbers;
                            else if (temp == "map") tv.AutoLinkMask = MatchOptions.MapAddresses;
                            else if (temp == "all") tv.AutoLinkMask = MatchOptions.All;
                        }
                        return;
                    }
                case "ellipsize":
                    {
                        if (value is string temp) tv.Ellipsize = TruncateAt.ValueOf(temp);
                        return;
                    }
                case "gravity":
                    {
                        tv.Gravity = EnumParse<GravityFlags>(value);
                        return;
                    }
            }
        }

        var v = view as View;
        FrameLayout.LayoutParams newLp = v is ViewGroup ? new(LayoutParams.MatchParent, LayoutParams.MatchParent) : new(LayoutParams.WrapContent, LayoutParams.WrapContent);
        var lp = v.LayoutParameters as FrameLayout.LayoutParams
            ?? new FrameLayout.LayoutParams(v.LayoutParameters as MarginLayoutParams ?? newLp);

        switch (key)
        {
            case "id":
                {
                    if (value is string temp) view.CustomId = temp;
                    break;
                }
            case "w":
                {
                    lp.Width = Dp2Px(value);
                    v.LayoutParameters = lp;
                    break;
                }
            case "h":
                {
                    lp.Height = Dp2Px(value);
                    v.LayoutParameters = lp;
                    break;
                }
            case "minWidth":
                {
                    v.SetMinimumWidth(Dp2Px(value));
                    break;
                }
            case "minHeight":
                {
                    v.SetMinimumHeight(Dp2Px(value));
                    break;
                }
            case "weight":
                {
                    var _lp = v.LayoutParameters as LinearLayout.LayoutParams ?? new(v.LayoutParameters as MarginLayoutParams ?? new(LayoutParams.WrapContent, LayoutParams.WrapContent));
                    _lp.Weight = Convert.ToInt32(value);
                    v.LayoutParameters = _lp;
                    break;
                }
            case "margin":
                {
                    var margin = new int[4];
                    if (value is int i32) margin[0] = margin[1] = margin[2] = margin[3] = i32;
                    else if (value is int[] arr)
                    {
                        margin[0] = Dp2Px(arr[0]);
                        margin[1] = Dp2Px(arr[1]);
                        margin[2] = Dp2Px(arr[2]);
                        margin[3] = Dp2Px(arr[3]);
                    }
                    else if (value is string str)
                    {
                        var strArr = str.Split(",");
                        if (strArr.Length == 1)
                        {
                            var temp = Dp2Px(strArr[0]);
                            margin[0] = margin[1] = margin[2] = margin[3] = temp;
                        }
                        else if (strArr.Length == 2)
                        {
                            margin[0] = margin[2] = Dp2Px(strArr[0]);
                            margin[1] = margin[3] = Dp2Px(strArr[1]);
                        }
                        else if (strArr.Length == 4)
                        {
                            margin[0] = Dp2Px(strArr[0]);
                            margin[1] = Dp2Px(strArr[1]);
                            margin[2] = Dp2Px(strArr[2]);
                            margin[3] = Dp2Px(strArr[3]);
                        }
                    }
                    lp.SetMargins(margin[0], margin[1], margin[2], margin[3]);
                    v.LayoutParameters = lp;
                    break;
                }
            case "layoutGravity":
                {
                    lp.Gravity = EnumParse<GravityFlags>(value);
                    v.LayoutParameters = lp;
                    break;
                }
            case "padding":
                {
                    var padding = new int[4];
                    if (value is int i32) padding[0] = padding[1] = padding[2] = padding[3] = i32;
                    else if (value is int[] arr)
                    {
                        padding[0] = Dp2Px(arr[0]);
                        padding[1] = Dp2Px(arr[1]);
                        padding[2] = Dp2Px(arr[2]);
                        padding[3] = Dp2Px(arr[3]);
                    }
                    else if (value is string str)
                    {
                        var strArr = str.Split(",");
                        if (strArr.Length == 1)
                        {
                            var temp = Dp2Px(strArr[0]);
                            padding[0] = padding[1] = padding[2] = padding[3] = temp;
                        }
                        else if (strArr.Length == 2)
                        {
                            padding[0] = padding[2] = Dp2Px(strArr[0]);
                            padding[1] = padding[3] = Dp2Px(strArr[1]);
                        }
                        else if (strArr.Length == 4)
                        {
                            padding[0] = Dp2Px(strArr[0]);
                            padding[1] = Dp2Px(strArr[1]);
                            padding[2] = Dp2Px(strArr[2]);
                            padding[3] = Dp2Px(strArr[3]);
                        }
                    }
                    v.SetPadding(padding[0], padding[1], padding[2], padding[3]);
                    break;
                }
            case "alpha":
                {
                    v.Alpha = Convert.ToSingle(value);
                    break;
                }
            case "bg":
                {
                    if (value is string temp) v.SetBackgroundColor(Color.ParseColor(temp));
                    if (value is Color color) v.SetBackgroundColor(color);
                    break;
                }
            case "fg":
                {
                    if (value is string temp) v.Foreground = new ColorDrawable(Color.ParseColor(temp.Trim()));
                    if (value is Color color) v.Foreground = new ColorDrawable(color);
                    break;
                }
            case "visibility":
                {
                    v.Visibility = EnumParse<ViewStates>(value);
                    break;
                }
            case "rotation":
                {
                    v.Rotation = Dp2Px<float>(value);
                    break;
                }
            case "transformPivotX":
                {
                    v.TranslationX = Dp2Px<float>(value);
                    break;
                }
            case "transformPivotY":
                {
                    v.TranslationY = Dp2Px<float>(value);
                    break;
                }
            case "radius":
                {
                    v.ClipToOutline = true;
                    v.OutlineProvider = new RadiusOutlineProvider(Dp2Px<float>(value));
                    break;
                }
            case "tag":
                {
                    if (value is string temp) v.Tag = temp;
                    if (value is int i32) v.Tag = i32;
                    break;
                }
            default:
                throw new AttributeNotExistException(key);
        }
    }

    public static object GetAttr(IView view, string key)
    {
        if (view is TextView tv)
        {
            switch (key)
            {
                case "text": return tv.Text;
                case "textSize": return tv.TextSize;
                case "textColor": return tv.CurrentTextColor;
                case "maxLines": return tv.MaxLines;
                case "typeface": return tv.Typeface;
                case "autoLink": return tv.AutoLinkMask;
                case "ellipsize": return tv.Ellipsize;
                case "gravity": return tv.Gravity;
            };
        }

        var v = view as View;

        FrameLayout.LayoutParams newLp = v is ViewGroup ? new(LayoutParams.MatchParent, LayoutParams.MatchParent) : new(LayoutParams.WrapContent, LayoutParams.WrapContent);
        var lp = v.LayoutParameters as FrameLayout.LayoutParams
            ?? new FrameLayout.LayoutParams(v.LayoutParameters as MarginLayoutParams ?? newLp);

        return key switch
        {
            "id" => view.CustomId,
            "w" => Px2Dp(v.Width),
            "h" => Px2Dp(v.Height),
            "minWidth" => Px2Dp(v.MinimumWidth),
            "minHeight" => Px2Dp(v.MinimumHeight),
            "weight" => () =>
            {
                var _lp = v.LayoutParameters as LinearLayout.LayoutParams ?? new(v.LayoutParameters as MarginLayoutParams ?? new(LayoutParams.WrapContent, LayoutParams.WrapContent));
                return _lp.Weight;
            }
            ,
            "margin" => new int[] { Px2Dp(lp.LeftMargin), Px2Dp(lp.TopMargin), Px2Dp(lp.RightMargin), Px2Dp(lp.BottomMargin) },
            "layoutGravity" => lp.Gravity,
            "padding" => new int[] { Px2Dp(v.PaddingLeft), Px2Dp(v.PaddingTop), Px2Dp(v.PaddingRight), Px2Dp(v.PaddingBottom) },
            "alpha" => Px2Dp<float>(v.Alpha),
            "bg" => v.Background,
            "fg" => v.Foreground,
            "visibility" => v.Visibility,
            "rotation" => Px2Dp<float>(v.Rotation),
            "translationX" => Px2Dp<float>(v.TranslationX),
            "translationY" => Px2Dp<float>(v.TranslationY),
            _ => throw new AttributeNotExistException(key)
        };
    }

    public static void SetDefaultValue(this IView view, ref ViewArgs args)
    {
        args ??= new ViewArgs();
        args["id"] ??= $"scriptView-{UiManager.CreateCount++}";

        if (view is not ScriptEditText && view is not ScriptButton)
        {
            args["bg"] ??= DefaultTheme.LayoutBackgroundColor;
        }

        if (view is TextView)
        {
            args["textColor"] ??= DefaultTheme.TextColor;
            args["textSize"] ??= DefaultTheme.TextSize;
        }
    }
}
