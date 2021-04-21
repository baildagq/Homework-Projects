#pragma once
#include "common.h"
enum{
    DUP_TOP,
    POP_TOP,
    ROT_TWO,
    BINARY_ADD,
    BINARY_SUBTRACT,
    BINARY_MULTIPLY,
    BINARY_FLOOR_DIVIDE,
    BINARY_TRUE_DIVIDE,
    BINARY_MODULE,
    BINARY_POWER,
    BINARY_LSHIFT,
    BINARY_RSHIFT,
    BINARY_OR,
    BINARY_XOR,
    BINARY_AND,
    GET_ITEM,
    SET_ITEM,
    BUILD_SLICE,
    INPLACE_ADD,
    INPLACE_SUBTRACT,
    INPLACE_MULTIPLY,
    INPLACE_FLOOR_DIVIDE,
    INPLACE_TRUE_DIVIDE,
    INPLACE_MODULE,
    INPLACE_POWER,
    INPLACE_LSHIFT,
    INPLACE_RSHIFT,
    INPLACE_OR,
    INPLACE_XOR,
    INPLACE_AND,
    UNARY_INVERT,
    UNARY_NOT,
    UNARY_POSITIVE,
    UNARY_NEGATIVE,
    COMPARE_EQ,
    COMPARE_NE,
    COMPARE_LT,
    COMPARE_LE,
    COMPARE_GT,
    COMPARE_GE,
    COMPARE_IS,
    COMPARE_IS_NOT,
    COMPARE_IN,
    COMPARE_NOT_IN,
    RETURN_VALUE,
    GET_ITER,
    NEXT_ITER,
    END_ITER,
    LOAD_BUILD_CLASS,
    YIELD_VALUE,
    GEN2LIST,

    HAVE_ARGUMENT,

    LOAD_NAME,
    LOAD_FAST,
    LOAD_CONST,
    LOAD_GLOBAL,
    LOAD_CELL_REF,
    LOAD_FREE_REF,
    STORE_NAME,
    STORE_FAST,
    STORE_GLOBAL,
    STORE_CELL_REF,
    STORE_FREE_REF,
    JUMP_ABSOLUTE,
    JUMP_FORWARD,
    POP_JUMP_IF_FALSE,
    POP_JUMP_IF_TRUE,
    JUMP_IF_FALSE_OR_POP,
    JUMP_IF_TRUE_OR_POP,
    //EXTENDED_ARG,
    CALL_FUNCTION,
    MAKE_FUNCTION,
    MAKE_TUPLE,
    MAKE_LIST,
    UNPACK_TUPLE,
    UNPACK_LIST,
    LOAD_CELL_CLOSURE,
    LOAD_FREE_CLOSURE,
    FOR_ITER,
    LOAD_ATTR,
    STORE_ATTR,
    MAKE_DICT,
    MAKE_GENERATOR,
};
bool HasArgument(int opcode);
std::string CodeMap(int opcode);