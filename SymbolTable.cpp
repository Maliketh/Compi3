#ifndef SEMANTIC_ANALYZER_HPP
#define SEMANTIC_ANALYZER_HPP

#include "visitor.hpp"
#include "SymbolTable.hpp"
#include "output.hpp"
#include <iostream>


static int convert_int_to_byte (int num, int line);

static bool is_num_type (ast::BuiltInType type);

static bool compare_exp_list (std::vector<ast::BuiltInType>& paramTypesDecl, std::vector<ast::BuiltInType>& paramTypesCall);

std::vector<std::string> builtInTypeToString(const std::vector<ast::BuiltInType>& types);

std::vector<std::string> builtInTypeVectorToString(const std::vector<ast::BuiltInType>& types) ;


class SemanticAnalyzer : public Visitor {
public:
    // Implement visit methods for each type of AST node

    class SymbolTable sym_table;

    void register_func(ast::FuncDecl& node)
    {
        std::cout << "adding a func" << std::endl;
        std::vector<ast::BuiltInType> paramTypes;
        for (auto formal : node.formals->formals)
            paramTypes.push_back(formal->type->type);
        sym_table.insertSymbolFunc(node.id->value, node.return_type->type ,paramTypes);
        std::cout << "adding a func" << std::endl;
    }

    ast::BuiltInType visit(ast::Num& node, int* val) override {
    if (val != nullptr) {
        *val = node.value;
    }
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
    std::cout << "=== Starting BinOp Analysis ===" << std::endl;
    
    // Create local storage for values
    int left_val = 0;
    int right_val = 0;
    
    // Visit left operand
    ast::BuiltInType type_1 = node.left->accept(*this, &left_val);
    std::cout << "Left operand type: " << static_cast<int>(type_1) << std::endl;
    
    // Visit right operand
    ast::BuiltInType type_2 = node.right->accept(*this, &right_val);
    std::cout << "Right operand type: " << static_cast<int>(type_2) << std::endl;

    // Check numeric types
    if (!is_num_type(type_1) || !is_num_type(type_2)) {
        std::cout << "Error: Non-numeric types in arithmetic operation" << std::endl;
        output::errorMismatch(node.line);
        return ast::BuiltInType::NONE;
    }

    // Calculate result type
    ast::BuiltInType result_type;
    if (type_1 == ast::BuiltInType::INT || type_2 == ast::BuiltInType::INT) {
        result_type = ast::BuiltInType::INT;
    } else {
        result_type = ast::BuiltInType::BYTE;
    }

    // Only perform computation if we need to store the result
    if (val != nullptr) {
        // Calculate result value
        switch (node.op) {
            case ast::BinOpType::ADD:
                *val = left_val + right_val;
                break;
            case ast::BinOpType::SUB:
                *val = left_val - right_val;
                break;
            case ast::BinOpType::MUL:
                *val = left_val * right_val;
                break;
            case ast::BinOpType::DIV:
                if (right_val == 0) {
                    output::errorMismatch(node.line);
                    return ast::BuiltInType::NONE;
                }
                *val = left_val / right_val;
                break;
        }

        // Handle byte conversion if needed
        if (result_type == ast::BuiltInType::BYTE) {
            *val = convert_int_to_byte(*val, node.line);
        }
        std::cout << "Stored result value: " << *val << std::endl;
    } else {
        std::cout << "No output pointer provided" << std::endl;
    }

    std::cout << "=== Finished BinOp Analysis ===" << std::endl;
    return result_type;
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
        std::cout << "Analyzing Call node" << std::endl;
        Symbol *p_sym = sym_table.getFunctionSymbol(node.func_id->value); 
        if (p_sym == nullptr)
            std::cout << "WELL FUCK" << std::endl;
        if (!sym_table.checkFunctionCall(node.func_id->value))
            output::errorUndefFunc(node.line, node.func_id->value);
        std::cout << p_sym->is_func << std::endl;
        if (!p_sym->is_func)
            output::errorDefAsVar(node.line, node.func_id->value);
        std::vector<ast::BuiltInType>   params;
        visit(*node.args, &params);
        if (!compare_exp_list(params, p_sym->paramTypes)){
            std::vector<std::string> paramTypesCopy = builtInTypeVectorToString(p_sym->paramTypes);
            output::errorPrototypeMismatch(node.line, node.func_id->value, paramTypesCopy);

        }


        return p_sym->type;
    }

    ast::BuiltInType visit(ast::Call& node, int* p) override {
        return visit(node);
    }

    ast::BuiltInType visit(ast::Statements& node) override {
        std::cout << "Analyzing Statements node" << std::endl;
       for (auto statment : node.statements)
           statment->accept(*this);
        return  ast::BuiltInType::NONE;
    }

    ast::BuiltInType visit(ast::Break& node) override {
        std::cout << "Analyzing Break node" << std::endl;
        if (sym_table.currentScope != nullptr ||
            sym_table.currentScope->scopeType != ScopeType::COND)
             output::errorUnexpectedBreak (node.line);

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
        std::cout << "Analyzing While node" << std::endl;
        if (node.condition->accept(*this, nullptr) != ast::BuiltInType::BOOL)
            output::errorMismatch( node.line);
        sym_table.enterScope(ScopeType::COND);
        node.body->accept(*this);
        sym_table.exitScope();

        return ast::BuiltInType::NONE;
    }

