package com.example.newsapp;

import android.content.Intent;
import android.os.Bundle;
import android.view.MenuItem;
import android.view.View;
import android.widget.Toast;

import androidx.annotation.Nullable;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.billy.android.swipe.SmartSwipe;
import com.billy.android.swipe.consumer.ActivitySlidingBackConsumer;
import com.example.newsapp.adapter.MyNewsListAdapter;
import com.example.newsapp.bean.NewsCollectionsOrHistoryBean;
import com.example.newsapp.database.NewsCollectionsDao;
import com.example.newsapp.database.NewsHistoryDao;
import com.example.newsapp.model.SingleNews;
import com.example.newsapp.util.Constant;

import java.util.ArrayList;
import java.util.List;


public class HistoryActivity extends BaseActivity{
    private List<NewsCollectionsOrHistoryBean> historyList;
    private List<SingleNews> newsList;
    private NewsHistoryDao dao = new NewsHistoryDao();
    private RecyclerView recyclerView;
    private MyNewsListAdapter adapter;

    public HistoryActivity(){}

    @Override
    public void onCreate(Bundle savedInstanceState ) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_history);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);

        initData();
        initView();
    }

    public void initData(){
        historyList = dao.query();
        newsList = new ArrayList<>();
        for(int i = historyList.size() - 1; i >= 0; i--){
            newsList.add(new SingleNews(
                    historyList.get(i).getImage(),
                    historyList.get(i).getPublishTime(),
                    historyList.get(i).getPublisher(),
                    historyList.get(i).getTitle(),
                    historyList.get(i).getContent(),
                    null,
                    historyList.get(i).getVideo(),
                    historyList.get(i).getNewsID(),
                    null,
                    null,
                    null,
                    null,
                    null,
                    null,
                    null,
                    null
            ));
        }

    }
    public void initView(){
        adapter = new MyNewsListAdapter(newsList, this, Constant.LIST_TYPE_HISTORY);
        recyclerView = findViewById(R.id.history_rcv);
        recyclerView.setLayoutManager(new LinearLayoutManager(this));
        recyclerView.setAdapter(adapter);
        initAdapter();

        SmartSwipe.wrap(this)
                .addConsumer(new ActivitySlidingBackConsumer(this))
                //设置联动系数
                .setRelativeMoveFactor(0.5F)
                //指定可侧滑返回的方向，如：enableLeft() 仅左侧可侧滑返回
                .enableLeft()
        ;
    }

    public void initAdapter(){
        adapter.setOnItemClickListener(new MyNewsListAdapter.OnItemClickListener() {
            @Override
            public void onItemClick(View view, int position) {
                Intent intent = new Intent(HistoryActivity.this, NewsDetailActivity.class);
                SingleNews news= newsList.get(position);
                intent.putExtra("news", news);

                // start newsDetail page
                startActivityForResult(intent, 0);
            }
        });
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
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
}
