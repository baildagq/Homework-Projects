# 小学期大作业文档 第3周

## 主要内容

* 作业

  * 爬虫部分

    * 抓取页面信息

      ![](https://raw.githubusercontent.com/qistout/2018summer_semester_week3_greatWork/master/mysearch/ReadmePic/Snipaste_2018-09-16_11-17-11.png)

    * 抓取当前页内链接

      ![](https://github.com/qistout/2018summer_semester_week3_greatWork/blob/master/mysearch/ReadmePic/spider2.png?raw=true)

  

  

  * web部分

    * 启动服务器

    * ![](https://github.com/qistout/2018summer_semester_week3_greatWork/blob/master/mysearch/ReadmePic/web1.png?raw=true)

    * 总浏览页面

      ![](https://github.com/qistout/2018summer_semester_week3_greatWork/blob/master/mysearch/ReadmePic/web2.png?raw=true)

      

    * 新闻页面

      ![](https://github.com/qistout/2018summer_semester_week3_greatWork/blob/master/mysearch/ReadmePic/web3.png?raw=true)

    * 搜索框和搜索用时

      ![](https://github.com/qistout/2018summer_semester_week3_greatWork/blob/master/mysearch/ReadmePic/web4.png?raw=true)

  * 

## 爬虫部分实现

* 实现思路
  * 数据爬取：
    * 爬取网页为新华网（因为单新闻页面会比较干净一点）
    * 进行爬虫之前因为人工对页面html进行了查看，确定了可爬取只有文字新闻的页面的url格式，爬取页面时对页面url进行一次判断，如果不是正常格式的话，会停止爬取该网页
    * 进行爬取时，会将该新闻页面中的链接加到链接的队列中去，供后续的爬虫进行使用
    * 进行爬取时，采用的是深搜的方式
  * 数据处理:
    * 爬取过程中，会将新闻页面数据存储到数据库中，同时对页面的标题和正文进行分词处理，将分得的每个词和当前id存到数据库中的另一份表单中，供后续检索使用



## web部分实现

* 实现思路：

  * 应用Django框架，主要函数为view.py中的几个函数，包括 all, search, page 函数，分别对应网络进行不同页面请求时的调用

  * 对于页面搜索的请求，会调用Server，引自同一文件夹下的Server.py函数，其中有一个server类，实例化后，可以对于请求的数据进行搜索

  * 上述实例化的server类，会在执行 python3 manage.py runserver 命令时，将之前爬虫爬好的数据加载进内存当中，加快后续搜索的速度

  * 函数返回时调用模板中的几个html文件，应用map进行替换掉

    

* web部分代码结构

* ![](https://github.com/qistout/2018summer_semester_week3_greatWork/blob/master/mysearch/ReadmePic/web5.png?raw=true)

* 关键代码 :

  * view.py:

    ```python
    from django.shortcuts import render, get_object_or_404
    from django.core.paginator import Paginator, EmptyPage, PageNotAnInteger
    from django.shortcuts import render_to_response
    from django.template import loader
    from django.http import HttpResponse
    from . import Server
    
    import re
    import time
    
    print('Initing searching server..')
    server = Server.Server('../pages.db')
    server.load_to_memory()
    print('Init finished')
    
    number_article_per_page = 10
    
    def index(request):
    	return render_to_response('index.html')
    
    def get_item_list(ids):
        if len(ids) == 0: return None
        item_list = []
        for i in range(0, len(ids)-1):
            id = ids[i]
            item = {'url': u'page?id=' + str(id), 'title': server.get_title(id), 'date': server.get_time(id), 'brief': server.get_brief(id)}
            item_list.append(item)
        return item_list
    
    def search(request):
    	time_start = time.time()
    	request.encoding = 'utf-8'
    	if 'text' in request.GET:
    		pass
    	else:
    		return HttpResponse('ERROR 404')
    
    	text = request.GET['text']
    	if len(text) == 0:
    		return render_to_response('index.html')
    
    	ids = server.search_item(text)
    	current_search_list = get_item_list(ids)
    	paginator = Paginator(current_search_list, 10)
    
    	page = 1
    	if 'pager' in request.GET:
    		page = int(request.GET['pager'])
    		print(page)
    #		try:
    #			page = page
    #		except PageNotAnInteger:
    #			page = 1
    #		except EmptyPage:
    #			page = paginator.num_pages
    
    	current_show_list = paginator.page(page)
    	
    	original_text = text
    	mapped = {}
    	mapped['page_title'] = text + u"input search keyword"
    	mapped['user_input'] = original_text
    	mapped['result_count'] = str(len(ids))
    	mapped['item_list'] = current_show_list
    	mapped['current_page'] = str(page)
    	mapped['total_pages'] = str(paginator.num_pages)
    	mapped['timer'] = str(time.time() - time_start)
    
    	return render(request, 'result.html', mapped)
    
    
    def all(request):
    	request.encoding = 'utf-8'
    
    		
    
    	ids = range(1, len(server.pages)+1)
    	current_search_list = get_item_list(ids)
    	paginator = Paginator(current_search_list, 10)
    
    	page = 1
    	if 'paper' in request.GET:
    		page = int(request.GET['papaer'])
    		try:
    			page = page
    		except PageNotAnInteger:
    			page = 1
    		except EmptyPage:
    			page = paginator.num_pages
    		current_show_list = paginator.page(page)
    	else:
    		current_show_list = paginator.page(1)
    
    	mapped = {}
    	mapped['page_title'] = u"input search keyword"
    	mapped['user_input'] = u""
    	mapped['result_count'] = str(len(ids))
    	mapped['item_list'] = current_show_list
    	mapped['current_page'] = str(page)
    	mapped['total_pages'] = str(paginator.num_pages)
    
    	return render(request, 'show.html', mapped)
    
    def page(request):
        request.encoding = 'utf-8'
        if 'id' in request.GET:
            pass
        else: return HttpResponse('ERROR 404')
    
        id = int(request.GET['id'])
        mapped = {}
        mapped['news_title'] = server.get_title(id)
        mapped['mod_date'] = server.get_time(id)
        mapped['main_content'] = server.get_content(id)
    
        return render(request, 'page.html', mapped)
    
    ```

  * Server.py

    ```python
    #-*-coding:utf-8-*-
    
    import os
    import re
    import gc
    #import Queue
    import sqlite3
    
    
    class Server:
    	def __init__(self, db_path):
    		if not os.path.exists(db_path):
    			raise RuntimeError('ERROR: The database file does not exist.')
    		self.db = sqlite3.connect(db_path)
    		self.max_brief_length = 75
    
    	def load_to_memory(self):
    
    		cursor = self.db.cursor()
    
            # fetch pages
    		cursor.execute('SELECT * from pages')
    		self.pages = cursor.fetchall()
    		print('The size of database is', len(self.pages))
    
            # fetch dict, while at this time , some same keywords is exist
    		cursor.execute('SELECT * from dicts')
    		dicts = cursor.fetchall()
    		print('The size of dictionary is', len(dicts))
    
    		#build dict
    		self.dict = {}
    		for pair in dicts:
    			if not pair[0] in self.dict:
    				self.dict[pair[0]] = [pair[1]]
    			else:
    				self.dict[pair[0]].append(pair[1])
    
    		del dicts
    		gc.collect()
    
    	def get_title(self, id):
    		return self.pages[id - 1][2]
    	def get_time(self, id):
    		return self.pages[id - 1][3]
    	def get_content(self, id):
    		return self.pages[id - 1][4].split()
    	def get_brief(self, id):
    		content = (self.pages[id - 1][4]).strip()
    		content.encode('utf-8')
    		return content[0 : min(self.max_brief_length, len(content))] + u"..."
    
    	def search_item(self, keyword):
    		return self.dict[keyword]
    
    
    ```

    