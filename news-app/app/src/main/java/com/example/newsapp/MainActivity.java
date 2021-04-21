package com.example.newsapp;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;

import com.arlib.floatingsearchview.FloatingSearchView;
import com.example.newsapp.adapter.MyPagerAdapter;
import com.example.newsapp.bean.NewsChannelBean;
import com.example.newsapp.bean.SearchHistoryBean;
import com.example.newsapp.database.NewsChannelDao;
import com.example.newsapp.database.SearchHistoryDao;
import com.example.newsapp.fragment.NewsRecycleView;
import com.example.newsapp.fragment.SettingsFragment;
import com.example.newsapp.util.Constant;
import com.example.newsapp.util.Variable;
import com.google.android.material.floatingactionbutton.FloatingActionButton;

import android.view.Gravity;
import android.view.View;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatDelegate;
import androidx.appcompat.widget.PopupMenu;
import androidx.core.view.GravityCompat;

import android.view.MenuItem;

import com.google.android.material.navigation.NavigationView;
import com.google.android.material.tabs.TabLayout;
import com.mancj.materialsearchbar.MaterialSearchBar;

import androidx.drawerlayout.widget.DrawerLayout;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import androidx.fragment.app.Fragment;
import androidx.preference.PreferenceFragmentCompat;
import androidx.preference.PreferenceManager;
import androidx.viewpager.widget.ViewPager;

import android.view.Menu;
import android.view.inputmethod.InputMethodManager;
import android.widget.ImageView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;


