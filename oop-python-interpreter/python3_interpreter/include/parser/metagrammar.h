#pragma once
#include "common.h"
#include "token.h"
#include "grammar.h"
enum{
    MSTART=NT_OFFSET,
    RULE,
    RHS,
    ALT,
    ITEM,
    ATOM
};
Grammar *MetaGrammar();
