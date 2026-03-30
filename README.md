# Lux_Language

Intérprete de lenguaje de programación construido desde cero en C++.

## Compilar

```bash
g++ -std=c++17 -I src src/main.cpp src/lexer.cpp src/parser.cpp -o lux-language
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

## Módulo 4 — Parser Parte 1

| Archivo | Contenido |
|---|---|
| `src/parser.h` | Declaración de la clase `Parser`, enum `Precedence` |
| `src/parser.cpp` | Implementación del Pratt Parser |

Convierte la lista de tokens en un AST navegable.
Implementa el algoritmo Pratt Parser (Top-Down Operator Precedence).

### Precedencias

| Nivel | Nombre | Operadores |
|---|---|---|
| 0 | LOWEST | inicio de expresión |
| 1 | EQUALS | `==` `!=` |
| 2 | LESSGREATER | `<` `>` `<=` `>=` |
| 3 | SUM | `+` `-` |
| 4 | PRODUCT | `*` `/` |
| 5 | PREFIX | `-x` `!x` |
| 6 | CALL | `f(x)` |

### Métodos principales

| Método | Rol Pratt |
|---|---|
| `parseExpression(minima)` | Algoritmo central |
| `parseIdentifier()` / `parseNumberLiteral()` / etc. | nud |
| `parseInfixExpression(left)` | led |
```

---

Cuando compiles y corras, deberías ver el AST impreso para cada prueba. Por ejemplo para `2 + 3 * 4;`:
```
((2 + (3 * 4)));