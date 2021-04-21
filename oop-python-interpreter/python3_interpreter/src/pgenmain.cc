#include <cstdio>
#include <cstdlib>
#include "grammar.h"
#include "metagrammar.h"
#include "node.h"
#include "parser.h"
#include "pgen.h"
FILE *openfile(const char *filename,const char *mode){
    FILE *fp=fopen(filename,mode);
    if (fp==nullptr){
        perror(filename);
        exit(1);
    }
    return fp;
}
void print(Node *p){
    static int c=0;
    assert(p!=nullptr);
    //for (int i=0;i<c;i++) printf("    ");
    //printf("type:%d str:\"%s\" r:%d c:%d nr_child:%d\n",p->type,p->str.c_str(),p->lineno,p->col_offset,(int)p->childs.size());
    c++;
    for (Node *q:p->childs) print(q);
    c--;
}
Grammar *GetGrammar(FILE *fp){
    Grammar *g0=MetaGrammar();
    Parser *meta_parser=new Parser(g0,MSTART);
    Node *n=meta_parser->ParseFile(fp);
    print(n);
    Grammar *g=pgen(n);
    delete meta_parser;
    return g;
}
int main(int argc,char *argv[]){
    if (argc!=4){
        fprintf(stdout,"usage: %s grammar graminit.h graminit.c\n",argv[0]);
        return 0;
    }
    FILE *fp;
    const char *gramfile=argv[1],*gram_h=argv[2],*gram_c=argv[3];

    fp=openfile(gramfile,"r");
    auto g=GetGrammar(fp);
    fclose(fp);

    fp=openfile(gram_h,"w");
    g->PrintNonterminals(fp);
    fclose(fp);

    fp=openfile(gram_c,"w");
    g->PrintGrammar(fp);
    fclose(fp);

    delete g;

    return 0;
}
