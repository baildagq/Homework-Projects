package com.example.newsapp.adapter;

import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentPagerAdapter;
import androidx.fragment.app.FragmentStatePagerAdapter;

import com.example.newsapp.bean.NewsChannelBean;
import com.example.newsapp.database.NewsChannelDao;

import java.util.List;

public class MyPagerAdapter extends FragmentStatePagerAdapter {
    private List<Fragment> fragments;
    private List<NewsChannelBean> channelList;

    public MyPagerAdapter(FragmentManager fragmentManager, List<Fragment> fragments, List<NewsChannelBean> channelList){
        super(fragmentManager);
        this.fragments = fragments;
        this.channelList = channelList;
    }

    @Override
    public int getCount() {
        return fragments.size();
    }

    @Override
    public Fragment getItem(int position) {
        return fragments.get(position);
    }

    @Nullable
    @Override
    public CharSequence getPageTitle(int position) {
        return channelList.get(position).getChannelName();
    }
}
