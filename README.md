# ycc (young c compiler)

## Overview
This is a project to develop C compiler, which converts source codes into assembly. This compiler imitates mostly  [9cc](https://github.com/rui314/9cc), developed by Rui Ueyama and I follow his website: https://www.sigbus.info/compilerbook  
This C compiler is written by C, so my current goal is compiling itself.  
I develop this compiler incrementally. In other words, I write code of tokenizer, parser, and code generator little by little when I add a new feature. 

# Generative grammer

A: nonterminal symbol  
e: null

| Expression |          Meaning           |
| :--------: | :------------------------: |
|     A*     | Repeat A more than 0 times |
|     A?     |           A or e           |
|   A \| B   |           A or B           |
|   (...)    |          Grouping          |

```
program    = stmt*
stmt       = expr ";"
            | "{" stmt* "}"
            | "if" "(" expr ")" stmt ("else" stmt)?
expr       = assign
assign     = equality
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">= add)*
add        = mul ("+" mul | "-" mul)
mul        = unary ("*" unary | "/" unary)*
unary      = ("+" | "-")? term
term       = num | ident ("(" ")")? | "(" expr ")"
```
