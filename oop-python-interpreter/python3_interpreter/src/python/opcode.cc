#include "opcode.h"
bool HasArgument(int opcode){
    assert(opcode!=HAVE_ARGUMENT);
    return opcode>HAVE_ARGUMENT;
}
std::string CodeMap(int opcode){
    switch(opcode){
        case DUP_TOP:
            return "DUP_TOP";
        case POP_TOP:
            return "POP_TOP";
        case ROT_TWO:
            return "ROT_TWO";
        case BINARY_ADD:
            return "BINARY_ADD";
        case BINARY_SUBTRACT:
            return "BINARY_SUBTRACT";
        case BINARY_MULTIPLY:
            return "BINARY_MULTIPLY";
        case BINARY_FLOOR_DIVIDE:
            return "BINARY_FLOOR_DIVIDE";
        case BINARY_TRUE_DIVIDE:
            return "BINARY_TRUE_DIVIDE";
        case BINARY_MODULE:
            return "BINARY_MODULE";
        case BINARY_POWER:
            return "BINARY_POWER";
        case BINARY_LSHIFT:
            return "BINARY_LSHIFT";
        case BINARY_RSHIFT:
            return "BINARY_RSHIFT";
        case BINARY_OR:
            return "BINARY_OR";
        case BINARY_XOR:
            return "BINARY_XOR";
        case BINARY_AND:
            return "BINARY_AND";
        case INPLACE_ADD:
            return "INPLACE_ADD";
        case INPLACE_SUBTRACT:
            return "INPLACE_SUBTRACT";
        case INPLACE_MULTIPLY:
            return "INPLACE_MULTIPLY";
        case INPLACE_FLOOR_DIVIDE:
            return "INPLACE_FLOOR_DIVIDE";
        case INPLACE_TRUE_DIVIDE:
            return "INPLACE_TRUE_DIVIDE";
        case INPLACE_MODULE:
            return "INPLACE_MODULE";
        case INPLACE_POWER:
            return "INPLACE_POWER";
        case INPLACE_LSHIFT:
            return "INPLACE_LSHIFT";
        case INPLACE_RSHIFT:
            return "INPLACE_RSHIFT";
        case INPLACE_OR:
            return "INPLACE_OR";
        case INPLACE_XOR:
            return "INPLACE_XOR";
        case INPLACE_AND:
            return "INPLACE_AND";
        case UNARY_NEGATIVE:
            return "UNARY_NEGATIVE";
        case STORE_NAME:
            return "STORE_NAME";
        case STORE_FAST:
            return "STORE_FAST";
        case STORE_GLOBAL:
            return "STORE_GLOBAL";
        case STORE_CELL_REF:
            return "STORE_CELL_REF";
        case STORE_FREE_REF:
            return "STORE_FREE_REF";
        case LOAD_NAME:
            return "LOAD_NAME";
        case LOAD_FAST:
            return "LOAD_FAST";
        case LOAD_CONST:
            return "LOAD_CONST";
        case LOAD_GLOBAL:
            return "LOAD_GLOBAL";
        case LOAD_CELL_REF:
            return "LOAD_CELL_REF";
        case LOAD_FREE_REF:
            return "LOAD_FREE_REF";
        case MAKE_TUPLE:
            return "MAKE_TUPLE";
        case MAKE_LIST:
            return "MAKE_LIST";
        case UNPACK_TUPLE:
            return "UNPACK_TUPLE";
        case UNPACK_LIST:
            return "UNPACK_LIST";
        case LOAD_CELL_CLOSURE:
            return "LOAD_CELL_CLOSURE";
        case LOAD_FREE_CLOSURE:
            return "LOAD_FREE_CLOSURE";
        case CALL_FUNCTION:
            return "CALL_FUNCTION";
        case MAKE_FUNCTION:
            return "MAKE_FUNCTION";
        case GET_ITER:
            return "GET_ITER";
        case FOR_ITER:
            return "FOR_ITER";
        case RETURN_VALUE:
            return "RETURN_VALUE";
        case JUMP_ABSOLUTE:
            return "JUMP_ABSOLUTE";
        case JUMP_FORWARD:
            return "JUMP_FORWARD";
        case POP_JUMP_IF_FALSE:
            return "POP_JUMP_IF_FALSE";
        case POP_JUMP_IF_TRUE:
            return "POP_JUMP_IF_TRUE";
        case JUMP_IF_FALSE_OR_POP:
            return "JUMP_IF_FALSE_OR_POP";
        case JUMP_IF_TRUE_OR_POP:
            return "JUMP_IF_TRUE_OR_POP";
        case COMPARE_EQ:
            return "COMPARE_EQ";
        case COMPARE_NE:
            return "COMPARE_NE";
        case COMPARE_LT:
            return "COMPARE_LT";
        case COMPARE_LE:
            return "COMPARE_LE";
        case COMPARE_GT:
            return "COMPARE_GT";
        case COMPARE_GE:
            return "COMPARE_GE";
        case COMPARE_IS:
            return "COMPARE_IS";
        case COMPARE_IS_NOT:
            return "COMPARE_IS_NOT";
        case COMPARE_IN:
            return "COMPARE_IN";
        case COMPARE_NOT_IN:
            return "COMPARE_NOT_IN";
        case NEXT_ITER:
            return "NEXT_ITER";
        case END_ITER:
            return "END_ITER";
        case LOAD_ATTR:
            return "LOAD_ATTR";
        case STORE_ATTR:
            return "STORE_ATTR";
        case LOAD_BUILD_CLASS:
            return "LOAD_BUILD_CLASS";
        case GET_ITEM:
            return "GET_ITEM";
        case SET_ITEM:
            return "SET_ITEM";
        case YIELD_VALUE:
            return "YIELD_VALUE";
        case GEN2LIST:
            return "GEN2LIST";
        default:
            return std::to_string(opcode);
    }
}
