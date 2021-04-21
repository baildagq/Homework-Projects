#include "common.h"
void Assert(bool flag,std::string msg){
    if (flag) return;
    throw CustomException("AssertError: "+msg);
}
