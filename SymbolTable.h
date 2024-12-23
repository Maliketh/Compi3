#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>

// Enum for symbol types
enum class SymbolType {
    VARIABLE,
    FUNCTION
};

// Symbol structure to represent variables and functions
struct Symbol {
    std::string name;
    SymbolType type;
    std::string returnType;  // Type for variable or function return type
    std::vector<std::string> argumentTypes;  // Arguments for functions (empty for non-functions)
    size_t offset;  // Offset for variables or function arguments

    // Constructor for function or variable
    Symbol(const std::string& name, SymbolType type, const std::string& returnType, size_t offset = 0)
        : name(name), type(type), returnType(returnType), offset(offset) {}
};

// Scope structure to represent a scope and its symbols
class Scope {
public:
    std::unordered_map<std::string, Symbol> symbols;
    int nextOffset = 0;

    bool hasSymbol(const std::string& name) {
        return symbols.find(name) != symbols.end();
    }

    bool insertSymbol(const std::string& name, SymbolType type, const std::string& returnType, const std::vector<std::string>& argTypes);
    Symbol* getSymbol(const std::string& name);
};

// SymbolTable class to manage multiple scopes
class SymbolTable {
private:
    Scope* currentScope;
    std::unordered_map<std::string, Symbol> globalFunctionRegistry; // Global function registry

public:
    SymbolTable();
    ~SymbolTable();

    bool insertSymbol(const std::string& name, SymbolType type, const std::string& returnType = "", const std::vector<std::string>& argTypes = {});
    Symbol* lookupSymbol(const std::string& name);
    bool isFunctionDefined(const std::string& funcName) const;
    bool checkFunctionCall(const std::string& funcName, const std::vector<std::string>& argTypes);
    void enterScope();
    void exitScope();
    void initializeGlobalScope();
};

#endif // SYMBOLTABLE_H
