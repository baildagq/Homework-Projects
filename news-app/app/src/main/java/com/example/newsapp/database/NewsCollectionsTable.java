package com.example.newsapp.database;

import com.example.newsapp.model.SingleNews;

public class NewsCollectionsTable {
    /**
     * 新闻收藏表
     */
    public static final String TABLENAME = "NewsCollectionsTable";

    /**
     * 字段部分
     */
    public static final String ID = "newsID";
    public static final String IMAGE = "image";
    public static final String VIDEO = "video";
    public static final String PUBLISHTIME = "publishTime";
    public static final String PUBLISHER = "publisher";
    public static final String TITLE = "title";
    public static final String CONTENT = "content";
    public static final String KEYWORDS = "keywords";

    /**
     * 字段ID 数据库操作建立字段对应关系 从0开始
     */
    public static final int ID_ID = 0;
    public static final int ID_IMAGE = 1;
    public static final int ID_VIDEO = 2;
    public static final int ID_PUBLISHTIME = 3;
    public static final int ID_PUBLISHER = 4;
    public static final int ID_TITLE = 5;
    public static final int ID_CONSTANT = 6;
    public static final int ID_KEYWORDS = 7;

    /**
     * 创建表
     */
    public static final String CREATE_TABLE = "create table if not exists " + TABLENAME + "(" +
            ID + " text primary key unique, " +
            IMAGE + " text, " +
            VIDEO + " text, " +
            PUBLISHTIME + " text, " +
            PUBLISHER + " text, " +
            TITLE + " text, " +
            CONTENT + " text, " +
            KEYWORDS + " text) ";
}
