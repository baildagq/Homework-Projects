#include "eval.h"
#include "opcode.h"
#include "call.h"
namespace{
template <typename T,typename D> void EnsureSize(T &vec,int n,const D &empty){
    while ((int)vec.size()<=n) vec.push_back(empty);
}
}
Evaluator::Evaluator(CodeObject *_codeobject,std::map<std::string,Object*> *_builtins,std::map<std::string,Object*> *_globals,std::map<std::string,Object*> *_locals,TupleObject *_closure):codeobject(_codeobject),builtins(_builtins),globals(_globals),locals(_locals),closure(_closure){
    for (int i=0;i<closure->GetSize();i++){
        auto value=dynamic_cast<RefObject*>(closure->Get(i));
        assert(value!=nullptr);
        this->freevars.push_back(value);
    }
}
void Evaluator::EnsureFastLocal(int i){
    EnsureSize(this->fastlocals,i,nullptr);
    //while ((int)this->fastlocals.size()<=i) this->fastlocals.push_back(nullptr);
}
void Evaluator::SetFastLocal(int i,Object *o){
    this->EnsureFastLocal(i);
    this->fastlocals[i]=o;
}
Object *Evaluator::GetFastLocal(int i){
    this->EnsureFastLocal(i);
    return this->fastlocals[i];
}
Object *Evaluator::GetLocal(const std::string &name){
    auto it=this->locals->find(name);
    assert(it!=this->locals->end());
    return it->second;
}
Object *Evaluator::Eval(){
    assert(eval_index>=0);
    if (first){
        for (auto pos:codeobject->cellargs){
            this->cellvars.push_back(new RefObject(pos!=-1?GetFastLocal(pos):nullptr));
        }
    } else first=false;
    //puts("\neval begin:\n");

    for (;eval_index<(int)codeobject->codes.size();eval_index++){
        auto code=codeobject->codes[eval_index];
        auto opcode=code->opcode;
        auto oparg=code->oparg;
        //printf("%d:%s %s\n",eval_index,CodeMap(opcode).c_str(),oparg>=0?std::to_string(oparg).c_str():"");
        switch(opcode){
            case DUP_TOP:{
                auto obj=stack.top();
                stack.push(obj);
                break;
            }
            case POP_TOP:{
                stack.pop();
                break;
            }
            case ROT_TWO:{
                auto first=stack.top();stack.pop();
                auto second=stack.top();stack.pop();
                stack.push(first);
                stack.push(second);
                break;
            }
            case STORE_FAST:{
                this->SetFastLocal(oparg,stack.top());stack.pop();
                break;
            }
            case LOAD_FAST:{
                auto value=this->GetFastLocal(oparg);
                //printf("load fast %d %s\n",oparg,value->ToString().c_str());
                stack.push(value);
                break;
            }
            case STORE_NAME:{
                std::string name=codeobject->names[oparg];
                //printf("store name %s\n",name.c_str());
                (*this->locals)[name]=stack.top();
                stack.pop();
                break;
            }
            case LOAD_NAME:{
                std::string name=codeobject->names[oparg];
                //printf("load name %s\n",name.c_str());
                auto it=this->locals->find(name);
                if (it!=this->locals->end()){
                    stack.push(it->second);
                    break;
                }
                it=this->globals->find(name);
                if (it!=this->globals->end()){
                    stack.push(it->second);
                    break;
                }
                it=this->builtins->find(name);
                if (it!=this->builtins->end()){
                    stack.push(it->second);
                    break;
                }
                assert(false);
            }
            case LOAD_CONST:{
                stack.push(codeobject->consts[oparg]);
                //printf("load const %s\n",stack.top()->ToString().c_str());
                break;
            }
            case LOAD_GLOBAL:{
                const std::string &name=codeobject->names[oparg];
                //printf("load global %s\n",name.c_str());
                auto it=this->globals->find(name);
                if (it!=this->globals->end()){
                    stack.push(it->second);
                    break;
                }
                it=this->builtins->find(name);
                if (it!=this->builtins->end()){
                    stack.push(it->second);
                    break;
                }
                assert(false);
            }
            case STORE_GLOBAL:{
                const std::string &name=codeobject->names[oparg];
                (*this->globals)[name]=stack.top();stack.pop();
                break;
            }
            case LOAD_CELL_REF:{
                auto &cell=cellvars[oparg];
                auto value=cell->GetRef();
                stack.push(value);
                break;
            }
            case LOAD_FREE_REF:{
                auto &free=freevars[oparg];
                auto value=free->GetRef();
                stack.push(value);
                break;
            }
            case STORE_CELL_REF:{
                auto value=stack.top();stack.pop();
                auto &cell=this->cellvars[oparg];
                cell->SetRef(value);
                break;
            }
            case STORE_FREE_REF:{
                auto value=stack.top();stack.pop();
                auto &free=this->freevars[oparg];
                free->SetRef(value);
                break;
            }
            case MAKE_TUPLE:{
                auto tuple=new TupleObject(oparg);
                for (int i=oparg;i--;){
                    tuple->Set(i,stack.top());stack.pop();
                }
                stack.push(tuple);
                break;
            }
            case MAKE_LIST:{
                auto list=new ListObject(oparg);
                for (int i=oparg;i--;){
                    list->Set(i,stack.top());stack.pop();
                }
                stack.push(list);
                break;
            }
            case LOAD_CELL_CLOSURE:{
                stack.push(this->cellvars[oparg]);
                break;
            }
            case LOAD_FREE_CLOSURE:{
                stack.push(this->freevars[oparg]);
                break;
            }
            case CALL_FUNCTION:{
                std::vector<Object*> args;
                for (int i=oparg;i--;){
                    args.push_back(stack.top());stack.pop();
                }
                auto func=stack.top();stack.pop();
                std::reverse(args.begin(),args.end());
                stack.push(Call(func,args,{}));
                break;
            }
            case MAKE_FUNCTION:{
                auto name=stack.top();stack.pop();
                auto code=dynamic_cast<CodeObject*>(stack.top());stack.pop();
                assert(code!=nullptr);
                auto closure=dynamic_cast<TupleObject*>(stack.top());stack.pop();
                assert(closure!=nullptr);
                auto func_object=new FuncObject(name->ToString(),code,this->builtins,this->globals,closure);
                stack.push(func_object); 
                break;
            }
            case RETURN_VALUE:{
                eval_index=-1;
                return !stack.empty()?stack.top():nullptr;
            }
            case GET_ITER:{
                auto iterable=stack.top();stack.pop();
                assert(iterable->type_obj->iter_get_iter!=nullptr);
                auto iter=(*iterable->type_obj->iter_get_iter)(iterable);
                stack.push(iter);
                break;
            }
            case NEXT_ITER:{
                auto iter=stack.top();
                assert(iter->type_obj->iter_get_next!=nullptr);
                auto next=(*iter->type_obj->iter_get_next)(iter);
                assert(next!=nullptr);
                stack.push(next);
                break;
            }
            case END_ITER:{
                auto iter=stack.top();stack.pop();
                assert(iter->type_obj->iter_get_next!=nullptr);
                auto next=(*iter->type_obj->iter_get_next)(iter);
                assert(next==nullptr);
                break;
            }
            case FOR_ITER:{
                auto iter=stack.top();
                assert(iter->type_obj->iter_get_next!=nullptr);
                auto next=(*iter->type_obj->iter_get_next)(iter);
                if (next!=nullptr){
                    stack.push(next);
                    break;
                } else{
                    stack.pop();
                    eval_index+=oparg;
                    break;
                }
                assert(false);
            }
            case JUMP_ABSOLUTE:{
                eval_index=oparg-1;
                break;
            }
            case JUMP_FORWARD:{
                eval_index+=oparg;
                break;
            }
            case POP_JUMP_IF_TRUE:{
                auto value=stack.top();stack.pop();
                if (value->ToBool()) eval_index=oparg-1;
                break;
            }
            case POP_JUMP_IF_FALSE:{
                auto value=stack.top();stack.pop();
                if (!value->ToBool()) eval_index=oparg-1;
                break;
            }
            case JUMP_IF_TRUE_OR_POP:{
                auto value=stack.top();
                if (value->ToBool()){
                    eval_index=oparg-1;
                } else{
                    stack.pop();
                }
                break;
            }
            case JUMP_IF_FALSE_OR_POP:{
                auto value=stack.top();
                if (!value->ToBool()){
                    eval_index=oparg-1;
                } else{
                    stack.pop();
                }
                break;
            }
            case BINARY_ADD:
            case BINARY_SUBTRACT:
            case BINARY_MULTIPLY:
            case BINARY_FLOOR_DIVIDE:
            case BINARY_MODULE:
            case BINARY_LSHIFT:
            case BINARY_RSHIFT:
            case BINARY_OR:
            case BINARY_XOR:
            case BINARY_AND:{
                auto right=stack.top();stack.pop();
                auto left=stack.top();stack.pop();
                Object *res=nullptr;
                switch(opcode){
                    case BINARY_ADD:
                        res=NumberMethod::NumberAdd(left,right);
                        break;
                    case BINARY_SUBTRACT:
                        res=NumberMethod::NumberSubtract(left,right);
                        break;
                    case BINARY_MULTIPLY:
                        res=NumberMethod::NumberMultiply(left,right);
                        break;
                    case BINARY_FLOOR_DIVIDE:
                        res=NumberMethod::NumberFloorDiv(left,right);
                        break;
                    case BINARY_TRUE_DIVIDE:
                        res=NumberMethod::NumberTrueDiv(left,right);
                        break;
                    case BINARY_MODULE:
                        res=NumberMethod::NumberModule(left,right);
                        break;
                    case BINARY_LSHIFT:
                        res=NumberMethod::NumberLShift(left,right);
                        break;
                    case BINARY_RSHIFT:
                        res=NumberMethod::NumberRShift(left,right);
                        break;
                    case BINARY_OR:
                        res=NumberMethod::NumberBitOr(left,right);
                        break;
                    case BINARY_XOR:
                        res=NumberMethod::NumberBitXor(left,right);
                        break;
                    case BINARY_AND:
                        res=NumberMethod::NumberBitAnd(left,right);
                        break;
                    default:
                        assert(false);
                }
                if (res==nullptr) printf("opr between %s and %s not implemented\n",left->type_obj->name.c_str(),right->type_obj->name.c_str());
                assert(res!=nullptr);
                stack.push(res);
                //stack.push((*static_cast<IntObject*>(left))+(*static_cast<IntObject*>(right)));
                break;
            }
            case GET_ITEM:{
                auto index = stack.top();stack.pop();
                auto str=stack.top();stack.pop();
                auto func=str->type_obj->tp_getitem;
                assert(func!=nullptr);
                stack.push((*func)(str,index));
                break;
            }
            case SET_ITEM:{
                auto sub=stack.top();stack.pop();
                auto obj=stack.top();stack.pop();
                auto v=stack.top();stack.pop();
                auto func=obj->type_obj->tp_setitem;
                assert(func!=nullptr);
                (*func)(obj,sub,v);
                break;
            }
            case BUILD_SLICE:{
                auto step = stack.top();stack.pop();
                auto upper = stack.top();stack.pop();
                auto lower = stack.top();stack.pop();
                Object* slice=new SliceObject(lower,upper,step);
                assert(slice!=nullptr);
                stack.push(slice);
                break;
            }
            case INPLACE_ADD:
            case INPLACE_SUBTRACT:
            case INPLACE_MULTIPLY:
            case INPLACE_FLOOR_DIVIDE:
            case INPLACE_MODULE:
            case INPLACE_LSHIFT:
            case INPLACE_RSHIFT:
            case INPLACE_OR:
            case INPLACE_XOR:
            case INPLACE_AND:{
                auto right=stack.top();stack.pop();
                auto left=stack.top();stack.pop();
                BinaryFunc *func=nullptr;
                switch(opcode){
                    case INPLACE_ADD:
                        func=left->type_obj->nb_inplace_add;
                        break;
                    case INPLACE_SUBTRACT:
                        func=left->type_obj->nb_inplace_subtract;
                        break;
                    case INPLACE_MULTIPLY:
                        func=left->type_obj->nb_inplace_multiply;
                        break;
                    case INPLACE_FLOOR_DIVIDE:
                        func=left->type_obj->nb_inplace_floor_div;
                        break;
                    case INPLACE_MODULE:
                        func=left->type_obj->nb_inplace_module;
                        break;
                    case INPLACE_LSHIFT:
                        func=left->type_obj->nb_inplace_lshift;
                        break;
                    case INPLACE_RSHIFT:
                        func=left->type_obj->nb_inplace_rshift;
                        break;
                    case INPLACE_OR:
                        func=left->type_obj->nb_inplace_bit_or;
                        break;
                    case INPLACE_XOR:
                        func=left->type_obj->nb_inplace_bit_xor;
                        break;
                    case INPLACE_AND:
                        func=left->type_obj->nb_inplace_bit_and;
                        break;
                    default:
                        assert(false);
                }
                if (func==nullptr) printf("opr between %s and %s not implemented\n",left->type_obj->name.c_str(),right->type_obj->name.c_str());
                assert(func!=nullptr);
                stack.push((*func)(left,right));
                break;
            }
            case UNARY_POSITIVE:
            case UNARY_NEGATIVE:{
                auto value=stack.top();stack.pop();
                UnaryFunc *func=nullptr;
                switch(opcode){
                    case UNARY_POSITIVE:
                        func=value->type_obj->nb_positive;
                        break;
                    case UNARY_NEGATIVE:
                        func=value->type_obj->nb_negative;
                        break;
                    default:
                        assert(false);
                }
                assert(func!=nullptr);
                stack.push((*func)(value));
                break;
            }
            case COMPARE_EQ:
            case COMPARE_NE:
            case COMPARE_LT:
            case COMPARE_LE:
            case COMPARE_GT:
            case COMPARE_GE:{
                auto right=stack.top();stack.pop();
                auto left=stack.top();stack.pop();
                Object *res=nullptr;
                switch (opcode){
                    case COMPARE_EQ:
                        res=CompareMethod::CmpEQ(left,right);
                        break;
                    case COMPARE_NE:
                        res=CompareMethod::CmpNE(left,right);
                        break;
                    case COMPARE_LT:
                        res=CompareMethod::CmpLT(left,right);
                        break;
                    case COMPARE_LE:
                        res=CompareMethod::CmpLE(left,right);
                        break;
                    case COMPARE_GT:
                        res=CompareMethod::CmpGT(left,right);
                        break;
                    case COMPARE_GE:
                        res=CompareMethod::CmpGE(left,right);
                        break;
                    default:
                        assert(false);
                }
                assert(res!=nullptr);
                stack.push(res);
                break;
            }
            case LOAD_ATTR:{
                auto name=new StringObject(codeobject->names[oparg]);
                auto obj=stack.top();stack.pop();
                stack.push(obj->GetAttr(name));
                break;
            }
            case STORE_ATTR:{
                auto name=new StringObject(codeobject->names[oparg]);
                auto obj=stack.top();stack.pop();
                auto value=stack.top();stack.pop();
                obj->SetAttr(name,value);
                break;
            }
            case LOAD_BUILD_CLASS:{
                auto it=this->builtins->find("__build_class__");
                assert(it!=this->builtins->end());
                stack.push(it->second);
                break;
            }
            case MAKE_DICT:{
                auto dict=new DictObject();
                for (int i=oparg;i;i--){
                    auto value=stack.top();stack.pop();
                    auto key=stack.top();stack.pop();
                    dict->Set(key,value);
                }
                stack.push(dict);
                break;
            }
            case YIELD_VALUE:{
                auto obj=stack.top();stack.pop();
                eval_index++;
                return obj;
            }
            case GEN2LIST:{
                auto obj=stack.top();stack.pop();
                auto func_object=dynamic_cast<FuncObject*>(obj);
                assert(func_object!=nullptr);
                auto eval=new Evaluator(func_object->code,func_object->builtins,func_object->globals,new std::map<std::string,Object*>(),func_object->closure);
                auto list=new ListObject(0);
                for (;;){
                    auto res=eval->Eval();
                    if (res!=nullptr) list->PushBack(res);
                    else break;
                }
                stack.push(list);
                break;
            }
            default:
                printf("%d\n",opcode);
                assert(false);
        }
    }
    assert(stack.empty());
    eval_index=-1;
    return nullptr;
}
