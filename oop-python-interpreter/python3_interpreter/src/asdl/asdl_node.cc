#include "asdl_node.h"
inline std::string TransToTypeName(const std::string &str){
    //if (str=="identifier") return str;
    //if (str=="object") return str;
    //if (str=="string") return str;
    return str+"_ty";
}
void Definition::PrintDef(FILE *fp) const{
    if (this->kind==this->sum_kind){
        fprintf(fp,"enum %s_enum{\n",TransToTypeName(this->type_id).c_str());
        for (auto constructor:sum->constructors){
            fprintf(fp,"    %sKind,\n",constructor->constructor_id.c_str());
        }
        fprintf(fp,"};\n");
        fprintf(fp,"struct %s{\n",TransToTypeName(this->type_id).c_str());
        auto sum=this->sum;

        fprintf(fp,"    %s_enum kind;\n",TransToTypeName(this->type_id).c_str());

        fprintf(fp,"    union{\n");
        for (auto constructor:sum->constructors){
            if (constructor->fields.empty()&&sum->attributes.empty()) continue;
            fprintf(fp,"        struct{\n");
            for (auto field:constructor->fields){
                if (field->is_star){
                    fprintf(fp,"            asdl_seq<%s*> *%s;\n",TransToTypeName(field->type_id).c_str(),field->id.c_str());
                } else{
                    fprintf(fp,"            %s *%s;\n",TransToTypeName(field->type_id).c_str(),field->id.c_str());
                }
            }
            fprintf(fp,"        } %s;\n",constructor->constructor_id.c_str());
        }
        fprintf(fp,"    } v;\n");

        for (auto constructor:sum->constructors){
            if (constructor->fields.empty()) continue;
            fprintf(fp,"    auto %s()->decltype(&v.%s);\n",constructor->constructor_id.c_str(),constructor->constructor_id.c_str());
        }

        for (auto field:sum->attributes){
            fprintf(fp,"    %s %s;\n",field->type_id.c_str(),field->id.c_str());
        }

        fprintf(fp,"};\n");
    } else if (this->kind==this->product_kind){
        fprintf(fp,"struct %s{\n",TransToTypeName(this->type_id).c_str());
        auto product=this->product;
        for (auto field:product->fields){
            if (field->is_star){
                fprintf(fp,"    asdl_seq<%s*> *%s;\n",TransToTypeName(field->type_id).c_str(),field->id.c_str());
            } else{
                fprintf(fp,"    %s *%s;\n",TransToTypeName(field->type_id).c_str(),field->id.c_str());
            }
        }
        for (auto field:product->attributes){
            fprintf(fp,"    %s %s;\n",field->type_id.c_str(),field->id.c_str());
        }
        fprintf(fp,"};\n");
    } else{
        assert(false);
    }
}
void Module::PrintDef(FILE *fp) const{
    for (const auto def:this->definitions){
        fprintf(fp,"struct %s;\n",TransToTypeName(def->type_id).c_str());
    }
    fprintf(fp,"\n");
    for (auto def:this->definitions){
        def->PrintDef(fp);
    }
    for (auto def:this->definitions){
        if (def->kind==def->sum_kind){
            auto sum=def->sum;
            for (auto constructor:sum->constructors){
                //if (constructor->fields.empty()&&sum->attributes.empty()) continue;
                bool first=true;
                fprintf(fp,"%s *%s(",TransToTypeName(def->type_id).c_str(),constructor->constructor_id.c_str());
                for (auto field:constructor->fields){
                    if (!first) fprintf(fp,",");
                    first=false;
                    if (field->is_star){
                        fprintf(fp,"asdl_seq<%s*> *%s",TransToTypeName(field->type_id).c_str(),field->id.c_str());
                    } else{
                        fprintf(fp,"%s *%s",TransToTypeName(field->type_id).c_str(),field->id.c_str());
                    }
                }
                for (auto field:sum->attributes){
                    if (!first) fprintf(fp,",");
                    first=false;
                    fprintf(fp,"%s %s",field->type_id.c_str(),field->id.c_str());
                }
                fprintf(fp,");\n");
            }
        } else if (def->kind==def->product_kind){
            auto product=def->product;
            fprintf(fp,"%s *%s(",TransToTypeName(def->type_id).c_str(),def->type_id.c_str());
            bool first=true;
            for (auto field:product->fields){
                if (!first) fprintf(fp,",");
                first=false;
                if (field->is_star){
                    fprintf(fp,"asdl_seq<%s*> *%s",TransToTypeName(field->type_id).c_str(),field->id.c_str());
                } else{
                    fprintf(fp,"%s *%s",TransToTypeName(field->type_id).c_str(),field->id.c_str());
                }
            }
            for (auto field:product->attributes){
                if (!first) fprintf(fp,",");
                first=false;
                fprintf(fp,"%s %s",field->type_id.c_str(),field->id.c_str());
            }
            fprintf(fp,");\n");
        } else{
            assert(false);
        }
    }
}
void Module::PrintImplement(FILE *fp) const{
    for (auto def:this->definitions){
        if (def->kind==def->sum_kind){
            auto sum=def->sum;
            for (auto constructor:sum->constructors){
                if (constructor->fields.empty()) continue;
                fprintf(fp,"auto %s::%s()->decltype(&v.%s){\n",TransToTypeName(def->type_id.c_str()).c_str(),constructor->constructor_id.c_str(),constructor->constructor_id.c_str());
                fprintf(fp,"    assert(kind==%sKind);\n",constructor->constructor_id.c_str());
                fprintf(fp,"    return &v.%s;\n",constructor->constructor_id.c_str());
                fprintf(fp,"}\n");
            }
        }
    }
    for (auto def:this->definitions){
        if (def->kind==def->sum_kind){
            auto sum=def->sum;
            for (auto constructor:sum->constructors){
                //if (constructor->fields.empty()&&sum->attributes.empty()) continue;
                bool first=true;
                fprintf(fp,"%s *%s(",TransToTypeName(def->type_id).c_str(),constructor->constructor_id.c_str());
                for (auto field:constructor->fields){
                    if (!first) fprintf(fp,",");
                    first=false;
                    if (field->is_star){
                        fprintf(fp,"asdl_seq<%s*> *%s",TransToTypeName(field->type_id).c_str(),field->id.c_str());
                    } else{
                        fprintf(fp,"%s *%s",TransToTypeName(field->type_id).c_str(),field->id.c_str());
                    }
                }
                for (auto field:sum->attributes){
                    if (!first) fprintf(fp,",");
                    first=false;
                    fprintf(fp,"%s %s",field->type_id.c_str(),field->id.c_str());
                }
                fprintf(fp,"){\n");
                for (auto field:constructor->fields){
                    if (!field->is_star&&!field->is_opt){
                        fprintf(fp,"    assert(%s!=nullptr);\n",field->id.c_str());
                    }
                }
                fprintf(fp,"    auto *ptr=new %s();\n",TransToTypeName(def->type_id).c_str());
                fprintf(fp,"    ptr->kind=%sKind;\n",constructor->constructor_id.c_str());
                for (auto field:constructor->fields){
                    fprintf(fp,"    ptr->v.%s.%s=%s;\n",constructor->constructor_id.c_str(),field->id.c_str(),field->id.c_str());
                }
                for (auto field:sum->attributes){
                    fprintf(fp,"    ptr->%s=%s;\n",field->id.c_str(),field->id.c_str());
                }
                fprintf(fp,"    return ptr;\n");

                fprintf(fp,"};\n");
 
            }
        } else if (def->kind==def->product_kind){
            auto product=def->product;
            fprintf(fp,"%s *%s(",TransToTypeName(def->type_id).c_str(),def->type_id.c_str());
            bool first=true;
            for (auto field:product->fields){
                if (!first) fprintf(fp,",");
                first=false;
                if (field->is_star){
                    fprintf(fp,"asdl_seq<%s*> *%s",TransToTypeName(field->type_id).c_str(),field->id.c_str());
                } else{
                    fprintf(fp,"%s *%s",TransToTypeName(field->type_id).c_str(),field->id.c_str());
                }
            }
            for (auto field:product->attributes){
                if (!first) fprintf(fp,",");
                first=false;
                fprintf(fp,"%s %s",field->type_id.c_str(),field->id.c_str());
            }
            fprintf(fp,"){\n");
            for (auto field:product->fields){
                if (!field->is_star&&!field->is_opt){
                    fprintf(fp,"    assert(%s!=nullptr);\n",field->id.c_str());
                }
            }
            fprintf(fp,"    auto *ptr=new %s();\n",TransToTypeName(def->type_id).c_str());
            for (auto field:product->fields){
                fprintf(fp,"    ptr->%s=%s;\n",field->id.c_str(),field->id.c_str());
            }
            for (auto field:product->attributes){
                fprintf(fp,"    ptr->%s=%s;\n",field->id.c_str(),field->id.c_str());
            }
            fprintf(fp,"    return ptr;\n");
            fprintf(fp,"}\n");
        } else{
            assert(false);
        }
    }
}
