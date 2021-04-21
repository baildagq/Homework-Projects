## 我们的关关关关键指导：

**1. <https://github.com/haerulmuttaqin/PopularNews> ** 

**2.<https://github.com/iMeiji/Toutiao>**





## Done: UI

#### MainActivity

* Navigation drawer Activity (**Now Chosen**))
* Tabbed Activity
* 或者组合

#### NewsDetailActivity

* .......



## Done: Network

- [x] Retrofit : 网络请求，返回数据处理



## TODO: Data List

- [x] RecycleView: ListView 升级版
- [x] CardView:  beautify
- [x] SwipeRefreshLayout: 下拉刷新
- [x] easyRefreshLayout: 上拉加载更多
- [x] Click jump to news detail page



## Done: Main Activity

- [x] Search:  words
- [x] Settings:



## Done: NewsDetail Activity

- [x] 由MainActivity页面向 NewsDetail 页面传递信息：选择了传递自定义对象SingleNews,要不然一项一项传入过于麻烦了， 需要 SingleNews 类 实现 Serializable接口（不需要做什么）
- [x] Glide : 图片加载框架：用于新闻具体页面中图片(其他选择：picasso, fresco)
- [x] 左上角返回上一级
- [x] 分享
- [x] 收藏
- [x] 保存本地



## Done: 本地存储



## 遇到的(大)坑/问题 + 解决

* <https://blog.csdn.net/qq_32623363/article/details/54895817> 神他么巨坑



## 下拉刷新，上拉加载更多的实现方式（选择了方式3）

1. SwipeRreshLayout + Recyclerview: 
   1. SwipeRreshLayout 实现下拉刷新
   2. RecyclerView 通过添加底部 FootView 方式，实现上拉加载更多
2. [BaseRecyclerViewAdapterHelper](http://www.recyclerview.org/)
   1. 内部集成了许多功能，感觉很方便实现
   2. 但是单纯的通过Adapter实现下拉刷新有些困难（奇怪），所以一般还是通过SwipeFreshLayout配合实现
3. [BaseRecyclerViewAdapterHelper](http://www.recyclerview.org/) + [EasyRefreshLayout](<https://github.com/anzaizai/EasyRefreshLayout>)
   1. 配合使用，不用Base实现上拉下拉，用EasyRefreshLayout实现



## 关键词搜索+历史记录

1. 开始使用了Android 自带的控件，美观好用但是无法进行历史记录
2. 打算新建SearchActivity手写实现，但是感觉过于麻烦（其实自己后来查找框架的时间应该也能手写出来了）
3. 尝试了几个SearchView框架，[floatingSearchView](<https://github.com/arimorty/floatingsearchview>) 对于实现Suggestions(History)的方式有点看不明白，又尝试了，看到过内置好数据库的[MaterialSearchView](<https://github.com/lapism/MaterialSearchView>)，但是也没用起来，最后用了[MaterialSearchBar](<https://github.com/mancj/MaterialSearchBar>), 加上自己实现的数据库
4. **目前问题**：
   1. 因为想要实现当搜索词清空后返回主界面，关键词变为空，所以进行了一些设置，但是现在导致每次点击搜索框不干什么事也会导致重新加载（删去了增加的设置，感觉正常的逻辑也是可以的）
   2. 键盘不会自己下降（已修复）
   3. Bug: list里第一个记录无法删除





|          | 功能     | 子功能                               | 分   | 完成情况 | 子性能         | 分   | 完成情况 |
| -------- | -------- | ------------------------------------ | ---- | -------- | -------------- | ---- | -------- |
| 基础功能 | 系统支持 | 正常运行不崩溃                       | 8    | Done     | 不卡顿         | 2    | Done     |
|          | 页面布局 | 合理正确                             | 8    | Done     | 美观、图片     | 2    | Done     |
|          | 分类列表 | 删除添加                             | 4    | Done     | 修改动态       | 1    | Done     |
|          | 新闻列表 | 正确显示,进入                        | 8    | Done     | 图片、视频     | 2    | Done     |
|          |          | 本地存储<br />离线浏览<br />页面标灰 | 8    | Done     | 存储大量新闻   | 2    | Done     |
|          |          | 上拉更多<br />下拉刷新               | 4    | Done     | 上拉、下拉特效 | 1    | Done     |
|          |          | 显示来源、时间                       | 4    | Done     | 合理美观       | 1    |          |
|          |          | 新闻关键词搜索<br />历史记录         | 4    | Done     | 合理美观       | 1    |          |
|          | 分享     | 分享                                 | 4    |          | 合理美观       | 1    |          |
|          |          | 收藏添加删除                         | 4    | Done     | 合理美观       | 1    |          |
|          |          | 所有收藏列表                         | 4    | Done     | 合理美观       | 1    |          |
|          | 推荐     | 看过新闻推荐                         | 8    | Done     | 推荐准确       | 2    |          |
| 附加     | 更换主题 | 夜间模式                             |      | Done     |                |      |          |
|          |          |                                      |      |          |                |      |          |
|          |          |                                      |      |          |                |      |          |




