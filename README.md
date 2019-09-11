# ycc

## Overview
This is a project to develop C compiler, which converts source codes into assembly. This compiler imitates mostly  [9cc](https://github.com/rui314/9cc), developed by Rui Ueyama and I follow his website: https://www.sigbus.info/compilerbook  
This C compiler is written by C, so my current goal is compiling itself.  
I develop this compiler incrementally. In other words, I write code of tokenizer, parser, and code generator little by little when I add a new feature.  
"ycc" is named after my name. 

# Generative grammar

A: nonterminal symbol  
e: null

| Expression |          Meaning           |
| :--------: | :------------------------: |
|     A*     | Repeat A more than 0 times |
|     A?     |           A or e           |
|   A \| B   |           A or B           |
|   (...)    |          Grouping          |

```
program    = definition*
define_func = identifier "(" params? ")" "{" stmt * "}"
params     = expr ("," expr)*
stmt       = expr ";"
            | "{" stmt* "}"
            | "if" "(" equality ")" stmt ("else" stmt)?
            | "while "(" equality ")" "{" stmt* "}"
            | "return" expr ";"
            | decl_var ";"
expr       = assign
assign     = or "="? assign
or         = and "||"? and
and        = equality "&&"? equality
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">= add)*
add        = mul ("+" mul | "-" mul)
mul        = unary ("*" unary | "/" unary)*
unary      = "+" term
            | "-" term
            | "!" or
            | "&" unary
            | "*" unary
            | term
term       = num | identifier ("(" params? ")")? | "(" expr ")"
decl_var   = type identifier
```
