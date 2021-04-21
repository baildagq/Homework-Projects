#include "slice_object.h"
#include "int_object.h"
SliceTypeObject *SliceTypeObject::Singleton(){
    static auto singleton=new SliceTypeObject();
    return singleton;
}

SliceTypeObject::SliceTypeObject():TypeObject("slice"){}

std::string SliceObject::ToString(){
    std::string str="slice(";
    std::string s_lower,s_upper,s_step;
    if(lower==nullptr)
        s_lower="None";
    else
        s_lower=std::to_string(*lower);
    if(upper==nullptr)
        s_upper="None";
    else
        s_upper=std::to_string(*upper);
    if(step==nullptr)
        s_step="None";
    else
        s_step=std::to_string(*step);
    str=str+s_lower+","+s_upper+","+s_step+")";
    return str;
}
SliceObject::SliceObject(Object* start, Object* stop, Object* len):Object(SliceTypeObject::Singleton()){
    if(start->ToString()=="None")
        lower = nullptr;
    else{
    IntObject *_lower=dynamic_cast<IntObject*>(start);
    assert(_lower!=nullptr);
    lower = new int(_lower->ToInt());
    }
    if(stop->ToString()=="None")
        upper = nullptr;
    else{
        IntObject *_upper=dynamic_cast<IntObject*>(stop);
        assert(_upper!=nullptr);
        upper = new int(_upper->ToInt());
    }
    if(len->ToString()=="None"){
        step = nullptr;
    }
    else{
        IntObject *_step=dynamic_cast<IntObject*>(len);
        assert(_step!=nullptr);
        step = new int(_step->ToInt());
    }
}











