# MiniPascal Language Specification

## Overview

MiniPascal is a subset of Pascal supporting integer arithmetic, conditionals, loops, functions with recursion, and basic I/O. All values are 32-bit signed integers.

## Grammar (EBNF)

```ebnf
program      = 'begin' decl_list ';' stmt_list 'end' .

decl_list    = decl { ';' decl } .
decl         = var_decl | func_decl .
var_decl     = 'integer' IDENT .
func_decl    = 'integer' 'function' IDENT '(' var_decl ')' ';'
               'begin' decl_list ';' stmt_list 'end' .

stmt_list    = stmt { ';' stmt } .
stmt         = assign_stmt | read_stmt | write_stmt | if_stmt
             | while_stmt | compound_stmt .
assign_stmt  = IDENT ':=' expr .
read_stmt    = 'read' '(' IDENT ')' .
write_stmt   = 'write' '(' expr ')' .
if_stmt      = 'if' condition 'then' stmt 'else' stmt .
while_stmt   = 'while' condition 'do' stmt .
compound_stmt = 'begin' stmt_list 'end' .

condition    = expr relop expr .
relop        = '<' | '<=' | '>' | '>=' | '=' | '<>' .

expr         = term { ('+' | '-') term } .
term         = factor { ('*' | 'div' | 'mod') factor } .
factor       = CONSTANT | IDENT | IDENT '(' expr ')' | '(' expr ')' .

IDENT        = letter { letter | digit | '_' } .
CONSTANT     = digit { digit } .
```

## Keywords

```
begin  end  integer  if  then  else  function  read  write  while  do  div  mod
```

## Operators

| Operator | Meaning               |
|----------|-----------------------|
| `+`      | Addition              |
| `-`      | Subtraction           |
| `*`      | Multiplication        |
| `div`    | Integer division      |
| `mod`    | Modulo                |
| `:=`     | Assignment            |
| `=`      | Equality comparison   |
| `<>`     | Not equal             |
| `<`      | Less than             |
| `<=`     | Less or equal         |
| `>`      | Greater than          |
| `>=`     | Greater or equal      |

## Types

Only `integer` (32-bit signed) is supported. All variables, parameters, and function return values are integers.

## Functions

- Functions take exactly **one** integer parameter.
- Functions return an integer by assigning to the function's own name (e.g., `f := expr`).
- Recursion is supported.
- Functions must be declared before use in the declaration section.

## I/O

- `read(x)` reads one integer from stdin.
- `write(expr)` writes one integer to stdout followed by a newline.

## Limitations

- **No arrays or records.** All data is scalar integers.
- **No string type.** Only integer literals and variables.
- **No boolean type.** Conditions evaluate to 0 (false) or 1 (true) internally.
- **Single parameter functions only.** Multi-parameter functions are not supported.
- **Mandatory else.** `if-then-else` requires both branches. Use a no-op assignment for unused branches.
- **No `for` loops.** Use `while` instead.
- **No nested function declarations.** Functions are declared at the top level only.
- **No `var` keyword.** Declarations use `integer` directly.
- **No unary minus in expressions.** Use `0 - x` instead.
- **Integer overflow.** No overflow detection; wraps silently via C++ `int` semantics.
