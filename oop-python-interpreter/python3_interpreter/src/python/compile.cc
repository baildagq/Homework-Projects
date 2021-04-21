#include "compile.h"
#include "parser.h"
#include "ast.h"
#include "graminit.h"
#include "opcode.h"
namespace{
int AddMap(std::map<std::string,int> &map,const std::string &str){
    auto it=map.find(str);
    if (it!=map.end()) return it->second;
    map[str]=(int)map.size();
    return map[str];
}

int BinOP(operator_ty *op){
    switch (op->kind){
        case AddKind:
            return BINARY_ADD;
        case SubKind:
            return BINARY_SUBTRACT;
        case MultKind:
            return BINARY_MULTIPLY;
        case DivKind:
            return BINARY_TRUE_DIVIDE;
        case ModKind:
            return BINARY_MODULE;
        case PowKind:
            return BINARY_POWER;
        case LSHIFTKind:
            return BINARY_LSHIFT;
        case RSHIFTKind:
            return BINARY_RSHIFT;
        case BitOrKind:
            return BINARY_OR;
        case BitXorKind:
            return BINARY_XOR;
        case BitAndKind:
            return BINARY_AND;
        case FloorDivKind:
            return BINARY_FLOOR_DIVIDE;
        default:
            assert(false);
    }
}
int InplaceOP(operator_ty *op){
    switch (op->kind){
        case AddKind:
            return INPLACE_ADD;
        case SubKind:
            return INPLACE_SUBTRACT;
        case MultKind:
            return INPLACE_MULTIPLY;
        case DivKind:
            return INPLACE_TRUE_DIVIDE;
        case ModKind:
            return INPLACE_MODULE;
        case PowKind:
            return INPLACE_POWER;
        case LSHIFTKind:
            return INPLACE_LSHIFT;
        case RSHIFTKind:
            return INPLACE_RSHIFT;
        case BitOrKind:
            return INPLACE_OR;
        case BitXorKind:
            return INPLACE_XOR;
        case BitAndKind:
            return INPLACE_AND;
        case FloorDivKind:
            return INPLACE_FLOOR_DIVIDE;
        default:
            assert(false);
    }
}
int UnaryOP(unaryop_ty *op){
    switch (op->kind){
        case InvertKind:
            return UNARY_INVERT;
        case NotKind:
            return UNARY_NOT;
        case UAddKind:
            return UNARY_POSITIVE;
        case USubKind:
            return UNARY_NEGATIVE;
        default:
            assert(false);
    }
}
int CmpOP(cmpop_ty *op){
    switch(op->kind){
        case EqKind:
            return COMPARE_EQ;
        case NotEqKind:
            return COMPARE_NE;
        case LtKind:
            return COMPARE_LT;
        case LtEKind:
            return COMPARE_LE;
        case GtKind:
            return COMPARE_GT;
        case GtEKind:
            return COMPARE_GE;
        case IsKind:
            return COMPARE_IS;
        case IsNotKind:
            return COMPARE_IS_NOT;
        case InKind:
            return COMPARE_IN;
        case NotInKind:
            return COMPARE_NOT_IN;
        default:
            assert(false);
    }
}
}
Instruction::Instruction(int _opcode):opcode(_opcode){}
Instruction::Instruction(int _opcode,int _arg):opcode(_opcode),arg(_arg){}
Instruction::Instruction(int _opcode,BasicBlock *_jump_block):opcode(_opcode),jump_block(_jump_block){}
int Instruction::Size() const{
    return 1;
    /*
    for (int i=1,arg=this->arg;;i++,arg>>=8)
        if (arg<=8) return i;
    */
    assert(false);
}
void BasicBlock::AddOP(int opcode){
    assert(!HasArgument(opcode));
    this->ins.push_back(new Instruction(opcode));
    if (opcode==RETURN_VALUE) this->has_return=true;
}
void BasicBlock::AddOP(int opcode,int arg){
    assert(HasArgument(opcode));
    this->ins.push_back(new Instruction(opcode,arg));
    if (opcode==RETURN_VALUE) this->has_return=true;
}
void BasicBlock::AddJumpOP(int opcode,BasicBlock *block,bool absolute){
    assert(HasArgument(opcode));
    assert(block!=nullptr);
    auto ins=new Instruction(opcode,block);
    if (absolute) ins->jump_abs=true; else ins->jump_rel=true;
    this->ins.push_back(ins);
}
int BasicBlock::Size() const{
    int ans=0;
    for (auto i:this->ins)
        ans+=i->Size();
    return ans;
}
FrameBlock::FrameBlock(FrameBlockType _type,BasicBlock *_enter_block,BasicBlock *_exit_block):type(_type),enter_block(_enter_block),exit_block(_exit_block){}
CompilerUnit::CompilerUnit(SymTableEntry *_ste):ste(_ste){
    for (int i=0;i<(int)ste->varnames.size();i++)
        this->varnames[ste->varnames[i]]=i;
}
void CompilerUnit::AddOP(int opcode){
    this->block->AddOP(opcode);
}
void CompilerUnit::AddOP(int opcode,int arg){
    this->block->AddOP(opcode,arg);
}
void CompilerUnit::AddJumpOP(int opcode,BasicBlock *block,bool absolute){
    this->block->AddJumpOP(opcode,block,absolute);
}
int CompilerUnit::AddName(const std::string &str){
    return AddMap(this->names,str);
}
int CompilerUnit::AddCellName(const std::string &str){
    return AddMap(this->cellvars,str);
}
int CompilerUnit::AddFreeName(const std::string &str){
    return AddMap(this->freevars,str);
}
int CompilerUnit::AddConst(Object *ptr){
    auto it=this->consts.find(ptr);
    if (it!=this->consts.end()) return it->second;
    this->consts[ptr]=(int)this->consts.size();
    return this->consts[ptr];
}
void ControlFlowGraph::DFS(BasicBlock *block){
    if (block==nullptr) return;
    if (vis.find(block)!=vis.end()) return;
    this->vis.insert(block);
    this->order.push_back(block);
    this->DFS(block->next);
    for (auto i:block->ins)
        if (i->jump_block!=nullptr){
            this->DFS(i->jump_block);
        }
}
ByteCode::ByteCode(){
}
void ByteCode::AddOP(int opcode,int oparg){
    this->codes.push_back(new ByteCodeUnit(opcode,oparg));
}
void ControlFlowGraph::CalcJump(){
    int total_size=0;
    for (auto block:this->order){
        block->offset=total_size;
        total_size+=block->Size();
    }
    bool size_change=false;
    for (auto block:this->order){
        int size=block->offset;
        for (auto i:block->ins){
            size+=i->Size();
            if (i->jump_block!=nullptr){
                i->arg=i->jump_block->offset;
                if (i->jump_rel){
                    assert(i->arg>=size);
                    int isize=i->Size();
                    i->arg-=size;
                    if (isize!=i->Size()){
                        size_change=true;
                    }
                }
            }
        }
    }
    if (size_change) return this->CalcJump();
}