    ast::BuiltInType visit(ast::VarDecl& node) override {
    std::cout << "\n=== Starting VarDecl Analysis ===" << std::endl;
    std::cout << "Variable name: " << node.id->value << std::endl;
    
    // Get and print the declared type
    ast::BuiltInType declared_type = visit(*node.type);
    std::cout << "Declared type is: " << static_cast<int>(declared_type) 
              << " (INT=" << static_cast<int>(ast::BuiltInType::INT)
              << ", BYTE=" << static_cast<int>(ast::BuiltInType::BYTE) << ")" << std::endl;

    if (node.init_exp != nullptr) {
        std::cout << "Has initialization expression" << std::endl;
        int init_value = 0;
        
        // Get the type and value of the initialization expression
        ast::BuiltInType exp_type = node.init_exp->accept(*this, &init_value);
        std::cout << "Expression evaluated to:" << std::endl;
        std::cout << "  Type: " << static_cast<int>(exp_type) << std::endl;
        std::cout << "  Value: " << init_value << std::endl;
        
        // Detailed type compatibility check
        std::cout << "\nType compatibility check:" << std::endl;
        std::cout << "1. Direct type match? " << (declared_type == exp_type ? "Yes" : "No") << std::endl;
        
        if (declared_type == ast::BuiltInType::INT) {
            std::cout << "2. Assigning to INT:" << std::endl;
            if (exp_type == ast::BuiltInType::BYTE) {
                std::cout << "   BYTE->INT conversion allowed" << std::endl;
            } else if (exp_type == ast::BuiltInType::INT) {
                std::cout << "   INT->INT assignment allowed" << std::endl;
            } else {
                std::cout << "   Invalid type for INT assignment" << std::endl;
                output::errorMismatch(node.line);
                return ast::BuiltInType::NONE;
            }
        } 
        else if (declared_type == ast::BuiltInType::BYTE) {
            std::cout << "2. Assigning to BYTE:" << std::endl;
            if (exp_type == ast::BuiltInType::BYTE) {
                std::cout << "   BYTE->BYTE assignment allowed" << std::endl;
            } else if (exp_type == ast::BuiltInType::INT) {
                std::cout << "   Attempting INT->BYTE conversion" << std::endl;
                try {
                    init_value = convert_int_to_byte(init_value, node.line);
                    std::cout << "   Conversion successful" << std::endl;
                } catch (...) {
                    std::cout << "   Conversion failed - value out of range" << std::endl;
                    output::errorMismatch(node.line);
                    return ast::BuiltInType::NONE;
                }
            } else {
                std::cout << "   Invalid type for BYTE assignment" << std::endl;
                output::errorMismatch(node.line);
                return ast::BuiltInType::NONE;
            }
        }
    }
    
    // If we got here, types are compatible
    sym_table.insertSymbol(node.id->value, declared_type);
    std::cout << "=== Successfully completed VarDecl Analysis ===" << std::endl;
    
    return ast::BuiltInType::NONE;
}

    ast::BuiltInType visit(ast::Assign& node) override {
        std::cout << "Analyzing Assign node" << std::endl;
        ast::BuiltInType dest_type = node.id->accept(*this, nullptr);
        ast::BuiltInType src_type= node.exp->accept(*this, nullptr);
        if(dest_type != src_type || dest_type == ast::BuiltInType::NONE)
            output::errorMismatch(node.line);

        return ast::BuiltInType::NONE;
    }

    ast::BuiltInType visit(ast::Formal& node) override {
        std::cout << "Analyzing Formal node" << std::endl;
        if (sym_table.lookupSymbol(node.id->value))
            output::errorDef(node.line, node.id->value);

        return visit(*node.type);
        std::cout << "Analyzing Formal node" << std::endl;
    }

    ast::BuiltInType visit(ast::Formals& node, std::vector<ast::BuiltInType>* params_type,std::vector<std::string>* params_name )  {
        std::cout << "Analyzing Formals node" << std::endl;
        for (auto formal : node.formals)
        {
            params_type->push_back(formal->type->type);
            params_name->push_back(formal->id->value);
        }

        return ast::BuiltInType::NONE;
    }


    ast::BuiltInType visit(ast::FuncDecl& node) override {
        std::cout << "Analyzing FuncDecl node" << std::endl;
        std::vector<ast::BuiltInType> params_type;
        std::vector<std::string> params_name;
        visit(*node.formals, &params_type, &params_name);
        sym_table.enterScope(ScopeType::FUNC, params_type, params_name , node.return_type->type);
        visit (*node.body);
        sym_table.exitScope();
        return  ast::BuiltInType::NONE;
    }


    ast::BuiltInType visit(ast::Funcs& node) override {
        std::cout << "Analyzing Funcs node" << std::endl;
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

std::string builtInTypeToString(const ast::BuiltInType &type) {
    switch (type) {
        case ast::BuiltInType::INT:
            return "int";
        case ast::BuiltInType::BOOL:
            return "bool";
        case ast::BuiltInType::BYTE:
            return "byte";
        case ast::BuiltInType::VOID:
            return "void";
        case ast::BuiltInType::STRING:
            return "string";
        default:
            return "unknown";
    }
}

std::vector<std::string> builtInTypeVectorToString(const std::vector<ast::BuiltInType>& types) {
    std::vector<std::string> result;
    for (const auto &type : types) {
        result.push_back(builtInTypeToString(type));
    }
    return result;
}



#endif // SEMANTIC_ANALYZER_HPP
