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
