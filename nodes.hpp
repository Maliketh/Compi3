#ifndef NODES_HPP
#define NODES_HPP

#include <memory>
#include <string>
#include <vector>
#include "visitor.hpp"

namespace ast {

    /* Arithmetic operations */
    enum BinOpType {
        ADD, // Addition
        SUB, // Subtraction
        MUL, // Multiplication
        DIV  // Division
    };

    /* Relational operations */
    enum RelOpType {
        EQ, // Equal
        NE, // Not equal
        LT, // Less than
        GT, // Greater than
        LE, // Less than or equal
        GE  // Greater than or equal
    };


    /* Built-in types */


    /* Base class for all AST nodes */
    class Node {
    public:
        // Line number in the source code
        int line;

        // Use this constructor only while parsing in bison or flex
        Node();

        // Accept method for visitor pattern
        virtual BuiltInType accept(Visitor &visitor) = 0;
        virtual  BuiltInType accept(Visitor &visitor, int* p) = 0;
        virtual  BuiltInType accept(Visitor &visitor, std::vector<BuiltInType>* params_type,std::vector<std::string>* params_name) = 0;
    };

    /* Base class for all expressions */
    class Exp : virtual public Node {
    public:
        Exp() = default;
    };

    /* Base class for all statements */
    class Statement : virtual public Node {
    public:
        bool is_scope;

        Statement () :is_scope(false) {};
    };

    /* Number literal */
    class Num : public Exp {
    public:
        // Value of the number
        int value;

        // Constructor that receives a C-style string that represents the number
        explicit Num(const char *str);

        BuiltInType accept(Visitor &visitor, int* p_val) override {
            return visitor.visit(*this, p_val);
        }
        BuiltInType accept(Visitor &visitor) override {
            return accept(visitor, nullptr);
        }
        BuiltInType accept(Visitor &visitor, std::vector<BuiltInType>* params_type,std::vector<std::string>* params_name) override {
            return accept(visitor);
        }
    };

    /* Byte literal */
    class NumB : public Exp {
    public:
        // Value of the number
        int value;

        // Constructor that receives a C-style (including b character) string that represents the number
        explicit NumB(const char *str);

        BuiltInType accept(Visitor &visitor, int* p_val) override {
            return visitor.visit(*this, p_val);
        }
        BuiltInType accept(Visitor &visitor) override {
            return accept(visitor, nullptr);
        }
        BuiltInType accept(Visitor &visitor, std::vector<BuiltInType>* params_type,std::vector<std::string>* params_name) override {
            return accept(visitor);
        }
    };

    /* String literal */
    class String : public Exp {
    public:
        // Value of the string
        std::string value;

        // Constructor that receives a C-style string that represents the string *including quotes*
        explicit String(const char *str);

        BuiltInType accept(Visitor &visitor, int* p_val) override {
            return visitor.visit(*this, p_val);
        }
        BuiltInType accept(Visitor &visitor) override {
            return accept(visitor, nullptr);
        }
        BuiltInType accept(Visitor &visitor, std::vector<BuiltInType>* params_type,std::vector<std::string>* params_name) override {
            return accept(visitor);
        }
    };

    /* Boolean literal */
    class Bool : public Exp {
    public:
        // Value of the boolean
        bool value;

        // Constructor that receives the boolean value
        explicit Bool(bool value);

        BuiltInType accept(Visitor &visitor, int* p_val) override {
            return visitor.visit(*this, p_val);
        }
        BuiltInType accept(Visitor &visitor) override {
            return accept(visitor, nullptr);
        }
        BuiltInType accept(Visitor &visitor, std::vector<BuiltInType>* params_type,std::vector<std::string>* params_name) override {
            return accept(visitor);
        }
    };

    /* Identifier */
    class ID : public Exp {
    public:
        // Name of the identifier
        std::string value;

        // Constructor that receives a C-style string that represents the identifier
        explicit ID(const char *str);

