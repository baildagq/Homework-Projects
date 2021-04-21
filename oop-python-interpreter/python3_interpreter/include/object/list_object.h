/*************************************************************************
    > File Name: list_object.h
    > Author: stout
    > Mail: 18811181719@163.com 
    > Created Time: Sat 30 Jun 2018 02:48:33 PM DST
 ************************************************************************/
#pragma once
#include"object.h"
class ListTypeObject:public TypeObject{
	ListTypeObject();
public:
	static ListTypeObject *Singleton();
	Object *Set(Object* a, Object *oi, Object *o);
	Object *Get(Object* a, Object *oi);
	Object *Size(Object* a);
	Object *Append(Object* a,Object *o);
	Object *Pop(Object *oa,Object *oi = nullptr);
	Object *Count(Object* a, Object* o);
	Object *Reverse(Object* a);
};

class ListObject:public Object{
	friend class ListTypeObject;
	std::vector<Object*> vec;
public:
	ListObject(int n);
    ListObject(const std::vector<Object*>& v);
    std::vector<Object*>& ToVector();
    void Set(int i, Object *obj);
    Object *Get(int i);
    int Size();
    void PushBack(Object *obj);
    std::string ToString() override;
    int GetSize() override{return (int)this->vec.size();}
    Object *Pop(Object *obj=nullptr);
    Object *Count(Object* b);
    void Reverse();
    void Clear();
    void Insert(Object* i,Object* obj);
    void Remove(Object* obj);
};
class ListIterTypeObject:public TypeObject{
    ListIterTypeObject();
public:
    static ListIterTypeObject *Singleton();
};
class ListIterObject:public Object{
    friend class ListIterTypeObject;
    ListObject *list_obj;
    int x=0;
public:
    ListIterObject(ListObject *_list_obj);
    std::string ToString() override{return "list iter of "+list_obj->ToString();}
};
