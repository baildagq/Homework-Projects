package com.example.newsapp.database;

import android.content.ContentValues;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;

import com.example.newsapp.bean.NewsCollectionsOrHistoryBean;

import java.util.ArrayList;
import java.util.List;

public class NewsHistoryDao {

    private SQLiteDatabase db;
    public NewsHistoryDao(){
        this.db = DatabaseHelper.getDatabase();
    }

    public void addInitData(){
    }

    public boolean add(String newsID, String image, String video,String publishTime, String publisher, String title, String content, String keywords){
        ContentValues values = new ContentValues();
        values.put(NewsHistoryTable.ID, newsID);
        values.put(NewsHistoryTable.IMAGE, image);
        values.put(NewsHistoryTable.VIDEO, video);
        values.put(NewsHistoryTable.PUBLISHTIME, publishTime);
        values.put(NewsHistoryTable.PUBLISHER, publisher);
        values.put(NewsHistoryTable.TITLE, title);
        values.put(NewsHistoryTable.CONTENT, content);
        values.put(NewsHistoryTable.KEYWORDS, keywords);
        long result = db.insertWithOnConflict(NewsHistoryTable.TABLENAME, null, values, SQLiteDatabase.CONFLICT_REPLACE);
        return result != -1;
    }

    public List<NewsCollectionsOrHistoryBean> query() {
        Cursor cursor = db.query(NewsHistoryTable.TABLENAME, null, null, null, null, null, null);
        List<NewsCollectionsOrHistoryBean> list = new ArrayList<>();
        while (cursor.moveToNext()) {
            NewsCollectionsOrHistoryBean bean = new NewsCollectionsOrHistoryBean();
            bean.setNewsID(cursor.getString(NewsHistoryTable.ID_ID));
            bean.setImage(cursor.getString(NewsHistoryTable.ID_IMAGE));
            bean.setVideo(cursor.getString(NewsHistoryTable.ID_VIDEO));
            bean.setPublishTime(cursor.getString(NewsHistoryTable.ID_PUBLISHTIME));
            bean.setPublisher(cursor.getString(NewsHistoryTable.ID_PUBLISHER));
            bean.setTitle(cursor.getString(NewsHistoryTable.ID_TITLE));
            bean.setContent(cursor.getString(NewsHistoryTable.ID_CONSTANT));
            bean.setKeywords(cursor.getString(NewsHistoryTable.ID_KEYWORDS));
            list.add(bean);
        }
        cursor.close();
        return list;
    }

    public boolean contain(String newsID){
        Cursor cursor = db.query(NewsHistoryTable.TABLENAME, null, NewsCollectionsTable.ID + "=?", new String[]{newsID}, null, null, null, null);
        if(cursor.moveToNext()){
            return true;
        }
        return false;
    }

    public void updateAll(List<NewsCollectionsOrHistoryBean> list) {
    }

    public int getCount(){
        Cursor cursor = db.query(NewsHistoryTable.TABLENAME, null, null, null, null, null, null);
        int count = cursor.getCount();
        cursor.close();
        return count;
    }

    public boolean removeAll() {
        int result = db.delete(NewsHistoryTable.TABLENAME, null, null);
        return result != -1;
    }

    public List<String> getHistoryBrowseKeyWords(){
        List<String> res = new ArrayList<>();
        Cursor cursor = db.query(NewsHistoryTable.TABLENAME, null, null, null, null, null, null);
        if(cursor.moveToLast()){
            String[] ss = cursor.getString(NewsHistoryTable.ID_KEYWORDS).split(",");
            for(int i = 0; i < Math.min(2, ss.length);i ++)
                res.add(ss[i]);
        }
        if(cursor.moveToPrevious()){
            String[] ss = cursor.getString(NewsHistoryTable.ID_KEYWORDS).split(",");
            for(int i = 0; i < Math.min(2, ss.length);i ++)
                res.add(ss[i]);
        }
        if(cursor.moveToPrevious()){
            String[] ss = cursor.getString(NewsHistoryTable.ID_KEYWORDS).split(",");
            for(int i = 0; i < Math.min(2, ss.length);i ++)
                res.add(ss[i]);
        }
        cursor.close();
        return res;
    }
}