        BuiltInType accept(Visitor &visitor, int* p_val) override {
            return visitor.visit(*this, p_val);
        }
        BuiltInType accept(Visitor &visitor) override {
            return accept(visitor, nullptr);
        }
        BuiltInType accept(Visitor &visitor, std::vector<BuiltInType>* params_type,std::vector<std::string>* params_name) override {
            return accept(visitor);
        }
    };


    /* Binary arithmetic operation */
    class BinOp : public Exp {
    public:
        // Left operand
        std::shared_ptr <Exp> left;
        // Right operand
        std::shared_ptr <Exp> right;
        // Operation
        BinOpType op;

        // Constructor that receives the left and right operands and the operation
        BinOp(std::shared_ptr <Exp> left, std::shared_ptr <Exp> right, BinOpType op);


        BuiltInType accept(Visitor &visitor, int* p_val) override {
            return visitor.visit(*this, p_val);
        }
        BuiltInType accept(Visitor &visitor) override {
            return accept(visitor, nullptr);
        }
        BuiltInType accept(Visitor &visitor, std::vector<BuiltInType>* params_type,std::vector<std::string>* params_name) override {
            return accept(visitor);
        }
    };

    /* Binary relational operation */
    class RelOp : public Exp {
    public:
        // Left operand
        std::shared_ptr <Exp> left;
        // Right operand
        std::shared_ptr <Exp> right;
        // Operation
        RelOpType op;

        // Constructor that receives the left and right operands and the operation
        RelOp(std::shared_ptr <Exp> left, std::shared_ptr <Exp> right, RelOpType op);

        BuiltInType accept(Visitor &visitor, int* p_val) override {
            return visitor.visit(*this, p_val);
        }
        BuiltInType accept(Visitor &visitor) override {
            return accept(visitor, nullptr);
        }
        BuiltInType accept(Visitor &visitor, std::vector<BuiltInType>* params_type,std::vector<std::string>* params_name) override {
            return accept(visitor);
        }
    };

    /* Unary logical NOT operation */
    class Not : public Exp {
    public:
        // Operand
        std::shared_ptr <Exp> exp;

        // Constructor that receives the operand
        explicit Not(std::shared_ptr <Exp> exp);

        BuiltInType accept(Visitor &visitor, int* p_val) override {
            return visitor.visit(*this, p_val);
        }
        BuiltInType accept(Visitor &visitor) override {
            return accept(visitor, nullptr);
        }
        BuiltInType accept(Visitor &visitor, std::vector<BuiltInType>* params_type,std::vector<std::string>* params_name) override {
            return accept(visitor);
        }
    };

    /* Binary logical AND operation */
    class And : public Exp {
    public:
        // Left operand
        std::shared_ptr <Exp> left;
        // Right operand
        std::shared_ptr <Exp> right;

        // Constructor that receives the left and right operands
        And(std::shared_ptr <Exp> left, std::shared_ptr <Exp> right);

        BuiltInType accept(Visitor &visitor, int* p_val) override {
            return visitor.visit(*this, p_val);
        }
        BuiltInType accept(Visitor &visitor) override {
            return accept(visitor, nullptr);
        }
        BuiltInType accept(Visitor &visitor, std::vector<BuiltInType>* params_type,std::vector<std::string>* params_name) override {
            return accept(visitor);
        }
    };

    /* Binary logical OR operation */
    class Or : public Exp {
    public:
        // Left operand
        std::shared_ptr <Exp> left;
        // Right operand
        std::shared_ptr <Exp> right;

        // Constructor that receives the left and right operands
        Or(std::shared_ptr <Exp> left, std::shared_ptr <Exp> right);

        BuiltInType accept(Visitor &visitor, int* p_val) override {
            return visitor.visit(*this, p_val);
        }
        BuiltInType accept(Visitor &visitor) override {
            return accept(visitor, nullptr);
        }
        BuiltInType accept(Visitor &visitor, std::vector<BuiltInType>* params_type,std::vector<std::string>* params_name) override {
            return accept(visitor);
        }
    };

    /* Type symbol */
    class Type : public Node {
    public:
        // Type
        BuiltInType type;

        // Constructor that receives the type
        explicit Type(BuiltInType type);

