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
bool SymbolTable::insertSymbolFunc(const std::string& name, ast::BuiltInType type, const std::vector<ast::BuiltInType> &paramTypes) {
    if (globalFunctionRegistry.find(name) != globalFunctionRegistry.end()) {
        //std::cerr << "Error: Symbol '" << name << "' already defined in this scope.\n";
        return false;
    }

    // Insert the symbol into the current scope

    globalFunctionRegistry[name] = Symbol(name, type, 0);  // Offset irrelevant for global functions
    globalFunctionRegistry[name].paramTypes = paramTypes;
    global->scopePrinter.emitFunc(name, type, paramTypes);

    return true;
}

bool SymbolTable::insertSymbol(const std::string& name, ast::BuiltInType type) {
    // Check if the symbol already exists in the current scope or any parent scopes
    Scope* scope = currentScope;
   // while (scope != nullptr) {
        if (scope->hasSymbolInScope(name)) {
            return false;
        }
       // scope = scope->parent_scope;
   //}
    if (scope->scopeType == ScopeType::WHILE ||scope->scopeType == ScopeType::IF || scope->scopeType == ScopeType::INFUNC )
        if (scope->hasCondSymbol(name))
            return false;
    // Insert the symbol into the current scope
    //std::cout <<"entering symbole " << name << "to scope type: " << currentScope->scopeType << std::endl;
    int location = currentScope->insertSymbol(name, type);

    // Optionally, you can also insert it into the global scope
    global->scopePrinter.emitVar(name, type,location);

    return true;
}



// Look up a symbol in the current scope or higher scopes
Symbol SymbolTable::lookupSymbol(const std::string& name) {
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
        //std::cerr << "Error: Function '" << funcName << "' is not defined.\n";
        return false;
    }

    return true;
}

// Enter a new scope
void SymbolTable::enterScope(ScopeType type) {
    Scope* newScope = new Scope(type);
    if (type != ScopeType::GLOBAL)
        global->scopePrinter.beginScope();
    if (type ==ScopeType::WHILE || type ==ScopeType::IF || type ==ScopeType::INFUNC)
        newScope->offset = currentScope->offset;
    newScope->parent_scope = currentScope;
    newScope->ret_scope_type = ast::BuiltInType::NONE;
    //std::cout <<"Entered scope type: " << type << std::endl;
    /*if (newScope->parent_scope != nullptr)
        std::cout <<"prev scope type: " << newScope->parent_scope->scopeType << std::endl;
    else
        std::cout <<"prev scope null "  << std::endl;*/

    currentScope = newScope;
}

void SymbolTable::enterScope(ScopeType type, const std::set<std::string>& cond_symbols)
{
    Scope* newScope = new Scope(type);
    if (type != ScopeType::GLOBAL)
        global->scopePrinter.beginScope();
    if (type ==ScopeType::WHILE || type ==ScopeType::IF || type ==ScopeType::INFUNC) {
        newScope->condition_symbols = cond_symbols;
        newScope->offset = currentScope->offset;
    }
    if (type ==ScopeType::WHILE || type ==ScopeType::IF ) {
        newScope->symbols = currentScope->symbols;
    }

    newScope->parent_scope = currentScope;
    newScope->ret_scope_type = ast::BuiltInType::NONE;
    //std::cout <<"Entered scope type: " << type << std::endl;
    /*if (newScope->parent_scope != nullptr)
        std::cout <<"prev scope type: " << newScope->parent_scope->scopeType << std::endl;
    else
        std::cout <<"prev scope null "  << std::endl;*/

    currentScope = newScope;
}

void SymbolTable::enterScope(ScopeType type, std::vector<ast::BuiltInType>& params_type, std::vector<std::string>& params_names, ast::BuiltInType ret_type) {
    Scope* newScope = new Scope(type);
    if (type != ScopeType::GLOBAL)
        global->scopePrinter.beginScope();
    newScope->parent_scope = currentScope;
    newScope->ret_scope_type = ret_type;
    /*std::cout <<"Entered scope type: Func"   << std::endl;
    if (newScope->parent_scope != nullptr)
        std::cout <<"prev scope type: " << newScope->parent_scope->scopeType << std::endl;
    else
        std::cout <<"prev scope null "  << std::endl;*/
    int location = -1;
    std::string name;
    ast::BuiltInType type_param = ast::BuiltInType::NONE;
    for (int i = 0; i < params_type.size() ; i++)
    {
        name =params_names[i];
        type_param = params_type[i];
        newScope->insertSymbol(name,type_param, location);
        global->scopePrinter.emitVar(name, type_param,location);
        location--;
    }
  //  newScope->ret_scope_type = ast::BuiltInType::NONE;
    currentScope = newScope;
}

