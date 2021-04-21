package com.example.newsapp;

import android.content.Intent;
import android.os.Bundle;
import android.os.PersistableBundle;

public class WelcomeActivity extends BaseActivity {
    @Override
    public void onCreate(Bundle savedInstanceState ) {
        super.onCreate(savedInstanceState);
        startActivity(new Intent(this, MainActivity.class));
        finish();
    }
}
