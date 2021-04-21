package com.example.newsapp.adapter;

import android.content.Context;
import android.graphics.Bitmap;
import android.os.Handler;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.Animation;
import android.view.animation.TranslateAnimation;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.ItemTouchHelper;
import androidx.recyclerview.widget.RecyclerView;

import com.example.newsapp.Interfaces.OnMyChannelItemDragListener;
import com.example.newsapp.Interfaces.OnMyChannelItemMoveListener;
import com.example.newsapp.R;
import com.example.newsapp.bean.NewsChannelBean;

import java.util.List;

public class MyNewsChannelAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> implements OnMyChannelItemMoveListener{
    public static final int TYPE_ENABLE_CHANNEL_HEADER = 0;
    public static final int TYPE_ENABLE = 1;
    public static final int TYPE_DISABLE_CHANNEL_HEADER = 2;
    public static final int TYPE_DISABLE = 3;

    // enable频道之前的header数量  该demo中 即标题部分 为 1
    private static final int COUNT_PRE_ENABLE_HEADER = 1;
    // disable频道之前的header数量  该demo中 即标题部分 为 COUNT_PRE_ENABLE_HEADER + 1
    private static final int COUNT_PRE_DISABLE_HEADER = COUNT_PRE_ENABLE_HEADER+ 1;
    // 间隔时间
    private static final long ANIM_TIME = 360L;

    private LayoutInflater layoutInflater;
    private ItemTouchHelper itemTouchHelper;
    private List<NewsChannelBean> enableItems, disableItems;
    private Handler delayHandler = new Handler();

    public MyNewsChannelAdapter(Context context, ItemTouchHelper helper, List<NewsChannelBean> enableItems, List<NewsChannelBean> disableItems){
        this.layoutInflater =LayoutInflater.from(context);
        this.itemTouchHelper =helper;
        this.enableItems = enableItems;
        this.disableItems = disableItems;
    }

