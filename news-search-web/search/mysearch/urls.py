from django.urls import path
from django.conf.urls import url

from . import view

#app_name = 'mysearch'
urlpatterns = [
#	path('', views.index, name = 'index'),
#    path('<int:question_id>/', views.detail, name = 'detail'),
#	path('<int:question_id>/results/', views.results, name = 'results'),
#	path('<int:question_id>/vote/', views.vote, name = 'vote'),
    url(r'^$', view.all),
    url(r'^search$', view.search),
    url(r'^page$', view.page),
    url(r'^all$', view.all),
]
