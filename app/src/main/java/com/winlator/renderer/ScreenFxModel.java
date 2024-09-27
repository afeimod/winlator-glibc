package com.winlator.renderer;

import android.content.Context;
import android.content.SharedPreferences;

import androidx.preference.PreferenceManager;

public class ScreenFxModel {

    private static ScreenFxModel instance;

    public boolean antialiasing;
    public float brightness;
    public float contrast;
    public float gamma;
    public float reflection;
    public float saturation;

    public static ScreenFxModel getInstance() {
        if (instance == null) {
            instance = new ScreenFxModel();
        }
        return instance;
    }

    public void load(Context context) {
        SharedPreferences pref =  PreferenceManager.getDefaultSharedPreferences(context);
        antialiasing = pref.getBoolean("fx_antialiasing", false);
        brightness = pref.getFloat("fx_brightness", 1);
        contrast = pref.getFloat("fx_contrast", 1);
        gamma = pref.getFloat("fx_gamma", 1);
        reflection = pref.getFloat("fx_reflection", 0);
        saturation = pref.getFloat("fx_saturation", 1);
    }

    public void reset(Context context) {
        antialiasing = false;
        brightness = 1;
        contrast = 1;
        gamma = 1;
        reflection = 0;
        saturation = 1;
        save(context);
    }

    public void save(Context context) {
        SharedPreferences.Editor editor =  PreferenceManager.getDefaultSharedPreferences(context).edit();
        editor.putBoolean("fx_antialiasing", antialiasing);
        editor.putFloat("fx_brightness", brightness);
        editor.putFloat("fx_contrast", contrast);
        editor.putFloat("fx_gamma", gamma);
        editor.putFloat("fx_reflection", reflection);
        editor.putFloat("fx_saturation", saturation);
        editor.apply();
    }
}
