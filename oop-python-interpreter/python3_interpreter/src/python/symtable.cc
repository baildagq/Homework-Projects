#include "symtable.h"
inline std::string Formatter(int t){
    int x=t-(t>>SCOPE_OFFSET<<SCOPE_OFFSET),y=t>>SCOPE_OFFSET;
    std::string ans="usage:";
    bool first=true;
    auto add=[&ans,&first](const std::string &str){
        if (!first) ans+='|'; else first=false;
        ans+=str;
    };
    if (x&DEF_PARAM) add("DEF_PARAM");
    if (x&DEF_LOCAL) add("DEF_LOCAL");
    if (x&USE) add("USE");
    ans+=" scope:";
    switch(y){
        case LOCAL:
            ans+="LOCAL";
            break;
        case FREE:
            ans+="FREE";
            break;
        case GLOBAL_IMPLICIT:
            ans+="GLOBAL_IMPLICIT";
            break;
        case CELL:
            ans+="CELL";
            break;
        default:
            assert(false);
    }
    return ans;
}
SymTableEntry::SymTableEntry(const std::string &_name,BlockType _type,void *_key,SymTable *_table,int _lineno,int _col_offset):name(_name),type(_type),key(_key),table(_table),lineno(_lineno),col_offset(_col_offset){}

void SymTableEntry::AddDef(const std::string &str,int flag){
    auto it=this->symbols.find(str);
    if (it!=this->symbols.end()){
        assert(!(it->second&DEF_PARAM)||!(flag&DEF_PARAM));
        this->symbols[str]|=flag;
    } else{
        this->symbols[str]=flag;
    }
    if (flag&DEF_PARAM){
        this->varnames.push_back(str);
    }
}

void SymTableEntry::AnalyzeName(const std::string &name,int flag,std::map<std::string,int> *scopes,std::set<std::string> *bound,std::set<std::string> *local,std::set<std::string> *free,std::set<std::string> *global){
    assert(scopes->find(name)==scopes->end());
    //printf("analyze name %s\n",name.c_str());
    if (flag&DEF_GLOBAL){
        if (flag&DEF_NONLOCAL){
            assert(false);
        }
        (*scopes)[name]=GLOBAL_EXPLICIT;
        global->insert(name);
        if (bound!=nullptr) bound->erase(name);
        return;
    }
    if (flag&DEF_NONLOCAL){
        assert(bound!=nullptr);
        assert(bound->find(name)!=bound->end());
        (*scopes)[name]=FREE;
        free->insert(name);
        return;
    }
    if (flag&DEF_BOUND){
        //puts("LOCAL");
        (*scopes)[name]=LOCAL;
        local->insert(name);
        global->erase(name);
        return;
    }
    if (bound!=nullptr&&bound->find(name)!=bound->end()){
        //puts("FREE");
        (*scopes)[name]=FREE;
        this->has_free=true;
        free->insert(name);
        return;
    }
    (*scopes)[name]=GLOBAL_IMPLICIT;
    //puts("GLOBAL");
}
void SymTableEntry::AnalyzeBlock(std::set<std::string> *bound,std::set<std::string> *free,std::set<std::string> *global){
    //printf("analyze block %s:%d\n",this->name.c_str(),(int)this->childs.size());
    auto scopes=new std::map<std::string,int>();
    auto local=new std::set<std::string>();
    auto new_bound=new std::set<std::string>();
    auto new_free=new std::set<std::string>();
    auto new_global=new std::set<std::string>();
    if (this->type==ClassBlock){
        for (const auto &name:*global)
            new_global->insert(name);
        if (bound!=nullptr){
            for (const auto &name:*bound)
                new_bound->insert(name);
        }
    }
    for (const auto &it:this->symbols){
        this->AnalyzeName(it.first,it.second,scopes,bound,local,free,global);
    }
    switch(this->type){
        case FunctionBlock:{
            for (const auto &name:*local)
                new_bound->insert(name);
        }
        case ModuleBlock:{
            if (bound!=nullptr){
                for (const auto &name:*bound)
                    new_bound->insert(name);
            }
            for (const auto &name:*global)
                new_global->insert(name);
            break;
        }
        case ClassBlock:
            break;
        default:
            assert(false);
    }
    auto child_free=new std::set<std::string>();
    for (auto child:this->childs){
        //printf("analyze child %s->%s\n",this->name.c_str(),child->name.c_str());
        auto tmp_bound=new std::set<std::string>(*new_bound);
        auto tmp_free=new std::set<std::string>(*new_free);
        auto tmp_global=new std::set<std::string>(*new_global);
        child->AnalyzeBlock(tmp_bound,tmp_free,tmp_global);
        for (const auto &it:*tmp_free)
            child_free->insert(it);
    }
    for (const auto &name:*child_free)
        new_free->insert(name);

    if (this->type==FunctionBlock){
        for (auto &it:*scopes){
            const std::string &name=it.first;
            int scope=it.second;
            if (scope!=LOCAL) continue;
            if (new_free->find(name)==new_free->end()) continue;
            it.second=CELL;
            new_free->erase(name);
        }
    }
    for (auto &it:this->symbols){
        assert(scopes->find(it.first)!=scopes->end());
        it.second|=(*scopes)[it.first]<<SCOPE_OFFSET;
    }
    for (const auto &name:*new_free){
        if (this->symbols.find(name)!=this->symbols.end()){
            auto scope=this->symbols[name];
            if (this->type==ClassBlock){
                if (scope&(DEF_BOUND)){
                    symbols[name]=scope|DEF_FREE_CLASS;
                }
            }
            continue;
        }
        if (bound&&bound->find(name)==bound->end()) continue;
        this->symbols[name]=FREE<<SCOPE_OFFSET;
    }
    for (const auto &name:*new_free)
        free->insert(name);
    /*
    puts("");
    printf("end of analyze block %s\n",this->name.c_str());
    for (const auto &it:this->symbols){
        printf("symbols:%s %s\n",it.first.c_str(),Formatter(it.second).c_str());
    }
    puts("");
    */
}

