#ifndef SEMANTIC_ANALYZER_HPP
#define SEMANTIC_ANALYZER_HPP

#include "Visitor.hpp"
#include <iostream>

class SemanticAnalyzer : public Visitor {
public:
    // Implement visit methods for each type of AST node

    void visit(ast::Num& node) override {
        std::cout << "Analyzing Num node" << std::endl;
    }

    void visit(ast::NumB& node) override {
        std::cout << "Analyzing NumB node" << std::endl;
    }

    void visit(ast::String& node) override {
        std::cout << "Analyzing String node" << std::endl;
    }

    void visit(ast::Bool& node) override {
        std::cout << "Analyzing Bool node" << std::endl;
    }

    void visit(ast::ID& node) override {
        std::cout << "Analyzing ID node" << std::endl;
    }

    void visit(ast::BinOp& node) override {
        std::cout << "Analyzing BinOp node" << std::endl;
    }

    void visit(ast::RelOp& node) override {
        std::cout << "Analyzing RelOp node" << std::endl;
    }

    void visit(ast::Not& node) override {
        std::cout << "Analyzing Not node" << std::endl;
    }

    void visit(ast::And& node) override {
        std::cout << "Analyzing And node" << std::endl;
    }

    void visit(ast::Or& node) override {
        std::cout << "Analyzing Or node" << std::endl;
    }

    void visit(ast::Type& node) override {
        std::cout << "Analyzing Type node" << std::endl;
    }

    void visit(ast::Cast& node) override {
        std::cout << "Analyzing Cast node" << std::endl;
    }

    void visit(ast::ExpList& node) override {
        std::cout << "Analyzing ExpList node" << std::endl;
    }

    void visit(ast::Call& node) override {
        std::cout << "Analyzing Call node" << std::endl;
    }

    void visit(ast::Statements& node) override {
        std::cout << "Analyzing Statements node" << std::endl;
    }

    void visit(ast::Break& node) override {
        std::cout << "Analyzing Break node" << std::endl;
    }

    void visit(ast::Continue& node) override {
        std::cout << "Analyzing Continue node" << std::endl;
    }

    void visit(ast::Return& node) override {
        std::cout << "Analyzing Return node" << std::endl;
    }

    void visit(ast::If& node) override {
        std::cout << "Analyzing If node" << std::endl;
    }

    void visit(ast::While& node) override {
        std::cout << "Analyzing While node" << std::endl;
    }

    void visit(ast::VarDecl& node) override {
        std::cout << "Analyzing VarDecl node" << std::endl;
    }

    void visit(ast::Assign& node) override {
        std::cout << "Analyzing Assign node" << std::endl;
    }

    void visit(ast::Formal& node) override {
        std::cout << "Analyzing Formal node" << std::endl;
    }

    void visit(ast::Formals& node) override {
        std::cout << "Analyzing Formals node" << std::endl;
    }

    void visit(ast::FuncDecl& node) override {
        std::cout << "Analyzing FuncDecl node" << std::endl;
    }

    void visit(ast::Funcs& node) override {
        std::cout << "Analyzing Funcs node" << std::endl;
    }
};

#endif // SEMANTIC_ANALYZER_HPP
