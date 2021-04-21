#pragma once
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <cstdint>
#include <algorithm>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <set>
#include <map>
#include <memory>
class CustomException:public std::exception{
    std::string str;
public:
    CustomException(std::string str):str(str){}
    const char *what() const noexcept override{
        return str.c_str();
    }
};
void Assert(bool flag,std::string msg);
