#include "SymbolTable.hpp"
#include "output.hpp"
#include "nodes.hpp"

// Constructor initializes the symbol table
SymbolTable::SymbolTable() {
    currentScope = nullptr;
    initializeGlobalScope();  // Initialize the global scope with predefined functions
}

// Destructor (clean up memory)
SymbolTable::~SymbolTable() {
    delete currentScope;
}

// Insert a symbol into the current scope
bool SymbolTable::insertSymbolFunc(const std::string& name, ast::BuiltInType type,   const std::vector<ast::BuiltInType> &paramTypes) {
    if (currentScope->hasSymbol(name)) {
        std::cerr << "Error: Symbol '" << name << "' already defined in this scope.\n";
        return false;
    }

    // Insert the symbol into the current scope
    if (currentScope->insertSymbolFunc(name, type, paramTypes)) {
        globalFunctionRegistry[name] = Symbol(name, type, 0);  // Offset irrelevant for global functions
        globalFunctionRegistry[name].paramTypes = paramTypes;
    }
    return true;
}

bool SymbolTable::insertSymbol(const std::string& name, ast::BuiltInType type) {
    if (currentScope->hasSymbol(name)) {
        std::cerr << "Error: Symbol '" << name << "' already defined in this scope.\n";
        return false;
    }
    // Insert the symbol into the current scope
    currentScope->insertSymbol(name, type);
    return true;
}


// Look up a symbol in the current scope or higher scopes
Symbol* SymbolTable::lookupSymbol(const std::string& name) {
    return currentScope->getSymbol(name);
}

// Check if a function is defined globally (across all scopes)
bool SymbolTable::isFunctionDefined(const std::string& funcName) const {
    return globalFunctionRegistry.find(funcName) != globalFunctionRegistry.end();
}

// Check if a function call is valid
bool SymbolTable::checkFunctionCall(const std::string& funcName) {
    auto it = globalFunctionRegistry.find(funcName);
    if (it == globalFunctionRegistry.end()) {
        std::cerr << "Error: Function '" << funcName << "' is not defined.\n";
        return false;
    }

    return true;
}

// Enter a new scope
void SymbolTable::enterScope(ScopeType type) {
    Scope* newScope = new Scope(type);
    newScope->ret_scope_type = ast::BuiltInType::NONE;
    currentScope = newScope;
}

void SymbolTable::enterScope(ScopeType type, std::vector<ast::BuiltInType>& params_type, std::vector<std::string>& params_names, ast::BuiltInType ret_type) {
    Scope* newScope = new Scope(type);
    newScope->ret_scope_type = ret_type;
    int location = -1;
    std::string name;
    ast::BuiltInType type_param = ast::BuiltInType::NONE;
    for (int i = 0; i < params_type.size() ; i++)
    {
        name =params_names[i];
        type_param = params_type[i];
        newScope->insertSymbol(name,type_param, location);
        newScope->scopePrinter.emitVar(name, type_param, location);
        location--;
    }
    newScope->ret_scope_type = ast::BuiltInType::NONE;
    currentScope = newScope;
}

// Exit the current scope
void SymbolTable::exitScope() {
    Scope* oldScope = currentScope;
    currentScope = nullptr;
    std::cout << oldScope->scopePrinter <<std::endl;
    delete oldScope;
}

// Initialize the global scope with predefined functions (print, printi)
void SymbolTable::initializeGlobalScope() {
    enterScope(ScopeType::GLOBAL);
    std::vector<ast::BuiltInType> vec1 = { ast::BuiltInType::STRING };
    std::vector<ast::BuiltInType> vec12 = { ast::BuiltInType::INT };
    // Add predefined functions print and printi
    currentScope->insertSymbolFunc("print", ast::BuiltInType::VOID, vec1);
    currentScope->insertSymbolFunc("printi",ast::BuiltInType::VOID, vec12);
}


ast::BuiltInType SymbolTable::getSymbolType(std::string& name) {
    if (currentScope != nullptr)
        return currentScope->getSymbolType(name);
    return ast::BuiltInType::NONE;
}
//function
bool Scope::insertSymbolFunc(const std::string& name, ast::BuiltInType type,  const std::vector<ast::BuiltInType> &paramTypes) {
    if (this->hasSymbol(name)) {
        return false; // Symbol already exists
    }
    std::cout << "116 inserting " << name << std::endl;
    symbols[name] = Symbol(name, type,  paramTypes);
    std::cout << "116 emit " << name << std::endl;
    this->scopePrinter.emitFunc(name, type,  paramTypes);
    this->offset++;

    return true;
}

bool Scope::insertSymbol(const std::string& name, ast::BuiltInType type) {
    if (this->hasSymbol(name)) {
        return false; // Symbol already exists
    }
    std::cout << "128 inserting " << name << std::endl;
    symbols[name] = Symbol(name, type, this->offset);
    this->scopePrinter.emitVar(name, type,this->offset);
    this->offset++;

    return true;
}

bool Scope::insertSymbol(const std::string& name, ast::BuiltInType type, int count) {
    if (this->hasSymbol(name)) {
        return false; // Symbol already exists
    }
    std::cout << "140 inserting " << name << std::endl;
    symbols[name] = Symbol(name, type,  count);
    std::cout << "now " << name << std::endl;
    this->scopePrinter.emitVar(name, type, count);
    std::cout << "140 inserting is done" << name << std::endl;
    return true;
}

Symbol* Scope::getSymbol(const std::string& name) {
    if (symbols.find(name) != symbols.end()) {
        return &symbols[name];
    }
    if (this->parent_scope)
        return parent_scope->getSymbol(name);
    return nullptr;
}

