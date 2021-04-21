class TypeObject;

class Object{
public:
    TypeObject *type_obj;
    Object(TypeObject*);
};

typedef Object *(*UnaryFunc)(Object*);
typedef Object *(*BinaryFunc)(Object*,Object*);
typedef Object *(*TernaryFunc)(Object*,Object*,Object*);

class NumberMethod{
public:
    UnaryFunc nb_negative;
    BinaryFunc nb_add;
    BinaryFunc nb_inplace_add;
};
class CompareMethod{
public:
    BinaryFunc cmp_eq;
    BinaryFunc cmp_lt;
    BinaryFunc cmp_le;
};
class TypeObject:public Object,public NumberMethod,public CompareMethod{
public:
    TypeObject();
};
