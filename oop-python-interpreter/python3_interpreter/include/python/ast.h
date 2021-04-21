#pragma once
#include "common.h"
#include "python_ast.h"
#include "node.h"
#include "token.h"
class AST{
    operator_ty *ast_for_bin_op(Node *n);
    cmpop_ty *ast_for_comp_op(Node *n);
    arguments_ty *ast_for_parameters(Node *n);
    operator_ty *ast_for_augassign(Node *n);
    asdl_seq<comprehension_ty*> *ast_for_comp_for(Node *n);
    asdl_seq<expr_ty*> *ast_for_exprlist(Node *n);
    expr_ty *ast_for_expr(Node *n);
    expr_ty *ast_for_testlist(Node *n);
    asdl_seq<stmt_ty*> *ast_for_suite(Node *n);
    stmt_ty *ast_for_if_stmt(Node *n);
    stmt_ty *ast_for_while_stmt(Node *n);
    stmt_ty *ast_for_for_stmt(Node *n);
    stmt_ty *ast_for_funcdef_stmt(Node *n);
    stmt_ty *ast_for_classdef_stmt(Node *n);
    stmt_ty *ast_for_expr_stmt(Node *n);
    stmt_ty *ast_for_stmt(Node *n);
    mod_ty *ast_for_mod(Node *n);
public:
    AST(){}
    mod_ty *FromNode(Node *n);
};