// Exit the current scope
void SymbolTable::exitScope() {
    Scope* oldScope = currentScope;
    //std::cout << "exiting scope : "<< oldScope->scopeType << std::endl;
    /* if (oldScope->parent_scope != nullptr)
         std::cout << " parent_scope scope type : "<< oldScope->parent_scope->scopeType << std::endl;
     else
         std::cout << " parent_scope null " << std::endl;*/
    if (oldScope->scopeType != ScopeType::GLOBAL)
        global->scopePrinter.endScope();
    currentScope = oldScope->parent_scope;
   // if(currentScope != nullptr)
        //std::cout << "parent scope : "<< currentScope->scopeType << std::endl;

    delete oldScope;
}

// Initialize the global scope with predefined functions (print, printi)
void SymbolTable::initializeGlobalScope() {
    enterScope(ScopeType::GLOBAL);
    global = currentScope;
    std::vector<ast::BuiltInType> vec1 = { ast::BuiltInType::STRING };
    std::vector<ast::BuiltInType> vec12 = { ast::BuiltInType::INT };
    // Add predefined functions print and printi
    this->insertSymbolFunc("print", ast::BuiltInType::VOID, vec1);
    this->insertSymbolFunc("printi",ast::BuiltInType::VOID, vec12);
}


ast::BuiltInType SymbolTable::getSymbolType(std::string& name) {
    if (currentScope != nullptr)
        return currentScope->getSymbolType(name);
    return ast::BuiltInType::NONE;
}

Symbol SymbolTable::getFunctionSymbol(const std::string& funcName) {
    // Check if the function is in the global function registry
    auto it = globalFunctionRegistry.find(funcName);
    if (it != globalFunctionRegistry.end()) {
        // Print details of the found symbol
        Symbol& foundSymbol = it->second;
        //std::cout << "Function symbol found: " << std::endl;
        //std::cout << "Name: " << foundSymbol.name << std::endl;
        //std::cout << "Type: " << static_cast<int>(foundSymbol.type) << std::endl;
        //std::cout << "is_func: " << foundSymbol.is_func << std::endl;
        //std::cout << "Offset: " << foundSymbol.offset << std::endl;
        foundSymbol.is_func = true; // WORKS BUT IT'S DISGUSTING, SHOULD CHECK FURTHER!
        // Print the parameters' types
        //std::cout << "Parameters: ";
        //for (const auto& param : foundSymbol.paramTypes) {
        //std::cout << static_cast<int>(param) << " ";
        // }
        //std::cout << std::endl;

        // Return the symbol corresponding to the function
        return it->second;
    }

    // Return nullptr if the function is not found
    // //std::cout << "Function symbol not found: " << funcName << std::endl;
    return globalFunctionRegistry.end()->second;
}




int Scope::insertSymbol(const std::string& name, ast::BuiltInType type) {

    symbols[name] = Symbol(name, type, this->offset);
    //std::cout <<   "inserted to scope" << name << std::endl;
    //this->scopePrinter.emitVar(name, type,this->offset);
    this->offset++;

    return this->offset -1;
}

int Scope::insertSymbol(const std::string& name, ast::BuiltInType type, int count) {
    //if (this->hasSymbol(name)) {
    //   return false; // Symbol already exists
    //}
    //std::cout << "140 inserting " << name << std::endl;
    symbols[name] = Symbol(name, type,  count);
    //std::cout << "now " << name << std::endl;
    this->scopePrinter.emitVar(name, type, count);

    //std::cout << "140 inserting is done" << name << std::endl;
    return count;
}

Symbol Scope::getSymbol(const std::string& name) {
    //std::cout << "looking in current " << name << std::endl;
    if (symbols.find(name) != symbols.end()) {
        return symbols[name];
    }
    if (this->parent_scope) {
        //std::cout << "looking in Parent " << name << std::endl;
        return parent_scope->getSymbol(name);
    }
    return Symbol ();
}
