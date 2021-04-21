package com.example.newsapp;

import android.app.AlertDialog;
import android.app.Application;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.Configuration;
import android.os.Bundle;
import android.service.vr.VrListenerService;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.Switch;
import android.widget.Toast;

import androidx.annotation.DrawableRes;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.app.AppCompatDelegate;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentTransaction;
import androidx.preference.PreferenceFragmentCompat;
import androidx.preference.PreferenceManager;
import androidx.preference.PreferenceScreen;

import com.example.newsapp.fragment.SettingsFragment;
import com.example.newsapp.util.Variable;


public class SettingActivity extends AppCompatActivity implements PreferenceFragmentCompat.OnPreferenceStartScreenCallback {
//    private Switch switch_night;
//    private Switch switch_data;
//    private Button button_color;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        setNightMode();
        super.onCreate(savedInstanceState);
        SharedPreferences sharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);

        //Application app = (Application) this.getApplication();
        setContentView(R.layout.activity_set);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);

        if(savedInstanceState == null){
            // Create the fragment only when the activity is created for the first time.
            // ie. not after orientation changes
            Fragment fragment = getSupportFragmentManager().findFragmentByTag(SettingsFragment.FRAGMENT_TAG);
            if (fragment == null) {
                fragment = new SettingsFragment();
            }

            getSupportFragmentManager()
                .beginTransaction()
                .replace(R.id.fragment_container, fragment, SettingsFragment.FRAGMENT_TAG)
                .commit();
        }
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item){
        switch (item.getItemId()){
            case android.R.id.home:
                finish();
                return true;
            default:
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    public boolean onPreferenceStartScreen(PreferenceFragmentCompat caller, PreferenceScreen pref) {
        FragmentTransaction ft = getSupportFragmentManager().beginTransaction();
        SettingsFragment settingsFragment = new SettingsFragment();
        Bundle args = new Bundle();
        args.putString(PreferenceFragmentCompat.ARG_PREFERENCE_ROOT, pref.getKey());
        settingsFragment.setArguments(args);

        ft.add(R.id.fragment_container, settingsFragment, pref.getKey());
        ft.addToBackStack(pref.getKey());
        ft.commit();

        return false;
    }

    public void setNightMode(){
        SharedPreferences sharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
        if(sharedPreferences.getBoolean("night_mode", false)){
            AppCompatDelegate.setDefaultNightMode(AppCompatDelegate.MODE_NIGHT_YES);
        }
        else {
            AppCompatDelegate.setDefaultNightMode(AppCompatDelegate.MODE_NIGHT_NO);
        }
    }

//        setContentView(R.layout.activity_setting);
////        getSupportActionBar().setDisplayShowCustomEnabled(true);
////        ImageView icon=new ImageView(this);
////        icon.setImageDrawable(getResources().getDrawable(R.drawable.ic_favorite_border_black_24dp));
////        getSupportActionBar().setCustomView(icon);
//
////        switch_night=findViewById(R.id.switch_night);
////        switch_data=findViewById(R.id.switch_data);
////        button_color=findViewById(R.id.button_color);
//
//        if(switch_night != null) {
//            switch_night.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
//                @Override
//                public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
//                    if (b) {
//                        AppCompatDelegate.setDefaultNightMode(AppCompatDelegate.MODE_NIGHT_AUTO);
//                        AppCompatDelegate.setDefaultNightMode(AppCompatDelegate.MODE_NIGHT_YES);
//                        //recreate();
//                        finish();
//                        final Intent themeIntent=getIntent();
//                        themeIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK|Intent.FLAG_ACTIVITY_CLEAR_TASK);
//                        startActivity(themeIntent);
//                        overridePendingTransition(0,0);
//
//                    } else {
//                        AppCompatDelegate.setDefaultNightMode(AppCompatDelegate.MODE_NIGHT_NO);
//                        finish();
//                        final Intent themeIntent=getIntent();
//                        themeIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK|Intent.FLAG_ACTIVITY_CLEAR_TASK);
//                        startActivity(themeIntent);
//                        overridePendingTransition(0,0);
//                        //recreate();
//                    }
//                }
//            });
//        }
//        if(switch_data != null){
//            switch_data.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
//                @Override
//                public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
//                    if(b)
//                    {
//
//                    }
//                    else
//                    {
//
//                    }
//                }
//            });
//        }
//
//        if(button_color!=null)
//        {
//            button_color.setOnClickListener(new View.OnClickListener() {
//                @Override
//                public void onClick(View view) {
//                    final String items[] = {"color1", "color2", "color3", "color4"};
//                    AlertDialog dialog = new AlertDialog.Builder(SettingActivity.this)
//                            .setTitle("选择主题颜色")
//                            .setSingleChoiceItems(items, 1, new DialogInterface.OnClickListener() {
//                                @Override
//                                public void onClick(DialogInterface dialog, int which) {
//
//                                    Toast.makeText(SettingActivity.this, items[which], Toast.LENGTH_SHORT).show();
//                                }
//                            })
//                            .setNegativeButton("取消", new DialogInterface.OnClickListener() {
//                                @Override
//                                public void onClick(DialogInterface dialog, int which) {
//
//                                    dialog.dismiss();
//                                }
//                            })
//                            .setPositiveButton("确定", new DialogInterface.OnClickListener() {
//                                @Override
//                                public void onClick(DialogInterface dialog, int which) {
//
//                                    dialog.dismiss();
//                                }
//                            }).create();
//                    dialog.show();
//                }
//            });
//        }

}