        BuiltInType accept(Visitor &visitor) override {
            return visitor.visit(*this);
        }
        BuiltInType accept(Visitor &visitor, int* p) override {
            return accept(visitor);
        }
        BuiltInType accept(Visitor &visitor, std::vector<BuiltInType>* params_type,std::vector<std::string>* params_name) override {
            return accept(visitor);
        }
    };


    /* Type cast */
    class Cast : public Exp {
    public:
        // Expression to be cast
        std::shared_ptr <Exp> exp;
        // Target type
        std::shared_ptr <Type> target_type;

        // Constructor that receives the expression and the target type
        Cast(std::shared_ptr <Exp> exp, std::shared_ptr <Type> type);

        BuiltInType accept(Visitor &visitor, int* p_val) override {
            return visitor.visit(*this, p_val);
        }
        BuiltInType accept(Visitor &visitor) override {
            return accept(visitor, nullptr);
        }
        BuiltInType accept(Visitor &visitor, std::vector<BuiltInType>* params_type,std::vector<std::string>* params_name) override {
            return accept(visitor);
        }
    };

    /* List of expressions */
    class ExpList : public Node {
    public:
        // List of expressions
        std::vector <std::shared_ptr<Exp>> exps;

        // Constructor that receives no expressions
        ExpList() = default;

        // Constructor that receives the first expression
        explicit ExpList(std::shared_ptr <Exp> exp);

        // Method to add an expression at the beginning of the list
        void push_front(const std::shared_ptr <Exp> &exp);

        // Method to add an expression at the end of the list
        void push_back(const std::shared_ptr <Exp> &exp);

        BuiltInType accept(Visitor &visitor) override {
            return visitor.visit(*this, nullptr);
        }
        BuiltInType accept(Visitor &visitor, int* p) override {
            return accept(visitor);
        }
        BuiltInType accept(Visitor &visitor, std::vector<BuiltInType>* params_type,std::vector<std::string>* params_name) override {
            return accept(visitor);
        }
    };

    /* Function call */
    class Call : public Exp, public Statement {
    public:
        // Function identifier
        std::shared_ptr <ID> func_id;
        // List of arguments as expressions
        std::shared_ptr <ExpList> args;

        // Constructor that receives the function identifier and the list of arguments
        Call(std::shared_ptr <ID> func_id, std::shared_ptr <ExpList> args);

        // Constructor that receives only the function identifier (for parameterless functions)
        explicit Call(std::shared_ptr <ID> func_id);

        BuiltInType accept(Visitor &visitor, int* p_val) override {
            return visitor.visit(*this, p_val);
        }

        BuiltInType accept(Visitor &visitor) override {
            return visitor.visit(*this);
        }
        BuiltInType accept(Visitor &visitor, std::vector<BuiltInType>* params_type,std::vector<std::string>* params_name) override {
            return accept(visitor);
        }
    };

    /* List of statements */
    class Statements : public Statement {
    public:
        // List of statements
        std::vector <std::shared_ptr<Statement>> statements;
        // Constructor that receives no statements
        Statements() = default;

        // Constructor that receives the first statement
        explicit Statements(std::shared_ptr <Statement> statement);

        // Method to add a statement at the beginning of the list
        void push_front(const std::shared_ptr <Statement> &statement);

        // Method to add a statement at the end of the list
        void push_back(const std::shared_ptr <Statement> &statement);

        BuiltInType accept(Visitor &visitor) override {
            return visitor.visit(*this );
        }
        BuiltInType accept(Visitor &visitor, int* p) override {
            return accept(visitor);
        }
        BuiltInType accept(Visitor &visitor, std::vector<BuiltInType>* params_type,std::vector<std::string>* params_name) override {
            return accept(visitor);
        }
    };

    /* Break statement */
    class Break : public Statement {
        BuiltInType accept(Visitor &visitor) override {
            return visitor.visit(*this );
        }
        BuiltInType accept(Visitor &visitor, int* p) override {
            return accept(visitor);
        }
        BuiltInType accept(Visitor &visitor, std::vector<BuiltInType>* params_type,std::vector<std::string>* params_name) override {
            return accept(visitor);
        }
    };

