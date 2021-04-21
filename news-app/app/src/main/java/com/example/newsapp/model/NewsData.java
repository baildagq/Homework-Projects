package com.example.newsapp.model;

import com.google.gson.annotations.SerializedName;

import java.util.ArrayList;

public class NewsData {
    @SerializedName("pageSize")
    private Integer pageSize;
    @SerializedName("total")
    private Integer total;
    @SerializedName("data")
    private ArrayList<SingleNews> data;
    @SerializedName("currentPage")
    private Integer currentPage;

    NewsData(Integer pageSize,
         Integer total,
         ArrayList<SingleNews> data,
         Integer currentPage
    ){
        this.pageSize = pageSize;
        this.total = total;
        this.data = data;
        this.currentPage = currentPage;
    }

    public Integer getPageSize(){
        return this.pageSize;
    }
    public ArrayList<SingleNews> getData(){
        return this.data;
    }
}
