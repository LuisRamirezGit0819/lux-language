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

---

## Módulo 5 — Parser Parte 2
 
| Archivo | Cambios |
|---|---|
| `src/parser.h` | +`parseBlockStatement()`, +`parseIfExpression()`, +`parseFunctionLiteral()`, +`parseFunctionParameters()`, +`parseCallExpression()` |
| `src/parser.cpp` | Implementación de todos los métodos anteriores + `FN` e `IF` registrados como nud + `LPAREN` despacha a `parseCallExpression` |
| `src/main.cpp` | Pretty-printer extendido + pruebas de cada nuevo nodo + prueba integradora |
 
Completa la gramática del lenguaje Lux. El Parser ahora puede construir el AST completo de cualquier programa válido.
 
### Nuevos métodos del Parser
 
| Método | Tipo | Descripción |
|---|---|---|
| `parseBlockStatement()` | sentencia | Parsea `{ sentencias... }` — bucle hasta encontrar `}` |
| `parseIfExpression()` | nud | Parsea `if (cond) { } else { }` — `else` opcional |
| `parseFunctionLiteral()` | nud | Parsea `fn(params) { cuerpo }` |
| `parseFunctionParameters()` | helper | Lee lista de nombres separados por `,` entre `()` |
| `parseCallExpression(left)` | led | Parsea `expresión(args)` — el `(` como operador infijo |
 
### Decisiones de diseño del Parser
 
| Decisión | Razón |
|---|---|
| `if` es expresión (nud), no sentencia | Permite `let x = if (cond) { a } else { b }` |
| `fn` es expresión (nud), no declaración | Permite funciones anónimas y funciones como valores |
| `parseBlockStatement` termina con `currentToken = }` | El llamador necesita inspeccionar qué viene después del bloque |
| `parseFunctionParameters` devuelve `vector<string>` | Los parámetros son solo nombres — no hace falta un nodo completo |
| `parseCallExpression` es led via `LPAREN` | Permite `f(x)`, `fn(x){x}(5)` y `f()(x)` sin casos especiales |
 
### Programas Lux completos que el Parser maneja correctamente
 
```
let absoluto = fn(x) {
    if (x < 0) { return -x; } else { return x; }
};
 
let maximo = fn(a, b) {
    if (a > b) { return a; } else { return b; }
};
let resultado = maximo(10, 3);
 
let fibonacci = fn(n) {
    if (n < 2) { return n; }
    return fibonacci(n - 1) + fibonacci(n - 2);
};
```

---

## Módulo 6 — Entorno (Scope)

| Archivo | Contenido |
|---|---|
| `src/object.h` | Clase base `Object` — versión mínima, se expandirá en Módulo 7 |
| `src/environment.h` | Clase `Environment` completa con cadena de scopes |

Implementa el sistema de variables del intérprete: dónde se guardan y cómo se buscan.

### Diseño de `Environment`

```
Environment
├── store_  : std::map<string, shared_ptr<Object>>   variables locales
└── outer_  : shared_ptr<Environment>                scope padre (nullptr si global)
```

### Métodos

| Método | Descripción |
|---|---|
| `get(name)` | Busca en el mapa local; si no está, sube al padre recursivamente. Devuelve `nullptr` si no existe en ningún scope. |
| `set(name, value)` | Guarda en el mapa local. Nunca escribe en el padre. |
| `createEnclosed(outer)` | Función estática. Crea un scope hijo con `outer` como padre. |

### Comportamiento de la cadena de scopes

```
Scope S2 (hijo de S1)   →   Scope S1 (hijo de S0)   →   Scope S0 (global)
    b = 3                       a = 5                       x = 10

S2->get("b") → encontrado en S2 → devuelve 3
S2->get("a") → no en S2 → sube → encontrado en S1 → devuelve 5
S2->get("x") → no en S2 → sube → no en S1 → sube → encontrado en S0 → devuelve 10
S2->get("z") → no en S2 → sube → no en S1 → sube → no en S0 → nullptr (no existe)
```

### Reglas de diseño

| Regla | Comportamiento |
|---|---|
| `set` escribe solo en el scope actual | Una variable local nunca toca al padre |
| Shadowing | Una variable local con el mismo nombre que una del padre la oculta sin modificarla |
| Scopes hermanos | Dos hijos del mismo padre no se ven entre sí — la cadena solo va hacia arriba |
| Scope léxico | Una función ve el scope donde fue **definida**, no donde fue **llamada** |

### Por qué `shared_ptr` y no `unique_ptr`

Los scopes pueden ser compartidos por los closures. Cuando una función captura
su entorno de definición, ese scope tiene dos dueños: el intérprete que lo creó
y la función que lo capturó. `shared_ptr` mantiene el scope vivo mientras al
menos uno de los dos lo referencie, y lo destruye automáticamente cuando ambos
desaparecen.

### Concepto C++ nuevo: `shared_ptr`

`shared_ptr<T>` es el puntero inteligente para ownership compartido.
Mantiene un contador de referencias interno. El objeto se destruye
automáticamente cuando el contador llega a cero (ningún `shared_ptr`
lo referencia). A diferencia de `unique_ptr`, puede copiarse libremente.

### Estructura de archivos actualizada

```
lux/
├── src/
│   ├── token.h
│   ├── lexer.h
│   ├── lexer.cpp
│   ├── ast.h
│   ├── parser.h
│   ├── parser.cpp
│   ├── object.h          ← nuevo en Módulo 6
│   ├── environment.h     ← nuevo en Módulo 6
│   └── main.cpp
├── .gitignore
└── README.md
```

---

## Próximo módulo: 7 — Evaluador Parte 1

El Evaluador recorrerá el AST y producirá valores usando el sistema de tipos
(`Object` y sus subclases) y el `Environment` construido en este módulo.