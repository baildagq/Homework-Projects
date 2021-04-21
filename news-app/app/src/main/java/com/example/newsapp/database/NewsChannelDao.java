package com.example.newsapp.database;

import android.content.ContentValues;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;

import com.example.newsapp.MainActivity;
import com.example.newsapp.R;
import com.example.newsapp.bean.NewsChannelBean;
import com.example.newsapp.util.Constant;

import java.util.ArrayList;
import java.util.List;

public class NewsChannelDao {

    private SQLiteDatabase db;
    public  NewsChannelDao(){
        this.db = DatabaseHelper.getDatabase();
    }

    public void addInitData(){
        String categoryId[] = MainActivity.globalContext.getResources().getStringArray(R.array.news_category_id);
        String categoryName[] = MainActivity.globalContext.getResources().getStringArray(R.array.news_category_name);
        for (int i = 0; i < 6; i++) {
            add(categoryId[i], categoryName[i], Constant.NEWS_CHANNEL_ENABLE, i);
        }
        for (int i = 6; i < categoryId.length; i++) {
            add(categoryId[i], categoryName[i], Constant.NEWS_CHANNEL_DISABLE, i);
        }
    }

    public boolean add(String channelId, String channelName, int isEnable, int position) {
        ContentValues values = new ContentValues();
        values.put(NewsChannelTable.ID, channelId);
        values.put(NewsChannelTable.NAME, channelName);
        values.put(NewsChannelTable.IS_ENABLE, isEnable);
        values.put(NewsChannelTable.POSITION, position);
        long result = db.insertWithOnConflict(NewsChannelTable.TABLENAME, null, values, SQLiteDatabase.CONFLICT_REPLACE);
        return result != -1;
    }

    public List<NewsChannelBean> query(int isEnable) {
        Cursor cursor = db.query(NewsChannelTable.TABLENAME, null, NewsChannelTable.IS_ENABLE + "=?", new String[]{isEnable + ""}, null, null, null);
        List<NewsChannelBean> list = new ArrayList<>();
        while (cursor.moveToNext()) {
            NewsChannelBean bean = new NewsChannelBean();
            bean.setChannelId(cursor.getString(NewsChannelTable.ID_ID));
            bean.setChannelName(cursor.getString(NewsChannelTable.ID_NAME));
            bean.setIsEnable(cursor.getInt(NewsChannelTable.ID_ISENABLE));
            bean.setPosition(cursor.getInt(NewsChannelTable.ID_POSITION));
            list.add(bean);
        }
        cursor.close();
        return list;
    }

    public void updateAll(List<NewsChannelBean> list) {
    }

    public boolean removeAll() {
        int result = db.delete(NewsChannelTable.TABLENAME, null, null);
        return result != -1;
    }
}