    /* Continue statement */
    class Continue : public Statement {
        BuiltInType accept(Visitor &visitor) override {
            return visitor.visit(*this );
        }
        BuiltInType accept(Visitor &visitor, int* p) override {
            return accept(visitor);
        }
        BuiltInType accept(Visitor &visitor, std::vector<BuiltInType>* params_type,std::vector<std::string>* params_name) override {
            return accept(visitor);
        }
    };

    /* Return statement */
    class Return : public Statement {
    public:
        // Expression to be returned. If the return is expressionless, this field is nullptr
        std::shared_ptr <Exp> exp;

        // Constructor that receives the expression to be returned
        explicit Return(std::shared_ptr <Exp> exp = nullptr);

        BuiltInType accept(Visitor &visitor) override {
            return visitor.visit(*this );
        }
        BuiltInType accept(Visitor &visitor, int* p) override {
            return accept(visitor);
        }
        BuiltInType accept(Visitor &visitor, std::vector<BuiltInType>* params_type,std::vector<std::string>* params_name) override {
            return accept(visitor);
        }
    };

    /* If statement */
    class If : public Statement {
    public:
        // Condition expression
        std::shared_ptr <Exp> condition;
        // Statement to be executed if the condition is true
        std::shared_ptr <Statement> then;
        // Statement to be executed if the condition is false. For an if statement without else, this field is nullptr
        std::shared_ptr <Statement> otherwise;

        // Constructor that receives the condition, the statement to be executed if the condition is true, and the statement to be executed if the condition is false
        If(std::shared_ptr <Exp> condition, std::shared_ptr <Statement> then,
           std::shared_ptr <Statement> otherwise = nullptr);

        BuiltInType accept(Visitor &visitor) override {
            return visitor.visit(*this );
        }
        BuiltInType accept(Visitor &visitor, int* p) override {
            return accept(visitor);
        }
        BuiltInType accept(Visitor &visitor, std::vector<BuiltInType>* params_type,std::vector<std::string>* params_name) override {
            return accept(visitor);
        }
    };

    /* While statement */
    class While : public Statement {
    public:
        // Condition expression
        std::shared_ptr <Exp> condition;
        // Statement to be executed while the condition is true
        std::shared_ptr <Statement> body;

        // Constructor that receives the condition and the statement to be executed while the condition is true
        While(std::shared_ptr <Exp> condition, std::shared_ptr <Statement> body);

        BuiltInType accept(Visitor &visitor) override {
            return visitor.visit(*this );
        }
        BuiltInType accept(Visitor &visitor, int* p) override {
            return accept(visitor);
        }
        BuiltInType accept(Visitor &visitor, std::vector<BuiltInType>* params_type,std::vector<std::string>* params_name) override {
            return accept(visitor);
        }
    };

    /* Variable declaration */
    class VarDecl : public Statement {
    public:
        // Identifier of the variable
        std::shared_ptr <ID> id;
        // Type of the variable
        std::shared_ptr <Type> type;
        // Initial value of the variable. If the variable is not initialized, this field is nullptr
        std::shared_ptr <Exp> init_exp;

        // Constructor that receives the identifier, the type, and the initial value expression
        VarDecl(std::shared_ptr <ID> id, std::shared_ptr <Type> type, std::shared_ptr <Exp> init_exp = nullptr);

        BuiltInType accept(Visitor &visitor) override {
            return visitor.visit(*this );
        }
        BuiltInType accept(Visitor &visitor, int* p) override {
            return accept(visitor);
        }
        BuiltInType accept(Visitor &visitor, std::vector<BuiltInType>* params_type,std::vector<std::string>* params_name) override {
            return accept(visitor);
        }
    };

    /* Assignment statement */
    class Assign : public Statement {
    public:
        // Identifier of the variable
        std::shared_ptr <ID> id;
        // Expression to be assigned
        std::shared_ptr <Exp> exp;

        // Constructor that receives the identifier and the expression to be assigned
        Assign(std::shared_ptr <ID> id, std::shared_ptr <Exp> exp);

