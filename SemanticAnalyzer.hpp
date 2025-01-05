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
        //std::cout << "adding a func " << node.id->value << std::endl;
        std::vector<ast::BuiltInType> paramTypes;
        for (auto formal : node.formals->formals)
            paramTypes.push_back(formal->type->type);
        sym_table.insertSymbolFunc(node.id->value, node.return_type->type ,paramTypes);
        //std::cout << "adding a func done " << node.id->value << std::endl;
    }

    ast::BuiltInType visit(ast::Num& node, int* val) override {
    if (val != nullptr) {
        *val = node.value;
    }
    return ast::BuiltInType::INT;
}
    ast::BuiltInType visit(ast::NumB& node, int* val) override {
        if (val != nullptr)
             *val = node.value;
        //sstd::cout << "Analyzing NumB node" << std::endl;
        return ast::BuiltInType::BYTE;
    }

    ast::BuiltInType visit(ast::String& node, int* val) override {
        //sstd::cout << "Analyzing String node" << std::endl;
        return ast::BuiltInType::STRING;
    }

    ast::BuiltInType visit(ast::Bool& node, int* val) override {
        if (val != nullptr)
            *val = node.value;
        //sstd::cout << "Analyzing Bool node" << std::endl;
        return ast::BuiltInType::BOOL;
    }

    ast::BuiltInType visit(ast::ID& node, int* val) override {
        //sstd::cout << "Analyzing ID node for "<< node.value << std::endl;
        if (!sym_table.currentScope->hasSymbol(node.value))
        {
            //sstd::cout << "line 64 - sym not found" << std::endl;
            output::errorUndef(node.line, node.value);
        }
        ast::BuiltInType type = sym_table.getSymbolType(node.value);
        if (type ==  ast::BuiltInType::NONE)
            output::errorUndef(node.line, node.value);
        return type;

    }

    ast::BuiltInType visit(ast::BinOp& node, int* val) override {
    //sstd::cout << "=== Starting BinOp Analysis ===" << std::endl;
    
    // Create local storage for values
    int left_val = 0;
    int right_val = 0;
    
    // Visit left operand
    ast::BuiltInType type_1 = node.left->accept(*this, &left_val);
    //sstd::cout << "Left operand type: " << static_cast<int>(type_1) << std::endl;
    
    // Visit right operand
    ast::BuiltInType type_2 = node.right->accept(*this, &right_val);
    //sstd::cout << "Right operand type: " << static_cast<int>(type_2) << std::endl;

    // Check numeric types
    if (!is_num_type(type_1) || !is_num_type(type_2)) {
        //sstd::cout << "Error: Non-numeric types in arithmetic operation" << std::endl;
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
        if (result_type == ast::BuiltInType::BYTE && val != nullptr) {
            *val = convert_int_to_byte(*val, node.line);
        }
        //sstd::cout << "Stored result value: " << *val << std::endl;
    } else {
        //sstd::cout << "No output pointer provided" << std::endl;
    }

    //sstd::cout << "=== Finished BinOp Analysis ===" << std::endl;
    return result_type;
}


    ast::BuiltInType visit(ast::RelOp& node, int* val) override {
        //sstd::cout << "Analyzing RelOp node" << std::endl;
        int* val_1 = nullptr;
        int* val_2 = nullptr;
        ast::BuiltInType type_1 = node.left->accept(*this, nullptr);
        ast::BuiltInType type_2 = node.right->accept(*this, nullptr);
        if (!is_num_type(type_1) || !is_num_type(type_2))
            output::errorMismatch(node.line);
        return ast::BuiltInType::BOOL;
    }

    ast::BuiltInType visit(ast::Not& node, int* val)  override {
        //sstd::cout << "Analyzing Not node" << std::endl;
        ast::BuiltInType type = node.exp->accept(*this, nullptr);
        if (type != ast::BuiltInType::BOOL)
            output::errorMismatch(node.line);

        return ast::BuiltInType::BOOL;
    }

    ast::BuiltInType visit(ast::And& node, int* val) override {
        //sstd::cout << "Analyzing And node" << std::endl;
        ast::BuiltInType type_1 = node.left->accept(*this, nullptr);
        ast::BuiltInType type_2 = node.right->accept(*this, nullptr);

        if (type_1 != ast::BuiltInType::BOOL || type_2 != ast::BuiltInType::BOOL)
            output::errorMismatch(node.line);

        return ast::BuiltInType::BOOL;

    }

    ast::BuiltInType visit(ast::Or& node, int* val) override {
        //sstd::cout << "Analyzing Or node" << std::endl;
        ast::BuiltInType type_1 = node.left->accept(*this, nullptr);
        ast::BuiltInType type_2 = node.right->accept(*this, nullptr);

        if (type_1 != ast::BuiltInType::BOOL || type_2 != ast::BuiltInType::BOOL)
            output::errorMismatch(node.line);

        return ast::BuiltInType::BOOL;
    }

    ast::BuiltInType visit(ast::Type& node) override {
        //sstd::cout << "Analyzing Type node" << std::endl;
        return node.type;
    }

    ast::BuiltInType visit(ast::Cast& node, int* val) override {
        //sstd::cout << "Analyzing Cast node" << std::endl;
        int *exp_val = nullptr;
        ast::BuiltInType exp_type = node.exp->accept(*this,exp_val);
        if (node.target_type->type == ast::BuiltInType::BYTE && exp_type == ast::BuiltInType::INT) {
            if (exp_val != nullptr && val!= nullptr)
                *val = convert_int_to_byte (*exp_val, node.line);
        }
        else if (node.target_type->type == ast::BuiltInType::INT && exp_type == ast::BuiltInType::BYTE) {
            if (exp_val != nullptr && val != nullptr)
                *val = *exp_val;
        }
        else if (exp_val != nullptr && *exp_type != node.target_type->type)
            output::errorMismatch(node.line);
        return node.target_type->type;

    }

    ast::BuiltInType visit(ast::ExpList& node,  std::vector<ast::BuiltInType>* paramTypes) override {
        //sstd::cout << "Analyzing ExpList node" << std::endl;
        ast::BuiltInType exp_type =  ast::BuiltInType::NONE;
        for (auto exp_node : node.exps) {
            exp_type = exp_node->accept(*this, nullptr);
            paramTypes->push_back(exp_type);
        }
        return  ast::BuiltInType::NONE;
    } //change i belive

    ast::BuiltInType visit(ast::Call& node) override {
        //sstd::cout << "Analyzing Call node" << std::endl;
        bool is_defined = sym_table.isFunctionDefined(node.func_id->value);
       // //std::cout <<node.func_id->value<<  " - func scope " << is_defined <<std::endl;
        if (!is_defined && sym_table.currentScope->hasSymbol(node.func_id->value))
            output::errorDefAsVar(node.line, node.func_id->value);
        if (!is_defined)
            output::errorUndefFunc(node.line, node.func_id->value);
        Symbol sym = sym_table.getFunctionSymbol(node.func_id->value);
        //sstd::cout << " got sym " << sym.name <<std::endl;
        std::vector<ast::BuiltInType>   params;
        visit(*node.args, &params);
        //std::cout << " got params "  <<std::endl;
        if (!compare_exp_list(params, sym.paramTypes)){
            std::vector<std::string> paramTypesCopy = builtInTypeVectorToString(sym.paramTypes);
            output::errorPrototypeMismatch(node.line, node.func_id->value, paramTypesCopy);

        }
        //std::cout << " End Analyzing Call node" << std::endl;

        return sym.type;
    }

    ast::BuiltInType visit(ast::Call& node, int* p) override {
        return visit(node);
    }

    ast::BuiltInType visit(ast::Statements& node) override {
        //sstd::cout << "Analyzing Statements node" << std::endl;

       for (auto statment : node.statements)
           statment->accept(*this);
        return  ast::BuiltInType::NONE;
    }

    ast::BuiltInType visit(ast::Break& node) override {
        //sstd::cout << "Analyzing Break node" << std::endl;
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
            //sstd::cout << "check error return" << std::endl;
        if (node.exp != NULL && sym_table.currentScope->ret_scope_type == ast::BuiltInType::VOID)
            output::errorMismatch( node.line);
        if (node.exp == NULL && sym_table.currentScope->ret_scope_type != ast::BuiltInType::VOID)
            output::errorMismatch( node.line);
        if (node.exp->accept(*this, nullptr) != sym_table.currentScope->ret_scope_type)
            output::errorMismatch( node.line);
        return  ast::BuiltInType::NONE;

    }

    ast::BuiltInType visit(ast::If& node) override {
        //std::cout << "Analyzing If node" << std::endl;
        if (node.condition->accept(*this) != ast::BuiltInType::BOOL)
            output::errorMismatch( node.line);
        sym_table.enterScope(ScopeType::COND);
        if (node.then->is_scope)
            sym_table.enterScope(ScopeType::COND);
        node.then->accept(*this);
        if (node.then->is_scope)
            sym_table.exitScope();
        sym_table.exitScope();
        if (node.otherwise != nullptr)
        {
            sym_table.enterScope(ScopeType::COND);
            if (node.otherwise->is_scope)
                sym_table.enterScope(ScopeType::COND);
            node.otherwise->accept(*this);
            if (node.otherwise->is_scope)
                sym_table.exitScope();
            sym_table.exitScope();
        }
        sym_table.exitScope();
        //sstd::cout << "Analyzing If node" << std::endl;
        return  ast::BuiltInType::NONE;
    }

    ast::BuiltInType visit(ast::While& node) override { //scope printer
        //std::cout << "Analyzing While node" << std::endl;
        if (node.condition->accept(*this, nullptr) != ast::BuiltInType::BOOL)
            output::errorMismatch( node.line);
        sym_table.enterScope(ScopeType::COND);
        if (node.body->is_scope)
            sym_table.enterScope(ScopeType::COND);
        node.body->accept(*this);
        if (node.body->is_scope)
            sym_table.exitScope();
        sym_table.exitScope();

        return ast::BuiltInType::NONE;
    }

    ast::BuiltInType visit(ast::VarDecl& node) override {
    //sstd::cout << "\n=== Starting VarDecl Analysis ===" << std::endl;
    //sstd::cout << "Variable name: " << node.id->value << std::endl;
    
    // Get and print the declared type
    ast::BuiltInType declared_type = visit(*node.type);
    //sstd::cout << "Declared type is: " << static_cast<int>(declared_type) 
    //          << " (INT=" << static_cast<int>(ast::BuiltInType::INT)
      //        << ", BYTE=" << static_cast<int>(ast::BuiltInType::BYTE) << ")" << std::endl;

    if (node.init_exp != nullptr) {
        //sstd::cout << "Has initialization expression" << std::endl;
        int init_value = 0;
        
        // Get the type and value of the initialization expression
        ast::BuiltInType exp_type = node.init_exp->accept(*this, &init_value);
        //sstd::cout << "Expression evaluated to:" << std::endl;
        //sstd::cout << "  Type: " << static_cast<int>(exp_type) << std::endl;
        //sstd::cout << "  Value: " << init_value << std::endl;
        
        // Detailed type compatibility check
        //sstd::cout << "\nType compatibility check:" << std::endl;
        //sstd::cout << "1. Direct type match? " << (declared_type == exp_type ? "Yes" : "No") << std::endl;
        
        if (declared_type == ast::BuiltInType::INT) {
            //sstd::cout << "2. Assigning to INT:" << std::endl;
            if (exp_type == ast::BuiltInType::BYTE) {
                //sstd::cout << "   BYTE->INT conversion allowed" << std::endl;
            } else if (exp_type == ast::BuiltInType::INT) {
                //sstd::cout << "   INT->INT assignment allowed" << std::endl;
            } else {
                //sstd::cout << "   Invalid type for INT assignment" << std::endl;
                output::errorMismatch(node.line);
                return ast::BuiltInType::NONE;
            }
        } 
        else if (declared_type == ast::BuiltInType::BYTE) {
            //sstd::cout << "2. Assigning to BYTE:" << std::endl;
            if (exp_type == ast::BuiltInType::BYTE) {
                //sstd::cout << "   BYTE->BYTE assignment allowed" << std::endl;
            } else if (exp_type == ast::BuiltInType::INT) {
                //sstd::cout << "   Attempting INT->BYTE conversion" << std::endl;
                try {
                    init_value = convert_int_to_byte(init_value, node.line);
                    //sstd::cout << "   Conversion successful" << std::endl;
                } catch (...) {
                    //sstd::cout << "   Conversion failed - value out of range" << std::endl;
                    output::errorMismatch(node.line);
                    return ast::BuiltInType::NONE;
                }
            } else {
                //sstd::cout << "   Invalid type for BYTE assignment" << std::endl;
                output::errorMismatch(node.line);
                return ast::BuiltInType::NONE;
            }
        }
    }
    
    // If we got here, types are compatible
    sym_table.insertSymbol(node.id->value, declared_type);
    //sstd::cout << "=== Successfully completed VarDecl Analysis ===" << std::endl;
    
    return ast::BuiltInType::NONE;
}

    ast::BuiltInType visit(ast::Assign& node) override {
        //sstd::cout << "Analyzing Assign node" << std::endl;
        ast::BuiltInType dest_type = node.id->accept(*this, nullptr);
        ast::BuiltInType src_type= node.exp->accept(*this, nullptr);
        if(dest_type != src_type || dest_type == ast::BuiltInType::NONE)
            output::errorMismatch(node.line);

        return ast::BuiltInType::NONE;
    }

    ast::BuiltInType visit(ast::Formal& node) override {
        //sstd::cout << "Analyzing Formal node" << std::endl;
        if (sym_table.currentScope->hasSymbol(node.id->value))
            output::errorDef(node.line, node.id->value);

        return visit(*node.type);
        //sstd::cout << "Analyzing Formal node" << std::endl;
    }

    ast::BuiltInType visit(ast::Formals& node, std::vector<ast::BuiltInType>* params_type,std::vector<std::string>* params_name )  {
        //sstd::cout << "Analyzing Formals node" << std::endl;
        for (auto formal : node.formals)
        {
            params_type->push_back(formal->type->type);
            params_name->push_back(formal->id->value);
        }

        return ast::BuiltInType::NONE;
    }


    ast::BuiltInType visit(ast::FuncDecl& node) override {
        //std::cout << "Analyzing FuncDecl node " << node.id->value << std::endl;
        std::vector<ast::BuiltInType> params_type;
        std::vector<std::string> params_name;
        visit(*node.formals, &params_type, &params_name);
        sym_table.enterScope(ScopeType::FUNC, params_type, params_name , node.return_type->type);
        visit (*node.body);
        sym_table.exitScope();
        return  ast::BuiltInType::NONE;
    }


    ast::BuiltInType visit(ast::Funcs& node) override {
        //std::cout << "Analyzing Funcs node" << std::endl;
        for (auto func : node.funcs)
            register_func (*func);
        if (!sym_table.isFunctionDefined("main"))
            output::errorMainMissing();
        for (auto func : node.funcs)
            visit(*func);
        std::cout << sym_table.global->scopePrinter <<std::endl;
        return  ast::BuiltInType::NONE;
        //sstd::cout << "Analyzing Funcs node" << std::endl;
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
        for (int i = 0; i < paramTypesDecl.size() && is_legal; i++)
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
