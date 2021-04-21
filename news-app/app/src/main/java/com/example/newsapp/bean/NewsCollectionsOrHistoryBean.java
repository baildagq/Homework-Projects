package com.example.newsapp.bean;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

/*
    collection news or history news(readed)
 */
public class NewsCollectionsOrHistoryBean {

    private String newsID;
    private String image;
    private String video;
    private String publishTime;
    private String publisher;
    private String title;
    private String content;
    private String keywords;


    public void setNewsID(String newsID){
        this.newsID = newsID;
    }
    public String getNewsID(){
        return this.newsID;
    }

    public void setTitle(String title){
        this.title = title;
    }
    public String getTitle() {
        return this.title;
    }

    public void setContent(String content){
        this.content = content;
    }
    public String getContent() {
        return this.content;
    }
    public String getAbstract(){
        String a = this.content.substring(0, Math.min(this.content.length(), 110)) + "...";
        Pattern p = Pattern.compile("(\\s|\n)");
        Matcher m = p.matcher(a);
        a = m.replaceAll("");
        return a;
    }

    public void setPublisher(String publisher){
        this.publisher = publisher;
    }
    public String getPublisher(){return this.publisher;}

    public void setPublishTime(String publishTime){
        this.publishTime = publishTime;
    }
    public String getPublishTime(){return this.publishTime;}

    public void setImage(String image){
        this.image = image;
    }
    public String getImage(){
        return this.image;
    }

    public void setVideo(String video){
        this.video = video;
    }
    public String getVideo(){
        return this.video;
    }
    public void setKeywords(String keywords){this.keywords = keywords;}
    public String getKeywords(){return this.keywords;}
}