ControlFlowGraph::ControlFlowGraph(BasicBlock *entryblock){
    this->DFS(entryblock);
    this->CalcJump();
}
ByteCode *ControlFlowGraph::GenByteCode(){
    auto bytecode=new ByteCode();
    for (auto block:this->order){
        for (auto i:block->ins){
            bytecode->AddOP(i->opcode,i->arg);
        }
    }
    return bytecode;
}
void Compiler::EnterScope(void *key){
    auto ste=this->symtable->Find(key);
    assert(ste!=nullptr);
    auto u=new CompilerUnit(ste);
    this->stack.push(u);
    u->block=this->NewBlock();
}
void Compiler::ExitScope(){
    this->stack.pop();
}
void Compiler::AddOP(int opcode){
    this->stack.top()->AddOP(opcode);
}
void Compiler::AddOP(int opcode,int arg){
    this->stack.top()->AddOP(opcode,arg);
}
void Compiler::AddJumpOP(int opcode,BasicBlock *block,bool absolute){
    this->stack.top()->AddJumpOP(opcode,block,absolute);
}
void Compiler::AddNameOP(identifier_ty *id,expr_context_ty *ctx){
    enum{
        OP_NAME,
        OP_FAST,
        OP_GLOBAL,
        OP_CELL_REF,
        OP_FREE_REF,
    } scope;
    std::string name=static_cast<StringObject*>(id)->ToString();
    int name_idx;
    switch(this->stack.top()->ste->symbols[name]>>SCOPE_OFFSET){
        case LOCAL:
            scope=OP_NAME;
            if (this->stack.top()->ste->type==FunctionBlock)
                scope=OP_FAST;
            name_idx=this->stack.top()->AddName(name);
            break;
        case GLOBAL_IMPLICIT:
            scope=OP_NAME;
            if (this->stack.top()->ste->type==FunctionBlock)
                scope=OP_GLOBAL;
            name_idx=this->stack.top()->AddName(name);
            break;
        case GLOBAL_EXPLICIT:{
            scope=OP_GLOBAL;
            name_idx=this->stack.top()->AddName(name);
            break;
        }
        case FREE:
            scope=OP_FREE_REF;
            name_idx=this->stack.top()->AddFreeName(name);
            break;
        case CELL:{
            scope=OP_CELL_REF;
            name_idx=this->stack.top()->AddCellName(name);
            break;
        }
        default:
            assert(false);
    }
    switch (scope){
        case OP_NAME:
            switch (ctx->kind){
                case LoadKind:
                    this->AddOP(LOAD_NAME,name_idx);
                    return;
                case StoreKind:
                    this->AddOP(STORE_NAME,name_idx);
                    return;
                default:
                    assert(false);
            }
        case OP_FAST:
            switch (ctx->kind){
                case LoadKind:
                    this->AddOP(LOAD_FAST,name_idx);
                    return;
                case StoreKind:
                    this->AddOP(STORE_FAST,name_idx);
                    return;
                default:
                    assert(false);
            }
        case OP_GLOBAL:
            switch (ctx->kind){
                case LoadKind:
                    this->AddOP(LOAD_GLOBAL,name_idx);
                    return;
                case StoreKind:
                    this->AddOP(STORE_GLOBAL,name_idx);
                    return;
                default:
                    assert(false);
            }
        case OP_CELL_REF:
            switch (ctx->kind){
                case LoadKind:
                    this->AddOP(LOAD_CELL_REF,name_idx);
                    return;
                case StoreKind:
                    this->AddOP(STORE_CELL_REF,name_idx);
                    return;
                default:
                    assert(false);
            }
        case OP_FREE_REF:
            switch (ctx->kind){
                case LoadKind:
                    this->AddOP(LOAD_FREE_REF,name_idx);
                    return;
                case StoreKind:
                    this->AddOP(STORE_FREE_REF,name_idx);
                    return;
                default:
                    assert(false);
            }
        default:
            assert(false);
    }
}
void Compiler::AddLoadConst(Object *o){
    int i=this->stack.top()->AddConst(o);
    this->AddOP(LOAD_CONST,i);
}

