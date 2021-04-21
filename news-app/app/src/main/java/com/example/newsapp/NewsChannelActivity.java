package com.example.newsapp;

import android.content.Intent;
import android.os.Bundle;
import android.view.MenuItem;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.ItemTouchHelper;
import androidx.recyclerview.widget.RecyclerView;

import com.example.newsapp.adapter.MyNewsChannelAdapter;
import com.example.newsapp.bean.NewsChannelBean;
import com.example.newsapp.database.NewsChannelDao;
import com.example.newsapp.helper.ItemDragHelperCallback;
import com.example.newsapp.util.Constant;

import java.util.List;

public class NewsChannelActivity extends BaseActivity {

    private RecyclerView recyclerView;
    private MyNewsChannelAdapter myNewsChannelAdapter;
    private NewsChannelDao dao = new NewsChannelDao();

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_channel);
        initView();
        initData();
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item){
        switch (item.getItemId()){
            case android.R.id.home:
                finish();
                break;
            default:
        }
        return true;
    }

    @Override
    protected void onPause() {
        super.onPause();
        onSaveData();
    }

    @Override
    protected void onStop() {
        super.onStop();
        this.setResult(RESULT_OK, new Intent());
    }

    public void initView(){
        //initToolBar((Toolbar)findViewById(R.id.toolbar), true, getString(R.string.channel_manager));
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        recyclerView = findViewById(R.id.news_channel_rcv);
    }

    public void initData(){
        final List<NewsChannelBean> enableItems = dao.query(Constant.NEWS_CHANNEL_ENABLE);
        final List<NewsChannelBean> disableItems = dao.query(Constant.NEWS_CHANNEL_DISABLE);

        GridLayoutManager manager = new GridLayoutManager(this, 4);
        recyclerView.setLayoutManager(manager);
        // 根据不同类型的viewType进行不同的布置排列, header一排1个，category item 一排4个
        manager.setSpanSizeLookup(new GridLayoutManager.SpanSizeLookup() {
            @Override
            public int getSpanSize(int position) {
                int viewType = myNewsChannelAdapter.getItemViewType(position);
                return viewType == MyNewsChannelAdapter.TYPE_ENABLE || viewType == MyNewsChannelAdapter.TYPE_DISABLE ? 1 : 4;
            }
        });

        final ItemTouchHelper itemTouchHelper = new ItemTouchHelper(new ItemDragHelperCallback());
        itemTouchHelper.attachToRecyclerView(recyclerView);

        myNewsChannelAdapter = new MyNewsChannelAdapter(this, itemTouchHelper, enableItems, disableItems);
        recyclerView.setAdapter(myNewsChannelAdapter);
    }

    public void onSaveData(){
        List<NewsChannelBean> enableItems = myNewsChannelAdapter.getEnableItems();
        List<NewsChannelBean> disableItems = myNewsChannelAdapter.getDisableItems();
        dao.removeAll();
        for (int i = 0; i < enableItems.size(); i++) {
            NewsChannelBean bean = enableItems.get(i);
            dao.add(bean.getChannelId(), bean.getChannelName(), Constant.NEWS_CHANNEL_ENABLE, i);
        }
        for (int i = 0; i < disableItems.size(); i++) {
            NewsChannelBean bean = disableItems.get(i);
            dao.add(bean.getChannelId(), bean.getChannelName(), Constant.NEWS_CHANNEL_DISABLE, i);
        }
    }
}

