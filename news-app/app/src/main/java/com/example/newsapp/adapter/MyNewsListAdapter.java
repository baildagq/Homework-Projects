package com.example.newsapp.adapter;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.recyclerview.widget.RecyclerView;

import com.bumptech.glide.Glide;
import com.bumptech.glide.Priority;
import com.bumptech.glide.load.DataSource;
import com.bumptech.glide.load.engine.GlideException;
import com.bumptech.glide.request.RequestListener;
import com.bumptech.glide.request.RequestOptions;
import com.bumptech.glide.request.target.Target;
import com.example.newsapp.R;
import com.example.newsapp.database.NewsHistoryDao;
import com.example.newsapp.model.SingleNews;
import com.example.newsapp.util.Constant;
import com.example.newsapp.util.Variable;

import java.util.List;

public class MyNewsListAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> {
    private List<SingleNews> newsList;
    private Context context;
    private NewsHistoryDao historyDao = new NewsHistoryDao();
    private int listType;



    //点击 RecyclerView 某条的监听
    public interface OnItemClickListener{
        void onItemClick(View view, int position);

    }

    private OnItemClickListener onItemClickListener;

    /**
     * 设置RecyclerView某个的监听
     * @param onItemClickListener
     */
    public void setOnItemClickListener(OnItemClickListener onItemClickListener){
        this.onItemClickListener = onItemClickListener;
    }

    public MyNewsListAdapter(List<SingleNews> newsList, Context context, int listType){
        this.newsList = newsList;
        this.context = context;
        this.listType = listType;
    }

    public class MyViewHolder extends RecyclerView.ViewHolder {
        TextView title;
        TextView contentAbstract;
        TextView publishInfo;
        ImageView image_right;
        String newsID;
        LinearLayout linearLayout;

        public MyViewHolder(View itemView){
            super(itemView);

            linearLayout = itemView.findViewById(R.id.item_liner_layout);
            title = itemView.findViewById(R.id.item_newsTitle);
            contentAbstract = itemView.findViewById(R.id.item_newsAbstract);
            publishInfo = itemView.findViewById(R.id.item_newsPublishInfo);
            image_right = itemView.findViewById(R.id.image_right);
            newsID = "";

            itemView.setOnClickListener(new View.OnClickListener(){
                @Override
                public void onClick(View view) {
                    if(onItemClickListener != null){
                        onItemClickListener.onItemClick(view, getAdapterPosition());
                    }
                }
            });
        }
    }

    @NonNull
    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View view;
        view = LayoutInflater.from(context)
                .inflate(R.layout.item_news_list, parent, false);
        return new MyViewHolder(view);
    }

    @SuppressLint("ResourceAsColor")
    @Override
    public void onBindViewHolder(@NonNull RecyclerView.ViewHolder holders, int position) {
        MyViewHolder holder = (MyViewHolder) holders;
        SingleNews news = newsList.get(position);

        // 读过变色
        if(listType == Constant.LIST_TYPE_NEWS && historyDao.contain(news.getNewsID())){
        //    holder.linearLayout.setBackgroundColor(R.color.colorReaded);
        //    holder.title.setBackgroundColor(R.color.grey);
        //    holder.contentAbstract.setBackgroundColor(R.color.grey);
        //    holder.publishInfo.setBackgroundColor(R.color.grey);
            holder.title.setTextColor(R.color.grey);
            holder.contentAbstract.setTextColor(R.color.grey);
            holder.publishInfo.setTextColor(R.color.grey);
        }

        if(!holder.newsID.equals(news.getNewsID())){
            holder.title.setText(news.getTitle());
            holder.contentAbstract.setText(news.getAbstract());
            holder.publishInfo.setText("  " + news.getPublisher() + "     " + news.getPublishTime());

            String urls[]= news.getImage();//images lists

            if(urls.length>0 && !Variable.saveStreamMode)
            {
                RequestListener mRequestListener = new RequestListener() {
                    @Override
                    public boolean onLoadFailed(@Nullable GlideException e, Object model, Target target, boolean isFirstResource) {
                        Log.d("NewsDetailActivity", "onException: " + e.toString() + "  model:" + model + " isFirstResource: " + isFirstResource);
                        holder.image_right.setImageResource(R.mipmap.ic_launcher);
                        return false;
                    }
                    @Override
                    public boolean onResourceReady(Object resource, Object model, Target target, DataSource dataSource, boolean isFirstResource) {
                        Log.e("NewsDetailActivity",  "model:"+model+" isFirstResource: "+isFirstResource);
                        return false;
                    }
                };
                RequestOptions options = new RequestOptions()
                        //.centerCrop()
                        .placeholder(new ColorDrawable(Color.BLACK))
                        .error(new ColorDrawable(Color.RED))
                        .centerCrop()
                        .priority(Priority.HIGH);

                Glide.with(context.getApplicationContext())
                        .load( urls[0])
                        .apply(options)
                        .listener(mRequestListener)
                        .into(holder.image_right);
            }
            else{
                holder.image_right.setVisibility(View.GONE);
            }
        }
    }

    @Override
    public int getItemCount() {
        return newsList.size();
    }

    @Override
    public int getItemViewType(int position) {
        return position;
    }
}
