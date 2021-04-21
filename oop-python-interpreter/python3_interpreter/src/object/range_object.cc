#include "range_object.h"
#include "int_object.h"
RangeTypeObject *RangeTypeObject::Singleton(){
    static auto singleton=new RangeTypeObject();
    return singleton;
}
RangeTypeObject::RangeTypeObject():TypeObject("range"){
    this->iter_get_iter=new UnaryFuncPtrWrapper([](Object *oa)->Object*{
        auto a=dynamic_cast<RangeObject*>(oa);
        assert(a!=nullptr);
        return new RangeIterObject(a);
    });
}
RangeObject::RangeObject(int _l,int _r,int _step):Object(RangeTypeObject::Singleton()),l(_l),r(_r),step(_step){
    assert(step!=0);
}
RangeObject::RangeObject(int _l,int _r):RangeObject(_l,_r,1){}
RangeObject::RangeObject(int _r):RangeObject(0,_r){}

RangeIterTypeObject *RangeIterTypeObject::Singleton(){
    static auto singleton=new RangeIterTypeObject();
    return singleton;
}
RangeIterTypeObject::RangeIterTypeObject():TypeObject("range_iterator"){
    this->iter_get_next=new UnaryFuncPtrWrapper([](Object *oa)->Object*{
        auto a=dynamic_cast<RangeIterObject*>(oa);
        assert(a!=nullptr);
        auto x=a->x;
        if (a->range_obj->step>0){
            if (x>=a->range_obj->r) return nullptr;
        } else{
            if (x<=a->range_obj->r) return nullptr;
        }
        a->x+=a->range_obj->step;
        return new IntObject(x);
    });
}
RangeIterObject::RangeIterObject(RangeObject *_range_obj):Object(RangeIterTypeObject::Singleton()),range_obj(_range_obj),x(range_obj->l){
}
