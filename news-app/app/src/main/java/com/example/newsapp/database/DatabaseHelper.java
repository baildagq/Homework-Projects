package com.example.newsapp.database;

import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

import com.example.newsapp.MainActivity;

public class DatabaseHelper extends SQLiteOpenHelper {
    private static final String DB_NAME = "NewsApp";
    private static final int DB_VERSION = 1;
    private static final String CLEAR_TABLE_DATA = "delete from ";
    private static final String DROP_TABLE = "drop table if exists ";
    private static DatabaseHelper instance = null;
    private static SQLiteDatabase db = null;

    private DatabaseHelper(Context context, String name, SQLiteDatabase.CursorFactory factory, int version) {
        super(context, name, factory, version);
    }

    @Override
    public void onCreate(SQLiteDatabase db) {
        db.execSQL(NewsChannelTable.CREATE_TABLE);
        db.execSQL(NewsCollectionsTable.CREATE_TABLE);
        db.execSQL(NewsHistoryTable.CREATE_TABLE);
        db.execSQL(SearchHistoryTable.CREATE_TABLE);
    }

    // 升级数据库时需要用到，然而我们现在并不需要..emm..
    @Override
    public void onUpgrade(SQLiteDatabase sqLiteDatabase, int i, int i1) {

    }

    public static synchronized DatabaseHelper getInstance() {
        if (instance == null) {
            instance = new DatabaseHelper(MainActivity.globalContext, DB_NAME, null, DB_VERSION);
        }
        return instance;
    }
    public static synchronized SQLiteDatabase getDatabase() {
        if (db == null) {
            db = getInstance().getWritableDatabase();
        }
        return db;
    }
    public static synchronized void closeDatabase() {
        if (db != null) {
            db.close();
        }
    }
}
