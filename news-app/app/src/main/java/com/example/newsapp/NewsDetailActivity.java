package com.example.newsapp;

import android.graphics.Color;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.Switch;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContextCompat;

import com.billy.android.swipe.SmartSwipe;
import com.billy.android.swipe.consumer.ActivitySlidingBackConsumer;
import com.bumptech.glide.*;
import com.bumptech.glide.request.*;
import com.example.newsapp.database.NewsCollectionsDao;
import com.example.newsapp.model.SingleNews;


import com.example.newsapp.util.Variable;
import com.shuyu.gsyvideoplayer.GSYVideoManager;
import com.shuyu.gsyvideoplayer.utils.OrientationUtils;
import com.shuyu.gsyvideoplayer.video.StandardGSYVideoPlayer;
import com.stx.xhb.xbanner.XBanner;
//import com.stx.xhb.androidx.XBanner;

import java.util.ArrayList;
import java.util.List;


import cn.sharesdk.onekeyshare.OnekeyShare;

public class NewsDetailActivity extends AppCompatActivity {

    private SingleNews news;
    private TextView contentTextView;
    private TextView titleTextView;
    private TextView publisherInfo;
    private TextView publishTime;

    private Switch switchbutton;
    private NewsCollectionsDao collectionsDao = new NewsCollectionsDao();
    private StandardGSYVideoPlayer videoPlayer;
    private OrientationUtils orientationUtils;
    private XBanner xBanner;


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_news);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        getSupportActionBar().setDisplayShowCustomEnabled(true);

        // find component
        news = (SingleNews) getIntent().getSerializableExtra("news");

        if(news != null){

            initView();
        }
    }

    public void initView(){
        titleTextView = findViewById(R.id.new_detail_title_tv);
        titleTextView.setText(news.getTitle());

        contentTextView= findViewById(R.id.new_detail_content_tv);
        contentTextView.setText(news.getContent());

        publisherInfo = findViewById(R.id.news_detail_publisherInfo_tv);
        publisherInfo.setText(news.getPublisher());

        publisherInfo = findViewById(R.id.news_detail_publisherInfo_tv);
        publisherInfo.setText(news.getPublisher());

        publishTime = findViewById(R.id.news_detail_publishTime_tv);
        publishTime.setText(news.getPublishTime());

        xBanner = findViewById(R.id.xbanner);
        videoPlayer = findViewById(R.id.video_player);

        if(Variable.saveStreamMode){
            xBanner.setVisibility(View.GONE);
            videoPlayer.setVisibility(View.GONE);
        }
        else {
            initBanner();
            initVideoView();
        }

        SmartSwipe.wrap(this)
                .addConsumer(new ActivitySlidingBackConsumer(this))
                //设置联动系数
                .setRelativeMoveFactor(0.5F)
                //指定可侧滑返回的方向，如：enableLeft() 仅左侧可侧滑返回
                .enableLeft()
        ;
    }

    public void initBanner(){
        String[] images = news.getImage();
        List<String> imagesList = new ArrayList<>();
        for(String image : images){
            imagesList.add(image);
        }
        if(imagesList.size() == 0){
            xBanner.setVisibility(View.GONE);

        }
        else {
            xBanner.setData(imagesList, null);
            xBanner.loadImage(new XBanner.XBannerAdapter() {
                @Override
                public void loadBanner(XBanner banner, Object model, View view, int position) {
                    RequestOptions options = new RequestOptions()
                            .placeholder(new ColorDrawable(Color.BLACK))
                            .error(new ColorDrawable(Color.RED))
                            .centerCrop()
                            .priority(Priority.HIGH);
                    Glide.with(NewsDetailActivity.this)
                            .load(imagesList.get(position))
                            .apply(options)
                            .into((ImageView) view);
                }
            }
            );
        }
    }

    public void initVideoView(){
        String videoUrl = news.getVideo();
        if(videoUrl.equals("")){
            videoPlayer.setVisibility(View.GONE);
        }
        else{
            //String videoUrl = "http://9890.vod.myqcloud.com/9890_4e292f9a3dd011e6b4078980237cc3d3.f20.mp4";
            videoPlayer.setUp(videoUrl, true, "测试视频");
            ImageView imageView = new ImageView(this);
            imageView.setScaleType(ImageView.ScaleType.CENTER_CROP);
            imageView.setImageResource(R.drawable.ic_favorite_white_24dp);
            videoPlayer.setThumbImageView(imageView);
            //增加title
            videoPlayer.getTitleTextView().setVisibility(View.VISIBLE);
            //设置返回键
            videoPlayer.getBackButton().setVisibility(View.VISIBLE);
            //设置旋转
            orientationUtils = new OrientationUtils(this, videoPlayer);
            //设置全屏按键功能,这是使用的是选择屏幕，而不是全屏
            videoPlayer.getFullscreenButton().setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    orientationUtils.resolveByClick();
                }
            });
            //是否可以滑动调整
            videoPlayer.setIsTouchWiget(true);
            //设置返回按键功能
            videoPlayer.getBackButton().setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    onBackPressed();
                }
            });
            videoPlayer.startPlayLogic();
        }
    }

    @Override
    public void onBackPressed() {
        if (GSYVideoManager.backFromWindowFull(this)) {
            return;
        }
        super.onBackPressed();
    }

    @Override
    protected void onPause() {
        super.onPause();
        GSYVideoManager.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        GSYVideoManager.onResume();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        GSYVideoManager.releaseAllVideos();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu){
        getMenuInflater().inflate(R.menu.hide,menu);

        switchbutton=(Switch) menu.findItem(R.id.switchbutton).getActionView().findViewById(R.id.switchForActionBar);

        if(news != null){
            if(collectionsDao.contain(news.getNewsID())){
                switchbutton.setChecked(true);
            }
        }

        if(switchbutton != null){
            switchbutton.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
                @Override
                public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                    if(b)
                    {
                        collectionsDao.add(
                                news.getNewsID(),
                                news.getImageString(),
                                news.getVideo(),
                                news.getPublishTime(),
                                news.getPublisher(),
                                news.getTitle(),
                                news.getContent(),
                                news.getKeywords()
                        );
                    }
                    else
                    {
                        collectionsDao.delete(news.getNewsID());
                    }
                }
            });
        }
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item){
        switch (item.getItemId()){
            case R.id.myShare:
                showShare();
                break;
            case android.R.id.home:
                finish();
                break;
            default:
        }
        return true;
    }

    private void showShare() {

        OnekeyShare oks = new OnekeyShare();
        // title标题，微信、QQ和QQ空间等平台使用
        oks.setTitle(news.getTitle());
        // titleUrl QQ和QQ空间跳转链接
        //oks.setTitleUrl(news.getUrl());
        // text是分享文本，所有平台都需要这个字段
        oks.setText(news.getAbstract());
        //分享网络图片，新浪微博分享网络图片需要通过审核后申请高级写入接口，否则请注释掉测试新浪微博
        if(news.getImage().length!=0)
            oks.setImageUrl(news.getImage()[0]);
        else
        // imagePath是图片的本地路径，确保SDcard下面存在此张图片
        //换成我们最后的APP图标放在对的路径里
        {
            {
                BitmapDrawable draw=(BitmapDrawable) ContextCompat.getDrawable(this,R.drawable.ic_desktop);
                oks.setImageData(draw.getBitmap());
            }

        }

        // url在微信、Facebook等平台中使用
        oks.setUrl(news.getUrl());
        // 启动分享GUI
        oks.show(this);

//        //Intent method
//        Intent shareIntent = new Intent();
//        shareIntent.setAction(Intent.ACTION_SEND);
//        //shareIntent.setData(Uri.parse("myapp://dosomething"));
//        shareIntent.putExtra(Intent.EXTRA_TEXT, news.getAbstract() + "\n" + news.getUrl());
//        shareIntent.setType("text/plain");
//        startActivity(Intent.createChooser(shareIntent, "send to..."));
    }
};
