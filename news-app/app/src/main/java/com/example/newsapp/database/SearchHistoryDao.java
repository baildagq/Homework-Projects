package com.example.newsapp.database;

import android.content.ContentValues;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;

import com.example.newsapp.MainActivity;
import com.example.newsapp.R;
import com.example.newsapp.bean.NewsChannelBean;
import com.example.newsapp.bean.SearchHistoryBean;
import com.example.newsapp.util.Constant;

import java.util.ArrayList;
import java.util.List;

public class SearchHistoryDao {
    private SQLiteDatabase db;
    public  SearchHistoryDao(){
        this.db = DatabaseHelper.getDatabase();
    }

    public void addInitData(){
    }

    public boolean add(String searchWord) {
        ContentValues values = new ContentValues();
        values.put(SearchHistoryTable.SEARCH_WORD, searchWord);
        long result = db.insertWithOnConflict(SearchHistoryTable.TABLENAME, null, values, SQLiteDatabase.CONFLICT_REPLACE);
        return result != -1;
    }

    public List<SearchHistoryBean> query() {
        Cursor cursor = db.query(SearchHistoryTable.TABLENAME, null, null, null, null, null, null);
        List<SearchHistoryBean> list = new ArrayList<>();
        while (cursor.moveToNext()) {
            SearchHistoryBean bean = new SearchHistoryBean();
            bean.setSearchWord(cursor.getString(SearchHistoryTable.ID_SEARCH_WORD));
            list.add(bean);
        }

        cursor.close();
        return list;
    }

    public void updateAll(List<SearchHistoryBean> list) {
    }

    public boolean removeAll() {
        int result = db.delete(SearchHistoryTable.TABLENAME, null, null);
        return result != -1;
    }
}
