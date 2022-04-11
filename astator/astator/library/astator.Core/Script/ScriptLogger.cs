﻿
using System;
using System.Collections.Concurrent;
using System.IO;
using System.Linq;
using System.Text;
using NLog;
using NLog.Config;
using NLog.Targets;
using NLogger = NLog.Logger;

namespace astator.Core.Script;

public class ScriptLogger
{
    private static ScriptLogger instance;

    public static ScriptLogger Instance
    {
        get
        {
            if (instance is null)
            {
                instance = new ScriptLogger();
            }
            return instance;
        }
    }

    private readonly NLogger logger;

    private readonly ConcurrentDictionary<string, Action<LogLevel, DateTime, string>> callbacks = new();


    private static readonly object locker = new();

    public static string AddCallback(string key, Action<LogLevel, DateTime, string> action)
    {
        lock (locker)
        {
            if (Instance.callbacks.ContainsKey(key))
            {
                key += DateTime.Now.ToString("_HH-mm-ss-fff");
            }
            Instance.callbacks.TryAdd(key, action);
            return key;
        }
    }

    public static void RemoveCallback(string key)
    {
        lock (locker)
        {
            foreach (var _key in Instance.callbacks.Keys.ToList())
            {
                if (_key.StartsWith(key))
                {
                    Instance.callbacks.TryRemove(_key, out _);
                }
            }
        }
    }

    public ScriptLogger()
    {
        var config = new LoggingConfiguration();
        var methodCallTarget = new MethodCallTarget("AddMessage", (logEvent, parameters) =>
        {
            foreach (var action in this.callbacks.Values)
            {
                try
                {
                    action.Invoke(logEvent.Level, DateTime.Now, logEvent.FormattedMessage);
                }
                catch { }
            }
        });
        config.LoggingRules.Add(new LoggingRule("*", LogLevel.Trace, methodCallTarget));

        var path = Path.Combine(Android.App.Application.Context.GetExternalFilesDir("log").ToString(), "log.txt");
        var fileTarget = new FileTarget
        {
            FileName = path,
            Layout = @"loger*/${level}*/${date::universalTime=false:format=MM-dd HH\:mm\:ss\.fff}*/: ${message}",
        };
        config.LoggingRules.Add(new LoggingRule("*", LogLevel.Warn, fileTarget));

        this.logger = LogManager.Setup()
            .SetupExtensions(s => s.AutoLoadAssemblies(true))
            .LoadConfiguration(config)
            .GetCurrentClassLogger();
    }

    public static void Log(string msg)
    {
        Instance.logger.Trace(msg);
    }
    public static void Debug(string msg)
    {
        Instance.logger.Debug(msg);
    }

    public static void Info(string msg)
    {
        Instance.logger.Info(msg);
    }

    public static void Error(string msg)
    {
        Instance.logger.Error(msg);
    }

    public static void Warn(string msg)
    {
        Instance.logger.Warn(msg);
    }

    public static void Trace(string msg)
    {
        Instance.logger.Trace(msg);
    }

    public static void Fatal(string msg)
    {
        Instance.logger.Fatal(msg);
    }

    public static void Log(params object[] items)
    {
        Instance.logger.Trace(GetMessage(items));
    }

    public static void Debug(params object[] items)
    {
        Instance.logger.Debug(GetMessage(items));
    }

    public static void Info(params object[] items)
    {
        Instance.logger.Info(GetMessage(items));
    }

    public static void Error(params object[] items)
    {
        Instance.logger.Error(GetMessage(items));
    }

    public static void Warn(params object[] items)
    {
        Instance.logger.Warn(GetMessage(items));
    }

    public static void Trace(params object[] items)
    {
        Instance.logger.Trace(GetMessage(items));
    }

    public static void Fatal(params object[] items)
    {
        Instance.logger.Fatal(GetMessage(items));
    }

    private static StringBuilder GetMessage(params object[] items)
    {
        var message = new StringBuilder();
        for (var i = 0; i < items.Length; i++)
        {
            if (i > 0)
            {
                message.Append(' ');
            }

            message.Append(items[i].ToString());
        }
        return message;
    }
}