void SymTable::EnterBlock(const std::string &name,BlockType type,void *node,int lineno,int col_offset){
    auto ste=new SymTableEntry(name,type,node,this,lineno,col_offset);
    this->blocks[node]=ste;
    if (!this->stack.empty()) this->stack.top()->childs.push_back(ste);
    this->stack.push(ste);
}
void SymTable::ExitBlock(){
    this->stack.pop();
}

void SymTable::AddDef(const std::string &str,int flag){
    this->stack.top()->AddDef(str,flag);
}

void SymTable::visit_expr(expr_ty *expr){
    //printf("symtable visit expr %d\n",expr->kind);
    switch (expr->kind){
        case BoolOpKind:
            for (auto value:expr->BoolOp()->values->ToVector())
                this->visit_expr(value);
            break;
        case BinOpKind:{
            auto binop=expr->BinOp();
            this->visit_expr(binop->left);
            this->visit_expr(binop->right);
            break;
        }
        case UnaryOpKind:
            this->visit_expr(expr->UnaryOp()->operand);
            break;
        case CompareKind:{
            auto compare=expr->Compare();
            this->visit_expr(compare->left);
            for (auto value:compare->comparators->ToVector())
                this->visit_expr(value);
            break;
        }
        case CallKind:{
            auto call=expr->Call();
            this->visit_expr(call->func);
            if (call->args!=nullptr)
                for (auto args:call->args->ToVector())
                    this->visit_expr(args);
            if (call->keywords!=nullptr)
                for (auto keyword:call->keywords->ToVector())
                    this->visit_expr(keyword->value);
            break;
        }
        case NameKind:{
            auto name=expr->Name();
            this->AddDef(static_cast<StringObject*>(name->id)->ToString(),name->ctx->kind==LoadKind?USE:DEF_LOCAL);
            break;
        }
        case NameConstantKind:{
            break;
        }
        case TupleKind:{
            auto tuple=expr->Tuple();
            for (auto elt:tuple->elts->ToVector())
                this->visit_expr(elt);
            break;
        }
        case ListKind:{
            auto list=expr->List();
            for (auto elt:list->elts->ToVector())
                this->visit_expr(elt);
            break;
        }
        case AttributeKind:{
            auto attr=expr->Attribute();
            this->visit_expr(attr->value);
            break;
        }
        case SubscriptKind:{
            auto subs=expr->Subscript();
            this->visit_expr(subs->value);
            break;
        }
        case IfExprKind:{
            auto ifexpr=expr->IfExpr();
            this->visit_expr(ifexpr->test);
            this->visit_expr(ifexpr->body);
            this->visit_expr(ifexpr->orelse);
            break;
        }
        case DictKind:{
            auto dict=expr->Dict();
            if (dict->keys!=nullptr){
                assert(dict->values!=nullptr);
                assert(dict->keys->Size()==dict->values->Size());
                auto keys=dict->keys->ToVector();
                auto values=dict->values->ToVector();
                for (int i=0;i<(int)keys.size();i++){
                    this->visit_expr(keys[i]);
                    this->visit_expr(values[i]);
                }
                break;
            }
            assert(dict->values==nullptr);
            break;
        }
        case ListCompKind:{
            this->EnterBlock("listcomp",FunctionBlock,expr,expr->lineno,expr->col_offset);
            auto list=expr->ListComp();
            this->visit_expr(list->elt);
            for (auto comp:list->generators->ToVector()){
                this->visit_expr(comp->target);
                this->visit_expr(comp->iter);
                if (comp->ifs!=nullptr){
                    for (auto expr:comp->ifs->ToVector())
                        this->visit_expr(expr);
                }
            }
            this->ExitBlock();
            break;
        }
        case YieldKind:{
            auto yield=expr->Yield();
            this->stack.top()->has_yield=true;
            if (yield->value!=nullptr)
                this->visit_expr(yield->value);
            break;
        }
        case NumKind:
        case StrKind:
        case EllipsisKind:
            break;
        default:
            assert(false);
    }
}

