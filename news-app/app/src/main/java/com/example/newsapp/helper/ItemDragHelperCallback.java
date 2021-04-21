package com.example.newsapp.helper;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.ItemTouchHelper;
import androidx.recyclerview.widget.RecyclerView;

import com.example.newsapp.Interfaces.OnMyChannelItemDragListener;
import com.example.newsapp.Interfaces.OnMyChannelItemMoveListener;
import com.example.newsapp.R;

public class ItemDragHelperCallback extends ItemTouchHelper.Callback {
    @Override
    public int getMovementFlags(@NonNull RecyclerView recyclerView, @NonNull RecyclerView.ViewHolder viewHolder) {
        int dragFlags;
        dragFlags = ItemTouchHelper.UP | ItemTouchHelper.DOWN | ItemTouchHelper.LEFT | ItemTouchHelper.RIGHT;
        int swipeFlags = 0;
        return makeMovementFlags(dragFlags, swipeFlags);
    }

    @Override
    public boolean onMove(@NonNull RecyclerView recyclerView, @NonNull RecyclerView.ViewHolder viewHolder, @NonNull RecyclerView.ViewHolder target) {
        if(viewHolder.getItemViewType() != target.getItemViewType())
            return false;
//        if(recyclerView.getAdapter() instanceof OnMyChannelItemMoveListener){
//        }
        OnMyChannelItemMoveListener onMyChannelItemMoveListener = (OnMyChannelItemMoveListener)recyclerView.getAdapter();
        onMyChannelItemMoveListener.onItemMove(viewHolder.getAdapterPosition(), target.getAdapterPosition());
        return true;
    }

    @Override
    public void onSwiped(@NonNull RecyclerView.ViewHolder viewHolder, int direction) {

    }

    @Override
    public boolean isLongPressDragEnabled() {
         return false;
    }

    @Override
    public boolean isItemViewSwipeEnabled() {
        return false;
    }

    /*
        为了让item选中时颜色变黑，凸显选中状态, 通过增加接口 OnMyChannelItemDragListener 实现
        分别重写 onSelectedChanged clearView 函数
    */
    @Override
    public void onSelectedChanged(RecyclerView.ViewHolder viewHolder, int actionState) {
        if (actionState != ItemTouchHelper.ACTION_STATE_IDLE) {
            if (viewHolder instanceof OnMyChannelItemDragListener) {
                OnMyChannelItemDragListener itemViewHolder = (OnMyChannelItemDragListener) viewHolder;
                itemViewHolder.onItemSelected();
            }
        }
        super.onSelectedChanged(viewHolder, actionState);
    }

    @Override
    public void clearView(RecyclerView recyclerView, RecyclerView.ViewHolder viewHolder) {
        if (viewHolder instanceof OnMyChannelItemDragListener) {
            OnMyChannelItemDragListener itemViewHolder = (OnMyChannelItemDragListener) viewHolder;
            itemViewHolder.onItemFinished();
        }
        super.clearView(recyclerView, viewHolder);
    }
}
