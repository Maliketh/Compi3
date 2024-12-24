#ifndef VISITOR_HPP
#define VISITOR_HPP



namespace ast {
    class Num;
    class NumB;
    class String;
    class Bool;
    class ID;
    class BinOp;
    class RelOp;
    class Not;
    class And;
    class Or;
    class Type;
    class Cast;
    class ExpList;
    class Call;
    class Statements;
    class Break;
    class Continue;
    class Return;
    class If;
    class While;
    class VarDecl;
    class Assign;
    class Formal;
    class Formals;
    class FuncDecl;
    class Funcs;

    enum BuiltInType {
        NONE = -1,
        VOID,
        BOOL,
        BYTE,
        INT,
        STRING
    };

}

class Visitor {
public:
    virtual ast::BuiltInType visit(ast::Num &node, int* val) = 0;

    virtual ast::BuiltInType visit(ast::NumB &node, int* val) = 0;

    virtual ast::BuiltInType visit(ast::String &node, int* val) = 0;

    virtual ast::BuiltInType visit(ast::Bool &node, int* val) = 0;

    virtual ast::BuiltInType visit(ast::ID &node, int* val) = 0;

    virtual ast::BuiltInType visit(ast::BinOp &node, int* val) = 0;

    virtual ast::BuiltInType visit(ast::RelOp &node, int* val) = 0;

    virtual ast::BuiltInType visit(ast::Not &node, int* val) = 0;

    virtual ast::BuiltInType visit(ast::And &node, int* val) = 0;

    virtual ast::BuiltInType visit(ast::Or &node, int* val) = 0;

    virtual ast::BuiltInType visit(ast::Type &node) = 0;

    virtual ast::BuiltInType visit(ast::Cast &node, int* val) = 0;

    virtual ast::BuiltInType visit(ast::ExpList &node,  std::vector<ast::BuiltInType>* paramTypes) = 0;

    virtual ast::BuiltInType visit(ast::Call &node) = 0;

    virtual ast::BuiltInType visit(ast::Statements &node) = 0;

    virtual ast::BuiltInType visit(ast::Break &node) = 0;

    virtual ast::BuiltInType visit(ast::Continue &node) = 0;

    virtual ast::BuiltInType visit(ast::Return &node) = 0;

    virtual ast::BuiltInType visit(ast::If &node) = 0;

    virtual ast::BuiltInType visit(ast::While &node) = 0;

    virtual ast::BuiltInType visit(ast::VarDecl &node) = 0;

    virtual ast::BuiltInType visit(ast::Assign &node) = 0;

    virtual ast::BuiltInType visit(ast::Formal &node) = 0;

    virtual ast::BuiltInType visit(ast::Formals &node, std::vector<ast::BuiltInType>* params_type,std::vector<std::string>* params_name) = 0;

    virtual ast::BuiltInType visit(ast::FuncDecl &node) = 0;

    virtual ast::BuiltInType visit(ast::Funcs &node) = 0;
    virtual ast::BuiltInType visit(ast::Call& node, int* p) =0 ;
};

#endif //VISITOR_HPP