void SymTable::visit_stmt(stmt_ty *stmt){
    //printf("symtable visit stmt %d\n",stmt->kind);
    switch (stmt->kind){
        case FunctionDefKind:{
            auto funcdef=stmt->FunctionDef();
            this->AddDef(static_cast<StringObject*>(funcdef->name)->ToString(),DEF_LOCAL);
            this->EnterBlock(funcdef->name->ToString(),FunctionBlock,funcdef,stmt->lineno,stmt->col_offset);
            if (funcdef->args->args!=nullptr){
                for (auto args:funcdef->args->args->ToVector())
                    this->AddDef(args->args->ToString(),DEF_PARAM);
            }
            for (auto stmt:funcdef->body->ToVector())
                this->visit_stmt(stmt);
            this->ExitBlock();
            break;
        }
        case ClassDefKind:{
            auto classdef=stmt->ClassDef();
            const auto &name=classdef->name->ToString();
            this->AddDef(name,DEF_LOCAL);
            if (classdef->bases!=nullptr){
                for (auto expr:classdef->bases->ToVector())
                    this->visit_expr(expr);
            }
            this->EnterBlock(name,ClassBlock,stmt,stmt->lineno,stmt->col_offset);
            for (auto stmt:classdef->body->ToVector())
                this->visit_stmt(stmt);
            this->ExitBlock();
            break;
        }
        case ReturnKind:{
            auto t=stmt->Return()->value;
            if (t!=nullptr)
                this->visit_expr(t);
            break;
        }
        case AssignKind:{
            auto assign=stmt->Assign();
            for (auto expr:assign->targets->ToVector())
                this->visit_expr(expr);
            this->visit_expr(assign->value);
            break;
        }
        case AugAssignKind:{
            auto aug=stmt->AugAssign();
            this->visit_expr(aug->target);
            this->visit_expr(aug->value);
            break;
        }
        case ForKind:{
            auto forkind=stmt->For();
            this->visit_expr(forkind->target);
            this->visit_expr(forkind->iter);
            for (auto stmt:forkind->body->ToVector())
                this->visit_stmt(stmt);
            if (forkind->orelse!=nullptr){
                for (auto stmt:forkind->orelse->ToVector())
                    this->visit_stmt(stmt);
            }
            break;
        }
        case WhileKind:{
            auto whilekind=stmt->While();
            this->visit_expr(whilekind->test);
            for (auto stmt:whilekind->body->ToVector())
                this->visit_stmt(stmt);
            if (whilekind->orelse!=nullptr){
                for (auto stmt:whilekind->orelse->ToVector())
                    this->visit_stmt(stmt);
            }
            break;
        }
        case IfKind:{
            auto ifkind=stmt->If();
            this->visit_expr(ifkind->test);
            for (auto stmt:ifkind->body->ToVector())
                this->visit_stmt(stmt);
            if (ifkind->orelse!=nullptr){
                for (auto stmt:ifkind->orelse->ToVector())
                    this->visit_stmt(stmt);
            }
            break;
        }
        case NonlocalKind:{
            auto nonlocal=stmt->Nonlocal();
            for (auto id:nonlocal->names->ToVector()){
                const auto &name=id->ToString();
                auto it=this->stack.top()->symbols.find(name);
                if (it!=this->stack.top()->symbols.end()){
                    int scope=it->second;
                    if (scope&(DEF_PARAM|DEF_LOCAL|USE)){
                        assert(false);
                    }
                }
                this->AddDef(name,DEF_NONLOCAL);
            }
            break;
        }
        case GlobalKind:{
            auto global=stmt->Global();
            for (auto id:global->names->ToVector()){
                const auto &name=id->ToString();
                auto it=this->stack.top()->symbols.find(name);
                if (it!=this->stack.top()->symbols.end()){
                    int scope=it->second;
                    if (scope&(DEF_PARAM|DEF_LOCAL|USE)){
                        assert(false);
                    }
                }
                this->AddDef(name,DEF_GLOBAL);
            }
            break;
        }
        case ExprKind:
            this->visit_expr(stmt->Expr()->value);
            break;
        case PassKind:
        case BreakKind:
        case ContinueKind:
            break;
        default:
            assert(false);
    }
}

void SymTable::visit_mod(mod_ty *mod){
    switch (mod->kind){
        case ModuleKind:
            for (auto stmt:mod->Module()->body->ToVector())
                this->visit_stmt(stmt);
            break;
        default:
            assert(false);
    }
}

SymTable::SymTable(mod_ty *mod){
    this->EnterBlock("global",ModuleBlock,mod,0,0);
    this->visit_mod(mod);
    auto free=new std::set<std::string>();
    auto global=new std::set<std::string>();
    this->stack.top()->AnalyzeBlock(nullptr,free,global);
}

SymTableEntry *SymTable::Find(void *key){
    auto it=this->blocks.find(key);
    if (it==this->blocks.end()) return nullptr;
    return it->second;
}
