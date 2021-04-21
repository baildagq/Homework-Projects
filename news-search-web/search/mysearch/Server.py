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