        BuiltInType accept(Visitor &visitor) override {
            return visitor.visit(*this );
        }
        BuiltInType accept(Visitor &visitor, int* p) override {
            return accept(visitor);
        }
        BuiltInType accept(Visitor &visitor, std::vector<BuiltInType>* params_type,std::vector<std::string>* params_name) override {
            return accept(visitor);
        }
    };

    /* Formal parameter */
    class Formal : public Node {
    public:
        // Identifier of the parameter
        std::shared_ptr <ID> id;
        // Type of the parameter
        std::shared_ptr <Type> type;

        // Constructor that receives the identifier and the type
        Formal(std::shared_ptr <ID> id, std::shared_ptr <Type> type);

        BuiltInType accept(Visitor &visitor) override {
            return visitor.visit(*this );
        }
        BuiltInType accept(Visitor &visitor, int* p) override {
            return accept(visitor);
        }
        BuiltInType accept(Visitor &visitor, std::vector<BuiltInType>* params_type,std::vector<std::string>* params_name) override {
            return accept(visitor);
        }
    };

    /* List of formal parameters */
    class Formals : public Node {
    public:
        // List of formal parameters
        std::vector <std::shared_ptr<Formal>> formals;

        // Constructor that receives no parameters
        Formals() = default;

        // Constructor that receives the first formal parameter
        explicit Formals(std::shared_ptr <Formal> formal);

        // Method to add a formal parameter at the beginning of the list
        void push_front(const std::shared_ptr <Formal> &formal);

        // Method to add a formal parameter at the end of the list
        void push_back(const std::shared_ptr <Formal> &formal);

        BuiltInType accept(Visitor &visitor, std::vector<BuiltInType>* params_type,std::vector<std::string>* params_name) override {
            return visitor.visit(*this , params_type, params_name);
        }
        BuiltInType accept(Visitor &visitor, int* p) override {
            return accept(visitor, nullptr, nullptr);
        }
        BuiltInType accept(Visitor &visitor) override {
            return accept(visitor, nullptr, nullptr);
        }
    };

    /* Function declaration */
    class FuncDecl : public Node {
    public:
        // Identifier of the function
        std::shared_ptr <ID> id;
        // Return type of the function
        std::shared_ptr <Type> return_type;
        // List of formal parameters
        std::shared_ptr <Formals> formals;
        // Body of the function
        std::shared_ptr <Statements> body;

        // Constructor that receives the identifier, the return type, the list of formal parameters, and the body
        FuncDecl(std::shared_ptr <ID> id, std::shared_ptr <Type> return_type, std::shared_ptr <Formals> formals,
                 std::shared_ptr <Statements> body);

        BuiltInType accept(Visitor &visitor) override {
            return visitor.visit(*this );
        }
        BuiltInType accept(Visitor &visitor, int* p) override {
            return accept(visitor);
        }
        BuiltInType accept(Visitor &visitor, std::vector<BuiltInType>* params_type,std::vector<std::string>* params_name) override {
            return accept(visitor);
        }
    };

    /* List of function declarations */
    class Funcs : public Node {
    public:
        // List of function declarations
        std::vector <std::shared_ptr<FuncDecl>> funcs;

        // Constructor that receives no function declarations
        Funcs() = default;

        // Constructor that receives the first function declaration
        explicit Funcs(std::shared_ptr <FuncDecl> func);

        // Method to add a function declaration at the beginning of the list
        void push_front(const std::shared_ptr <FuncDecl> &func);

        // Method to add a function declaration at the end of the list
        void push_back(const std::shared_ptr <FuncDecl> &func);

        BuiltInType accept(Visitor &visitor) override {
            return visitor.visit(*this );
        }
        BuiltInType accept(Visitor &visitor, int* p) override {
            return accept(visitor);
        }
        BuiltInType accept(Visitor &visitor, std::vector<BuiltInType>* params_type,std::vector<std::string>* params_name) override {
            return accept(visitor);
        }
    };

}



#define YYSTYPE std::shared_ptr<ast::Node>

#endif //NODES_HPP