    @NonNull
    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(@NonNull final ViewGroup parent, int viewType) {
        View view;
        switch (viewType) {
            case TYPE_ENABLE_CHANNEL_HEADER:
                view = layoutInflater.inflate(R.layout.item_channel_enable_header, parent, false);
                final EnableHeaderHolder enableHeaderHolder = new EnableHeaderHolder(view);
                return enableHeaderHolder;

            case TYPE_ENABLE:
                view = layoutInflater.inflate(R.layout.item_channel_enable, parent, false);
                final EnableItemHolder enableItemHolder = new EnableItemHolder(view);
                enableItemHolder.textView.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        int position = enableItemHolder.getAdapterPosition();
                        RecyclerView recyclerView = (RecyclerView) parent;
                        RecyclerView.LayoutManager manager = recyclerView.getLayoutManager();
                        View currentView = manager.findViewByPosition(position);
                        View targetView = manager.findViewByPosition(enableItems.size() - 1 + COUNT_PRE_ENABLE_HEADER);
                        if (recyclerView.indexOfChild(targetView) >= 0) {
                            int targetX, targetY;

                            int spanCount = ((GridLayoutManager) manager).getSpanCount();

                            // 移动后 高度将变化 (我的频道Grid 最后一个item在新的一行第一个)
                            if ((enableItems.size() - COUNT_PRE_ENABLE_HEADER) % spanCount == 0) {
                                View preTargetView = recyclerView.getLayoutManager().findViewByPosition(enableItems.size() + COUNT_PRE_DISABLE_HEADER - 1);
                                targetX = preTargetView.getLeft();
                                targetY = preTargetView.getTop();
                            } else {
                                targetX = targetView.getLeft();
                                targetY = targetView.getTop();
                            }

                            moveEnToDis(enableItemHolder);
                            startAnimation(recyclerView, currentView, targetX, targetY);

                        } else {
                            moveEnToDis(enableItemHolder);
                        }

                    }
                });

                enableItemHolder.textView.setOnLongClickListener(v -> {
                    itemTouchHelper.startDrag(enableItemHolder);
                    return true;
                });
                return enableItemHolder;


            case TYPE_DISABLE_CHANNEL_HEADER:
                view = layoutInflater.inflate(R.layout.item_channel_disable_header, parent, false);
                return new RecyclerView.ViewHolder(view) {};

            case TYPE_DISABLE:
                view = layoutInflater.inflate(R.layout.item_channel_disable, parent, false);
                final DisableItemHolder disableItemHolder = new DisableItemHolder(view);
                disableItemHolder.textView.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        int position = disableItemHolder.getAdapterPosition();
                        RecyclerView recyclerView = (RecyclerView) parent;
                        RecyclerView.LayoutManager manager = recyclerView.getLayoutManager();
                        View currentView = manager.findViewByPosition(position);
                        // 目标位置的前一个item  即当前EnableChannel的最后一个
                        View preTargetView = manager.findViewByPosition(enableItems.size() - 1 + 1);

                        // 如果targetView不在屏幕内,则为-1  此时不需要添加动画,因为此时notifyItemMoved自带一个向目标移动的动画  如果在屏幕内,则添加一个位移动画
                        if (recyclerView.indexOfChild(preTargetView) >= 0) {
                            int targetX = preTargetView.getLeft();
                            int targetY = preTargetView.getTop();
                            int targetPosition = enableItems.size() - 1 + COUNT_PRE_DISABLE_HEADER;
                            GridLayoutManager gridLayoutManager = (GridLayoutManager) manager;
                            int spanCount = gridLayoutManager.getSpanCount();

                            // target 在最后一行第一个
                            if ((targetPosition - COUNT_PRE_ENABLE_HEADER) % spanCount == 0) {
                                View targetView = manager.findViewByPosition(targetPosition);
                                targetX = targetView.getLeft();
                                targetY = targetView.getTop();
                            } else {
                                targetX += preTargetView.getWidth();
                                // 最后一个item可见
                                if (gridLayoutManager.findLastVisibleItemPosition() == getItemCount() - 1) {
                                    // 最后的item在最后一行第一个位置
                                    if ((getItemCount() - 1 - enableItems.size() - COUNT_PRE_DISABLE_HEADER) % spanCount == 0) {
                                        // RecyclerView实际高度 > 屏幕高度 && RecyclerView实际高度 < 屏幕高度 + item.height
                                        int firstVisiblePostion = gridLayoutManager.findFirstVisibleItemPosition();
                                        if (firstVisiblePostion == 0) {
                                            // FirstCompletelyVisibleItemPosition == 0 即 内容不满一屏幕 , targetY值不需要变化
                                            // // FirstCompletelyVisibleItemPosition != 0 即 内容满一屏幕 并且 可滑动 , targetY值 + firstItem.getTop
                                            if (gridLayoutManager.findFirstCompletelyVisibleItemPosition() != 0) {
                                                int offset = (-recyclerView.getChildAt(0).getTop()) - recyclerView.getPaddingTop();
                                                targetY += offset;
                                            }
                                        } else { // 在这种情况下 并且 RecyclerView高度变化时(即可见第一个item的 position != 0),
                                            // 移动后, targetY值  + 一个item的高度
                                            targetY += preTargetView.getHeight();
                                        }
                                    }
                                } else {
                                    System.out.println("current--No");
                                }
                            }
                            // 如果当前位置是otherChannel可见的最后一个
                            // 并且 当前位置不在grid的第一个位置
                            // 并且 目标位置不在grid的第一个位置
                            // 则 需要延迟250秒 notifyItemMove , 这是因为这种情况 , 并不触发ItemAnimator , 会直接刷新界面
                            // 导致我们的位移动画刚开始,就已经notify完毕,引起不同步问题
                            if (position == gridLayoutManager.findLastVisibleItemPosition()
                                    && (position - enableItems.size() - COUNT_PRE_DISABLE_HEADER) % spanCount != 0
                                    && (targetPosition - COUNT_PRE_ENABLE_HEADER) % spanCount != 0) {
                                moveDisToEnWithDelay(disableItemHolder);
                            } else {
                                moveDisToEn(disableItemHolder);
                            }
                            startAnimation(recyclerView, currentView, targetX, targetY);
                        } else {
                            moveDisToEn(disableItemHolder);
                        }
                    }
                });
                return disableItemHolder;
        }
        return null;
    }

    // 开始增删动画
    private void startAnimation(RecyclerView recyclerView, final View currentView, float targetX, float targetY) {
        final ViewGroup viewGroup = (ViewGroup) recyclerView.getParent();
        final ImageView mirrorView = addMirrorView(viewGroup, recyclerView, currentView);

        Animation animation = getTranslateAnimator(
                targetX - currentView.getLeft(), targetY - currentView.getTop());
        currentView.setVisibility(View.INVISIBLE);
        mirrorView.startAnimation(animation);

        animation.setAnimationListener(new Animation.AnimationListener() {
            @Override
            public void onAnimationStart(Animation animation) {
            }

            @Override
            public void onAnimationEnd(Animation animation) {
                viewGroup.removeView(mirrorView);
                if (currentView.getVisibility() == View.INVISIBLE) {
                    currentView.setVisibility(View.VISIBLE);
                }
            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }
        });
    }
    // 添加需要移动的镜像View
    private ImageView addMirrorView(ViewGroup parent, RecyclerView recyclerView, View view) {
        /**
         * 我们要获取cache首先要通过setDrawingCacheEnable方法开启cache，然后再调用getDrawingCache方法就可以获得view的cache图片了。
         buildDrawingCache方法可以不用调用，因为调用getDrawingCache方法时，若果cache没有建立，系统会自动调用buildDrawingCache方法生成cache。
         若想更新cache, 必须要调用destoryDrawingCache方法把旧的cache销毁，才能建立新的。
         当调用setDrawingCacheEnabled方法设置为false, 系统也会自动把原来的cache销毁。
         */
        view.destroyDrawingCache();
        view.setDrawingCacheEnabled(true);
        final ImageView mirrorView = new ImageView(recyclerView.getContext());
        Bitmap bitmap = Bitmap.createBitmap(view.getDrawingCache());
        mirrorView.setImageBitmap(bitmap);
        view.setDrawingCacheEnabled(false);
        int[] locations = new int[2];
        view.getLocationOnScreen(locations);
        int[] parenLocations = new int[2];
        recyclerView.getLocationOnScreen(parenLocations);
        FrameLayout.LayoutParams params = new FrameLayout.LayoutParams(bitmap.getWidth(), bitmap.getHeight());
        params.setMargins(locations[0], locations[1] - parenLocations[1], 0, 0);
        parent.addView(mirrorView, params);

        return mirrorView;
    }
    // 获取位移动画
    private TranslateAnimation getTranslateAnimator(float targetX, float targetY) {
        TranslateAnimation translateAnimation = new TranslateAnimation(
                Animation.RELATIVE_TO_SELF, 0f,
                Animation.ABSOLUTE, targetX,
                Animation.RELATIVE_TO_SELF, 0f,
                Animation.ABSOLUTE, targetY);
        // RecyclerView默认移动动画250ms 这里设置360ms 是为了防止在位移动画结束后 remove(view)过早 导致闪烁
        translateAnimation.setDuration(ANIM_TIME);
        translateAnimation.setFillAfter(true);
        return translateAnimation;
    }

    private void moveEnToDis(EnableItemHolder enableItemHolder) {
        int position = enableItemHolder.getAdapterPosition();

        int startPosition = position - COUNT_PRE_ENABLE_HEADER;
        if (startPosition > enableItems.size() - 1) {
            return;
        }
        NewsChannelBean item = enableItems.get(startPosition);
        enableItems.remove(startPosition);
        disableItems.add(0, item);

        notifyItemMoved(position, enableItems.size() + COUNT_PRE_DISABLE_HEADER);
    }
    private void moveDisToEn(DisableItemHolder disableItemHolder) {
        int position = processItemRemoveAdd(disableItemHolder);
        if (position == -1) {
            return;
        }
        notifyItemMoved(position, enableItems.size() - 1 + COUNT_PRE_ENABLE_HEADER);
    }
    private void moveDisToEnWithDelay(DisableItemHolder disableItemHolder) {
        final int position = processItemRemoveAdd(disableItemHolder);
        if (position == -1) {
            return;
        }
        delayHandler.postDelayed(() -> notifyItemMoved(position, enableItems.size() - 1 + COUNT_PRE_ENABLE_HEADER), ANIM_TIME);
    }
    private int processItemRemoveAdd(DisableItemHolder disableItemHolder) {
        int position = disableItemHolder.getAdapterPosition();
        int startPosition = position - enableItems.size() - COUNT_PRE_DISABLE_HEADER;
        if (startPosition > disableItems.size() - 1) {
            return -1;
        }
        NewsChannelBean item = disableItems.get(startPosition);
        disableItems.remove(startPosition);
        enableItems.add(item);
        return position;
    }

    @Override
    public void onBindViewHolder(@NonNull RecyclerView.ViewHolder holder, int position) {
        if(holder instanceof EnableItemHolder){
            EnableItemHolder enableItemHolder = (EnableItemHolder)holder;
            enableItemHolder.textView.setText(enableItems.get(position - COUNT_PRE_ENABLE_HEADER).getChannelName());
        }
        if(holder instanceof DisableItemHolder){
            DisableItemHolder disableItemHolder = (DisableItemHolder)holder;
            disableItemHolder.textView.setText(disableItems.get(position - enableItems.size() - COUNT_PRE_DISABLE_HEADER).getChannelName());
        }
    }

    @Override
    public int getItemCount() {
        return enableItems.size() + disableItems.size() + COUNT_PRE_DISABLE_HEADER;
    }

    @Override
    public int getItemViewType(int position) {
        if (position == 0) {    // 我的频道 标题部分
            return TYPE_ENABLE_CHANNEL_HEADER;
        } else if (position == enableItems.size() + 1) {    // 其他频道 标题部分
            return TYPE_DISABLE_CHANNEL_HEADER;
        } else if (position > 0 && position < enableItems.size() + 1) {
            return TYPE_ENABLE;
        } else {
            return TYPE_DISABLE;
        }
    }
    @Override
    public void onItemMove(int fromPosition, int toPosition) {
        NewsChannelBean item = enableItems.get(fromPosition - COUNT_PRE_ENABLE_HEADER);
        enableItems.remove(fromPosition - COUNT_PRE_ENABLE_HEADER);
        enableItems.add(toPosition - COUNT_PRE_ENABLE_HEADER, item);
        notifyItemMoved(fromPosition, toPosition);
    }

    class EnableHeaderHolder extends RecyclerView.ViewHolder{
        public EnableHeaderHolder(@NonNull View itemView) {
            super(itemView);
        }
    }
    class EnableItemHolder extends RecyclerView.ViewHolder implements OnMyChannelItemDragListener {
        public TextView textView;
        public EnableItemHolder(@NonNull View itemView) {
            super(itemView);
            textView = itemView.findViewById(R.id.item_channel_enable_tv);
        }
        /**
         * item 被选中时
         */
        @Override
        public void onItemSelected() {
            textView.setBackgroundResource(R.color.textColorPrimary);
        }

        /**
         * item 取消选中时
         */
        @Override
        public void onItemFinished() {
            textView.setBackgroundResource(R.color.viewBackground);
        }
    }
    class DisableItemHolder extends RecyclerView.ViewHolder{
        public TextView textView;
        public DisableItemHolder(@NonNull View itemView) {
            super(itemView);
            textView = itemView.findViewById(R.id.item_channel_disable_tv);
        }
    }

    public List<NewsChannelBean> getEnableItems(){
        return enableItems;
    }
    public List<NewsChannelBean> getDisableItems(){
        return disableItems;
    }
}
