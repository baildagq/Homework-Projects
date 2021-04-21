package com.example.newsapp.util;

import androidx.appcompat.app.AppCompatDelegate;

public class Variable {
    public static int darkMode = AppCompatDelegate.MODE_NIGHT_NO;
    // to optimize, add flat to avoid load sharepreferences every time
    public static boolean saveStreamMode = false;
    public static boolean settingChanged = false;
}
