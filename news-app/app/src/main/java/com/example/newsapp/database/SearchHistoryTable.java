package com.example.newsapp.database;

public class SearchHistoryTable{
    /**
     * 查询历史纪录表
     */
    public static final String TABLENAME = "SearchHistoryTable";

    /**
     * 字段部分
     */
    public static final String SEARCH_WORD = "search_word";

    /**
     * 字段ID 数据库操作建立字段对应关系 从0开始
     */
    public static final int ID_SEARCH_WORD = 0;

    /**
     * 创建表
     */
    public static final String CREATE_TABLE = "create table if not exists " + TABLENAME + "(" +
            SEARCH_WORD + " text primary key unique) ";
}
