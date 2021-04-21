package com.example.newsapp.fragment;

import android.content.SharedPreferences;
import android.os.Bundle;

import androidx.appcompat.app.AppCompatDelegate;
import androidx.preference.Preference;
import androidx.preference.PreferenceFragmentCompat;
import androidx.preference.PreferenceManager;

import com.example.newsapp.R;
import com.example.newsapp.util.Variable;

public class SettingsFragment extends PreferenceFragmentCompat {
    public static final String FRAGMENT_TAG = "preference_fragment";

    public SettingsFragment(){

    }

    @Override
    public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
        addPreferencesFromResource(R.xml.preference_setting);
    }

    @Override
    public boolean onPreferenceTreeClick(Preference preference) {
        if(preference.getKey().equals("night_mode")){
//            SharedPreferences sharedPreferences = PreferenceManager.getDefaultSharedPreferences(getContext());
//            if(sharedPreferences.getBoolean(preference.getKey(), false)){
//                Variable.darkMode = AppCompatDelegate.MODE_NIGHT_YES;
//            }
//            else {
//                Variable.darkMode = AppCompatDelegate.MODE_NIGHT_NO;
//            }
            getActivity().recreate();
        }
        if (preference.getKey().equals("save_stream_mode")){
            SharedPreferences sharedPreferences = PreferenceManager.getDefaultSharedPreferences(getContext());
            Variable.saveStreamMode = sharedPreferences.getBoolean(preference.getKey(), false);
        }
        Variable.settingChanged = true;
        return super.onPreferenceTreeClick(preference);
    }
}