std::vector<std::string> Compiler::GetNames(){
    std::vector<std::string> ans((int)this->stack.top()->names.size(),"");
    for (auto i:this->stack.top()->names)
        ans[i.second]=i.first;
    return ans;
}
std::vector<Object*> Compiler::GetConsts(){
    std::vector<Object*> ans((int)this->stack.top()->consts.size(),nullptr);
    for (auto i:this->stack.top()->consts)
        ans[i.second]=i.first;
    return ans;
}
std::vector<std::string> Compiler::GetVarNames(){
    return this->stack.top()->ste->varnames;
}
std::vector<int> Compiler::GetVarLocals(){
    std::vector<int> vec;
    for (const auto &name:this->GetVarNames())
        vec.push_back(this->stack.top()->names[name]);
    return vec;
}
std::vector<std::string> Compiler::GetFreeNames(){
    std::vector<std::string> ans((int)this->stack.top()->freevars.size(),"");
    for (auto i:this->stack.top()->freevars)
        ans[i.second]=i.first;
    return ans;
}
std::vector<std::string> Compiler::GetCellNames(){
    std::vector<std::string> ans((int)this->stack.top()->cellvars.size(),"");
    for (auto i:this->stack.top()->cellvars)
        ans[i.second]=i.first;
    return ans;
}
void Compiler::MakeClosure(CodeObject *codeobject){
    for (const auto name:codeobject->freenames){
        //printf("name:%s\n",name.c_str());
        switch (this->stack.top()->ste->symbols[name]>>SCOPE_OFFSET){
            case CELL:
                //puts("is cell");
                this->AddOP(LOAD_CELL_CLOSURE,this->stack.top()->AddCellName(name));
                break;
            case FREE:
                //puts("is free");
                this->AddOP(LOAD_FREE_CLOSURE,this->stack.top()->AddFreeName(name));
                break;
            default:
                assert(false);
        }
    }
    this->AddOP(MAKE_TUPLE,(int)codeobject->freenames.size());
}

