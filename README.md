# Lux_Language

Intérprete de lenguaje de programación construido desde cero en C++.

## Compilar

```bash
g++ -std=c++17 -I src src/main.cpp src/lexer.cpp -o lux-language
```

> A medida que agreguemos archivos `.cpp` al proyecto, hay que añadirlos al comando.

## Ejecutar

```bash
./lux-language
```

En Windows (PowerShell):
```powershell
.\lux-language.exe
```

## Arquitectura

### Módulo 1-2 — Lexer y Tokens
`src/token.h` — enum `TokenType`, struct `Token`, función `tokenTypeToString()`  
`src/lexer.h` / `src/lexer.cpp` — clase `Lexer`, método `tokenize()`

Convierte el código fuente en una lista plana de tokens.

### Módulo 3 — AST
`src/ast.h` — clase base `Node` y 12 nodos concretos

Define la estructura jerárquica que representa el código.
Los nodos usan `unique_ptr` para manejo automático de memoria.

| Nodo | Descripción |
|---|---|
| `Program` | Raíz del árbol — lista de sentencias |
| `LetStatement` | `let nombre = valor` |
| `ReturnStatement` | `return valor` |
| `ExpressionStatement` | Expresión usada como sentencia |
| `BlockStatement` | `{ sentencias... }` |
| `NumberLiteral` | Entero literal: `42` |
| `BooleanLiteral` | `true` o `false` |
| `Identifier` | Nombre de variable: `x` |
| `BinaryExpression` | `a + b`, `x == 5` |
| `PrefixExpression` | `-x`, `!true` |
| `IfExpression` | `if (cond) { } else { }` |
| `FunctionLiteral` | `fn(a, b) { ... }` |
| `CallExpression` | `suma(3, 4)` |