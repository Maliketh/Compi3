#include "SymbolTable.h"

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
bool SymbolTable::insertSymbol(const std::string& name, SymbolType type, const std::string& returnType, const std::vector<std::string>& argTypes) {
    if (currentScope->hasSymbol(name)) {
        std::cerr << "Error: Symbol '" << name << "' already defined in this scope.\n";
        return false;
    }

    // Insert the symbol into the current scope
    if (currentScope->insertSymbol(name, type, returnType, argTypes)) {
        // If it's a function, add it to the global function registry
        if (type == SymbolType::FUNCTION) {
            globalFunctionRegistry[name] = Symbol(name, type, returnType, 0);  // Offset irrelevant for global functions
            globalFunctionRegistry[name].argumentTypes = argTypes;
        }
        return true;
    }
    return false;
}

// Look up a symbol in the current scope or higher scopes
Symbol* SymbolTable::lookupSymbol(const std::string& name) {
    return currentScope->getSymbol(name);
}

// Check if a function is defined globally (across all scopes)
bool SymbolTable::isFunctionDefined(const std::string& funcName) const {
    return globalFunctionRegistry.find(funcName) != globalFunctionRegistry.end();
}

// Check if a function call is valid (function exists and argument types match)
bool SymbolTable::checkFunctionCall(const std::string& funcName, const std::vector<std::string>& argTypes) {
    auto it = globalFunctionRegistry.find(funcName);
    if (it == globalFunctionRegistry.end()) {
        std::cerr << "Error: Function '" << funcName << "' is not defined.\n";
        return false;
    }

    const Symbol& funcSymbol = it->second;

    // Check argument types match
    if (funcSymbol.argumentTypes != argTypes) {
        std::cerr << "Error: Function '" << funcName << "' argument types do not match.\n";
        return false;
    }
    return true;
}

// Enter a new scope
void SymbolTable::enterScope() {
    Scope* newScope = new Scope();
    currentScope = newScope;
}

// Exit the current scope
void SymbolTable::exitScope() {
    Scope* oldScope = currentScope;
    currentScope = nullptr;
    delete oldScope;
}

// Initialize the global scope with predefined functions (print, printi)
void SymbolTable::initializeGlobalScope() {
    enterScope();

    // Add predefined functions print and printi
    insertSymbol("print", SymbolType::FUNCTION, "void", { "string" });
    insertSymbol("printi", SymbolType::FUNCTION, "void", { "int" });
}

// Scope class: Insert and get symbols within a scope
bool Scope::insertSymbol(const std::string& name, SymbolType type, const std::string& returnType, const std::vector<std::string>& argTypes) {
    if (symbols.find(name) != symbols.end()) {
        return false; // Symbol already exists
    }
    symbols[name] = Symbol(name, type, returnType, nextOffset);
    return true;
}

Symbol* Scope::getSymbol(const std::string& name) {
    if (symbols.find(name) != symbols.end()) {
        return &symbols[name];
    }
    return nullptr;
}
