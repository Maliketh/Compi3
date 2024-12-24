#ifndef SEMANTIC_ANALYZER_HPP
#define SEMANTIC_ANALYZER_HPP

#include "visitor.hpp"
#include "SymbolTable.hpp"
#include "output.hpp"
#include <iostream>


static int convert_int_to_byte (int num, int line);

static bool is_num_type (ast::BuiltInType type);

static bool compare_exp_list (std::vector<ast::BuiltInType>& paramTypesDecl, std::vector<ast::BuiltInType>& paramTypesCall);

class SemanticAnalyzer : public Visitor {
public:
    // Implement visit methods for each type of AST node

    class SymbolTable sym_table;

    void register_func(ast::FuncDecl& node)
    {
        std::vector<ast::BuiltInType> paramTypes;
        for (auto formal : node.formals->formals)
            paramTypes.push_back(formal->type->type);
        sym_table.insertSymbol(node.id->value, node.return_type->type ,paramTypes);
        std::cout << "adding a func" << std::endl;
    }

    ast::BuiltInType visit(ast::Num& node, int* val) override {
        *val = node.value;
        std::cout << "Analyzing Num node" << std::endl;
        return ast::BuiltInType::INT;

    }

    ast::BuiltInType visit(ast::NumB& node, int* val) override {
        *val = node.value;
        std::cout << "Analyzing NumB node" << std::endl;
        return ast::BuiltInType::BYTE;
    }

    ast::BuiltInType visit(ast::String& node, int* val) override {
        std::cout << "Analyzing String node" << std::endl;
        return ast::BuiltInType::STRING;
    }

    ast::BuiltInType visit(ast::Bool& node, int* val) override {
        *val = node.value;
        std::cout << "Analyzing Bool node" << std::endl;
        return ast::BuiltInType::BOOL;
    }

    ast::BuiltInType visit(ast::ID& node, int* val) override {
        std::cout << "Analyzing ID node" << std::endl;
        ast::BuiltInType type = sym_table.getSymbolType(node.value);
        if (type ==  ast::BuiltInType::NONE)
            output::errorUndef(node.line, node.value);
        return type;

    }

    ast::BuiltInType visit(ast::BinOp& node, int* val) override {
        std::cout << "Analyzing BinOp node" << std::endl;
        int* val_1 = nullptr;
        int* val_2 = nullptr;
        ast::BuiltInType type_1 = node.left->accept(*this, nullptr);
        ast::BuiltInType type_2 = node.right->accept(*this, nullptr);
        if (!is_num_type(type_1) || !is_num_type(type_2))
            output::errorMismatch(node.line);
        ast::BuiltInType  type = ast::BuiltInType::BYTE;
        if (type_1 == ast::BuiltInType::INT || type_2 == ast::BuiltInType::INT )
            type = ast::BuiltInType::INT;

        switch (node.op)
        {
            case ast::BinOpType::ADD:
                *val = *val_1 + *val_2;
                break;
            case ast::BinOpType::SUB:
                *val = *val_1 - *val_2;
                break;
            case ast::BinOpType::MUL:
                *val = *val_1 * *val_2;
                break;
            case ast::BinOpType::DIV:
                *val = *val_1 / *val_2;
                break;
        }
        if (type == ast::BuiltInType::BYTE)
            convert_int_to_byte (*val, node.line);

        return type;

    }

    ast::BuiltInType visit(ast::RelOp& node, int* val) override {
        std::cout << "Analyzing RelOp node" << std::endl;
        int* val_1 = nullptr;
        int* val_2 = nullptr;
        ast::BuiltInType type_1 = node.left->accept(*this, nullptr);
        ast::BuiltInType type_2 = node.right->accept(*this, nullptr);
        if (!is_num_type(type_1) || !is_num_type(type_2))
            output::errorMismatch(node.line);
        return ast::BuiltInType::BOOL;
    }

    ast::BuiltInType visit(ast::Not& node, int* val)  override {
        std::cout << "Analyzing Not node" << std::endl;
        ast::BuiltInType type = node.exp->accept(*this, nullptr);
        if (type != ast::BuiltInType::BOOL)
            output::errorMismatch(node.line);

        return ast::BuiltInType::BOOL;
    }

    ast::BuiltInType visit(ast::And& node, int* val) override {
        std::cout << "Analyzing And node" << std::endl;
        ast::BuiltInType type_1 = node.left->accept(*this, nullptr);
        ast::BuiltInType type_2 = node.right->accept(*this, nullptr);

        if (type_1 != ast::BuiltInType::BOOL || type_2 != ast::BuiltInType::BOOL)
            output::errorMismatch(node.line);

        return ast::BuiltInType::BOOL;

    }

