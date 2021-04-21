#pragma once
#include "all_object.h"
Object *Call(Object *func,std::vector<Object*> args,std::map<std::string,Object*> kwargs);