BasicBlock *Compiler::NewBlock(){
    auto block=new BasicBlock();
    this->stack.top()->blocks.push_back(block);
    return block;
}
void Compiler::AddBlock(){
    auto block=this->NewBlock();
    this->stack.top()->block->next=block;
    this->stack.top()->block=block;
}
void Compiler::AddBlock(BasicBlock *block){
    this->stack.top()->block->next=block;
    this->stack.top()->block=block;
}
void Compiler::EnterFrameBlock(FrameBlockType type,BasicBlock *enter_block,BasicBlock *exit_block){
    auto frameblock=new FrameBlock(type,enter_block,exit_block);
    this->stack.top()->frameblocks.push_back(frameblock);
}
void Compiler::ExitFrameBlock(){
    this->stack.top()->frameblocks.pop_back();
}
void Compiler::UnwindFrameBlock(FrameBlock *frameblock,bool preserve_tos){
    switch(frameblock->type){
        case WHILE_LOOP:
            return;
        case FOR_LOOP:{
            if (preserve_tos) this->AddOP(ROT_TWO);
            this->AddOP(POP_TOP);
            return;
        }
        default:
            assert(false);
    }
}
void Compiler::UnwindAll(bool preserve_tos){
    for (int i=(int)this->stack.top()->frameblocks.size()-1;i>=0;i--){
        auto frameblock=this->stack.top()->frameblocks[i];
        this->UnwindFrameBlock(frameblock,preserve_tos);
    }
}
void Compiler::PopJumpIf(expr_ty *expr,BasicBlock *block,bool cond){
    this->visit_expr(expr);
    this->AddJumpOP(cond?POP_JUMP_IF_TRUE:POP_JUMP_IF_FALSE,block,true);
}

CodeObject *Compiler::Assemble(bool return_none){
    //puts("assemble");
    if (!this->stack.top()->block->has_return){
        this->AddBlock();
        if (return_none)
            this->AddLoadConst(NoneObject::Singleton());
        this->AddOP(RETURN_VALUE);
    }
    auto cfg=new ControlFlowGraph(*this->stack.top()->blocks.begin());
    auto bytecode=cfg->GenByteCode();
    auto codeobject=new CodeObject(this->GetNames(),this->GetConsts(),this->GetVarNames(),this->GetVarLocals(),this->GetFreeNames(),this->GetCellNames());
    /*
    puts("~~~~~~~~~~Assemble~~~~~~~~~");
    printf("%s\n",this->stack.top()->ste->name.c_str());
    puts("names:");
    for (const auto &name:this->GetNames())
        printf("%s\n",name.c_str());
    puts("varnames:");
    for (const auto &name:this->GetVarNames())
        printf("%s\n",name.c_str());
    puts("varlocals:");
    for (const auto &i:this->GetVarLocals())
        printf("%d\n",i);
    puts("cellnames:");
    for (const auto &name:this->GetCellNames())
        printf("%s\n",name.c_str());
    puts("freenames:");
    for (const auto &name:this->GetFreeNames())
        printf("%s\n",name.c_str());
    puts("bytecode:");
    for (int i=0;i<(int)bytecode->codes.size();i++){
        auto code=bytecode->codes[i];
        printf("%d:%s %s\n",i,CodeMap(code->opcode).c_str(),code->oparg>=0?std::to_string(code->oparg).c_str():"");
    }
    */
    for (int i=0;i<(int)bytecode->codes.size();i++){
        auto code=bytecode->codes[i];
        codeobject->AddCode(code->opcode,code->oparg);
    }
    return codeobject;
}
void Compiler::visit_listcomp(expr_ty *expr,int index){
    auto list=expr->ListComp();
    if (index>=list->generators->Size()){
        this->visit_expr(list->elt);
        this->AddOP(YIELD_VALUE);
        return;
    }
    auto loop=this->NewBlock();
    auto end=this->NewBlock();
    auto gen=list->generators->Get(index);
    this->visit_expr(gen->iter);
    this->AddOP(GET_ITER);
    this->AddBlock(loop);
    this->AddJumpOP(FOR_ITER,end,false);
    this->visit_expr(gen->target);
    if (gen->ifs!=nullptr){
        for (auto expr:gen->ifs->ToVector()){
            this->PopJumpIf(expr,loop,false);
        }
    }
    this->visit_listcomp(expr,index+1);
    this->AddJumpOP(JUMP_ABSOLUTE,loop,true);
    this->AddBlock(end);
}