public class MainActivity extends AppCompatActivity
        implements NavigationView.OnNavigationItemSelectedListener {

    private DrawerLayout drawer;
    private NavigationView navigationView;
    private TabLayout tabLayout;
    private ViewPager viewPager;
    private ImageView modifyNewsChannel;
    private MaterialSearchBar searchBar;
    private FloatingActionButton floatingActionButton;

    private String words = "";
    private final int ACTIVITY_TYPE_CHANNEL_MANAGER = 1;
    private final int ACTIVITY_TYPE_SEACHER = 2;
    private final int ACTIVITY_TYPE_SETTING = 3;
    public static Context globalContext;
    public static Activity mainActivity;
    private NewsChannelDao newsChannelDao;
    private List<NewsChannelBean> channelList;
    private List<Fragment> fragments;
    private MyPagerAdapter myPagerAdapter;
    private SearchHistoryDao searchHistoryDao;
    private List<SearchHistoryBean> historyBeansList;
    private List<String> historyStringList;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        initSetting();

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        globalContext = getApplicationContext();
        mainActivity = this;
        // initData 需要在 initView 之前完成，这样才知道要有多少个tab(fragment)需要建立
        initData();
        initView();
    }

    public void initSetting(){
        SharedPreferences sharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
        if(sharedPreferences.getBoolean("night_mode", false)){
            AppCompatDelegate.setDefaultNightMode(AppCompatDelegate.MODE_NIGHT_YES);
        }
        else {
            AppCompatDelegate.setDefaultNightMode(AppCompatDelegate.MODE_NIGHT_NO);
        }
        Variable.saveStreamMode = sharedPreferences.getBoolean("save_stream_mode", false);
    }

    public void initData(){
        // query channel data from database
        newsChannelDao = new NewsChannelDao();
        fragments = new ArrayList<>();
        channelList = newsChannelDao.query(Constant.NEWS_CHANNEL_ENABLE);
        if (channelList.size() == 0) {
            newsChannelDao.addInitData();
            channelList = newsChannelDao.query(Constant.NEWS_CHANNEL_ENABLE);
        }

        for (NewsChannelBean bean : channelList) {
            NewsRecycleView nrc = new NewsRecycleView();
            Bundle bundle = new Bundle();
            bundle.putString("words", this.words);
            bundle.putString("category", bean.getChannelName());
            nrc.setArguments(bundle);
            fragments.add(nrc);
        }

        // query search history from database
        searchHistoryDao = new SearchHistoryDao();
        historyStringList = new ArrayList<>();
        historyBeansList = searchHistoryDao.query();
        for(SearchHistoryBean bean : historyBeansList){
            historyStringList.add(bean.getSearchWord());
        }
    }

    public void initView(){
        drawer = findViewById(R.id.drawer_layout);
        navigationView = findViewById(R.id.nav_view);
        navigationView.setNavigationItemSelectedListener(this);

        initSearchBar();

        tabLayout = findViewById(R.id.tab_layout_news);
        viewPager = findViewById(R.id.view_pager_news);
        modifyNewsChannel = findViewById(R.id.modify_channel_iv);
        modifyNewsChannel.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View view) {
                startActivityForResult(new Intent(MainActivity.this, NewsChannelActivity.class), ACTIVITY_TYPE_CHANNEL_MANAGER);
            }
        });

        floatingActionButton = findViewById(R.id.searchButton);
        floatingActionButton.setOnClickListener(new View.OnClickListener() { @Override
        public void onClick(View v) {
            searchBar.enableSearch();
        }
        });

        myPagerAdapter = new MyPagerAdapter(getSupportFragmentManager(), fragments, channelList);
        viewPager.setAdapter(myPagerAdapter);
        tabLayout.setupWithViewPager(viewPager);

    }

    public void initSearchBar(){
        searchBar = findViewById(R.id.searchBar);
        searchBar.setHint("explore...");
        searchBar.setSpeechMode(true);
        //enable searchbar callbacks
        searchBar.setOnSearchActionListener(new MaterialSearchBar.OnSearchActionListener() {
            @Override
            public void onSearchStateChanged(boolean enabled) {
                if(enabled){
                }
                else{
                }
            }

            @Override
            public void onSearchConfirmed(CharSequence text) {
                MainActivity.this.words = text.toString();
                updateNewsTabs();
                InputMethodManager inputMethodManager = (InputMethodManager)getSystemService(Context.INPUT_METHOD_SERVICE);
                inputMethodManager.hideSoftInputFromWindow(getWindow().getDecorView().getWindowToken(), 0);
            }

            @Override
            public void onButtonClicked(int buttonCode) {
                switch (buttonCode) {
                    case MaterialSearchBar.BUTTON_NAVIGATION:
                        drawer.openDrawer(Gravity.LEFT);
                        break;
                    case MaterialSearchBar.BUTTON_SPEECH:
                        break;
                    case MaterialSearchBar.BUTTON_BACK:
                        searchBar.disableSearch();
                        break;
                }
            }
        });
        searchBar.setNavButtonEnabled(true);

        //restore last queries from disk
        searchBar.setLastSuggestions(historyStringList);
        //Inflate menu and setup OnMenuItemClickListener
//        searchBar.inflateMenu(R.menu.main);
//        searchBar.getMenu().setOnMenuItemClickListener(new PopupMenu.OnMenuItemClickListener() {
//            @Override
//            public boolean onMenuItemClick(MenuItem item) {
//                int id = item.getItemId();
//                if (id == R.id.action_search){
//                    //startActivityForResult(new Intent(MainActivity.globalContext, SearchActivity.class), ACTIVITY_TYPE_SEACHER);
//                }
//                if (id == R.id.action_settings) {
//                    return true;
//                }
//                return false;
//            }
//        });
    }

    public void updateNewsTabs(){
        fragments.clear();
        channelList.clear();
        channelList = newsChannelDao.query(Constant.NEWS_CHANNEL_ENABLE);
        for (NewsChannelBean bean : channelList) {
            // TODO : 推荐频道
            NewsRecycleView nrc = new NewsRecycleView();
            Bundle bundle = new Bundle();
            bundle.putString("words", this.words);
            bundle.putString("category", bean.getChannelName());
            nrc.setArguments(bundle);
            fragments.add(nrc);
        }

        myPagerAdapter = new MyPagerAdapter(getSupportFragmentManager(), fragments, channelList);
        viewPager.setAdapter(myPagerAdapter);
        viewPager.setOffscreenPageLimit(12);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        switch (requestCode){
            case ACTIVITY_TYPE_CHANNEL_MANAGER:
                Toast.makeText(this, "onActivityResult", Toast.LENGTH_SHORT).show();
                updateNewsTabs();

            case ACTIVITY_TYPE_SEACHER:
                Toast.makeText(this, "search return", Toast.LENGTH_SHORT).show();

            case ACTIVITY_TYPE_SETTING:
                SharedPreferences sharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
                // night-mode changed
                if(Variable.settingChanged)
                {
                    recreate();
                }
        }
    }

    // below : code come with template
    @Override
    public void onBackPressed() {
        DrawerLayout drawer = findViewById(R.id.drawer_layout);
        if (drawer.isDrawerOpen(GravityCompat.START)) {
            drawer.closeDrawer(GravityCompat.START);
        } else {
            super.onBackPressed();
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        // getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item_news_list clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.

        // int id = item.getItemId();

        // //noinspection SimplifiableIfStatement
        // if (id == R.id.action_search){
        //     startActivityForResult(new Intent(this, SearchActivity.class), ACTIVITY_TYPE_SEACHER);
        // }
        // if (id == R.id.action_settings) {
        //     return true;
        // }
        return super.onOptionsItemSelected(item);
    }

    @SuppressWarnings("StatementWithEmptyBody")
    @Override
    public boolean onNavigationItemSelected(MenuItem item) {
        // Handle navigation view item_news_list clicks here.
        int id = item.getItemId();

        if (id == R.id.nav_home) {
            // Handle the camera action
        }
        else if (id == R.id.nav_collections){
            startActivity(new Intent(this, CollectionActivity.class));
        } else if (id == R.id.nav_settings) {
            Variable.settingChanged = false;
            startActivityForResult(new Intent(this, SettingActivity.class), ACTIVITY_TYPE_SETTING);
        } else if (id == R.id.nav_history) {
            startActivity(new Intent(this, HistoryActivity.class));
        }

        DrawerLayout drawer = findViewById(R.id.drawer_layout);
        drawer.closeDrawer(GravityCompat.START);
        return true;
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        // save suggestions(history) to database
        historyStringList = searchBar.getLastSuggestions();
        searchHistoryDao.removeAll();
        for(String suggestions : historyStringList){
            searchHistoryDao.add(suggestions);
        }
    }
}
