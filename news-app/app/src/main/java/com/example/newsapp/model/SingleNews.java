package com.example.newsapp.model;

import com.google.gson.annotations.SerializedName;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import java.net.URL;


public class SingleNews implements Serializable {
    @SerializedName("publishTime")
    private String publishTime;
    @SerializedName("publisher")
    private String publisher;
    @SerializedName("language")
    private String language;
    @SerializedName("image")
    private String image;
    @SerializedName("video")
    private String video;
    @SerializedName("title")
    private String title;
    @SerializedName("content")
    private String content;
    @SerializedName("newsID")
    private String newsID;
    @SerializedName("category")
    private String category;

    @SerializedName("url")
    private String url;

    @SerializedName("persons")
    private ArrayList<Persons> persons;
    @SerializedName("locations")
    private ArrayList<Locations> locations;
    @SerializedName("organizations")
    private ArrayList<Organizations> organizations;

    @SerializedName("keywords")
    private ArrayList<ScoreAndWord> keyWords;
    @SerializedName("when")
    private ArrayList<ScoreAndWord> when;
    @SerializedName("where")
    private ArrayList<ScoreAndWord> where;
    @SerializedName("who")
    private ArrayList<ScoreAndWord> who;


    public SingleNews(
            String image,
            String publishTime,
            String publisher,
            String title,
            String content,

            String language,
            String video,
            String newsID,
            String category,

            ArrayList<Persons> persons,
            ArrayList<Locations> locations,
            ArrayList<Organizations> organzations,

            ArrayList<ScoreAndWord> when,
            ArrayList<ScoreAndWord> keyWords,
            ArrayList<ScoreAndWord> where,
            ArrayList<ScoreAndWord> who
    ){
        this.publishTime = publishTime;
        this.publisher = publisher;
        this.language = language;
        this.image = image;
        this.video = video;
        this.title = title;
        this.content = content;
        this.newsID = newsID;
        this.category = category;

        this.persons = persons;
        this.locations = locations;
        this.organizations = organzations;

        this.keyWords = keyWords;
        this.when = when;
        this.where = where;
        this.who = who;
    }

    public String getNewsID(){
        return this.newsID;
    }
    public String getTitle() {
        return this.title;
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
    public String getPublisher(){return this.publisher;}
    public String getPublishTime(){return this.publishTime;}
    public String getUrl(){return this.url;}
    public String getVideo(){
        return this.video;
    }
    public String getKeywords(){
        String key = "";
        if(keyWords != null){
            for(ScoreAndWord saw : keyWords){
                if (saw.score > 0.1){
                    key = key + saw.word + ",";
                }
            }
        }
        if(key.length() != 0)
            key = key.substring(0, key.length() - 1);
        return key;
    }
    public String getImageString(){
        return this.image;
    }
    public String[] getImage() {
        String tmpImage = image.substring(1,image.length()-1);
        String[] images;//
        if(tmpImage.length()>0) {
            tmpImage = tmpImage.replaceAll("\\]\\s+\\[", ",");
            images = tmpImage.split(",");
        }
        else {
            images=new String[0];
        }
        for(int i = 0;i < images.length;i ++){
            images[i].trim();
            images[i] = trimAllSpace(images[i]);
        }
        return images;
    }

    public static String trimAllSpace(String str) {
        return str == null ? str : str.replaceAll("^[\\s　]*|[\\s　]*$", "");
    }
}

class ScoreAndWord implements Serializable{
    @SerializedName("score")
    public float score;
    @SerializedName("word")
    public String word;

    ScoreAndWord(float score, String word){
        this.score = score;
        this.word = word;
    }

}

class Persons implements Serializable{
    @SerializedName("count")
    private Integer count;
    @SerializedName("linkedURL")
    private String linkedURL;
    @SerializedName("mention")
    private String mention;

    Persons(Integer count, String linkedURL, String mention){
        this.count = count;
        this.linkedURL = linkedURL;
        this.mention = mention;
    }
}

class Organizations implements Serializable{
    @SerializedName("count")
    private Integer count;
    @SerializedName("linkedURL")
    private String linkedURL;
    @SerializedName("mention")
    private String mention;

    Organizations(Integer count, String linkedURL, String mention){
        this.count = count;
        this.linkedURL = linkedURL;
        this.mention = mention;
    }
}

class Locations implements Serializable{
    @SerializedName("lng")
    private float lng;
    @SerializedName("count")
    private Integer count;
    @SerializedName("linkedURL")
    private String linkedURL;
    @SerializedName("lat")
    private float lat;
    @SerializedName("mention")
    private String mention;

    Locations(float lng, Integer count, String linkedURL, float lat, String mention){
        this.lng = lng;
        this.count = count;
        this.linkedURL = linkedURL;
        this.lat = lat;
        this.mention = mention;
    }
}