    ast::BuiltInType visit(ast::Or& node, int* val) override {
        std::cout << "Analyzing Or node" << std::endl;
        ast::BuiltInType type_1 = node.left->accept(*this, nullptr);
        ast::BuiltInType type_2 = node.right->accept(*this, nullptr);

        if (type_1 != ast::BuiltInType::BOOL || type_2 != ast::BuiltInType::BOOL)
            output::errorMismatch(node.line);

        return ast::BuiltInType::BOOL;
    }

    ast::BuiltInType visit(ast::Type& node) override {
        std::cout << "Analyzing Type node" << std::endl;
        return node.type;
    }

    ast::BuiltInType visit(ast::Cast& node, int* val) override {
        std::cout << "Analyzing Cast node" << std::endl;
        int *exp_val = nullptr;
        ast::BuiltInType exp_type = node.exp->accept(*this,exp_val);
        if (node.target_type->type == ast::BuiltInType::BYTE && exp_type == ast::BuiltInType::INT) {
            if (exp_val != nullptr)
                *val = convert_int_to_byte (*exp_val, node.line);
        }
        else if (node.target_type->type == ast::BuiltInType::INT && exp_type == ast::BuiltInType::BYTE) {
            if (exp_val != nullptr)
                *val = *exp_val;
        }
        else if (exp_type != node.target_type->type)
            output::errorMismatch(node.line);
        return node.target_type->type;

    }

    ast::BuiltInType visit(ast::ExpList& node,  std::vector<ast::BuiltInType>* paramTypes) override {
        std::cout << "Analyzing ExpList node" << std::endl;
        ast::BuiltInType exp_type =  ast::BuiltInType::NONE;
        for (auto exp_node : node.exps) {
            exp_type = exp_node->accept(*this, nullptr);
            paramTypes->push_back(exp_type);
        }
        return  ast::BuiltInType::NONE;
    } //change i belive

    ast::BuiltInType visit(ast::Call& node) override {
        Symbol *p_sym = sym_table.lookupSymbol(node.func_id->value);
        if (!sym_table.checkFunctionCall(node.func_id->value))
            output::errorUndefFunc(node.line, node.func_id->value);
        if (!p_sym->is_func)
            output::errorDefAsVar(node.line, node.func_id->value);
        std::vector<ast::BuiltInType>   params;
        visit(*node.args, &params);
        if (!compare_exp_list(params, p_sym->paramTypes))
            return p_sym->type;//output::errorPrototypeMismatch(node.line, node.func_id->value, p_sym->paramTypes);
        std::cout << "Analyzing Call node" << std::endl;
        return p_sym->type;
    }

    ast::BuiltInType visit(ast::Call& node, int* p) override {
        return visit(node);
    }

    ast::BuiltInType visit(ast::Statements& node) override {
       for (auto statment : node.statements)
           statment->accept(*this);
        return  ast::BuiltInType::NONE;
    }

    ast::BuiltInType visit(ast::Break& node) override {
        if (sym_table.currentScope != nullptr ||
            sym_table.currentScope->scopeType != ScopeType::COND)
             output::errorUnexpectedBreak (node.line);
        std::cout << "Analyzing Break node" << std::endl;
        return  ast::BuiltInType::NONE;
    }

    ast::BuiltInType visit(ast::Continue& node) override {
        if (sym_table.currentScope != nullptr ||
            sym_table.currentScope->scopeType != ScopeType::COND)
            output::errorUnexpectedContinue (node.line);
        return  ast::BuiltInType::NONE;
    }

    ast::BuiltInType  visit(ast::Return& node) override {
        if (sym_table.currentScope != nullptr ||
            sym_table.currentScope->scopeType != ScopeType::FUNC)
            std::cout << "check error return" << std::endl;
        if (node.exp != NULL && sym_table.currentScope->ret_scope_type == ast::BuiltInType::VOID)
            output::errorMismatch( node.line);
        if (node.exp == NULL && sym_table.currentScope->ret_scope_type != ast::BuiltInType::VOID)
            output::errorMismatch( node.line);
        if (node.exp->accept(*this, nullptr) != sym_table.currentScope->ret_scope_type)
            output::errorMismatch( node.line);
        return  ast::BuiltInType::NONE;

    }

    ast::BuiltInType visit(ast::If& node) override {
        if (node.condition->accept(*this) != ast::BuiltInType::BOOL)
            output::errorMismatch( node.line);
        sym_table.enterScope(ScopeType::COND);
        node.then->accept(*this);
        sym_table.exitScope();
        if (node.otherwise != nullptr)
        {
            sym_table.enterScope(ScopeType::COND);
            node.then->accept(*this);
            sym_table.exitScope();
        }
        std::cout << "Analyzing If node" << std::endl;
        return  ast::BuiltInType::NONE;
    }

