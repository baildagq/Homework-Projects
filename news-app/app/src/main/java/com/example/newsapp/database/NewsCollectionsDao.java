package com.example.newsapp.database;

import android.content.ContentValues;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;

import com.example.newsapp.bean.NewsCollectionsOrHistoryBean;

import java.util.ArrayList;
import java.util.List;

public class NewsCollectionsDao {

    private SQLiteDatabase db;
    public NewsCollectionsDao(){
        this.db = DatabaseHelper.getDatabase();
    }

    public void addInitData(){
    }

    public boolean add(String newsID, String image, String video, String publishTime, String publisher, String title, String content, String keyWords){
        ContentValues values = new ContentValues();
        values.put(NewsCollectionsTable.ID, newsID);
        values.put(NewsCollectionsTable.IMAGE, image);
        values.put(NewsCollectionsTable.VIDEO, video);
        values.put(NewsCollectionsTable.PUBLISHTIME, publishTime);
        values.put(NewsCollectionsTable.PUBLISHER, publisher);
        values.put(NewsCollectionsTable.TITLE, title);
        values.put(NewsCollectionsTable.CONTENT, content);
        values.put(NewsCollectionsTable.KEYWORDS, keyWords);
        long result = db.insertWithOnConflict(NewsCollectionsTable.TABLENAME, null, values, SQLiteDatabase.CONFLICT_REPLACE);
        return result != -1;
    }

    public boolean delete(String newsID){
        int result = db.delete(NewsCollectionsTable.TABLENAME, NewsCollectionsTable.ID + "=?", new String[]{newsID});
        return result != -1;
    }

    public boolean contain(String newsID){
        Cursor cursor = db.query(NewsCollectionsTable.TABLENAME, null, NewsCollectionsTable.ID + "=?", new String[]{newsID}, null, null, null, null);
        if(cursor.moveToNext()){
            return true;
        }
        cursor.close();
        return false;
    }

    public List<NewsCollectionsOrHistoryBean> query() {
        Cursor cursor = db.query(NewsCollectionsTable.TABLENAME, null, null, null, null, null, null);
        List<NewsCollectionsOrHistoryBean> list = new ArrayList<>();
        while (cursor.moveToNext()) {
            NewsCollectionsOrHistoryBean bean = new NewsCollectionsOrHistoryBean();

            bean.setNewsID(cursor.getString(NewsCollectionsTable.ID_ID));
            bean.setImage(cursor.getString(NewsCollectionsTable.ID_IMAGE));
            bean.setVideo(cursor.getString(NewsCollectionsTable.ID_VIDEO));
            bean.setPublishTime(cursor.getString(NewsCollectionsTable.ID_PUBLISHTIME));
            bean.setPublisher(cursor.getString(NewsCollectionsTable.ID_PUBLISHER));
            bean.setTitle(cursor.getString(NewsCollectionsTable.ID_TITLE));
            bean.setContent(cursor.getString(NewsCollectionsTable.ID_CONSTANT));
            bean.setKeywords(cursor.getString(NewsCollectionsTable.ID_KEYWORDS));

            list.add(bean);
        }
        cursor.close();
        return list;
    }

    public void updateAll(List<NewsCollectionsOrHistoryBean> list) {
    }

    public boolean removeAll() {
        int result = db.delete(NewsCollectionsTable.TABLENAME, null, null);
        return result != -1;
    }
    public List<String> getCollectionsKeyWords(){
        List<String> res = new ArrayList<>();
        Cursor cursor = db.query(NewsCollectionsTable.TABLENAME, null, null, null, null, null, null);
        if(cursor.moveToLast()){
            System.out.println("collection keyword" + cursor.getString(NewsCollectionsTable.ID_KEYWORDS));
            String[] ss = cursor.getString(NewsCollectionsTable.ID_KEYWORDS).split(",");
            for(int i = 0; i < Math.min(2, ss.length);i ++)
                res.add(ss[i]);
        }
        cursor.close();
        return res;
    }
}
