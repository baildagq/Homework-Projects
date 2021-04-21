package com.example.newsapp.util;

// util 为了标记 recyclerview中被选中的 item
public class SelectedItem {
    public static int selectedItemPosition = -1;
    public final static int TODO = 0;
    public final static int DATEEVENT = 1;


    public static int getSelectedItemPosition() {
        return selectedItemPosition;
    }

    public static void setSelectedNavItem(int selectedItemPosition) {
        SelectedItem.selectedItemPosition = selectedItemPosition;
    }
}
