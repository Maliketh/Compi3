#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include "nodes.hpp"
#include "output.hpp"

// Enum for symbol types
enum ScopeType {
    GLOBAL,
    FUNC,
    COND
};

// Symbol structure to represent variables and functions
class Symbol {
public:
    std::string name;
    ast::BuiltInType type;
    std::vector<ast::BuiltInType> paramTypes;  // Arguments for functions (empty for non-functions)
    int offset;  // Offset for variables or function arguments
    bool is_func;

    // Constructor for  variable
    Symbol() : type(ast::BuiltInType::NONE), offset(0), is_func(false) {
    paramTypes = {};  // Empty vector initialization for non-function symbols
    };

    Symbol(const std::string& _name, ast::BuiltInType type, int offset = 0)
              : name(_name), type(type), offset(offset) ,is_func(false) {};

    Symbol(const std::string& _name, ast::BuiltInType type ,const std::vector<ast::BuiltInType>& params, int offset = 0)
            : name(_name), type(type), offset(offset) ,paramTypes(params) , is_func(true) {};

    ast::BuiltInType getType() { return type; }
} ;

// Scope structure to represent a scope and its symbols
class Scope {
public:
    std::unordered_map<std::string, Symbol> symbols;
    Scope* parent_scope;
    ScopeType scopeType;
    ast::BuiltInType ret_scope_type;

    int offset;

    output::ScopePrinter scopePrinter;

   Scope (ScopeType type) : scopeType(type), parent_scope(nullptr), ret_scope_type(ast::BuiltInType::NONE) , offset(0) {};
   Scope (ScopeType type, Scope *p_scope) :  scopeType(type), ret_scope_type(ast::BuiltInType::NONE), parent_scope(p_scope), offset(p_scope->offset)  {};

    bool hasSymbol(const std::string& name) {
        bool found = symbols.find(name) != symbols.end();
        if (!found && parent_scope!= nullptr)
            return parent_scope->hasSymbol(name);
        return found;
    }
    ast::BuiltInType getSymbolType( std::string& name)
    {
        Symbol* p_symbol = getSymbol(name);
        if (p_symbol != nullptr)
            return p_symbol->getType();
        return ast::BuiltInType::NONE;
    }
    bool insertSymbolFunc(const std::string& name, ast::BuiltInType type,  const std::vector<ast::BuiltInType> &paramTypes);
    bool insertSymbol(const std::string& name, ast::BuiltInType type);
    bool insertSymbol(const std::string& name, ast::BuiltInType type, int count);

    Symbol* getSymbol(const std::string& name);
};

// SymbolTable class to manage multiple scopes
class SymbolTable {
public:
    Scope* currentScope;
    std::unordered_map<std::string, Symbol> globalFunctionRegistry; // Global function registry

    SymbolTable();
    ~SymbolTable();

    bool insertSymbolFunc(const std::string& name, ast::BuiltInType type,  const std::vector<ast::BuiltInType> &paramTypes);
    bool insertSymbol(const std::string& name, ast::BuiltInType type);
    Symbol* lookupSymbol(const std::string& name);
    bool isFunctionDefined(const std::string& funcName) const;
    bool checkFunctionCall(const std::string& funcName);
    void enterScope(ScopeType type);
    void enterScope (ScopeType type, std::vector<ast::BuiltInType>& params_type,std::vector<std::string>& params_name, ast::BuiltInType ret_type);
    void exitScope();
    void initializeGlobalScope();
    ast::BuiltInType getSymbolType( std::string& name);
    Symbol* getFunctionSymbol(const std::string& funcName);
};

#endif // SYMBOLTABLE_H
