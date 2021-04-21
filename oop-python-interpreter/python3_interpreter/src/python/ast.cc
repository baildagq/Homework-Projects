#include "token.h"
#include "graminit.h"
#include "node.h"
#include "ast.h"
#include "all_object.h"
namespace{
void SetContext(expr_ty *expr,expr_context_ty *ctx);
void SetContext(asdl_seq<expr_ty*> *exprs,expr_context_ty *ctx){
    for (auto expr:exprs->ToVector())
        SetContext(expr,ctx);
}
void SetContext(expr_ty *expr,expr_context_ty *ctx){
    assert(ctx->kind!=AugStoreKind);
    assert(ctx->kind!=AugLoadKind);
    switch (expr->kind){
        case AttributeKind:
            expr->Attribute()->ctx=ctx;
            break;
        case NameKind:
            expr->Name()->ctx=ctx;
            break;
        case TupleKind:
            expr->Tuple()->ctx=ctx;
            SetContext(expr->Tuple()->elts,ctx);
            break;
        case ListKind:
            expr->List()->ctx=ctx;
            SetContext(expr->List()->elts,ctx);
            break;
        case SubscriptKind:{
            expr->Subscript()->ctx=ctx;
            break;
        }
        default:
            assert(false);
    }
}
}
operator_ty *AST::ast_for_bin_op(Node *n){
    switch(n->type){
        case VBAR:
            return BitOr();
        case CIRCUMFLEX:
            return BitXor();
        case AMPER:
            return BitAnd();
        case LEFTSHIFT:
            return LSHIFT();
        case RIGHTSHIFT:
            return RSHIFT();
        case PLUS:
            return Add();
        case MINUS:
            return Sub();
        case STAR:
            return Mult();
        case SLASH:
            return Div();
        case PERCENT:
            return Mod();
        case DOUBLESLASH:
            return FloorDiv();
    }
    assert(false);
}
cmpop_ty *AST::ast_for_comp_op(Node *n){
    switch(n->childs[0]->str[0]){
        case '<':
            if ((int)n->childs[0]->str.size()>1)
                return LtE();
            return Lt();
        case '>':
            if ((int)n->childs[0]->str.size()>1)
                return GtE();
            return Gt();
        case '=':
            return Eq();
        case '!':
            return NotEq();
    }
    assert(false);
}
arguments_ty *AST::ast_for_parameters(Node *n){
    if ((int)n->childs.size()==2) return arguments(nullptr);
    if (n->childs[1]->type==typedargslist){
        auto args=new asdl_seq<arg_ty*>({});
        for (int i=0;i<(int)n->childs[1]->childs.size();i+=2){
            if (i+1<(int)n->childs[1]->childs.size()) assert(n->childs[1]->childs[i+1]->type!=EQUAL);
            if (i>0) assert(n->childs[1]->childs[i-1]->type==COMMA);
            assert((int)n->childs[1]->childs[i]->childs.size()==1);
            assert(n->childs[1]->childs[i]->childs[0]->type==NAME);
            args->PushBack(arg(new StringObject(n->childs[1]->childs[i]->childs[0]->str),n->lineno,n->col_offset));
        }
        return arguments(args);
    } else{
        assert(false);
    }
}
expr_ty *AST::ast_for_expr(Node *n){
    //printf("ast_for_expr %d %d \"%s\"\n",(int)n->childs.size(),n->type,n->str.c_str());
    switch(n->type){
        case test:
            if ((int)n->childs.size()==1){
                if (n->childs[0]->type==or_test){
                    return this->ast_for_expr(n->childs[0]);
                }
            }
            return IfExpr(this->ast_for_expr(n->childs[2]),this->ast_for_expr(n->childs[0]),this->ast_for_expr(n->childs[4]),n->lineno,n->col_offset);
        case or_test:
        case and_test:{
            if ((int)n->childs.size()==1)
                return this->ast_for_expr(n->childs[0]);
            auto values=new asdl_seq<expr_ty*>({});
            for (int i=0;i<(int)n->childs.size();i+=2)
                values->PushBack(this->ast_for_expr(n->childs[i]));
            return BoolOp(n->childs[1]->str=="or"?Or():And(),values,n->lineno,n->col_offset);
        }
        case not_test:
            if ((int)n->childs.size()==1)
                return this->ast_for_expr(n->childs[0]);
            return UnaryOp(Not(),this->ast_for_expr(n->childs[1]),n->lineno,n->col_offset);
        case comparison:{
            if ((int)n->childs.size()==1)
                return this->ast_for_expr(n->childs[0]);
            auto left=this->ast_for_expr(n->childs[0]);
            auto ops=new asdl_seq<cmpop_ty*>({});
            auto comparators=new asdl_seq<expr_ty*>({});
            for (int i=1;i<(int)n->childs.size();i+=2){
                ops->PushBack(this->ast_for_comp_op(n->childs[i]));
                comparators->PushBack(this->ast_for_expr(n->childs[i+1]));
            }
            return Compare(left,ops,comparators,n->lineno,n->col_offset);
        }
        case expr:
        case xor_expr:
        case and_expr:
        case shift_expr:
        case arith_expr:
        case term:{
            if ((int)n->childs.size()==1)
                return this->ast_for_expr(n->childs[0]);
            auto left=this->ast_for_expr(n->childs[0]);
            for (int i=1;i<(int)n->childs.size();i+=2){
                auto op=this->ast_for_bin_op(n->childs[i]);
                auto right=this->ast_for_expr(n->childs[i+1]);
                left=BinOp(left,op,right,n->lineno,n->col_offset);
            }
            return left;
        }
        case factor:{
            if ((int)n->childs.size()==1)
                return this->ast_for_expr(n->childs[0]);
            auto *operand=this->ast_for_expr(n->childs[1]);
            switch(n->childs[0]->type){
                case PLUS:
                    return UnaryOp(UAdd(),operand,n->lineno,n->col_offset);
                case MINUS:
                    return UnaryOp(USub(),operand,n->lineno,n->col_offset);
                case TILDE:
                    return UnaryOp(Invert(),operand,n->lineno,n->col_offset);
                default:
                    assert(false);
            }
        }
        case power:{
            if ((int)n->childs.size()==1)
                return this->ast_for_expr(n->childs[0]);
            auto left=this->ast_for_expr(n->childs[0]);
            auto right=this->ast_for_expr(n->childs[2]);
            return BinOp(left,Pow(),right,n->lineno,n->col_offset);
        }
        case atom_expr:{
            if ((int)n->childs.size()==1)
                return this->ast_for_expr(n->childs[0]);
            auto expr=this->ast_for_expr(n->childs[0]);
            for (int i=1;i<(int)n->childs.size();i++){
                switch(n->childs[i]->childs[0]->type){
                    case LPAR:{
                        if (n->childs[i]->childs[1]->type==RPAR){
                            expr=Call(expr,nullptr,nullptr,n->lineno,n->col_offset);
                            break;
                        }
                        auto args=new asdl_seq<expr_ty*>({});
                        for (int j=0;j<(int)n->childs[i]->childs[1]->childs.size();j+=2){
                            assert((int)n->childs[i]->childs[1]->childs[j]->childs.size()==1);
                            args->PushBack(this->ast_for_expr(n->childs[i]->childs[1]->childs[j]->childs[0]));
                        }
                        expr=Call(expr,args,nullptr,n->lineno,n->col_offset);
                        break;
                    }
                    case LSQB:{
                        auto subl=n->childs[i]->childs[1];
                        assert((int)subl->childs.size()==1);
                        if((int)subl->childs[0]->childs.size()==1){
                            if((int)subl->childs[0]->childs[0]->type==test){//test
                                auto test = subl->childs[0]->childs[0];
                                auto value=ast_for_expr(test);
                                auto slice=Index(value);
                                expr=Subscript(expr, slice, Load(), n->lineno,n->col_offset);
                                break;
                            }
                            else if((int)subl->childs[0]->childs[0]->type==COLON){//:
                                auto slice=Slice(nullptr,nullptr,nullptr);
                                expr=Subscript(expr,slice,Load(),n->lineno,n->col_offset);
                                break;
                            }
                            assert(false);
                        }
                        else if((int)subl->childs[0]->childs.size()==2){
                            if((int)subl->childs[0]->childs[0]->type==test){//test :
                                assert((int)subl->childs[0]->childs[1]->type==COLON);
                                auto test = subl->childs[0]->childs[0];
                                auto value=ast_for_expr(test);
                                auto slice=Slice(value,nullptr,nullptr);
                                expr=Subscript(expr,slice,Load(),n->lineno,n->col_offset);
                                break;
                            }
                            else if((int)subl->childs[0]->childs[0]->type==COLON){
                                if((int)subl->childs[0]->childs[1]->type==test){//:test
                                    auto test = subl->childs[0]->childs[1];
                                    auto value=ast_for_expr(test);
                                    auto slice=Slice(nullptr,value,nullptr);
                                    expr=Subscript(expr, slice, Load(), n->lineno,n->col_offset);
                                    break;
                                }
                                else if((int)subl->childs[0]->childs[1]->type==sliceop){//:[sliceop]
                                    auto sliceop=subl->childs[0]->childs[1];
                                    if((int)sliceop->childs.size()==1){//::
                                        assert(sliceop->childs[0]->type==COLON);
                                        auto slice=Slice(nullptr,nullptr,nullptr);
                                        expr=Subscript(expr,slice,Load(),n->lineno,n->col_offset);
                                        break;
                                    }
                                    else if((int)sliceop->childs.size()==2){//::test
                                        assert(sliceop->childs[0]->type==COLON);
                                        assert(sliceop->childs[1]->type==test);
                                        auto test = sliceop->childs[1];
                                        auto value = ast_for_expr(test);
                                        auto slice=Slice(nullptr,nullptr,value);
                                        expr=Subscript(expr,slice,Load(),n->lineno,n->col_offset);
                                        break;
                                    }
                                    assert(false);
                                }
                            }
                            assert(false);
                        }
                        else if((int)subl->childs[0]->childs.size()==3){
                            if((int)subl->childs[0]->childs[0]->type==test){
                                assert((int)subl->childs[0]->childs[1]->type==COLON);
                                if((int)subl->childs[0]->childs[2]->type==test){//test:test
                                    auto test1=subl->childs[0]->childs[0];
                                    auto test2=subl->childs[0]->childs[2];
                                    auto value1=ast_for_expr(test1);
                                    auto value2=ast_for_expr(test2);
                                    auto slice=Slice(value1,value2,nullptr);
                                    expr=Subscript(expr, slice, Load(), n->lineno,n->col_offset);
                                    break;
                                }
                                else if((int)subl->childs[0]->childs[2]->type==sliceop){//test:[sliceop]
                                    auto test1=subl->childs[0]->childs[0];
                                    auto value1=ast_for_expr(test1);
                                    auto sliceop=subl->childs[0]->childs[2];
                                    if(sliceop->childs.size()==1){//test::
                                        auto slice=Slice(value1,nullptr,nullptr);
                                        expr=Subscript(expr,slice,Load(),n->lineno,n->col_offset);
                                        break;
                                    }
                                    else if(sliceop->childs.size()==2){//test::test
                                        auto test2=sliceop->childs[1];
                                        auto value2=ast_for_expr(test2);
                                        auto slice=Slice(value1,nullptr,value2);
                                        expr=Subscript(expr,slice,Load(),n->lineno,n->col_offset);
                                        break;
                                    }
                                }
                            }
                            else if((int)subl->childs[0]->childs[0]->type==COLON){//:test[sliceop]
                                assert((int)subl->childs[0]->childs[1]->type==test);
                                assert((int)subl->childs[0]->childs[2]->type==sliceop);
                                auto test1 = subl->childs[0]->childs[1];
                                auto value1 = ast_for_expr(test1);
                                auto sliceop=subl->childs[0]->childs[2];
                                if(sliceop->childs.size()==1){//:test:
                                    auto slice=Slice(nullptr,value1,nullptr);
                                    expr=Subscript(expr, slice, Load(), n->lineno,n->col_offset);
                                    break;
                                }
                                else if(sliceop->childs.size()==2){//:test:test
                                    assert((int)sliceop->childs[0]->type==COLON);
                                    auto test2 = sliceop->childs[1];
                                    auto value2 = ast_for_expr(test2);
                                    auto slice=Slice(nullptr,value1,value2);
                                    expr=Subscript(expr, slice, Load(), n->lineno,n->col_offset);
                                    break;
                                }
                            assert(false);
                            }
                        }
                        else if((int)subl->childs[0]->childs.size()==4){//test:test[sliceop]
                            assert((int)subl->childs[0]->childs[0]->type==test);
                            assert((int)subl->childs[0]->childs[1]->type==COLON);
                            assert((int)subl->childs[0]->childs[2]->type==test);
                            assert((int)subl->childs[0]->childs[3]->type==sliceop);
                            auto test1 = subl->childs[0]->childs[0];
                            auto test2 = subl->childs[0]->childs[2];
                            auto value1 = ast_for_expr(test1);
                            auto value2 = ast_for_expr(test2);
                            auto sliceop = subl->childs[0]->childs[3];
                            if(sliceop->childs.size()==1){//test:test:
                                auto slice = Slice(value1,value2,nullptr);
                                expr=Subscript(expr,slice,Load(),n->lineno,n->col_offset);
                                break;
                            }
                            else if(sliceop->childs.size()==2){//test:test:test
                                assert((int)sliceop->childs[0]->type==COLON);
                                auto test3 = sliceop->childs[1];
                                auto value3 = ast_for_expr(test3);
                                auto slice=Slice(value1,value2,value3);
                                expr=Subscript(expr,slice,Load(),n->lineno,n->col_offset);
                                break;
                            }
                            assert(false);
                        }
                        assert(false);
                    }
                    case DOT:{
                        auto id=new StringObject(n->childs[i]->childs[1]->str);
                        expr=Attribute(expr,id,Load(),n->lineno,n->col_offset);
                        break;
                    }
                    default:
                        //printf("%s\n",NonTerminalMap(n->type).c_str());
                        assert(false);
                }
            }
            return expr;
        }
        case atom:{
            switch(n->childs[0]->type){
                case LPAR:
                    if (n->childs[1]->type==RPAR)
                        return Tuple(new asdl_seq<expr_ty*>({}),Load(),n->lineno,n->col_offset);
                    return this->ast_for_testlist(n->childs[1]);
                case LSQB:{
                    if(n->childs[1]->type==RSQB)
                        return List(new asdl_seq<expr_ty*>({}),Load(),n->lineno,n->col_offset);
                    assert(n->childs[1]->type==testlist_comp);
                    n=n->childs[1];
                    if (n->childs.end()[-1]->type!=comp_for){
                        auto testlist=this->ast_for_testlist(n);
                        return List(testlist->Tuple()->elts,testlist->Tuple()->ctx,n->lineno,n->col_offset);
                    } else{
                        auto expr=this->ast_for_expr(n->childs[0]);
                        auto comp=this->ast_for_comp_for(n->childs[1]);
                        return ListComp(expr,comp,n->lineno,n->col_offset);
                    }
                }
                case LBRACE:{
                    if (n->childs[1]->type==RBRACE){
                        return Dict(nullptr,nullptr,n->lineno,n->col_offset);
                    }
                    if (n->childs[1]->type==dictorsetmaker){
                        n=n->childs[1];
                        auto keys=new asdl_seq<expr_ty*>({});
                        auto values=new asdl_seq<expr_ty*>({});
                        if (n->childs.end()[-1]->type!=comp_for){
                            for (int i=0;i<(int)n->childs.size();i+=4)
                                keys->PushBack(this->ast_for_expr(n->childs[i]));
                            for (int i=2;i<(int)n->childs.size();i+=4)
                                values->PushBack(this->ast_for_expr(n->childs[i]));
                            return Dict(keys,values,n->lineno,n->col_offset);
                        }
                    }
                    assert(false);
                }
                case NAME:
                    if (n->childs[0]->str=="None"){
                        return NameConstant(NoneObject::Singleton(),n->lineno,n->col_offset);
                    }
                    if (n->childs[0]->str=="True"){
                        return NameConstant(BoolObject::Singleton(true),n->lineno,n->col_offset);
                    }
                    if (n->childs[0]->str=="False"){
                        return NameConstant(BoolObject::Singleton(false),n->lineno,n->col_offset);
                    }
                    return Name(new StringObject(n->childs[0]->str),Load(),n->lineno,n->col_offset);
                case NUMBER:
                    return Num(new IntObject(n->childs[0]->str),n->lineno,n->col_offset);
                case STRING:
                    assert((int)n->childs.size()==1);
                    return Str(new StringObject(n->childs[0]->str,true),n->lineno,n->col_offset);
                case ELLIPSIS:
                    return Ellipsis(n->lineno,n->col_offset);
                default:
                    assert(false);
            }
        }
        default:
            printf("Error in CST2AST, Token: %s\n",NonTerminalMap(n->type).c_str());
            assert(false);
    }
}
asdl_seq<expr_ty*> *AST::ast_for_exprlist(Node *n){
    auto exprlist=new asdl_seq<expr_ty*>({});
    for (int i=0;i<(int)n->childs.size();i+=2)
        exprlist->PushBack(this->ast_for_expr(n->childs[i]));
    return exprlist;
}
asdl_seq<stmt_ty*> *AST::ast_for_suite(Node *n){
    if (n->childs[0]->type==NEWLINE){
        auto stmts=new asdl_seq<stmt_ty*>({});
        for (int i=2;i<(int)n->childs.size()-1;i++){
            if (n->childs[i]->childs[0]->type==simple_stmt){
                for (int j=0;j<(int)n->childs[i]->childs[0]->childs.size()-1;j+=2){
                    stmts->PushBack(this->ast_for_stmt(n->childs[i]->childs[0]->childs[j]));
                }
            } else if (n->childs[i]->childs[0]->type==compound_stmt){
                stmts->PushBack(this->ast_for_stmt(n->childs[i]));
            } else{
                assert(false);
            }
        }
        return stmts;
    } else if (n->childs[0]->type==simple_stmt){
        auto stmts=new asdl_seq<stmt_ty*>({});
        for (int i=0;i<(int)n->childs.size()-1;i+=2){
            stmts->PushBack(this->ast_for_stmt(n->childs[i]));
        }
        return stmts;
    } else{
        assert(false);
    }
}
expr_ty *AST::ast_for_testlist(Node *n){
    //printf("ast_for_testlist %d %d \"%s\"\n",(int)n->childs.size(),n->type,n->str.c_str());
    if ((int)n->childs.size()==1){
        return this->ast_for_expr(n->childs[0]);
    }
    auto elts=new asdl_seq<expr_ty*>({});
    for (int i=0;i<(int)n->childs.size();i+=2){
        elts->PushBack(this->ast_for_expr(n->childs[i]));
    }
    return Tuple(elts,Load(),n->lineno,n->col_offset);
}
operator_ty *AST::ast_for_augassign(Node *n){
    switch(n->childs[0]->str[0]){
        case '+':
            return Add();
        case '-':
            return Sub();
        case '*':
            if (n->childs[0]->str[1]=='*')
                return Pow();
            return Mult();
        case '/':
            if (n->childs[0]->str[1]=='/')
                return FloorDiv();
            return Div();
        case '<':
            return LSHIFT();
        case '>':
            return RSHIFT();
        case '&':
            return BitAnd();
        case '|':
            return BitOr();
        case '^':
            return BitXor();
    }
    assert(false);
}
asdl_seq<comprehension_ty*> *AST::ast_for_comp_for(Node *n){
    auto exprlist=this->ast_for_testlist(n->childs[1]);
    SetContext(exprlist,Store());
    auto gen=this->ast_for_expr(n->childs[3]);
    n=n->childs.end()[-1];
    if (n->type!=comp_iter){
        auto comps=new asdl_seq<comprehension_ty*>({});
        comps->PushBack(comprehension(exprlist,gen,nullptr));
        return comps;
    } else{
        auto ifs=new asdl_seq<expr_ty*>({});
        for (;;){
            assert(n->type==comp_iter);
            assert(n->childs.size()==1);
            n=n->childs[0];
            if (n->type==comp_for){
                auto comps=this->ast_for_comp_for(n);
                comps->PushFront(comprehension(exprlist,gen,ifs));
                return comps;
            }
            assert(n->type==comp_if);
            ifs->PushBack(this->ast_for_expr(n->childs[1]));
            n=n->childs.end()[-1];
            if (n->type!=comp_iter) break;
        }
        auto comps=new asdl_seq<comprehension_ty*>({});
        comps->PushBack(comprehension(exprlist,gen,ifs));
        return comps;
    }
    assert(false);
}
stmt_ty *AST::ast_for_if_stmt(Node *n){
    auto test=this->ast_for_expr(n->childs[1]);
    auto body=this->ast_for_suite(n->childs[3]);
    if ((int)n->childs.size()==4)
        return If(test,body,nullptr,n->lineno,n->col_offset);
    if (n->childs[4]->str[2]=='s'){
        auto orelse=this->ast_for_suite(n->childs[6]);
        return If(test,body,orelse,n->lineno,n->col_offset);
    }
    if (n->childs[(int)n->childs.size()-3]->str[2]=='s'){
        auto orelse=this->ast_for_suite(n->childs[(int)n->childs.size()-1]);
        for (int i=(int)n->childs.size()-7;i>3;i-=4){
            orelse=new asdl_seq<stmt_ty*>({If(this->ast_for_expr(n->childs[i+1]),this->ast_for_suite(n->childs[i+3]),orelse,n->childs[i+1]->lineno,n->childs[i+1]->col_offset)});
        }
        return If(test,body,orelse,n->lineno,n->col_offset);
    } else{
        asdl_seq<stmt_ty*> *orelse=nullptr;
        for (int i=(int)n->childs.size()-4;i>3;i-=4){
            orelse=new asdl_seq<stmt_ty*>({If(this->ast_for_expr(n->childs[i+1]),this->ast_for_suite(n->childs[i+3]),orelse,n->childs[i+1]->lineno,n->childs[i+1]->col_offset)});
        }
        return If(test,body,orelse,n->lineno,n->col_offset);
    }
}
stmt_ty *AST::ast_for_while_stmt(Node *n){
    auto test=this->ast_for_expr(n->childs[1]);
    auto body=this->ast_for_suite(n->childs[3]);
    if ((int)n->childs.size()==4)
        return While(test,body,nullptr,n->lineno,n->col_offset);
    auto orelse=this->ast_for_suite(n->childs[6]);
    return While(test,body,orelse,n->lineno,n->col_offset);
}
stmt_ty *AST::ast_for_for_stmt(Node *n){
    expr_ty *target=nullptr;
    if ((int)n->childs[1]->childs.size()==1){
        target=this->ast_for_expr(n->childs[1]->childs[0]);
        SetContext(target,Store());
    } else{
        auto exprlist=this->ast_for_exprlist(n->childs[1]);
        for (auto expr:exprlist->ToVector())
            SetContext(expr,Store());
        target=Tuple(exprlist,Store(),n->lineno,n->col_offset);
    }
    auto iter=this->ast_for_testlist(n->childs[3]);
    auto body=this->ast_for_suite(n->childs[5]);
    if ((int)n->childs.size()==6)
        return For(target,iter,body,nullptr,n->lineno,n->col_offset);
    auto orelse=this->ast_for_suite(n->childs[8]);
    return For(target,iter,body,orelse,n->lineno,n->col_offset);
}
stmt_ty *AST::ast_for_funcdef_stmt(Node *n){
    auto name=new StringObject(n->childs[1]->str);
    auto args=this->ast_for_parameters(n->childs[2]);
    auto body=this->ast_for_suite(n->childs[(int)n->childs.size()-1]);
    return FunctionDef(name,args,body,n->lineno,n->col_offset);
}
stmt_ty *AST::ast_for_classdef_stmt(Node *n){
    assert(n->type==classdef);
    auto name=new StringObject(n->childs[1]->str);
    auto body=this->ast_for_suite(n->childs.end()[-1]);
    if ((int)n->childs.size()<=6){
        return ClassDef(name,nullptr,body,n->lineno,n->col_offset);
    }
    auto args=new asdl_seq<expr_ty*>({});
    for (int i=0;i<(int)n->childs[3]->childs.size();i+=2){
        assert((int)n->childs[3]->childs[i]->childs.size()==1);
        args->PushBack(this->ast_for_expr(n->childs[3]->childs[i]->childs[0]));
    }
    return ClassDef(name,args,body,n->lineno,n->col_offset);
}
stmt_ty *AST::ast_for_expr_stmt(Node *n){
    //printf("ast_for_expr_stmt %d n:%d c:%d\n",(int)n->childs.size(),n->lineno,n->col_offset);
    if ((int)n->childs.size()==1){
        auto expr=this->ast_for_testlist(n->childs[0]);
        return Expr(expr,n->lineno,n->col_offset);
    }
    if (n->childs[1]->type==augassign){
        auto expr0=this->ast_for_testlist(n->childs[0]);
        SetContext(expr0,Store());
        switch(expr0->kind){
            case NameKind:
            case AttributeKind:
            break;
            default:
                assert(false);
        }
        auto augassign=this->ast_for_augassign(n->childs[1]);
        auto expr1=this->ast_for_testlist(n->childs[2]);
        return AugAssign(expr0,augassign,expr1,n->lineno,n->col_offset);
    }
    auto *targets=new asdl_seq<expr_ty*>({});
    for (int i=0;i<(int)n->childs.size()-1;i+=2){
        assert(n->childs[i+1]->type==EQUAL);
        auto testlist=this->ast_for_testlist(n->childs[i]);
        SetContext(testlist,Store());
        targets->PushBack(testlist);
    }
    return Assign(targets,this->ast_for_testlist(n->childs[(int)n->childs.size()-1]),n->lineno,n->col_offset);
}
stmt_ty *AST::ast_for_stmt(Node *n){
    switch(n->type){
        case stmt:
            assert((int)n->childs.size()==1);
            return this->ast_for_stmt(n->childs[0]);
        case simple_stmt:
            assert((int)n->childs.size()<=3);
            return this->ast_for_stmt(n->childs[0]);
        case small_stmt:{
            switch(n->childs[0]->type){
                case expr_stmt:
                    return this->ast_for_expr_stmt(n->childs[0]);
                case flow_stmt:
                    switch (n->childs[0]->childs[0]->type){
                        case break_stmt:
                            return Break(n->lineno,n->col_offset);
                        case continue_stmt:
                            return Continue(n->lineno,n->col_offset);
                        case return_stmt:{
                            if ((int)n->childs[0]->childs[0]->childs.size()==1)
                                return Return(nullptr,n->lineno,n->col_offset);
                            return Return(this->ast_for_testlist(n->childs[0]->childs[0]->childs[1]),n->lineno,n->col_offset);
                        }
                        case yield_expr:{
                            if (n->childs[0]->childs[0]->childs.size()==1){
                                return Expr(Yield(nullptr,n->lineno,n->col_offset),n->lineno,n->col_offset);
                            }
                            return Expr(Yield(this->ast_for_testlist(n->childs[0]->childs[0]->childs[1]),n->lineno,n->col_offset),n->lineno,n->col_offset);
                        }
                        default:
                            assert(false);
                    }
                case pass_stmt:
                    return Pass(n->lineno,n->col_offset);
                case nonlocal_stmt:{
                    auto names=new asdl_seq<identifier_ty*>({});
                    for (int i=1;i<(int)n->childs[0]->childs.size();i+=2){
                        names->PushBack(new StringObject(n->childs[0]->childs[i]->str));
                    }
                    return Nonlocal(names,n->lineno,n->col_offset);
                }
                case global_stmt:{
                    auto names=new asdl_seq<identifier_ty*>({});
                    for (int i=1;i<(int)n->childs[0]->childs.size();i+=2){
                        names->PushBack(new StringObject(n->childs[0]->childs[i]->str));
                    }
                    return Global(names,n->lineno,n->col_offset);
                }
                default:
                    assert(false);
            }
        }
        case compound_stmt:{
            switch(n->childs[0]->type){
                case if_stmt:
                    return this->ast_for_if_stmt(n->childs[0]);
                case while_stmt:
                    return this->ast_for_while_stmt(n->childs[0]);
                case for_stmt:
                    return this->ast_for_for_stmt(n->childs[0]);
                case funcdef:
                    return this->ast_for_funcdef_stmt(n->childs[0]);
                case classdef:
                    return this->ast_for_classdef_stmt(n->childs[0]);
                default:
                    assert(false);
            }
        }
        default:
            assert(false);
    }
}
mod_ty *AST::ast_for_mod(Node *n){
    switch(n->type){
        case single_input:{
            auto stmts=new asdl_seq<stmt_ty*>({});
            if (n->childs[0]->type==NEWLINE){
                stmts->PushBack(Pass(n->lineno,n->col_offset));
            } else if (n->childs[0]->type==simple_stmt){
                stmts->PushBack(this->ast_for_stmt(n->childs[0]));
            } else if (n->childs[0]->type==compound_stmt){
                for (int i=0;i<(int)n->childs[0]->childs.size()-1;i+=2)
                    stmts->PushBack(this->ast_for_stmt(n->childs[0]->childs[i]));
            }
            return Interactive(stmts);
        }
        case file_input:{
            auto stmts=new asdl_seq<stmt_ty*>({});
            for (int i=0;i<(int)n->childs.size()-1;i++){
                if (n->childs[i]->type==NEWLINE) continue;
                if (n->childs[i]->childs[0]->type==simple_stmt){
                    for (int j=0;j<(int)n->childs[i]->childs[0]->childs.size()-1;j+=2)
                        stmts->PushBack(this->ast_for_stmt(n->childs[i]->childs[0]->childs[j]));
                } else if (n->childs[i]->childs[0]->type==compound_stmt){
                    stmts->PushBack(this->ast_for_stmt(n->childs[i]->childs[0]));
                } else{
                    assert(false);
                }
            }
            return Module(stmts,nullptr);
        }
        case eval_input:{
            auto body=this->ast_for_expr(n->childs[0]);
            return Expression(body);
        }
    }
    assert(false);
}
mod_ty *AST::FromNode(Node *n){
    return this->ast_for_mod(n);
}