void Compiler::visit_expr(expr_ty *expr){
    //puts("visit_expr");
    switch (expr->kind){
        case BoolOpKind:{
            auto boolop=expr->BoolOp();
            auto end=this->NewBlock();
            int opcode=expr->BoolOp()->op->kind==AndKind?JUMP_IF_FALSE_OR_POP:JUMP_IF_TRUE_OR_POP;
            for (int i=0;i<boolop->values->Size()-1;i++){
                this->visit_expr(boolop->values->Get(i));
                this->AddJumpOP(opcode,end,true);
            }
            this->visit_expr(boolop->values->Get(boolop->values->Size()-1));
            this->AddBlock(end);
            break;
        }
        case BinOpKind:{
            auto binop=expr->BinOp();
            this->visit_expr(binop->left);
            this->visit_expr(binop->right);
            this->AddOP(BinOP(binop->op));
            break;
        }
        case UnaryOpKind:{
            auto unaryop=expr->UnaryOp();
            this->visit_expr(unaryop->operand);
            this->AddOP(UnaryOP(unaryop->op));
            break;
        }
        case CompareKind:{
            auto compare=expr->Compare();
            int n=(int)compare->comparators->Size();
            if (n==1){
                this->visit_expr(compare->left);
                this->visit_expr(compare->comparators->Get(0));
                this->AddOP(CmpOP(compare->ops->Get(0)));
                break;
            }
            assert(false);
            break;
        }
        case CallKind:{
            auto call=expr->Call();
            this->visit_expr(call->func);
            assert(call->keywords==nullptr);
            if (call->args!=nullptr){
                for (auto expr:call->args->ToVector())
                    this->visit_expr(expr);
                this->AddOP(CALL_FUNCTION,call->args->Size());
            } else{
                this->AddOP(CALL_FUNCTION,0);
            }
            break;
        }
        case NameKind:{
            auto name=expr->Name();
            //printf("%s\n",static_cast<StringObject*>(name->id)->ToString().c_str());
            //printf("ctx:%d\n",name->ctx->kind);
            this->AddNameOP(name->id,name->ctx);
            break;
        }
        case NameConstantKind:
            this->AddLoadConst(expr->NameConstant()->value);
            break;
        case TupleKind:{
            auto tuple=expr->Tuple();
            std::vector<Object*> vec;
            switch (tuple->ctx->kind){
                case LoadKind:{
                    for (auto expr:tuple->elts->ToVector())
                        this->visit_expr(expr);
                    this->AddOP(MAKE_TUPLE,(int)tuple->elts->Size());
                    break;
                }
                case StoreKind:{
                    this->AddOP(GET_ITER);
                    for (auto expr:tuple->elts->ToVector()){
                        this->AddOP(NEXT_ITER);
                        this->visit_expr(expr);
                    }
                    this->AddOP(END_ITER);
                    break;
                }
                default:
                    assert(false);
            }
            break;
        }
        case ListKind:{
            auto list=expr->List();
            std::vector<Object*> vec;
            switch (list->ctx->kind){
                case LoadKind:{
                    for (auto expr:list->elts->ToVector())
                        this->visit_expr(expr);
                    this->AddOP(MAKE_LIST,list->elts->Size());
                    break;
                }
                case StoreKind:{
                    this->AddOP(GET_ITER);
                    for (auto expr:list->elts->ToVector()){
                        this->AddOP(NEXT_ITER);
                        this->visit_expr(expr);
                    }
                    this->AddOP(END_ITER);
                    break;
                }
                default:
                    assert(false);
            }
            break;
        }
        case AttributeKind:{
            auto attr=expr->Attribute();
            switch (attr->ctx->kind){
                case LoadKind:{
                    this->visit_expr(attr->value);
                    int arg=this->stack.top()->AddName(attr->id->ToString());
                    this->AddOP(LOAD_ATTR,arg);
                    break;
                }
                case StoreKind:{
                    this->visit_expr(attr->value);
                    int arg=this->stack.top()->AddName(attr->id->ToString());
                    this->AddOP(STORE_ATTR,arg);
                    break;
                }
                default:
                    assert(false);
            }
            break;
        }
        case NumKind:
            this->AddLoadConst(expr->Num()->n);
            break;
        case StrKind:
            this->AddLoadConst(expr->Str()->s);
            break;
        case EllipsisKind:
            assert(false);
            break;
        case SubscriptKind:{
            auto subscript=expr->Subscript();
            expr_context_ty *ctx;
            switch (subscript->ctx->kind){
                case LoadKind:
                    this->visit_expr(subscript->value);
                    ctx=Load();
                    break;
                case StoreKind:
                    this->visit_expr(subscript->value);
                    ctx=Store();
                    break;
                default:
                    assert(false);
            }
            switch (subscript->slice->kind){
                case IndexKind:
                    if (ctx->kind!=AugStoreKind){
                        this->visit_expr(subscript->slice->Index()->value);
                    }
                    break;
                case SliceKind:
                    if(subscript->slice->Slice()->lower==nullptr){
                        this->AddLoadConst(NoneObject::Singleton());
                    }
                    else{
                        this->visit_expr(subscript->slice->Slice()->lower);
                    }
                    if(subscript->slice->Slice()->upper==nullptr){
                        this->AddLoadConst(NoneObject::Singleton());
                    }
                    else{
                        this->visit_expr(subscript->slice->Slice()->upper);
                    }
                    if(subscript->slice->Slice()->step==nullptr){
                        this->AddLoadConst(NoneObject::Singleton());
                    }
                    else{
                        this->visit_expr(subscript->slice->Slice()->step);
                    }
                    this->AddOP(BUILD_SLICE);
                    break;
                default:
                    assert(false);
            }
            switch (ctx->kind){
                case LoadKind:
                    this->AddOP(GET_ITEM);
                    break;
                case StoreKind:
                    this->AddOP(SET_ITEM);
                    break;
                default:
                    assert(false);
            }
            break;
        }
        case IfExprKind:{
            auto ifexpr=expr->IfExpr();
            auto block=this->NewBlock();
            auto end=this->NewBlock();
            this->PopJumpIf(ifexpr->test,block,false);
            this->visit_expr(ifexpr->body);
            this->AddJumpOP(JUMP_FORWARD,end,false);
            this->AddBlock(block);
            this->visit_expr(ifexpr->orelse);
            this->AddBlock(end);
            break;
        }
        case DictKind:{
            auto dict=expr->Dict();
            if (dict->keys==nullptr&&dict->values==nullptr){
                this->AddOP(MAKE_DICT,0);
                break;
            }
            assert(dict->keys!=nullptr&&dict->values!=nullptr);
            assert(dict->keys->Size()==dict->values->Size());
            auto keys=dict->keys->ToVector();
            auto values=dict->values->ToVector();
            for (int i=(int)keys.size();i--;){
                this->visit_expr(keys[i]);
                this->visit_expr(values[i]);
            }
            this->AddOP(MAKE_DICT,(int)keys.size());
            break;
        }
        case ListCompKind:{
            this->EnterScope(expr);
            this->visit_listcomp(expr);
            auto codeobject=this->Assemble(false);
            this->ExitScope();
            this->MakeClosure(codeobject);
            this->AddLoadConst(codeobject);
            this->AddLoadConst(new StringObject("listcomp"));
            this->AddOP(MAKE_FUNCTION,0);
            this->AddOP(GEN2LIST);
            break;
        }
        case YieldKind:{
            auto yield=expr->Yield();
            assert(this->stack.top()->ste->type==FunctionBlock);
            if (yield->value!=nullptr){
                this->visit_expr(yield->value);
            } else{
                this->AddLoadConst(NoneObject::Singleton());
            }
            this->AddOP(YIELD_VALUE);
            this->AddLoadConst(NoneObject::Singleton());
            break;
        }
        default:
            assert(false);
    }
}
void Compiler::visit_stmts(asdl_seq<stmt_ty*> *stmts){
    for (auto stmt:stmts->ToVector())
        this->visit_stmt(stmt);
}
void Compiler::visit_stmt(stmt_ty *stmt){
    //puts("visit_stmt");
    this->stack.top()->lineno=stmt->lineno;
    this->stack.top()->col_offset=stmt->col_offset;
    switch (stmt->kind){
        case FunctionDefKind:{
            auto funcdef=stmt->FunctionDef();
            this->EnterScope(funcdef);
            bool has_yield=stack.top()->ste->has_yield;
            this->visit_stmts(funcdef->body);
            auto codeobject=this->Assemble(!has_yield?true:false);
            codeobject->has_yield=has_yield;
            this->ExitScope();
            this->MakeClosure(codeobject);
            this->AddLoadConst(codeobject);
            this->AddLoadConst(funcdef->name);
            this->AddOP(MAKE_FUNCTION,0);
            this->AddNameOP(funcdef->name,Store());
            break;
        }
        case ClassDefKind:{
            auto classdef=stmt->ClassDef();
            this->EnterScope(stmt);
            this->visit_stmts(classdef->body);
            auto codeobject=this->Assemble(true);
            this->ExitScope();
            this->AddOP(LOAD_BUILD_CLASS);
            this->MakeClosure(codeobject);
            this->AddLoadConst(codeobject);
            this->AddLoadConst(classdef->name);
            this->AddOP(MAKE_FUNCTION,0);
            this->AddNameOP(classdef->name,Store());
            this->AddNameOP(classdef->name,Load());
            this->AddLoadConst(classdef->name);
            if (classdef->bases!=nullptr&&classdef->bases->Size()>0){
                for (int i=0;i<classdef->bases->Size();i++) this->visit_expr(classdef->bases->Get(i));
                this->AddOP(MAKE_TUPLE,classdef->bases->Size());
            } else{
                this->AddLoadConst(ObjectTypeObject::Singleton());
                this->AddOP(MAKE_TUPLE,1);
            }
            this->AddOP(CALL_FUNCTION,3);
            this->AddNameOP(classdef->name,Store());
            break;
        }
        case ReturnKind:{
            auto returnkind=stmt->Return();
            assert(this->stack.top()->ste->type==FunctionBlock);
            if (this->stack.top()->ste->has_yield){
                assert(returnkind->value==nullptr);
                this->UnwindAll(false);
                this->AddOP(RETURN_VALUE);
                break;
            }
            if (returnkind->value!=nullptr){
                this->visit_expr(returnkind->value);
            } else{
                this->AddLoadConst(NoneObject::Singleton());
            }
            this->UnwindAll(true);
            this->AddOP(RETURN_VALUE);
            break;
        }
        case AssignKind:{
            auto assign=stmt->Assign();
            this->visit_expr(assign->value);
            for (int i=0;i<assign->targets->Size();i++){
                if (i<assign->targets->Size()-1) this->AddOP(DUP_TOP);
                this->visit_expr(assign->targets->Get(i));
            }
            break;
        }
        case AugAssignKind:{
            auto augassign=stmt->AugAssign();
            auto target=augassign->target;
            switch (target->kind){
                case AttributeKind:{
                    auto auge=Attribute(target->Attribute()->value,target->Attribute()->id,AugLoad(),target->lineno,target->col_offset);
                    this->visit_expr(auge);
                    this->visit_expr(augassign->value);
                    this->AddOP(InplaceOP(augassign->op));
                    auge->Attribute()->ctx=AugStore();
                    this->visit_expr(auge);
                    break;
                }
                case NameKind:{
                    auto name=target->Name();
                    this->AddNameOP(name->id,Load());
                    this->visit_expr(augassign->value);
                    this->AddOP(InplaceOP(augassign->op));
                    this->AddNameOP(name->id,Store());
                    break;
                }
                default:
                    assert(false);
            }
            break;
        }
        case ForKind:{
            auto forkind=stmt->For();
            auto loop=this->NewBlock();
            auto end=this->NewBlock();
            this->EnterFrameBlock(FOR_LOOP,loop,end);
            this->visit_expr(forkind->iter);
            this->AddOP(GET_ITER);
            this->AddBlock(loop);
            this->AddJumpOP(FOR_ITER,end,false);
            this->visit_expr(forkind->target);
            this->visit_stmts(forkind->body);
            this->AddJumpOP(JUMP_ABSOLUTE,loop,true);
            this->ExitFrameBlock();
            this->AddBlock(end);
            if (forkind->orelse!=nullptr){
                this->visit_stmts(forkind->orelse);
            }
            break;
        }
        case WhileKind:{
            auto whilekind=stmt->While();
            auto loop=this->NewBlock();
            auto end=this->NewBlock();
            this->EnterFrameBlock(WHILE_LOOP,loop,end);
            this->AddBlock(loop);
            this->PopJumpIf(whilekind->test,end,false);
            this->visit_stmts(whilekind->body);
            this->AddJumpOP(JUMP_ABSOLUTE,loop,true);
            this->ExitFrameBlock();
            this->AddBlock(end);
            if (whilekind->orelse!=nullptr){
                this->visit_stmts(whilekind->orelse);
            }
            break;
        }
        case IfKind:{
            auto ifkind=stmt->If();
            auto end=this->NewBlock();
            if (ifkind->orelse!=nullptr){
                auto next=this->NewBlock();
                this->PopJumpIf(ifkind->test,next,false);
                this->visit_stmts(ifkind->body);
                this->AddJumpOP(JUMP_FORWARD,end,false);
                this->AddBlock(next);
                this->visit_stmts(ifkind->orelse);
                this->AddBlock(end);
            } else{
                this->PopJumpIf(ifkind->test,end,false);
                this->visit_stmts(ifkind->body);
                this->AddBlock(end);
            }
            break;
        }
        case ExprKind:{
            this->visit_expr(stmt->Expr()->value);
            this->AddOP(POP_TOP);
            break;
        }
        case NonlocalKind:
        case GlobalKind:
        case PassKind:{
            break;
        }
        case BreakKind:{
            for (int i=(int)this->stack.top()->frameblocks.size()-1;;i--){
                assert(i>=0);
                auto frameblock=this->stack.top()->frameblocks[i];
                this->UnwindFrameBlock(frameblock,false);
                if (frameblock->type==WHILE_LOOP||frameblock->type==FOR_LOOP){
                    this->AddJumpOP(JUMP_ABSOLUTE,frameblock->exit_block,true);
                    break;
                }
            }
            break;
        }
        case ContinueKind:
            for (int i=(int)this->stack.top()->frameblocks.size()-1;;i--){
                assert(i>=0);
                auto frameblock=this->stack.top()->frameblocks[i];
                if (frameblock->type==WHILE_LOOP||frameblock->type==FOR_LOOP){
                    this->AddJumpOP(JUMP_ABSOLUTE,frameblock->enter_block,true);
                    break;
                }
                this->UnwindFrameBlock(frameblock,false);
            }
            break;
        default:
            assert(false);
    }
}
CodeObject *Compiler::compile_mod(mod_ty *mod){
    this->EnterScope(mod);
    switch (mod->kind){
        case ModuleKind:{
            this->visit_stmts(mod->Module()->body);
            break;
        }
        case InteractiveKind:
        case ExpressionKind:
        default:
            assert(false);
    }
    auto codeobject=this->Assemble(true);
    this->ExitScope();
    return codeobject;
}
CodeObject *Compiler::Compile(mod_ty *mod){
    this->symtable=new SymTable(mod);
    return this->compile_mod(mod);
}
CodeObject *Compiler::Compile(Node *n){
    auto ast=new AST();
    auto mod=ast->FromNode(n);
    return this->Compile(mod);
}
CodeObject *Compiler::Compile(FILE *fp){
    auto parser=new Parser(GetGrammar(),file_input);
    auto n=parser->ParseFile(fp);
    return this->Compile(n);
}
