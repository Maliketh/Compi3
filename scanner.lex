%{
#include "output.hpp"
#include "parser.tab.h"
#include "string"
%}

%option yylineno
%option noyywrap

whitespace           [ \t\n\r]
num                 (0|[1-9][0-9]*)

printable_ascii    [\x20-\x21\x23-\x7E]



%%

void                    return VOID;
int                     return INT;
byte                    return BYTE;
bool                    return BOOL;
and                     return AND;
or                      return OR;
not                     return NOT;
true                    return TRUE;
false                   return FALSE;
return                  return RETURN;
if                      return IF;
else                    return ELSE;
while                   return WHILE;
break                   return BREAK;
continue                return CONTINUE;

;                       return SC;
,                       return COMMA;
\(                      return LPAREN;
\)                      return RPAREN;
\{                      return LBRACE;
\}                      return RBRACE;
=                       return ASSIGN;

(==)                    return RELOP_EQ;
(!=)                    return RELOP_NEQ;
(<)                     return RELOP_LE;
(>)                     return RELOP_GE;
(<=)                    return RELOP_LEQ;
(>=)                    return RELOP_GEQ;


[\+]                 return BINOP_ADD;
[\/]                 return BINOP_DIV;
[\-]                 return BINOP_SUB;
[\*]                 return BINOP_MUL;

\/\/[^\n\r]*           ;



[a-zA-Z][a-zA-Z0-9]*    {yylval = std::make_shared<ast::ID>(yytext); return ID;}
(0|[1-9][0-9]*)         {  yylval = std::make_shared<ast::Num>(yytext); ; return NUM; };
(0|[1-9][0-9]*)+b       {  yylval = std::make_shared<ast::NumB>(yytext); ; return NUM_B; };


\"([^"\\]|\\.)*\"        { yylval=std::make_shared<ast::String>(yytext); return STRING; }
\"{printable_ascii}*\"   { yylval=std::make_shared<ast::String>(yytext); return STRING; }

{whitespace}            ;
.                       output::errorLex(yylineno); return ERR_GENERAL;





%%