    ast::BuiltInType visit(ast::While& node) override { //scope printer
        if (node.condition->accept(*this, nullptr) != ast::BuiltInType::BOOL)
            output::errorMismatch( node.line);
        sym_table.enterScope(ScopeType::COND);
        node.body->accept(*this);
        sym_table.exitScope();
        std::cout << "Analyzing While node" << std::endl;
        return ast::BuiltInType::NONE;
    }

    ast::BuiltInType visit(ast::VarDecl& node) override {
        std::string name = node.id->value;
        ast::BuiltInType type = visit (*node.type);
        ast::BuiltInType exp_type =  ast::BuiltInType::NONE;
        int *val = nullptr;
        if (node.init_exp != nullptr) {
            exp_type = node.init_exp->accept(*this, val);
            switch (type) {
                case ast::BuiltInType::INT:
                    if (exp_type !=  ast::BuiltInType::NONE || !is_num_type(exp_type))
                        output::errorMismatch(node.line);
                    break;

                case ast::BuiltInType::BYTE:
                    if (exp_type !=  ast::BuiltInType::NONE || exp_type != ast::BuiltInType::BYTE)
                        output::errorMismatch(node.line);
                    break;

                case ast::BuiltInType::BOOL:
                    if (exp_type !=  ast::BuiltInType::NONE || exp_type != ast::BuiltInType::BOOL)
                        output::errorMismatch(node.line);
                    break;

                case ast::BuiltInType::STRING:
                    //CHECK ITS FROM PRINT
                    output::errorMismatch(node.line);
                    break;

                case ast::BuiltInType::VOID:
                    output::errorMismatch(node.line);
                    break;

            }
        }
        sym_table.insertSymbol(name, type);
        std::cout << "Analyzing VarDecl node" << std::endl;
        return  ast::BuiltInType::NONE;
    }

    ast::BuiltInType visit(ast::Assign& node) override {
        ast::BuiltInType dest_type = node.id->accept(*this, nullptr);
        ast::BuiltInType src_type= node.exp->accept(*this, nullptr);
        if(dest_type != src_type || dest_type == ast::BuiltInType::NONE)
            output::errorMismatch(node.line);
        std::cout << "Analyzing Assign node" << std::endl;
        return ast::BuiltInType::NONE;
    }

    ast::BuiltInType visit(ast::Formal& node) override {
        if (sym_table.lookupSymbol(node.id->value))
            output::errorDef(node.line, node.id->value);

        return visit(*node.type);
        std::cout << "Analyzing Formal node" << std::endl;
    }

    ast::BuiltInType visit(ast::Formals& node, std::vector<ast::BuiltInType>* params_type,std::vector<std::string>* params_name )  {
        for (auto formal : node.formals)
        {
            params_type->push_back(formal->type->type);
            params_name->push_back(formal->id->value);
        }

        std::cout << "Analyzing Formals node" << std::endl;
        return ast::BuiltInType::NONE;
    }


    ast::BuiltInType visit(ast::FuncDecl& node) override {
        std::vector<ast::BuiltInType> params_type;
        std::vector<std::string> params_name;
        visit(*node.formals, &params_type, &params_name);
        sym_table.enterScope(ScopeType::FUNC, params_type, params_name , node.return_type->type);
        visit (*node.body);
        sym_table.exitScope();
        return  ast::BuiltInType::NONE;
    }


    ast::BuiltInType visit(ast::Funcs& node) override {
        for (auto func : node.funcs)
            register_func (*func);
        for (auto func : node.funcs)
            visit(*func);
        return  ast::BuiltInType::NONE;
        std::cout << "Analyzing Funcs node" << std::endl;
    }
};

static int convert_int_to_byte (int num, int line)
{
    if (num > 255 || num < 0)
        output::errorByteTooLarge(line, num);
    return num;
}

static bool is_num_type (ast::BuiltInType type) {
    return type == ast::BuiltInType::INT || type == ast::BuiltInType::BYTE;
}

static bool compare_exp_list (std::vector<ast::BuiltInType>& paramTypesDecl, std::vector<ast::BuiltInType>& paramTypesCall) {
    bool is_legal = false;
    if (paramTypesCall.size() == paramTypesDecl.size())
    {
        is_legal = true;
        for (int i = 0; i < paramTypesCall.size() && is_legal; i++)
            is_legal= paramTypesCall[i] == paramTypesDecl[i];
    }
    return is_legal;

}



#endif // SEMANTIC_ANALYZER_HPP
